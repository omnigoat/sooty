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
#include <sooty/lexing/detail/analyser.hpp>
#include <sooty/lexing/lexer.hpp>
#include <sooty/lexing/input_range.hpp>

#include <sooty/common/performer.hpp>

using namespace sooty::lexing;


using namespace sooty::lexing;




int main()
{
	std::string input_string = "BABAAABABABA";
	sooty::lexing::lexemes_t lexemes;
	sooty::lexing::detail::accumulator_t acc(lexemes, input_string.size());
	{
		std::stringstream input(input_string);
		sooty::lexing::input_range_t input_range(input);
	
		namespace slex = sooty::lexing;
		
		typedef sooty::common::performer_t<slex::detail::analyser_t> lexical_analysis_t;
		
		slex::lexer_t seq_and_test = slex::match('a') >> slex::match('b');
		slex::lexer_t zero_or_more_test = *slex::match('a');
		slex::lexer_t seq_and_zero_or_more_test = slex::match('a') >> *slex::match('b');
		slex::lexer_t zero_or_more_seq_and_test = *slex::match('b') >> slex::match('a');
		slex::lexer_t many_zero_or_more_seq_and_test = *(*slex::match('b') >> slex::match('a'));
		
		slex::lexer_t BANANA = +insert(1, slex::match('B') >> +slex::match('A'));
		
		lexical_analysis_t()(acc, input_range, BANANA.backend());
	}
	
	std::cout << lexemes << std::endl;
}

