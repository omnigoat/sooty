#include <sooty/parsing/parser.hpp>
//=====================================================================
#include <stack>
//=====================================================================
#include <sooty/parsing/detail/parseme_backend.hpp>
//=====================================================================
using sooty::lexing::lexemes;
using namespace sooty::parsing;

sooty::parsing::parsemes_t sooty::parsing::parse(parser& parser, lexemes::const_iterator begin, lexemes::const_iterator end)
{
	using sooty::parsing::parseme;
	using sooty::parsing::detail::parseme_backend;
	using sooty::parsing::detail::parseme_backend_ptr;
	using sooty::parsing::detail::parseme_backends_t;
	using sooty::parsing::detail::parser_backend;
	using sooty::parsing::detail::parser_backend_ptr;
	
	std::stack< std::pair<parseme, parsemes_t*> > ancestry_scope_stack;
	
	sooty::parsing::parsemes_t root_list;
	ancestry_scope_stack.push( std::make_pair(parseme(), &root_list) );
	
	detail::parser_backend_ptr current_parser = parser.backend_;
	while (current_parser)
	{
		parseme current_parent = ancestry_scope_stack.top().first;
		parsemes_t& current_container = *ancestry_scope_stack.top().second;
		
		bool success = 
			((current_parser->type_ == detail::parser_backend::type::matcher &&
			current_parser->match_from <= begin->id() && begin->id() <= current_parser->match_to))
			||
			(current_parser->type_ == parser_backend::type::diver)
			||
			(current_parser->type_ == parser_backend::type::surfacer)
			;
		
		if (success)
		{
			if (current_parser->type_ == parser_backend::type::diver) {
				ancestry_scope_stack.push( std::make_pair(parseme(current_container.back()), &current_container.back().children()) );
			}
			else if (current_parser->type_ == parser_backend::type::surfacer) {
				ancestry_scope_stack.pop();
			}
			else if (current_parser->to_insert != 0) {
				parseme p(current_parent, current_parser->to_insert, &*begin);
				//p.set_parent(current_container.owner());
				current_container.push_back(p);
				++begin;
			}
			current_parser = current_parser->on_success;
		}
		else
		{
			current_parser = current_parser->on_failure;
		}
	}
	
	// there should *always* be one scope left. the one we made beforehand.
	//return *ancestry_scope_stack.top().second;
	//return parsemes_t().insert(ancestry_scope_stack.top().second->begin(), acn
	return root_list;
}