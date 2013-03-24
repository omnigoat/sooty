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
			: lexemes_(lexemes), position_(1, 1), current_position_(1, 1), tabs_(), previous_tabs_()
		{
			characters_.reserve(buffer_size);
			beginning_ = characters_.begin();
			last_newline_ = characters_.end();
			last_newline_lx_ = 0;
		}
		
		void clear() {
			beginning_ = characters_.end();
			position_ = current_position_;
		}
		
		void combine(size_t id, channel_t const& ch) {
			lexemes_.push_back( lexeme_t(id, beginning_, characters_.end(), position_, ch) );
		}
		
		void push_back(char c) {
			characters_.push_back(c);
			
			++current_position_.column;
			if (c == '\n') {
				++current_position_.row;
				current_position_.column = 1;
			}
		}
		
		void inc_tabs() {
			++tabs_;
		}

		void blockify()
		{
			if (characters_.empty()) {
				return;
			}
			
			if (tabs_ < previous_tabs_) {
				for (int i = tabs_; i != previous_tabs_; ++i)
					lexemes_.push_back( lexeme_t(141414, beginning_, characters_.end(), current_position_) );
			}
			else if (tabs_ > previous_tabs_) {
				for (int i = previous_tabs_; i != tabs_; ++i)
					lexemes_.push_back( lexeme_t(151515, beginning_, characters_.end(), current_position_) );
			}

			beginning_ = characters_.end();

			previous_tabs_ = tabs_;
			tabs_ = 0;
		}

		void reset_tabs() {
			tabs_ = previous_tabs_;
		}
		
		
	private:
		typedef std::stack<size_t> frames_t;
		typedef std::string characters_t;
		
		lexemes_ref lexemes_;
		int last_newline_lx_;
		int previous_tabs_;
		int tabs_;

		characters_t characters_;
		characters_t::const_iterator beginning_;
		characters_t::const_iterator last_newline_;
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

