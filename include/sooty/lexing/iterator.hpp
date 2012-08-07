//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_ITERATOR_HPP
#define SOOTY_LEXING_ITERATOR_HPP
//=====================================================================
#include <istream>
#include <iterator>
//=====================================================================
namespace sooty {
namespace lexing {
//=====================================================================
	
	typedef std::istreambuf_iterator<char> stream_iterator_t;
	typedef const stream_iterator_t& const_stream_iterator_ref;
	
	typedef std::string::const_iterator characters_iterator_t;
	typedef const std::string::const_iterator& const_characters_iterator_ref;
	
//=====================================================================
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

