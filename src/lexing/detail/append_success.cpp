//=====================================================================
//
//  append_success
//  ----------------
//    appends rhs to lhs for all possibilities where lhs succeeds
//
//=====================================================================
#include <sooty/lexing/detail/base_lexer.hpp>
//=====================================================================

void sooty::lexing::detail::append_success( const base_lexer_ptr& lhs, const base_lexer_ptr& rhs ) {
	std::set<base_lexer_ptr> visited;
	append_success_impl(visited, lhs, rhs);
}

void sooty::lexing::detail::append_success_impl(std::set<base_lexer_ptr>& visited, const base_lexer_ptr& lhs, const base_lexer_ptr& rhs)
{
	if ( std::find(visited.begin(), visited.end(), lhs) == visited.end() )
	{
		visited.insert(lhs);
		
		if (lhs->on_success)
			append_success_impl(visited, lhs->on_success, rhs);
		else
			lhs->on_success = rhs;
		
		if (lhs->on_failure)
			append_success_impl(visited, lhs->on_failure, rhs);
	}
}

