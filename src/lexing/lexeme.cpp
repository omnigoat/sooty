#include <sooty/lexing/lexeme.hpp>
//=====================================================================
using namespace sooty::lexing;
//=====================================================================



//=====================================================================
lexeme_t::lexeme_t( id_t id, const_characters_iterator_ref begin, const_characters_iterator_ref end, const position_t& position )
	: id_(id), begin_(begin), end_(end), position_(position)
{
}

lexeme_t::id_t lexeme_t::id() const
{
	return id_;
}

std::string lexeme_t::text() const
{
	std::string result(begin_, end_);
	return result;
}

const position_t& lexeme_t::position() const
{
	return position_;
}