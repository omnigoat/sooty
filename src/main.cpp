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
#include <sooty/common/performer.hpp>

using namespace sooty::lexing;

// here's what we're testing:
//    10.1 is a float
//    10/1 is an integer, fwdslash, integer



using sooty::parsing::parser;

using namespace sooty::lexing;
using namespace sooty::parsing;


//typedef const walking_context_t& walking_context_ref;





using sooty::walking::detail::context_ref;
using sooty::walking::detail::how_to_traverse;


using namespace sooty::walking;






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
	std::string input_string = "dragon drake draggable";
	sooty::lexing::lexemes_t lexemes;
	sooty::lexing::detail::accumulator_t acc(lexemes, input_string.size());
	{
		std::stringstream input(input_string);
		sooty::lexing::input_range_t input_range(input);
	
		namespace slex = sooty::lexing;
		slex::lexer_t Mdragon = slex::insert(4, slex::string_("dragon"));
		slex::lexer_t Mdrake = slex::insert(6, slex::string_("drake"));
		slex::lexer_t Mdraggable = slex::insert(8, slex::string_("draggable"));
		
		slex::lexer_t animal = Mdragon | Mdrake | Mdraggable | slex::match(' ', false);
		slex::lexer_t some_animals = *animal;
		
		typedef sooty::common::performer_t<slex::detail::analyser_t> lexical_analysis_t;
		
		
		slex::lexer_t BANANA = slex::string_("bana") >> +slex::string_("na");
		
		lexical_analysis_t()(acc, input_range, some_animals.backend());
	}
	
	std::cout << lexemes << std::endl;
	
	return 0;
	/*
	additive_expression = insert(addition) [
		additive_expression,
		skip(plus),
		match(number)
	]
	|
	number
	*/
	using sooty::parsing::parseme;
	using sooty::parsing::match;
	
	
	
	lexemes_t the_lexemes;
	{
		
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

