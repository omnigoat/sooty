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
//#include <boost/lexical_cast.hpp>
//#include <boost/variant.hpp>
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
	std::string input_string = "(1 + 3) * 143 - 6";
	sooty::lexing::lexemes_t lexemes;
	sooty::lexing::detail::accumulator_t acc(lexemes, input_string.size());
	{
		using namespace sooty::lexing;
		typedef sooty::common::performer_t<detail::analyser_t> lexical_analysis_t;
		
		// input stream
		std::stringstream input(input_string);
		
		// input range
		input_range_t input_range(input);
		
		// have a banana
		lexer_t BANANA =
		+(
			insert(1, +match('0', '9'))
			|
			insert(10, match("+"))
			|
			insert(11, match("-"))
			|
			insert(12, match("*"))
			|
			insert(20, match("("))
			|
			insert(21, match(")"))
			|
			match(' ', false)
		);
		
		lexical_analysis_t()(acc, input_range, BANANA.backend());
	}
	
	sooty::parsing::parsemes_t parsemes;
	{
		/*
		           add_mark M
		           -rm_mark M
		           match number
		* match plus       * match dash       * e
		* match number     * match number
		* combine add      * combine sub
		* rm_mark M        * rm_mark M
		
		
		           match N
		* match plus       * match dash
		* match number     * match number
		* combine add      * combine sub
		
		*/
		
		using namespace sooty::parsing;
		
		parser
		  additive_expr,
		  multiplicative_expr
		  ;
		
		
		multiplicative_expr = 
			insert(12) [ multiplicative_expr, match(12, false), match(1) ]
			|
			(match(20, false), additive_expr, match(21, false))
			|
			match(1)
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
	//additive_expr.backend()->debug_print();
	std::cout << parsemes << std::endl;
}




