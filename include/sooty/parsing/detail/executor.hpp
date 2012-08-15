//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_DETAIL_EXECUTOR_HPP
#define SOOTY_LEXING_DETAIL_EXECUTOR_HPP
//=====================================================================
#include <set>
#include <map>
//=====================================================================
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
//=====================================================================
#include <sooty/common/node.hpp>
#include <sooty/parsing/accumulator.hpp>
#include <sooty/parsing/lexeme_range.hpp>
#include <sooty/parsing/detail/command.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
namespace detail {
//=====================================================================
	
	struct executor_t
	{
		typedef bool result_type;
		
		bool operator ()(accumulator_ref accumulator, lexeme_range_ref input, const command_t& command) const
		{
			switch (command.action)
			{
				case command_t::add_marker:
					accumulator.add_marker(command.mark);
					break;
				
				case command_t::rm_marker:
					accumulator.rm_marker(command.mark);
					break;
				
				case command_t::match:
					if (input.is_exhausted() || input.cv().id() < command.lower_id || command.upper_id < input.cv().id())
						return false;
					else if (command.insert_id != 0) {
						accumulator.insert(command.insert_id, &input.cv());
						input.advance();
					}
					else {
						input.advance();
					}
					break;
				
				case command_t::insert:
					accumulator.insert(command.insert_id);
					break;
				
				case command_t::combine:
					accumulator.merge_into(command.mark);
					break;
			}
			
			return true;
		}
	};
	
//=====================================================================
} // namespace detail
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

