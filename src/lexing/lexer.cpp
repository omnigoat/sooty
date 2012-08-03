#include <sooty/lexing/lexer.hpp>
//=====================================================================

sooty::lexing::lex_results_t sooty::lexing::detail::lex_impl(const lexer& lexer, input_iterator begin, input_iterator end)
{
	input_iterator remembered_begin = begin;
	detail::base_lexer_ptr current_lexer = lexer.base_lexer;
	
	std::map<int, input_iterator> things;
	
	while (current_lexer)
	{
		char character = '\0';
		if (current_lexer->type == detail::base_lexer::lexer_type::matcher) {
			if (begin != end)
				character = *begin;
		}
		
		bool match_success
			// success if we're a matcher with some input left and the character
			// in question falls within our range
			= (current_lexer->type == detail::base_lexer::lexer_type::matcher && 
			  (begin != end) && 
			  (current_lexer->from <= character) && 
			  (current_lexer->to >= character))
			
			// terminals match!
			|| current_lexer->type == detail::base_lexer::lexer_type::terminal
			
			// as do actors!
			|| current_lexer->type == detail::base_lexer::lexer_type::actor
			
			|| current_lexer->type == detail::base_lexer::lexer_type::marker
			;
		
		if (current_lexer->type == detail::base_lexer::lexer_type::marker) {
			things[current_lexer->from] = begin;
		}
		
		if (match_success)
		{
			// terminals and actors don't move the input position
			if (current_lexer->type == detail::base_lexer::lexer_type::matcher)
				++begin;
			
			lex_results_t current_results(true, remembered_begin, begin);
			if (current_lexer->type == detail::base_lexer::lexer_type::actor) {
				lex_results_t actor_results(true, things[current_lexer->from], begin);
				assert(current_lexer->action);
				current_lexer->action(actor_results);
			}
			
			if (current_lexer->on_success)
				current_lexer = current_lexer->on_success;
			else
				return current_results;
		}
		else
		{
			if (current_lexer->on_failure)
				current_lexer = current_lexer->on_failure;
			else
				return lex_results_t(false, remembered_begin, begin);
		}
	}
	
	return lex_results_t(false, begin, end);
}