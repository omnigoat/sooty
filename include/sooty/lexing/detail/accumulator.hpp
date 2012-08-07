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
		accumulator_t(lexemes_ref lexemes, size_t buffer_size)
			: lexemes_(lexemes), position_(1, 1), current_position_(1, 1)
		{
			characters_.reserve(buffer_size);
			beginning_ = characters_.begin();
		}
		
		void clear() {
			beginning_ = characters_.end();
			position_ = current_position_;
		}
		
		void combine(size_t id) {
			lexemes_.push_back( lexeme_t(id, beginning_, characters_.end(), position_) );
		}
		
		void push_back(char c) {
			characters_.push_back(c);
			
			++current_position_.column;
			if (c == '\n') {
				++current_position_.row;
				current_position_.column = 1;
			}
		}
		
		
		
		
	private:
		typedef std::stack<size_t> frames_t;
		typedef std::map<mark_t::value_type, frames_t> markers_t;
		typedef std::string characters_t;
		
		markers_t markers_;
		lexemes_ref lexemes_;
		
		characters_t characters_;
		characters_t::const_iterator beginning_;
		position_t position_, current_position_;
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

