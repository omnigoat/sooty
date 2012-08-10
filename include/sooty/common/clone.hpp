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

	
//=====================================================================
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

