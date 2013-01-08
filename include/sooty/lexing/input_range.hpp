//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_INPUT_RANGE_HPP
#define SOOTY_LEXING_INPUT_RANGE_HPP
//=====================================================================
#include <sooty/lexing/iterator.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
//=====================================================================
	
	struct input_range_t
	{
		template <typename T>
		input_range_t(const T& stream)
			: begin_(stream.rdbuf()), end_()
		{
		}
		
		
		// whether or not the iterator is invalid
		bool is_exhausted() const { return begin_ == end_; }
		
		// current value / clairvoyance
		char cv() const {
			//assert( !is_exhausted() );
			return *begin_;
		}
		
		
		// move forwards!~~
		void advance() {
			//assert(!is_exhausted());
			++begin_;
		}
		
		
	private:
		stream_iterator_t begin_, end_;
	};
	
	typedef input_range_t& input_range_ref;
	
//=====================================================================
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

