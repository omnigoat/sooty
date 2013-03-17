#include <sooty/lexing/lexeme.hpp>
//=====================================================================
using sooty::lexing::lexeme_t;
using sooty::lexing::position_t;
using sooty::lexing::channel_t;
using sooty::lexing::const_characters_iterator_ref;
//=====================================================================

lexeme_t::lexeme_t( id_t id, const_characters_iterator_ref begin, const_characters_iterator_ref end, position_t const& position, channel_t const& channel)
: id_(id), begin_(begin), end_(end), position_(position), channel_(channel)
{
}

auto lexeme_t::id() const -> lexeme_t::id_t const&
{
	return id_;
}

auto lexeme_t::position() const -> position_t const&
{
	return position_;
}

auto lexeme_t::channel() const -> channel_t const&
{
	return channel_;
}

auto lexeme_t::begin() const -> const_characters_iterator_ref
{
	return begin_;
}

auto lexeme_t::end() const -> const_characters_iterator_ref
{
	return end_;
}

auto lexeme_t::text() const -> std::string
{
	return std::string(begin_, end_);
}

