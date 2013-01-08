//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_COMMON_DETAIL_CLONE_HPP
#define SOOTY_COMMON_DETAIL_CLONE_HPP
//=====================================================================
#include <map>
//=====================================================================
#include <sooty/common/detail/layout.hpp>
//=====================================================================
namespace sooty {
namespace common {
namespace detail {
//=====================================================================

	template <typename NodePtr>
	NodePtr clone_tree_impl(std::map<NodePtr, NodePtr>& visited_nodes, const NodePtr& clonee)
	{
		if ( visited_nodes.find(clonee) != visited_nodes.end() ) {
			return visited_nodes[clonee];
		}
		
		NodePtr new_node = clonee->clone();	
		visited_nodes[clonee] = new_node;
		
		// recurse for children nodes
		std::for_each(clonee->children_.begin(), clonee->children_.end(), [&](const NodePtr& x) {
			new_node->children_.insert( clone_tree_impl(visited_nodes, x) );
		});

		std::transform(new_node->commands_.begin(), new_node->commands_.end(), new_node->commands_.begin(),
			&NodePtr::element_type::clone_command);
		
		return new_node;
	}

	
//=====================================================================
} // namespace detail
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

