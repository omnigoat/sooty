//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_LEXEME_HPP
#define SOOTY_LEXING_LEXEME_HPP
//=====================================================================
#include <ostream>
#include <sooty/lexing/iterator.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
//=====================================================================
	
	//=====================================================================
	// position in file
	//=====================================================================
	struct position_t
	{
		position_t() : row(), column() {}
		position_t(int row, int column)
			: row(row), column(column)
		{
		}
		int row, column;
	};
	
	typedef const position_t& const_position_ref;
	
	//=====================================================================
	// lexeme
	//=====================================================================
	struct lexeme_t
	{
		typedef size_t id_t;
		static std::string empty_text;
		
		lexeme_t(id_t, const_characters_iterator_ref begin, const_characters_iterator_ref end, const position_t&);
		id_t id() const;
		
		const_characters_iterator_ref begin() const { return begin_; }
		const_characters_iterator_ref end() const { return end_; }
		
		const position_t& position() const;
		std::string text() const;
		
	private:
		id_t id_;
		characters_iterator_t begin_, end_;
		position_t position_;
	};
	
	typedef const lexeme_t& const_lexeme_ref;
	
	
	inline std::ostream& operator << (std::ostream& stream, const_lexeme_ref L) {
		return stream << L.id() << "[" << L.position().row << ":" << L.position().column << "]: " << L.text().c_str();
	}
	
	
	
//=====================================================================
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

