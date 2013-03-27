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
			: begin_(stream.rdbuf()), end_(), matched_eof_()
		{
		}
		
		
		// whether or not the iterator is invalid
		auto is_exhausted() const -> bool {
			return begin_ == end_;
		}
		
		auto is_eof() const -> bool {
			return is_exhausted() && !matched_eof_ && (matched_eof_ = true); }

		
		// current value / clairvoyance
		auto cv() const -> char {
			ATMA_ASSERT( !is_exhausted() );
			return *begin_;
		}
		
		auto advance() -> void {
			ATMA_ASSERT(!is_exhausted());
			++begin_;
		}
		
		
	private:
		stream_iterator_t begin_, end_;
		mutable bool matched_eof_;
		
	};
	
	typedef input_range_t& input_range_ref;
	
//=====================================================================
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

