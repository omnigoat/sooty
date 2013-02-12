//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_COMMON_NODE_HPP
#define SOOTY_COMMON_NODE_HPP
//=====================================================================
#include <deque>
#include <vector>
#include <functional>
#include <memory>
#include <algorithm>
#include <stack>
//=====================================================================
#include <atma/algorithm.hpp>
#include <atma/assert.hpp>
//=====================================================================
#include <sooty/common/detail/clone_impl.hpp>
//=====================================================================
namespace sooty {
namespace common {
//=====================================================================
	
	//=====================================================================	
	// forward declares
	//=====================================================================
	template <typename Command> struct node_t;

	
	//=====================================================================
	// no_orders
	//=====================================================================
	struct no_orders {};
	
	//=====================================================================
	// node
	//=====================================================================
	template <typename Command>
	struct node_t : std::enable_shared_from_this<node_t<Command>>
	{
		enum class type_t {
			actor,
			control,
			placeholder,
			backreference
		};

		// node-ptr / command_t
		typedef std::shared_ptr<node_t> node_ptr;
		typedef Command command_t;
		
		// commands
		typedef std::deque<std::pair<bool, command_t>> commands_t;

		// children: sort by descending number of commands
		struct ordering_t;
		typedef std::set<node_ptr, ordering_t> children_t;
		
		struct merged_ordering_t;


		// constructors
		node_t(type_t);
		node_t(node_t const& rhs);
		node_t(node_t&& rhs);
		~node_t();

		auto operator = (node_t const&) -> node_t&;
		//auto operator = (node_t&& rhs) -> node_t&;
		
		
		// pure
		auto type() const -> type_t { return type_; }
		auto clone() -> node_ptr;

		
		// mutators
		auto push_back_command(const command_t&) -> node_ptr;
		auto push_back_failure(const command_t&) -> node_ptr;
		auto push_back_action(bool, const command_t&) -> node_ptr;
		auto add_child(node_ptr const&) -> node_ptr;
		auto add_self_as_child() -> node_ptr;
		auto append(node_ptr const&, bool append_to_backreference = true) -> node_ptr;
		auto append_self() -> node_ptr;
		auto merge(node_ptr const&) -> node_ptr;

		
		static auto make_backreference() -> node_ptr {  return node_ptr(new node_t(type_t::backreference));  }
		static auto make_placeholder() -> node_ptr {  return node_ptr(new node_t(type_t::placeholder));  }
		static auto make() -> node_ptr {  return node_ptr(new node_t(type_t::control));  }

		static bool is_failure(const std::pair<bool, command_t>& C) {
			return !C.first;
		}
		
		static bool is_command(const std::pair<bool, command_t>& C) {
			return C.first;
		}
		
		static bool equal_or_clone(node_ptr const& lhs, node_ptr const& rhs)
		{
			std::stack<node_t*> nodes;
			nodes.push(lhs.get());
			while (!nodes.empty()) {
				auto x = nodes.top();
				nodes.pop();
				if (x == rhs.get())
					return true;
				for (auto& y : x->clones_)
					nodes.push(y);
			}

			return false;
		}

		static bool share_ancestry(node_ptr const& lhs, node_ptr const& rhs)
		{
			return !lhs->ancestry_.empty() && !rhs->ancestry_.empty() && lhs->ancestry_.back() == rhs->ancestry_.back();
		}


	public:
		// mutators
		auto append_impl(std::map<node_ptr, int>& visited, node_ptr node) -> node_ptr;
		
		// statics
		static auto merge_commands(commands_t& combined, commands_t& new_lhs, commands_t& new_rhs, commands_t& lhs, commands_t& rhs) -> void;
		static auto clone_command(const std::pair<bool, command_t>& C) -> std::pair<bool, command_t>;

		// members
		type_t type_;
		commands_t commands_;
		children_t children_;
		std::set<node_t*> clones_;
		std::vector<node_t*> ancestry_;

		// friends
		template <typename ExecutorT> friend struct performer_t;
		template <typename NodePtr> friend NodePtr detail::clone_tree_impl(std::map<NodePtr, NodePtr>& visited_nodes, const NodePtr& clonee);

		template <typename node_ptr_tm, typename accumulator_tm, typename FN>
		friend void accumulate_depth_first(node_ptr_tm const& root, accumulator_tm acc, FN fn);

		template <typename node_ptr_tm, typename FN>
		void for_each_depth_first(node_ptr_tm const& root, FN fn);
	};

	template <typename Command>
	struct node_t<Command>::ordering_t {
		bool operator () (node_ptr const& lhs, node_ptr const& rhs) const
		{
			if (lhs->type_ != rhs->type_)
				return lhs->type_ < rhs->type_;
			
			if (lhs->commands_ < rhs->commands_)
				return true;
			else if (rhs->commands_ < lhs->commands_)
				return false;

			return lhs.get() < rhs.get();
		};
	};

	template <typename Command>
	struct node_t<Command>::merged_ordering_t {
		bool operator () (node_ptr const& lhs, node_ptr const& rhs) const
		{
			// descending order of commands
			if (lhs->commands_ > rhs->commands_)
				return true;
			else if (rhs->commands_ > lhs->commands_)
				return false;

			// if the two nodes share part of their ancestry
			for (auto& x : lhs->ancestry_) {
				if (std::find(rhs->ancestry_.begin(), rhs->ancestry_.end(), x) != rhs->ancestry_.end())
					return false;
			}

			return lhs.get() < rhs.get();
		};
	};

	template <typename node_ptr_tm, typename acc_tm, typename FN>
	void accumulate_depth_first(node_ptr_tm const& root, acc_tm acc, FN fn)
	{
		typedef std::tuple<node_ptr_tm, acc_tm> value_t;
		std::stack<value_t> nodes;
		nodes.push(std::make_tuple(root, acc));
		std::set<node_ptr_tm> visited;
		while (!nodes.empty())
		{
			auto x = nodes.top();
			auto const& xn = std::get<0>(x);
			auto const& xa = std::get<1>(x);
			nodes.pop();
			if (visited.find(xn) != visited.end())
				continue;
			visited.insert(xn);

			typename node_ptr_tm::element_type::children_t children = xn->children_;
			
			// call fn, which returns the new acc
			acc_tm combined_acc = fn(xa, xn);

			for (auto const& y : children) {
				nodes.push( std::make_tuple(y, combined_acc) );
			}
		}
	}

	template <typename node_ptr_tm, typename FN>
	void for_each_depth_first(node_ptr_tm const& root, FN fn)
	{
		std::stack<node_ptr_tm> nodes;
		nodes.push(root);
		std::set<node_ptr_tm> visited;
		while (!nodes.empty())
		{
			auto x = nodes.top();
			nodes.pop();
			if (visited.find(x) != visited.end())
				continue;
			visited.insert(x);
			typename node_ptr_tm::element_type::children_t children = x->children_;
			// call fn, which returns the new acc
			fn(x);

			for (auto const& y : children) {
				nodes.push(y);
			}
		}
	}



	// implementation
	#include "node_impl.hpp"


//=====================================================================
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

