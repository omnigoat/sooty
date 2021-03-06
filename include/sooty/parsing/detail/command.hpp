//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_COMMAND_HPP
#define SOOTY_PARSING_COMMAND_HPP
//=====================================================================
#include <iomanip>
#include <set>
//=====================================================================
#include <sooty/common/node.hpp>
#include <sooty/lexing/lexemes.hpp>
#include <sooty/parsing/detail/mark.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
namespace detail {
//=====================================================================

	struct command_t {
		enum class action_t {
			terminal,
			add_marker,
			rm_marker,
			match,
			insert,
			combine
		};
		
		command_t(action_t action, size_t lower_id, size_t upper_id, size_t insert_id)
		: action(action), lower_id(lower_id), upper_id(upper_id), insert_id(insert_id)
		{
		}
		
		command_t(action_t action, size_t lower_id, size_t upper_id)
		: command_t(action, lower_id, upper_id, 0)
		{
		}
		
		
		command_t(const command_t& rhs)
			: action(rhs.action), lower_id(rhs.lower_id), upper_id(rhs.upper_id), insert_id(rhs.insert_id)
		{
		}
		
		~command_t()
		{
		}
		
		command_t& operator = (const command_t& rhs) {
			action = rhs.action;
			lower_id = rhs.lower_id;
			upper_id = rhs.upper_id;
			insert_id = rhs.insert_id;
			
			return *this;
		}
		
		bool is_sentinel() const {
			return action == action_t::add_marker || action == action_t::rm_marker;
		}
		
		command_t clone() const {
			command_t result(*this);
			return result;
		}
	

		static command_t make_match(size_t from_id, size_t to_id) {
			return command_t(action_t::match, from_id, to_id);
		}
		
		static command_t make_insert(size_t insert_id) {
			return command_t(action_t::insert, 0, 0, insert_id);
		}
		
		static command_t make_terminal() {
			return command_t(action_t::terminal, 0, 0);
		}
		
		bool operator == (const command_t& rhs) const {
			return action == rhs.action && lower_id == rhs.lower_id && upper_id == rhs.upper_id && insert_id == rhs.insert_id;
		}
		
		
		action_t action;
		size_t lower_id, upper_id, insert_id;
	};

	inline bool operator < (command_t const& lhs, command_t const& rhs)
	{
		if (lhs.action != rhs.action)
			return lhs.action < rhs.action;

		if (lhs.lower_id != rhs.lower_id)
			return lhs.lower_id < rhs.lower_id;

		if (lhs.upper_id != rhs.upper_id)
			return lhs.upper_id < rhs.upper_id;

		return lhs.insert_id < rhs.insert_id;
	}
	
	inline command_t merged(command_t& lhs, command_t& rhs, bool& success) {
		if ((lhs.action == command_t::action_t::add_marker)
		    && rhs.action == lhs.action)
		{
			success = true;
			return command_t(lhs.action, lhs.lower_id, lhs.upper_id);
		}
		else {
			success = lhs == rhs;
		}
		
		return lhs;
	}
	
//=====================================================================
} // namespace detail
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

