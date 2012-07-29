#include <sooty/parsing/parser.hpp>
//=====================================================================
#include <sooty/parsing/accumulator.hpp>
#include <stack>
//=====================================================================
#include <sooty/parsing/detail/parseme_backend.hpp>
//=====================================================================
using sooty::lexing::lexemes;
using namespace sooty::parsing;

sooty::parsing::parsemes_t sooty::parsing::parse(parser& parser, lexemes::const_iterator& begin, lexemes::const_iterator& end)
{
	using sooty::parsing::parseme;
	using sooty::parsing::detail::parseme_backend;
	using sooty::parsing::detail::parseme_backend_ptr;
	using sooty::parsing::detail::parseme_backends_t;
	using sooty::parsing::detail::parser_backend;
	using sooty::parsing::detail::parser_backend_ptr;
	
	accumulator acc;
	
	detail::parser_backend_ptr current_parser = parser.backend_;
	while (current_parser && begin != end)
	{
		bool success = 
			((current_parser->type_ == detail::parser_backend::type::matcher &&
			current_parser->match_from <= begin->id() && begin->id() <= current_parser->match_to))
			||
			(current_parser->type_ != parser_backend::type::matcher)
			;
		
		if (success)
		{
			switch (current_parser->type_) {
				case parser_backend::type::superparser:
					acc.insert_all(
						sooty::parsing::parse(sooty::parsing::parser(current_parser->subparser), begin, end)
					);
					break;
					
				case parser_backend::type::inserter:
					acc.insert(current_parser->to_insert);
					break;
					
				case parser_backend::type::matcher:
					if (current_parser->to_insert > 0)
						acc.insert(current_parser->to_insert, &*begin);
					++begin;
					break;
				
				case parser_backend::type::set_marker:
					acc.set_marker(current_parser->mark);
					break;
				
				case parser_backend::type::merge_into:
					acc.merge_into(current_parser->mark);
					break;
			}
			
			current_parser = current_parser->on_success;
		}
		else
		{
			current_parser = current_parser->on_failure;
		}
	}
	
	return acc.container();
}