#include <sooty/lexing/lexeme.hpp>
//=====================================================================



sooty::lexing::lexeme_t::lexeme_t( id_t id, const input_iterator& begin, const input_iterator& end, const position_t& position )
	: id_(id), begin_(begin), end_(end), position_(position)
{
	text_ = make_string(begin_, end_);
}

sooty::lexing::lexeme_t::id_t sooty::lexing::lexeme_t::id() const
{
	return id_;
}

const std::string& sooty::lexing::lexeme_t::text() const
{
	return text_;
}

const sooty::lexing::position_t& sooty::lexing::lexeme_t::position() const
{
	return position_;
}