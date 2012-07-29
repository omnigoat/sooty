#include <sooty/parsing/parsemes.hpp>
//=====================================================================


sooty::parsing::parsemes_t::parsemes_t()
	: owner()
{
}

sooty::parsing::parsemes_t::iterator sooty::parsing::parsemes_t::insert( iterator where_, const_parseme_ref n )
{
	return parsemes_.insert(where_, n);
}

sooty::parsing::parsemes_t::iterator sooty::parsing::parsemes_t::erase( iterator where_ )
{
	return parsemes_.erase(where_);
}

sooty::parsing::parsemes_t::iterator sooty::parsing::parsemes_t::erase( iterator from, iterator to )
{
	return parsemes_.erase(from, to);
}

void sooty::parsing::parsemes_t::push_back( parseme_ref n )
{
	parsemes_.push_back(n);
}

sooty::parsing::parseme_ref sooty::parsing::parsemes_t::front() {
	return parsemes_.front();
}

sooty::parsing::parseme_ref sooty::parsing::parsemes_t::back()
{
	return parsemes_.back();
}

sooty::parsing::parsemes_t::iterator sooty::parsing::parsemes_t::begin() {
	return parsemes_.begin();
}

sooty::parsing::parsemes_t::iterator sooty::parsing::parsemes_t::end() {
	return parsemes_.end();
}

sooty::parsing::parsemes_t::const_iterator sooty::parsing::parsemes_t::begin() const {
	return parsemes_.begin();
}

sooty::parsing::parsemes_t::const_iterator sooty::parsing::parsemes_t::end() const {
	return parsemes_.end();
}

void sooty::parsing::parsemes_t::clear()
{
	parsemes_.clear();
}