#include <string>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <iterator>
#include <stack>
#include <algorithm>
#include <iostream>
//=====================================================================
//#include <atma/assert.hpp>
//=====================================================================
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>
#include <boost/tuple/tuple.hpp>
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
	std::string input_string = "dragongeese drake drake dragon";
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
		//lexer_t BANANA = +(insert(1, (string_("dragon") >> *string_("geese")) | string_("drake")) | match(' ', false));
		
		//lexical_analysis_t()(acc, input_range, BANANA.backend());
	}
	
	sooty::parsing::parsemes_t parsemes;
	{
		/*
		match(number),
		^match(plus | dash),
		match(number)
		
		additive_expression = 
			insert(addition) [
				additive_expression,
				match(plus, discard),
				match(number)
			]
			|
			insert(subtraction) [
				match(number),
				match(dash, discard),
				match(number)
			]
			|
			match(number)
			;
		
		add_mark ADD
		-rm_mark ADD
		match N
		rm_mark MN
		discard plus
		match number
		combine ADD
		rm_mark ADD
		
		
		
		
		           add_mark ADD
		           add_mark SUB
		           add_mark MN
		           match N
		           insert MN
		           rm_mark MN
		* match plus       * match dash       * e
		* match number     * match number
		* combine add      * combine sub
		* rm_mark ADD      * rm_mark SUB
		
		
		           match N
		* match plus       * match dash
		* match number     * match number
		* combine add      * combine sub
		
		*/
		
		using namespace sooty::parsing;
		
		parser K =
			insert(10) [
				match(1) >>
				match(2) >>
				match(1)
			]
			|
			insert(11) [
				match(1) >>
				match(3) >>
				match(1)
			]
			|
			match(1)
			;
		
		
		typedef sooty::common::performer_t<sooty::parsing::detail::executor_t> parsing_t;
		sooty::parsing::accumulator pracc;
		parsing_t()(pracc, sooty::parsing::lexeme_range_t(lexemes), K.backend());
	}
	
	std::cout << lexemes << std::endl;
}




