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
	// node
	//=====================================================================
	template <typename N>
	struct node_t : std::enable_shared_from_this<node_t<N>>
	{
		// node-ptr / command_t
		typedef std::shared_ptr<node_t> node_ptr;
		
		// children: sort by descending number of commands
		struct ordering_t;
		struct merged_ordering_t;
		typedef std::set<node_ptr, ordering_t> children_t;


		// constructors
		node_t();
		node_t(node_t const& rhs);
		node_t(node_t&& rhs);
		~node_t();

		auto operator = (node_t const&) -> node_t&;
		//auto operator = (node_t&& rhs) -> node_t&;
		auto assume(node_t&&) -> node_ptr;
		
		
		// pure
		auto terminal() const -> bool { return terminal_; }
		auto bypassable() const -> bool { return bypassable_; }
		auto clone() -> node_ptr;
		auto children() -> children_t const& { return children_; }
		
		// mutators
		auto add_child(node_ptr const&) -> node_ptr;
		auto merge(node_ptr const&) -> node_ptr;
		auto set_as_terminator() -> void { terminal_ = true; }
		auto set_as_bypassable() -> void { bypassable_ = true; }

		// statics
		static auto equal_or_clone(node_ptr const& lhs, node_ptr const& rhs) -> bool;
		static auto share_ancestry(node_ptr const& lhs, node_ptr const& rhs) -> bool;
		

	public:
		// members
		children_t children_;
		std::set<node_t*> clones_;
		std::vector<node_t*> ancestry_;
		bool terminal_;
		bool bypassable_;

		// friends
		template <typename ExecutorT> friend struct performer_t;
		template <typename NodePtr> friend NodePtr detail::clone_tree_impl(std::map<NodePtr, NodePtr>& visited_nodes, const NodePtr& clonee);
		template <typename node_ptr_tm, typename accumulator_tm, typename FN> friend void accumulate_depth_first(node_ptr_tm const& root, accumulator_tm acc, FN fn);
		template <typename node_ptr_tm> friend auto append_impl(std::set<node_ptr_tm>& visited, node_ptr_tm& x, node_ptr_tm const& n) -> void;
		template <typename C> auto merge_into_children(std::shared_ptr<node_t<C>>& x, std::shared_ptr<node_t<C>> const& node) -> void;
	};



	template <typename Command>
	struct node_t<Command>::ordering_t {
		bool operator () (node_ptr const& lhs, node_ptr const& rhs) const;
	};

	template <typename Command>
	struct node_t<Command>::merged_ordering_t {
		bool operator () (node_ptr const& lhs, node_ptr const& rhs) const;
	};



	//=====================================================================
	//
	//
	//  ALGORITHMS
	//
	//
	//=====================================================================
	//
	// for_each_depth_first
	//
	template <typename node_ptr_tm, typename FN> void for_each_depth_first(std::set<node_ptr_tm>& visited, node_ptr_tm const& root, FN fn);
	template <typename node_ptr_tm, typename FN> void for_each_depth_first(node_ptr_tm const& root, FN fn);
	
	
	//
	// append
	// append_backref
	// append_impl
	//
	template <typename C> auto append(std::shared_ptr<node_t<C>>& x, std::shared_ptr<node_t<C>> const& n) -> std::shared_ptr<node_t<C>>&;
	template <typename C> auto append_backref(std::shared_ptr<node_t<C>>& x, std::shared_ptr<node_t<C>> const& n) -> std::shared_ptr<node_t<C>>&;
	template <typename C> auto append_impl(std::set<std::shared_ptr<node_t<C>>>& visited, std::shared_ptr<node_t<C>>& x, std::shared_ptr<node_t<C>> const& node) -> void;


	//
	// merge_into_children
	//
	template <typename C> auto merge_into_children(std::shared_ptr<node_t<C>>& x, std::shared_ptr<node_t<C>> const& node) -> void;


	//
	// accumulate_depth_first
	//
	template <typename C, typename acc_tm, typename FN>
	void accumulate_depth_first(std::shared_ptr<node_t<C>> const& root, acc_tm acc, FN fn);





	

	// implementation
	#include "node_impl.hpp"


//=====================================================================
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

