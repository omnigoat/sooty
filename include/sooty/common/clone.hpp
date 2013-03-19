//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_COMMON_CLONE_HPP
#define SOOTY_COMMON_CLONE_HPP
//=====================================================================
#include <sooty/common/detail/clone_impl.hpp>
//=====================================================================
namespace sooty {
namespace common {
//=====================================================================
	
	template <typename NodePtr>
	NodePtr clone_tree(const NodePtr& root)
	{
		std::map<NodePtr, NodePtr> visited_nodes;
		return detail::clone_tree_impl(visited_nodes, root);
	}

	template <typename C>
	auto clone_nodes(C const& nodes) -> C
	{
		typedef typename C::element_type node_ptr;

		C result;
		std::map<node_ptr, node_ptr> visited_nodes;
		
		for (auto const& x : nodes)
			result.insert(detail::clone_tree_impl(visited_nodes, x););

		return result;
	}

//=====================================================================
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

