//=====================================================================
//
//  append_failure
//  ----------------
//
//=====================================================================
#include <sooty/lexing/detail/base_lexer.hpp>
//=====================================================================

void sooty::lexing::detail::append_failure(const base_lexer_ptr& lhs, const base_lexer_ptr& rhs) {
	std::set<base_lexer_ptr> visited;
	append_failure_impl(visited, lhs, rhs);
}
	
void sooty::lexing::detail::append_failure_impl(std::set<base_lexer_ptr>& visited, const base_lexer_ptr& lhs, const base_lexer_ptr& rhs)
{
	if (visited.find(lhs) == visited.end())
	{
		visited.insert(lhs);
		
		if (lhs->on_success)
			append_failure_impl(visited, lhs->on_success, rhs);
		
		if (lhs->on_failure)
			append_failure_impl(visited, lhs->on_failure, rhs);
		else
			lhs->on_failure = rhs;
	}
}
