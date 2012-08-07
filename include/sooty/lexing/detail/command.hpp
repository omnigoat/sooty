//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_DETAIL_COMMAND_HPP
#define SOOTY_LEXING_DETAIL_COMMAND_HPP
//=====================================================================
#include <sooty/common/node.hpp>
#include <sooty/lexing/detail/mark.hpp>
#include <sooty/lexing/detail/accumulator.hpp>
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
				clear,
				match,
				combine
			};
		};
		
		command_t(action_t::Enum action, char from_id, char to_id)
			: action(action), from_id(from_id), to_id(to_id)
		{
		}
		
		bool is_terminal() const {
			return action == action_t::terminal;
		}
		
		static command_t clear() {
			return command_t(action_t::clear, 0, 0);
		}
		
		static command_t match(char f, char t) {
			return command_t(action_t::match, f, t);
		}
		
		static command_t combine(char id) {
			return command_t(action_t::combine, 0, id);
		}
		
		static command_t discard() {
			return command_t(action_t::clear, 0, 0);
		}
		
		static command_t terminal() {
			return command_t(action_t::terminal, 0, 0);
		}
		
		action_t::Enum action;
		char from_id, to_id;
	};
	
	typedef const command_t& const_command_ref;
	
	inline bool operator == (const_command_ref lhs, const_command_ref rhs) {
		return lhs.action == rhs.action && lhs.from_id == rhs.from_id && lhs.to_id == rhs.to_id;
	}
	
//=====================================================================
} // namespace detail
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

