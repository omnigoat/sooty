//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_DETAIL_ACCUMULATOR_HPP
#define SOOTY_LEXING_DETAIL_ACCUMULATOR_HPP
//=====================================================================
#include <map>
#include <stack>
//=====================================================================
#include <sooty/lexing/lexemes.hpp>
#include <sooty/lexing/iterator.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
namespace detail {
//=====================================================================
	
	struct accumulator_t
	{
		accumulator_t(lexemes_ref lexemes)
			: lexemes_(lexemes)
		{
		}
		
		void clear(const_characters_iterator_ref iter, const_position_ref position) {
			beginning_ = iter;
			position_ = position;
		}
		
		void combine(size_t id, const_characters_iterator_ref end) {
			lexemes_.push_back( lexeme_t(id, beginning_, end, position_) );
		}
		
		
		
		
	private:
		typedef std::stack<size_t> frames_t;
		typedef std::map<mark_t::value_type, frames_t> markers_t;
		typedef std::vector<char> characters_t;
		
		markers_t markers_;
		lexemes_ref lexemes_;
		
		characters_iterator_t beginning_;
		position_t position_;
	};
	
	typedef const accumulator_t& const_accumulator_ref;
	typedef accumulator_t& accumulator_ref;
	
//=====================================================================
} // namespace detail
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

