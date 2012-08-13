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
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
//=====================================================================
#include <sooty/common/node.hpp>
#include <sooty/lexing/lexemes.hpp>
#include <sooty/parsing/accumulator.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
namespace detail {
//=====================================================================

	struct command_t {
		enum Enum {
			terminal,
			add_marker,
			rm_marker,
			match,
			insert,
			merge
		};
		
		bool is_sentinel() const {
			return action == add_marker;
		}
		
		static command_t make_add_marker(accumulator::const_mark_ref mark) {
			return command_t(add_marker, 0, 0, mark);
		}
		
		static command_t make_rm_marker(accumulator::const_mark_ref mark) {
			return command_t(rm_marker, 0, 0, mark);
		}
		
		static command_t make_match(size_t from_id, size_t to_id) {
			return command_t(match, from_id, to_id);
		}
		
		static command_t make_insert(size_t insert_id, accumulator::const_mark_ref mark) {
			return command_t(insert, insert_id, insert_id, mark);
		}
		
		static command_t make_terminal() {
			return command_t(terminal, 0, 0);
		}
		
		bool operator == (const command_t& rhs) const {
			return action == rhs.action && lower_id == rhs.lower_id && upper_id == rhs.upper_id
				&& (mark ? (rhs.mark ? *mark == *rhs.mark : false) : !rhs.mark);
		}
		
		command_t(Enum action, size_t lower_id, size_t upper_id)
			: action(action), lower_id(lower_id), upper_id(upper_id)
		{
		}
		
		command_t(Enum action, size_t lower_id, size_t upper_id, accumulator::const_mark_ref mark)
			: action(action), lower_id(lower_id), upper_id(upper_id), mark(mark)
		{
		}
		
		Enum action;
		accumulator::mark_t mark;
		size_t lower_id, upper_id;
	};
	
	typedef common::node_t<command_t> parser_backend_t;
	typedef boost::shared_ptr<parser_backend_t> parser_backend_ptr;
	
	
//=====================================================================
} // namespace detail
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

