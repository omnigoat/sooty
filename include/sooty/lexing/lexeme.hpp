//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_LEXEME_HPP
#define SOOTY_LEXING_LEXEME_HPP
//=====================================================================
#include <sooty/lexing/input_iterator.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
//=====================================================================
	
	//=====================================================================
	// position in file
	//=====================================================================
	struct position_t
	{
		int row, column;
	};


	//=====================================================================
	// lexeme
	//=====================================================================
	struct lexeme_t
	{
		typedef size_t id_t;
		static std::string empty_text;
		
		lexeme_t(id_t, const input_iterator& begin, const input_iterator& end, const position_t&);
		id_t id() const;
		const std::string& text() const;
		const position_t& position() const;
		
	private:
		id_t id_;
		sooty::lexing::input_iterator begin_, end_;
		position_t position_;
		std::string text_;
	};
	
//=====================================================================
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

