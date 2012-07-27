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
	
	//lexemes test_lexemes;
	struct lexid {
		enum Enum {
			variable,
			integer,
			lparam,
			rparam,
			plus,
			dash,
			fwdslash,
			star
		};
	};
	
	lexemes the_lexemes;
	{
		lexer combination
			= +(+in_range('a', 'z')[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::variable, _1)]
			| +in_range('0', '9')[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::integer, _1)]
			| char_('(')[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::lparam, _1)]
			| char_(')')[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::rparam, _1)]
			| char_('+')[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::plus, _1)]
			| char_('-')[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::dash, _1)]
			| char_('/')[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::fwdslash, _1)]
			| char_('*')[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::star, _1)]
			| char_(' '))
			;
		
		
		std::string test_string = "4 + 5";
		lex_results_t results = lex(combination, test_string.begin(), test_string.end());
	}
	
	
	struct parsid {
		enum Enum {
			addition = 1,
			subtraction,
			multiplication,
			division,
			number
		};
	};
	
	parsemes_t parsemes;
	{
		parser
			expression,
			additive_expression,
			multiplicative_expression
			;
		
		multiplicative_expression = 
			match_insert(lexid::integer, parsid::number);
		
		additive_expression =
			insert(parsid::addition) [
				multiplicative_expression >>
				discard(lexid::plus) >>
				multiplicative_expression
			]
			|
			multiplicative_expression
			;
		
		
		
		sooty::parsing::parsemes_t
			result = sooty::parsing::parse(additive_expression, the_lexemes.begin(), the_lexemes.end());
	}
	
}

