//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_LEXEMES_HPP
#define SOOTY_LEXING_LEXEMES_HPP
//=====================================================================
#include <vector>
//=====================================================================
#include <sooty/lexing/lexeme.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
//=====================================================================

	typedef std::vector<lexeme_t> lexemes_t;
	typedef lexemes_t& lexemes_ref;
	typedef const lexemes_t& const_lexemes_ref;
	
	inline std::ostream& operator << (std::ostream& stream, const_lexemes_ref rhs) 
	{
		stream << "lexemes\n---------" << std::endl;
		if (rhs.empty())
			stream << "  empty!" << std::endl;
		
		for (lexemes_t::const_iterator i = rhs.begin(); i != rhs.end(); ++i)
			stream << "  " << *i << std::endl;
		
		return stream << std::endl;
	}
	
//=====================================================================
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

