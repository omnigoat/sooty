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
		typedef std::shared_ptr<node_t> node_ptr;
		struct ordering_t;
		struct merged_ordering_t;
		typedef std::set<node_ptr, ordering_t> children_t;


		// constructors
		template <typename... Args>
		node_t(Args...);
		node_t(node_t const& rhs);
		node_t(node_t&& rhs);
		~node_t();

		auto operator = (node_t const&) -> node_t&;
		//auto operator = (node_t&& rhs) -> node_t&;
		auto assume(node_t&&) -> node_ptr;
		
		
		// pure
		auto terminal() const -> bool { return terminal_; }
		auto bypassable() const -> bool { return bypassable_; }
		auto children() -> children_t const& { return children_; }
		auto clone() -> node_ptr;
		
		// mutators
		auto add_child(node_ptr const&) -> node_ptr;
		auto merge(node_ptr const&) -> node_ptr;
		auto set_as_terminator() -> void { terminal_ = true; }
		auto set_as_bypassable() -> void { bypassable_ = true; }

		// statics
		static auto equal_or_clone(node_ptr const& lhs, node_ptr const& rhs) -> bool;
		static auto share_ancestry(node_ptr const& lhs, node_ptr const& rhs) -> bool;
		
		template <typename... Args>
		static node_ptr make(Args const&... args) {
			return node_ptr(new node_t(args...));
		}

	public:
		// members
		N value_;
		children_t children_;
		std::set<node_t*> clones_;
		std::vector<node_t*> ancestry_;
		bool terminal_;
		bool bypassable_;
		std::shared_ptr<children_t> subnodes_;

		// friends
		#if 0
		template <typename ExecutorT> friend struct performer_t;
		template <typename NodePtr> friend NodePtr detail::clone_tree_impl(std::map<NodePtr, NodePtr>& visited_nodes, const NodePtr& clonee);
		template <typename node_ptr_tm, typename accumulator_tm, typename FN> friend void accumulate_depth_first(node_ptr_tm const& root, accumulator_tm acc, FN fn);
		template <typename C> auto append_impl(std::set<typename C::value_type>& visited, C& dest, C const& n) -> void;
		template <typename C> auto merge_into_children(std::shared_ptr<node_t<C>>& x, std::shared_ptr<node_t<C>> const& node) -> void;
		#endif
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
	template <typename N, typename FN> void for_each_depth_first(std::set<std::shared_ptr<node_t<N>>>& visited, typename node_t<N>::children_t const&, FN fn);
	template <typename C, typename FN> void for_each_depth_first(C const&, FN fn);
	
	
	//
	// append
	// append_backref
	// append_impl
	//
	template <typename C> auto append(C& dest, C const& n) -> C&;
	template <typename C> auto append_backref(C& dest, C const& n) -> C&;
	template <typename C> auto append_impl(std::set<typename C::value_type>& visited, C& dest, C const& n) -> void;
	template <typename C, typename N> auto append(C& dest, std::shared_ptr<node_t<N>> const& n) -> C&;
	template <typename C, typename N> auto append_backref(C& dest, std::shared_ptr<node_t<N>> const& n) -> C&;
	

	//
	// merging
	//
	template <typename C> auto merge(C& dest, C const& src) -> C&;
	template <typename C> auto merge_into_children(std::shared_ptr<node_t<C>>& x, std::shared_ptr<node_t<C>> const& node) -> void;


	//
	// accumulate_depth_first
	//
	template <typename C, typename acc_tm, typename FN>
	void accumulate_depth_first(std::shared_ptr<node_t<C>> const& root, acc_tm acc, FN fn);

	template <typename C>
	auto interweave_nodes(C& nodes) -> void;



//=====================================================================
} // namespace common
} // namespace sooty
//=====================================================================
// implementation
#include "node_impl.hpp"
//=====================================================================
#endif
//=====================================================================

