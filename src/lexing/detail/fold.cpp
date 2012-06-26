#include <sooty/lexing/detail/base_lexer.hpp>
//=====================================================================

void sooty::lexing::detail::fold(const base_lexer_ptr& lhs, const base_lexer_ptr& rhs) {
	std::set<base_lexer_ptr> visited_nodes;
	fold_impl(visited_nodes, lhs, rhs);
}


void sooty::lexing::detail::fold_impl(std::set<base_lexer_ptr> visited_nodes, const base_lexer_ptr& lhs, const base_lexer_ptr& rhs)
{
	if (!lhs || !rhs)
		return;
	
	if (visited_nodes.find(lhs) != visited_nodes.end())
		return;
	visited_nodes.insert(lhs);
	
	if (lhs->type == base_lexer::lexer_type::marker) {
		fold_impl(visited_nodes, lhs->on_success, rhs);
	}
	else if (rhs->type == base_lexer::lexer_type::marker) {
		base_lexer_ptr saved_lhs = clone_tree(lhs);
		*lhs = *rhs;
		fold_impl(visited_nodes, lhs->on_success, saved_lhs);
	}
	// if lhs is partially equivalent to rhs, then we need to try rhs
	// if it fails, but try the one /after/ rhs if it succeeds
	else if ( partially_equivalent(lhs, rhs) )
	{
		if (lhs->on_failure)
			append_failure(lhs->on_failure, rhs);
		else
			lhs->on_failure = rhs;
		
		if (lhs->on_success)
			fold_impl(visited_nodes, lhs->on_success, rhs->on_success);
		else
			lhs->on_success = rhs->on_success;
	}
	// rhs is partially equivalent to lhs, which means that we have to
	// test rhs first, *then* lhs.
	else if ( partially_equivalent(rhs, lhs) )
	{
		base_lexer_ptr lhs_copy = clone_tree(lhs);
		*lhs = *rhs;
		visited_nodes.erase(lhs);
		fold_impl(visited_nodes, lhs, lhs_copy);
	}
	else if ( equivalent(lhs, rhs) )
	{
		if (lhs->on_success)
			fold_impl(visited_nodes, lhs->on_success, rhs->on_success);
		else {
			lhs->on_success = rhs->on_success;
			if (lhs->on_failure) {
				append_failure(lhs->on_success, lhs->on_failure);
				lhs->on_failure = base_lexer_ptr();
			}
		}
	}
	else {
		if (lhs->on_failure)
			fold_impl(visited_nodes, lhs->on_failure, rhs);
		else
			lhs->on_failure = rhs;
	}
}

