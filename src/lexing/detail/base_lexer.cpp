#include <sooty/lexing/detail/base_lexer.hpp>

sooty::lexing::detail::base_lexer::base_lexer( lexer_type::Enum type, char from, char to ) : type(type), from(from), to(to)
{

}

sooty::lexing::detail::base_lexer::base_lexer( lexer_type::Enum type, char from, char to, const base_lexer_ptr& on_success, const base_lexer_ptr& on_failure ) : type(type), from(from), to(to), on_success(on_success), on_failure(on_failure)
{

}





sooty::lexing::detail::base_lexer_ptr sooty::lexing::detail::clone_tree(const base_lexer_ptr& root) {
	std::map<base_lexer_ptr, base_lexer_ptr> visited_nodes;
	return clone_tree_impl(root, visited_nodes);
}

sooty::lexing::detail::base_lexer_ptr sooty::lexing::detail::clone_tree_impl(const base_lexer_ptr& root, std::map<base_lexer_ptr, base_lexer_ptr>& visited_nodes)
{
	if ( visited_nodes.find(root) == visited_nodes.end() )
	{
		base_lexer_ptr new_node(new base_lexer(*root));
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

bool sooty::lexing::detail::equivalent( base_lexer_ptr& lhs, base_lexer_ptr& rhs )
{
	return lhs->type == rhs->type && lhs->from == rhs->from && lhs->to == rhs->to;
}

