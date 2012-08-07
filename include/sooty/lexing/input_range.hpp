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
		input_range_t(const T& stream, size_t size)
			: begin_(stream.rdbuf()), end_()
		{
			++position_.row;
			++position_.column;
			
			 read_characters_.reserve(size);
			
			
			if (!exhausted())
				read_characters_.push_back(*begin_);
		}
		
		
		// current value / clairvoyance
		char cv() const {
			assert( !exhausted() );
			return *begin_;
		}
		
		// current iterator
		characters_iterator_t iterator() const { return --read_characters_.end(); }
		// whether or not the iterator is invalid
		bool exhausted() const { return begin_ == end_; }
		// the current logical position in the stream
		const_position_ref position() const { return position_; }
		
		
		void advance()
		{
			if (cv() == '\n') {
				++position_.row;
				position_.column = 1;
			}
			else {
				++position_.column;
			}
			
			if (!exhausted()) {
				++begin_;
				read_characters_.push_back(*begin_);
			}
		}
		
		
	private:
		stream_iterator_t begin_, end_;
		position_t position_;
		std::string read_characters_;
	};
	
	typedef input_range_t& input_range_ref;
	
//=====================================================================
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

