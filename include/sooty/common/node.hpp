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
		typedef node_ptr& node_ptr_ref;
		typedef const node_ptr& const_node_ptr_ref;
		typedef Command command_t;
		
		// commands
		typedef std::deque<std::pair<bool, command_t>> commands_t;
		typedef const commands_t& const_commands_ref;
		typedef commands_t& commands_ref;

		// children: sort by descending number of commands
		struct ordering_t;
		typedef std::multiset<node_ptr, ordering_t> children_t;
		typedef const children_t& const_children_ref;
		


		// constructors
		node_t(bool is_terminal = false);
		node_t(const node_t& rhs);
		~node_t();

		static node_ptr make(bool is_terminal = false) {
			return node_ptr(new node_t(is_terminal));
		}


		// pure
		auto clone() -> node_ptr;

		
		// mutators
		auto push_back_command(const command_t&) -> node_ptr;
		auto push_back_failure(const command_t&) -> node_ptr;
		auto push_back_action(bool, const command_t&) -> node_ptr;
		auto add_child(const_node_ptr_ref) -> node_ptr;
		auto add_self_as_child() -> node_ptr;
		auto append(const_node_ptr_ref) -> node_ptr;
		auto append_self() -> node_ptr;
		auto merge(const_node_ptr_ref) -> node_ptr;

		auto operator = (node_t const&) -> node_t&;

		
		// predicates
		static bool is_failure(const std::pair<bool, command_t>& C) {
			return !C.first;
		}
		
		static bool is_command(const std::pair<bool, command_t>& C) {
			return C.first;
		}
		
	private:
		// mutators
		auto append_impl(std::set<node_ptr>& visited, node_ptr node) -> node_ptr;
		
		// statics
		static auto merge_commands(commands_ref combined, commands_ref new_lhs, commands_ref new_rhs, commands_ref lhs, commands_ref rhs) -> void;
		static auto clone_command(const std::pair<bool, command_t>& C) -> std::pair<bool, command_t>;

		// members
		bool is_terminal;
		commands_t commands_;
		commands_t unchosen_;
		children_t children_;

		// what children have we cloned?
		static std::map<node_t*, std::set<node_t*>> cloned_nodes_;
		// reverse lookup so we can remove references to ourselves from our parents
		static std::map<node_t*, node_t*> cloner_node_;

		// friends
		template <typename ExecutorT> friend struct performer_t;
		template <typename NodePtr> friend NodePtr detail::clone_tree_impl(std::map<NodePtr, NodePtr>& visited_nodes, const NodePtr& clonee);
	};


	
	template <typename Command>
	struct node_t<Command>::ordering_t {
		bool operator () (const_node_ptr_ref lhs, const_node_ptr_ref rhs) const {
			return rhs->commands_ < lhs->commands_;
		};
	};

	// implementation
	#include "impl/node_impl.hpp"


//=====================================================================
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

