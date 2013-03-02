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
	
	struct channel_t
	{
		channel_t() : bits_(1) {}
		explicit channel_t(int ch) : bits_(1 | (1 << ch)) {}

		auto as_int() const -> int { return bits_; }

	private:
		int bits_;
	};

	//=====================================================================
	// lexeme
	//=====================================================================
	struct lexeme_t
	{
		typedef size_t id_t;
		static std::string empty_text;
		
		lexeme_t(id_t, const_characters_iterator_ref begin, const_characters_iterator_ref end, position_t const&, channel_t const& = channel_t());

		auto id() const -> id_t const&;
		auto position() const -> position_t const&;
		auto channel() const -> channel_t const&;
		auto begin() const -> const_characters_iterator_ref;
		auto end() const -> const_characters_iterator_ref;
		auto text() const -> std::string;
		
	private:
		id_t id_;
		characters_iterator_t begin_, end_;
		position_t position_;
		channel_t channel_;
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

