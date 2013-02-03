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
	std::string input_string = "bana";
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
			insert(1, match("ba"))
			|
			insert(2, match("na"))
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
		
		parser K;
		K = insert(10) [
				K >>
				match(10) >>
				match(1)
			]
			|
			insert(11) [
				K >>
				match(11) >>
				match(1)
			]
			|
			match(1)
			;
		
		
		typedef sooty::common::performer_t<sooty::parsing::detail::executor_t> parsing_t;
		sooty::parsing::accumulator pracc;
		parsing_t()(pracc, sooty::parsing::lexeme_range_t(lexemes), K.backend());
		
		parsemes = pracc.container();
	}
	
	std::cout << lexemes << std::endl;
	
	//std::cout << parsemes << std::endl;
}




