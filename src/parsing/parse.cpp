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
	using namespace sooty::parsing::detail;
	
	accumulator acc;
	
	detail::parse(acc, parser.backend_, begin, end);
	
	return acc.container();
}

void sooty::parsing::detail::parse(accumulator& acc,
	const abstract_parser_backend_ptr& parser, lexing::lexemes::const_iterator& begin, lexing::lexemes::const_iterator& end)
{
	abstract_parser_backend_ptr current_parser = parser;
	
	while (current_parser)
	{
		if (current_parser->applies(acc, begin, end)) {
			current_parser = current_parser->on_success;
		}
		else {
			current_parser = current_parser->on_failure;
		}
	}
}