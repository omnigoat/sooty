//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_DETAIL_COMMAND_HPP
#define SOOTY_LEXING_DETAIL_COMMAND_HPP
//=====================================================================
#include <sooty/common/node.hpp>
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
		
		command_t(action_t::Enum action, char from_id, char to_id, bool should_insert)
			: action(action), from_id(from_id), to_id(to_id), should_insert(should_insert)
		{
		}
		
		bool is_sentinel() const {
			return false;
		}
		
		command_t clone() const {
			return *this;
		}
		
		const command_t& merge(const command_t& rhs) const {
			return *this;
		}
		
		static command_t clear() {
			return command_t(action_t::clear, 0, 0, false);
		}
		
		static command_t match(char f, char t, bool should_insert) {
			return command_t(action_t::match, f, t, should_insert);
		}
		
		static command_t combine(int id, channel_t const& ch) {
			command_t result(action_t::combine, 0, id, false);
			result.channel = ch;
			return result;
		}
		
		static command_t discard() {
			return command_t(action_t::clear, 0, 0, false);
		}
		
		static command_t terminal() {
			return command_t(action_t::terminal, 0, 0, false);
		}
		
		action_t::Enum action;
		int from_id, to_id;
		bool should_insert;
		channel_t channel;
	};
	
	typedef const command_t& const_command_ref;
	
	inline bool operator < (const_command_ref lhs, const_command_ref rhs) {
		if (lhs.action != rhs.action)
			return lhs.action < rhs.action;

		if (lhs.from_id != rhs.from_id)
			return lhs.from_id < rhs.from_id;

		return lhs.to_id < rhs.to_id;
	}

	inline bool operator == (const_command_ref lhs, const_command_ref rhs) {
		return lhs.action == rhs.action && lhs.from_id == rhs.from_id && lhs.to_id == rhs.to_id;
	}
	
	inline command_t merged(const_command_ref lhs, const_command_ref rhs, bool& success) {
		success = false;
		return lhs;
	}
	
//=====================================================================
} // namespace detail
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

