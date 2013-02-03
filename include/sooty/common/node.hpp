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
		// node-ptr / command_t
		typedef std::shared_ptr<node_t> node_ptr;
		typedef Command command_t;
		
		// commands
		typedef std::deque<std::pair<bool, command_t>> commands_t;

		// children: sort by descending number of commands
		struct ordering_t;
		typedef std::multiset<node_ptr, ordering_t> children_t;
		


		// constructors
		node_t();
		node_t(node_t const& rhs);
		node_t(node_t&& rhs);
		~node_t();

		auto operator = (node_t const&) -> node_t&;
		//auto operator = (node_t&& rhs) -> node_t&;
		
		
		// pure
		auto clone() -> node_ptr;

		
		// mutators
		auto push_back_command(const command_t&) -> node_ptr;
		auto push_back_failure(const command_t&) -> node_ptr;
		auto push_back_action(bool, const command_t&) -> node_ptr;
		auto add_child(node_ptr const&) -> node_ptr;
		auto add_self_as_child() -> node_ptr;
		auto append(node_ptr const&) -> node_ptr;
		auto append_self() -> node_ptr;
		auto merge(node_ptr const&) -> node_ptr;

		

		static node_ptr make() {
			return node_ptr(new node_t);
		}

		static bool is_failure(const std::pair<bool, command_t>& C) {
			return !C.first;
		}
		
		static bool is_command(const std::pair<bool, command_t>& C) {
			return C.first;
		}
		
	public:
		// mutators
		auto append_impl(std::set<node_ptr>& visited, node_ptr node) -> node_ptr;
		
		// statics
		static auto merge_commands(commands_t& combined, commands_t& new_lhs, commands_t& new_rhs, commands_t& lhs, commands_t& rhs) -> void;
		static auto clone_command(const std::pair<bool, command_t>& C) -> std::pair<bool, command_t>;

		// members
		commands_t commands_;
		commands_t unchosen_;
		children_t children_;

		std::set<node_t*> clones_;
		std::vector<node_t*> ancestry_;

		// friends
		template <typename ExecutorT> friend struct performer_t;
		template <typename NodePtr> friend NodePtr detail::clone_tree_impl(std::map<NodePtr, NodePtr>& visited_nodes, const NodePtr& clonee);
	};


	
	template <typename Command>
	struct node_t<Command>::ordering_t {
		bool operator () (node_ptr const& lhs, node_ptr const& rhs) const
		{
			if (lhs->commands_ < rhs->commands_)
				return true;
			else if (rhs->commands_ < lhs->commands_)
				return false;

			// if the two nodes share part of their ancestry
			for (auto& x : lhs->ancestry_) {
				if (std::find(rhs->ancestry_.begin(), rhs->ancestry_.end(), x) != rhs->ancestry_.end())
					return false;
			}
			/*if (!lhs->ancestry_.empty() && !rhs->ancestry_.empty())
				if (lhs->ancestry_[0] == rhs->ancestry_[0])
					return false;*/

			return lhs.get() < rhs.get();
		};
	};

	// implementation
	#include "node_impl.hpp"


//=====================================================================
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

