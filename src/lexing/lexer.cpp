#include <sooty/lexing/lexer.hpp>
//=====================================================================
#include <sooty/common/clone.hpp>
//=====================================================================
using namespace sooty::lexing;
using sooty::common::clone_tree;

lexer_t lexer_t::operator * () const
{
	detail::lexer_backend_ptr B = detail::lexer_backend_t::make();
	
	return lexer_t(
		B->add_child(clone_tree(backend_)->append(B))
		 ->add_child( detail::lexer_backend_t::make() )
	);
}

lexer_t lexer_t::operator + () const
{
	// this might be the best line of code I've ever written.
	return *this >> **this;
}

lexer_t sooty::lexing::operator >> ( const_lexer_ref lhs, const_lexer_ref rhs )
{
	return lexer_t(
		clone_tree(lhs.backend())->append( clone_tree(rhs.backend()) )
	);
}

lexer_t sooty::lexing::operator | ( const_lexer_ref lhs, const_lexer_ref rhs )
{
	return lexer_t(
		lhs.backend()->clone()->merge(rhs.backend())
	);
}

lexer_t sooty::lexing::match( char from, char to, bool should_insert)
{
	return lexer_t(
		detail::lexer_backend_t::make()
			->push_back_command( detail::command_t::match(from, to, should_insert) )
	);
}

lexer_t sooty::lexing::insert(size_t insert_id, const_lexer_ref L)
{
	return lexer_t(
		clone_tree(L.backend())->append(
			detail::lexer_backend_t::make()
				->push_back_command(detail::command_t::combine(insert_id))
		)
	);
}