#include <sooty/parsing/parsemes.hpp>
//=====================================================================
#include <stack>
#include <tuple>
//=====================================================================
using sooty::parsing::parsemes_t;
using sooty::parsing::parseme_ref;
using sooty::parsing::const_parseme_ref;

parsemes_t::parsemes_t()
: owner()
{
}


// accessors
auto parsemes_t::size() const -> size_t {
	return parsemes_.size();
}

auto parsemes_t::begin() const -> parsemes_t::const_iterator {
	return parsemes_.begin();
}

auto parsemes_t::end() const -> parsemes_t::const_iterator {
	return parsemes_.end();
}

// mutators
auto parsemes_t::insert( iterator where_, const_parseme_ref n ) -> parsemes_t::iterator {
	return parsemes_.insert(where_, n);
}

auto parsemes_t::erase( iterator where_ ) -> parsemes_t::iterator {
	return parsemes_.erase(where_);
}

auto parsemes_t::erase( iterator from, iterator to ) -> parsemes_t::iterator
{
	return parsemes_.erase(from, to);
}

auto parsemes_t::push_back( parseme_ref n ) -> void {
	parsemes_.push_back(n);
}

auto parsemes_t::front() -> parseme_ref {
	return parsemes_.front();
}

auto parsemes_t::back() -> parseme_ref {
	return parsemes_.back();
}

auto parsemes_t::begin() -> parsemes_t::iterator {
	return parsemes_.begin();
}

auto parsemes_t::end() -> parsemes_t::iterator {
	return parsemes_.end();
}

auto parsemes_t::clear() -> void {
	parsemes_.clear();
}

auto parsemes_t::pop_back() -> void {
	parsemes_.pop_back();
}



auto sooty::parsing::operator << (std::ostream& out, parsemes_t const& parsemes) -> std::ostream&
{
	using namespace sooty::parsing;

	std::stack<std::tuple<parseme, unsigned int>> nodes;
	for (auto const& x : parsemes) {
		nodes.push(std::make_tuple(x, 0));
	}

	unsigned int depth = 0;
	
	while (!nodes.empty())
	{
		auto x = nodes.top();
		nodes.pop();
		auto const& xn = std::get<0>(x);
		auto const& xd = std::get<1>(x);

		for (unsigned int i = 0; i != xd; ++i) {
			out << " ";
		}

		out << xn << std::endl;

		std::vector<parseme> tmp(xn.children().begin(), xn.children().end());
		std::reverse(tmp.begin(), tmp.end());
		for (auto y : tmp)
			nodes.push(std::make_tuple(y, xd + 1));
	}

	return out;
}




