//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_DETAIL_COMMAND_HPP
#define SOOTY_LEXING_DETAIL_COMMAND_HPP
//=====================================================================
#include <sooty/common/node.hpp>
#include <sooty/lexing/input_iterator.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
namespace detail {
//=====================================================================
	
	struct command_t
	{
		struct action_t {
			enum Enum {
				terminal,
				add_marker,
				rm_marker,
				performer
			};
		};
		
		
	};
	
	typedef node_t<command_t, accumulator_t> lexer_backend_t;
	typedef typename lexer_backend_t::node_ptr lexer_backend_ptr;
	
//=====================================================================
} // namespace detail
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

