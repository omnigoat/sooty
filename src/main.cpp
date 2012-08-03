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


using namespace sooty::walking;





void make_lexeme(lexemes_t& out, lexeme_t::id_t id, lex_results_t& results) {
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
				context.traverse(current_walker->on_invalid.traversal);
				current_walker = current_walker->on_invalid.node;
			}
			else {
				return false;
			}
		}
		else if ( current_walker->compare_impl(context) )
		{
			context.traverse(current_walker->on_success.traversal);
			current_walker = current_walker->on_success.node;
		}
		else if (current_walker->on_failure.node)
		{
			context.traverse(current_walker->on_failure.traversal);
			current_walker = current_walker->on_failure.node;
		}
		else {
			return false;
		}
	}
	
	return true;
}


struct lexid {
	enum Enum {
		variable,
		integer,
		lparen,
		rparen,
		plus,
		dash,
		fwdslash,
		star
	};
};

struct parsid {
	enum Enum {
		addition = 100,
		subtraction,
		multiplication,
		division,
		number,
		variable
	};
};

// debugging information for output
void print_parsemes_impl(parsemes_t& ps, int padding = 0)
{
	for (parsemes_t::iterator i = ps.begin(); i != ps.end(); ++i) {
		for (int j = 0; j != padding; ++j) {
			std::cout << " ";
		}
		std::cout << i->id();
		if (i->lexeme())
			std::cout << ": " << i->lexeme()->text();
		std::cout << std::endl;
		print_parsemes_impl(i->children(), padding + 2);
	}
}

void print_parsemes_prefix(parsemes_t& ps)
{
	for (parsemes_t::iterator i = ps.begin(); i != ps.end(); ++i) {
		std::cout << " ";
		if (i->id() != parsid::number && i->id() != parsid::variable)
			std::cout << "(";
		switch (i->id()) {
			case parsid::addition:
				std::cout << "+";
				break;
			
			case parsid::subtraction:
				std::cout << "-";
				break;
			
			case parsid::multiplication:
				std::cout << "*";
				break;
			
			case parsid::division:
				std::cout << "/";
				break;
			
			case parsid::number:
			case parsid::variable:
				std::cout << i->lexeme()->text();
				break;
		}
		
		print_parsemes_prefix(i->children());
		
		if (i->id() != parsid::number && i->id() != parsid::variable)
			std::cout << ")";
	}
}



int main()
{
	sooty::parsing::detail::parser_backend_ptr M1 = sooty::parsing::detail::match(1, 1);
	sooty::parsing::detail::parser_backend_ptr M2 = sooty::parsing::detail::match(2, 2);
	sooty::parsing::detail::parser_backend_ptr M3 = sooty::parsing::detail::match(3, 3);
	//sooty::parsing::detail::parser_backend_ptr M3 = sooty::parsing::detail::match(3, 3);
	
	sooty::parsing::detail::parser_backend_ptr seq_and_m1m2
		= sooty::parsing::detail::parser_backend_t::seq_and(M1, M2);
	
	sooty::parsing::detail::parser_backend_ptr seq_and_m1m3
		= sooty::parsing::detail::parser_backend_t::seq_and(M1, M3);
	
	sooty::parsing::detail::parser_backend_ptr or_m1m2_m1m3
		= sooty::parsing::detail::parser_backend_t::one(seq_and_m1m2, seq_and_m1m3);
	
	sooty::parsing::detail::parser_backend_ptr or_or_m1m2_m1m3_m1
		= sooty::parsing::detail::parser_backend_t::one(or_m1m2_m1m3, M1);
	
	sooty::parsing::accumulator acc;
	M1->perform(acc);
	
	return 0;
	
	using sooty::parsing::parseme;
	using sooty::parsing::match;
	
	
	
	lexemes_t the_lexemes;
	{
		lexer combination
			= +((+in_range('a', 'z'))[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::variable, _1)]
			| (+in_range('0', '9'))[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::integer, _1)]
			| char_('(')[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::lparen, _1)]
			| char_(')')[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::rparen, _1)]
			| char_('+')[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::plus, _1)]
			| char_('-')[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::dash, _1)]
			| char_('/')[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::fwdslash, _1)]
			| char_('*')[boost::bind(make_lexeme, boost::ref(the_lexemes), lexid::star, _1)]
			| char_(' '))
			;
		
		std::string test_string = "5 - 2 + 3";
		lex_results_t results = lex(combination, test_string.begin(), test_string.end());
	}
	
	
	
	parsemes_t parsemes;
	{
		parser
			expression,
			additive_expression,
			multiplicative_expression,
			factor
			;
		
		expression =
			additive_expression
			;
		
		additive_expression = 
			insert(parsid::addition) [
				match_insert(lexid::integer, parsid::number) >>
				discard(lexid::plus) >>
				additive_expression
			]
			|
			insert(parsid::subtraction) [
				match_insert(lexid::integer, parsid::number) >>
				discard(lexid::dash) >>
				additive_expression
			]
			|
			match_insert(lexid::integer, parsid::number)
			;
		/*
		multiplicative_expression = 
			insert(parsid::multiplication) [
				factor >>
				discard(lexid::star) >>
				multiplicative_expression
			]
			|
			insert(parsid::division) [
				factor >>
				discard(lexid::fwdslash) >>
				multiplicative_expression
			]
			|
			factor
			;
		*/
		//factor =
		//	(
		//		discard(lexid::lparen) >>
		//		additive_expression >>
		//		discard(lexid::rparen)
		//	)
		//	|
		//	match_insert(lexid::integer, parsid::number) |
		//	match_insert(lexid::variable, parsid::variable)
		//	
		//	;
		
		sooty::parsing::debug(additive_expression.backend_);
		parsemes = sooty::parsing::parse(additive_expression, the_lexemes.begin(), the_lexemes.end());
	}
	std::cout << std::endl;
	print_parsemes_impl(parsemes);
	std::cout << std::endl;
	std::cout << "before reduction:\n  ";
	print_parsemes_prefix(parsemes);
	
	// reduction
	{
		using sooty::walking::eq;
		
		walk( parsemes, 
			fuzzy_eq(parsid::addition) [
				eq(parsid::number),
				eq(parsid::number)
			].on_match()
		);
	}
	
	system("pause");
}

