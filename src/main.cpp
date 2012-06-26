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
#include <sooty/lexing/input_iterator.hpp>
#include <sooty/lexing/lexer.hpp>
#include <sooty/parsing/parseme.hpp>
#include <sooty/parsing/parsemes.hpp>
#include <sooty/parsing/parser.hpp>

//#include <sooty/walking/detail/layout.hpp>
//#include <sooty/walking/detail/context.hpp>
#include <sooty/walking/detail/abstract_walker_backend.hpp>
#include <sooty/walking/walker.hpp>
#include <sooty/walking/placeholder.hpp>

#include <sooty/transforming/transformer.hpp>

//#include <sooty/parsing/detail/parseme_backend.hpp>
#include <sooty/common/detail/append_success.hpp>
#include <sooty/common/detail/append_invalid.hpp>

using namespace sooty::lexing;

// here's what we're testing:
//    10.1 is a float
//    10/1 is an integer, fwdslash, integer







void print_identifier(lex_results_t& L) {
	std::cout << "identifier: " << make_string(L.begin, L.end) << std::endl;
}

void print_giraffe(lex_results_t& L) {
	std::cout << "giraffe: " << make_string(L.begin, L.end) << std::endl;
}

using sooty::parsing::parser;

using namespace sooty::lexing;
using namespace sooty::parsing;


//typedef const walking_context_t& walking_context_ref;






using sooty::walking::detail::context_ref;
using sooty::walking::detail::how_to_traverse;


bool traverse(context_ref context, how_to_traverse::Enum htt)
{
	switch (htt)
	{
		case how_to_traverse::dive:
			context.dive();
			break;
			
		case how_to_traverse::surface:
			context.surface();
			break;
			
		case how_to_traverse::next_sibling:
			context.next();
			break;
	}
	
	return true;
}






using namespace sooty::walking;





void make_lexeme(lexemes& out, lexeme_t::id_t id, lex_results_t& results) {
	out.push_back( lexeme_t(id, results.begin, results.end, position_t()) );
}




static sooty::walking::placeholder<1> _p1;
static sooty::walking::placeholder<2> _p2;


bool walk(parsemes_t& these, const walker& with)
{
	sooty::walking::detail::abstract_walker_backend_ptr current_walker = with.backend;
	sooty::walking::detail::context_t context(these);
	
	while (current_walker)
	{
		if (current_walker->invalid_when_empty() && context.invalid()) {
			if (current_walker->on_invalid.node) {
				traverse(context, current_walker->on_invalid.traversal);
				current_walker = current_walker->on_invalid.node;
			}
			else {
				return false;
			}
		}
		else if ( current_walker->compare_impl(context) )
		{
			traverse(context, current_walker->on_success.traversal);
			current_walker = current_walker->on_success.node;
		}
		else if (current_walker->on_failure.node)
		{
			traverse(context, current_walker->on_failure.traversal);
			current_walker = current_walker->on_failure.node;
		}
		else {
			return false;
		}
	}
	
	return true;
}

int main()
{
	/*char* giraffe = new char[7];
	strcpy(giraffe, "giraffe");
	reverse(giraffe, 7);
	std::cout << giraffe << std::endl;*/
	
	
	

	using sooty::parsing::parseme;
	using sooty::parsing::match;
	
	parser p = match(6) [ match(7) >> match(8) ];
	
	std::string whoo = "whoo";
	
	lexemes test_lexemes;

	
	{
		lexer giraffe = string_("giraffe");
		lexer zebra = string_("zebra");
		lexer identifier = in_range('a', 'z') >> *in_range('a', 'z');
		lexer whitespace = char_(' ');
		//lexer 
		lexer test
			= *(giraffe[boost::bind(make_lexeme, boost::ref(test_lexemes), 6, _1)]
			| identifier[boost::bind(make_lexeme, boost::ref(test_lexemes), 7, _1)]
			| zebra[boost::bind(make_lexeme, boost::ref(test_lexemes), 8, _1)]
			| whitespace)
			;
		
		std::string test_string = "giraffe elephant zebra";
		lex_results_t results = lex(test, test_string.begin(), test_string.end());
		
		std::string result_string = make_string(results.begin, results.end);
		
		sooty::parsing::parsemes_t resultant_parsemes = parse(p, test_lexemes.begin(), test_lexemes.end());
		
		//using sooty::_1;
		
		walker wtest = +eq(4) >> eq(7);
		
		walker w =
			_p1 = eq(6) [
				_p2 = eq(7),
				*eq(8)
			]
			.rewrite [
				sooty::transforming::mk(23) [
					sooty::transforming::mk(40),
					_p2
				]
			]
			;
		
		walk(resultant_parsemes, w);
	}
	
}

