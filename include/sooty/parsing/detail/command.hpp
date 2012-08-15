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
#include <sooty/parsing/detail/mark.hpp>
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
			combine
		};
		
		command_t(Enum action, size_t lower_id, size_t upper_id, size_t insert_id = 0)
			: action(action), lower_id(lower_id), upper_id(upper_id), insert_id(insert_id)
		{
		}
		
		command_t(Enum action, size_t lower_id, size_t upper_id, detail::const_mark_ref mark)
			: action(action), lower_id(lower_id), upper_id(upper_id), insert_id(), mark(mark)
		{
			if (mark)
				mark->add_command(this);
		}
		
		command_t(const command_t& rhs)
			: action(rhs.action), lower_id(rhs.lower_id), upper_id(rhs.upper_id), insert_id(rhs.insert_id), mark(rhs.mark)
		{
			if (mark)
				mark->add_command(this);
		}
		
		~command_t() {
			if (mark)
				mark->remove_command(this);
		}
		
		command_t& operator = (const command_t& rhs) {
			action = rhs.action;
			lower_id = rhs.lower_id;
			upper_id = rhs.upper_id;
			insert_id = rhs.insert_id;
			mark = rhs.mark;
			
			if (mark)
				mark->add_command(this);
			
			return *this;
		}
		
		bool is_sentinel() const {
			return action == add_marker;
		}
		
		static command_t make_add_marker(detail::const_mark_ref mark) {
			return command_t(add_marker, 0, 0, mark);
		}
		
		static command_t make_rm_marker(detail::const_mark_ref mark) {
			return command_t(rm_marker, 0, 0, mark);
		}
		
		static command_t make_match(size_t from_id, size_t to_id) {
			return command_t(match, from_id, to_id);
		}
		
		static command_t make_insert(size_t insert_id) {
			return command_t(insert, insert_id, 0);
		}
		
		static command_t make_insert(size_t insert_id, detail::const_mark_ref mark) {
			return command_t(insert, insert_id, insert_id, mark);
		}
		
		static command_t make_terminal() {
			return command_t(terminal, 0, 0);
		}
		
		bool operator == (const command_t& rhs) const {
			return action == rhs.action && lower_id == rhs.lower_id && upper_id == rhs.upper_id
				&& insert_id == rhs.insert_id
				&& (mark ? (rhs.mark ? *mark == *rhs.mark : false) : !rhs.mark);
		}
		
		
		
		Enum action;
		detail::mark_t mark;
		size_t lower_id, upper_id, insert_id;
	};
	
	typedef common::node_t<command_t> parser_backend_t;
	typedef boost::shared_ptr<parser_backend_t> parser_backend_ptr;
	
	inline command_t merged(const command_t& lhs, const command_t& rhs, bool& success) {
		if (lhs.action == command_t::add_marker ||
		    lhs.action == command_t::combine ||
		    lhs.action == command_t::rm_marker)
		{
			success = true;
			//return command_t(lhs.action, lhs.lower_id, lhs.upper_id, detail::combined_mark(lhs.mark, rhs.mark));
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

