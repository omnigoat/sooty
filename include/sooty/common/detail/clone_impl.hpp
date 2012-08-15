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
		
		std::transform(new_node->children_.begin(), new_node->children_.end(), new_node->children_.begin(),
			boost::bind(&clone_tree_impl<NodePtr>, boost::ref(visited_nodes), _1));
		
		std::transform(new_node->commands_.begin(), new_node->commands_.end(), new_node->commands_.begin(),
			boost::bind(&typename NodePtr::value_type::clone_command, new_node, _1));
		
		return new_node;
	}

	
//=====================================================================
} // namespace detail
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

