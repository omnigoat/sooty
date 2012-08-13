//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_INPUT_RANGE_HPP
#define SOOTY_PARSING_INPUT_RANGE_HPP
//=====================================================================
namespace sooty {
namespace parsing {
//=====================================================================
	
	struct lexeme_range_t
	{
		template <typename T>
		lexeme_range_t(lexing::const_lexemes_ref lexemes)
			: lexemes_(lexemes), begin_(lexemes_.begin()), end_(lexemes_.end())
		{
		}
		
		
		// whether or not the iterator is invalid
		bool is_exhausted() const { return begin_ == end_; }
		
		// current value / clairvoyance
		lexing::const_lexeme_ref cv() const {
			assert( !is_exhausted() );
			return *begin_;
		}
		
		
		// move forwards!~~
		void advance() {
			assert(!is_exhausted());
			++begin_;
		}
		
		
	private:
		lexing::lexemes_ref lexemes_;
		lexing::lexemes_t::const_iterator begin_, end_;
	};
	
	typedef lexeme_range_t& lexeme_range_ref;
	
//=====================================================================
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

