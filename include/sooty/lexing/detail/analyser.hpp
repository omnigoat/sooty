//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_DETAIL_ANALYSER_HPP
#define SOOTY_LEXING_DETAIL_ANALYSER_HPP
//=====================================================================
#include <set>
#include <map>
//=====================================================================
#include <sooty/common/node.hpp>
#include <sooty/lexing/detail/accumulator.hpp>
#include <sooty/lexing/detail/lexer_backend.hpp>
#include <sooty/lexing/input_range.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
namespace detail {
//=====================================================================
	
	struct analyser_t
	{
		typedef bool result_type;
		
		bool operator ()(accumulator_ref accumulator, input_range_ref input, lexer_node_t const& command) const
		{
			switch (command.action)
			{
				case lexer_node_t::action_t::eof:
					return input.is_eof();

				case lexer_node_t::action_t::match:
					if (input.is_exhausted() || input.cv() < command.from_id || command.to_id < input.cv())
						return false;
					else {
						if (command.should_insert)
							accumulator.push_back(input.cv());
						input.advance();
					}
					break;
				
				case lexer_node_t::action_t::combine:
					accumulator.combine(command.to_id, command.channel);
					accumulator.clear();
					break;
				
				case lexer_node_t::action_t::peek:
					return input.cv() == command.from_id;

				case lexer_node_t::action_t::terminal:
					break;

				case lexer_node_t::action_t::callback:
					command.callback(accumulator);
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

