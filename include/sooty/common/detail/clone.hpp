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

	/*
	template <typename Node>
	Node clone_tree_impl(const Node& root, std::map<Node, Node>& visited_nodes)
	{
		if ( visited_nodes.find(root) == visited_nodes.end() )
		{
			Node new_node(new typename Node::value_type(*root));
			visited_nodes[root] = new_node;
			
			if (root->on_success)
				new_node->on_success = clone_tree_impl(root->on_success, visited_nodes);
			if (root->on_failure)
				new_node->on_failure = clone_tree_impl(root->on_failure, visited_nodes);
			return new_node;
		}
		else 
		{
			return visited_nodes[root];
		}
	}
	
	template <typename Node, typename Edge>
	void clone_link(std::map<Node, Node>& visited_nodes, Edge& out_link, const Edge& link) {
		out_link = link;
	}
	
	template <typename Node, typename Edge>
	void clone_node(std::map<Node, Node>& visited_nodes, Node& out_node, const Node& node) {
		out_node = Node(new typename Node::value_type(node));
	}
	*/
	template <typename Node>
	Node clone_tree_impl2(std::map<Node, Node>& visited_nodes, const Node& clonee)
	{
		if ( visited_nodes.find(clonee) != visited_nodes.end() ) {
			return visited_nodes[clonee];
		}
		
		Node new_node = clonee->clone();
		visited_nodes[clonee] = new_node;
		
		if ( valid_edge(clonee->on_success) ) {
			new_node->on_success = new_edge_from(
				clone_tree_impl2<Node>(visited_nodes, node_of(clonee->on_success)),
				clonee->on_success
			);
		}
		if ( valid_edge(clonee->on_failure) ) {
			new_node->on_failure = new_edge_from(
				clone_tree_impl2<Node>(visited_nodes, node_of(clonee->on_failure)),
				clonee->on_failure
			);
		}
		if ( valid_edge(clonee->on_invalid) ) {
			new_node->on_invalid = new_edge_from(
				clone_tree_impl2<Node>(visited_nodes, node_of(clonee->on_invalid)),
				clonee->on_invalid
			);
		}
		
		return new_node;
	}
	
	template <typename Node>
	Node clone_tree(const Node& root)
	{
		std::map<Node, Node> visited_nodes;
		return clone_tree_impl2<Node>(visited_nodes, root);
	}

	
//=====================================================================
} // namespace detail
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

