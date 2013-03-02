#include <string>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <iterator>
#include <sstream>
#include <stack>
#include <algorithm>
#include <iostream>
//=====================================================================
//#include <atma/assert.hpp>
//=====================================================================
#include <sooty/common/performer.hpp>
//=====================================================================
#include <sooty/lexing/detail/analyser.hpp>
#include <sooty/lexing/lexer.hpp>
#include <sooty/lexing/input_range.hpp>
//=====================================================================
#include <sooty/parsing/parser.hpp>
#include <sooty/parsing/detail/executor.hpp>
//=====================================================================



int main()
{
	std::string input_string = "(1 - 3 / (my_variable + 5)) + 143 - 6";
	sooty::lexing::lexemes_t lexemes;
	sooty::lexing::detail::accumulator_t acc(lexemes, input_string.size());
	{
		using namespace sooty::lexing;
		typedef sooty::common::performer_t<detail::analyser_t> lexical_analysis_t;
		
		channel_t main_channel(1);

		// input stream
		std::stringstream input(input_string);
		
		// input range
		input_range_t input_range(input);
		
		// have a banana
		lexer_t BANANA =
		+(
			insert(1, main_channel, +match('0', '9'))
			|
			insert(2, main_channel, match('a', 'z') >> +(match('a', 'z') | match('_')) )
			|
			insert(10, main_channel, match("+"))
			|
			insert(11, main_channel, match("-"))
			|
			insert(12, main_channel, match("*"))
			|
			insert(13, main_channel, match("/"))
			|
			insert(20, main_channel, match("("))
			|
			insert(21, main_channel, match(")"))
			|
			insert(0, channel_t(2), match(' '))
		);
		
		lexical_analysis_t()(acc, input_range, BANANA.backend());
	}
	
	sooty::parsing::parsemes_t parsemes;
	{
		using namespace sooty::parsing;
		
		parser
		  additive_expr,
		  multiplicative_expr,
		  unary_expr
		  ;
		
		unary_expr =
			(match(20, false), additive_expr, match(21, false))
			|
			match(1)
			|
			match(2)
			;

		unary_expr.debug_print();

		multiplicative_expr = 
			insert(12) [ multiplicative_expr, match(12, false), unary_expr ]
			|
			insert(13) [ multiplicative_expr, match(13, false), unary_expr ]
			|
			unary_expr
			;

		multiplicative_expr.debug_print();

		additive_expr = 
			insert(10) [ additive_expr, match(10, false), multiplicative_expr ]
			|
			insert(11) [ additive_expr, match(11, false), multiplicative_expr ]
			|
			multiplicative_expr
			;
			
		additive_expr.debug_print();
		
		
		typedef sooty::common::performer_t<sooty::parsing::detail::executor_t> parsing_t;
		sooty::parsing::accumulator pracc;
		parsing_t()(pracc, sooty::parsing::lexeme_range_t(lexemes), additive_expr.resolved_backend());

		parsemes = pracc.container();
	}
	
	std::cout << lexemes << std::endl;
	
	std::cout << parsemes << std::endl;
}




