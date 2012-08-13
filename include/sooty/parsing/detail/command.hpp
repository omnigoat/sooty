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

	// forward declares
	struct command_t;
	typedef boost::shared_ptr<command_t> command_ptr;
	
	struct command_t {
		typedef bool orders_t;
		
		enum Enum {
			terminal,
			add_marker,
			rm_marker,
			match,
			insert,
			merge
		};
		
		static command_ptr make_add_marker(accumulator::const_mark_ref mark) {
			return command_ptr(new command_t(add_marker, 0, 0, mark));
		}
		
		static command_ptr make_rm_marker(accumulator::const_mark_ref mark) {
			return command_ptr(new command_t(rm_marker, 0, 0, mark));
		}
		
		static command_ptr make_match(size_t from_id, size_t to_id) {
			return command_ptr(new command_t(match, from_id, to_id));
		}
		
		static command_ptr make_insert(size_t insert_id, accumulator::const_mark_ref mark) {
			return command_ptr(new command_t(insert, insert_id, insert_id, mark));
		}
		
		static command_ptr make_terminal() {
			return command_ptr(new command_t(terminal, 0, 0));
		}
		
		bool operator == (const command_t& rhs) const {
			return action_ == rhs.action_ && lower_id_ == rhs.lower_id_ && upper_id_ == rhs.upper_id_
				&& (mark_ ? (rhs.mark_ ? *mark_ == *rhs.mark_ : false) : !rhs.mark_);
		}
		
		bool sentinel() const {
			return action_ == add_marker;
		}
		
		bool is_terminal() const {
			return action_ == terminal;
		}
		
		bool operator() (accumulator& acc) const {
			switch (action_) {
				case add_marker:
					acc.add_marker(mark_);
					break;
				
				case rm_marker:
					acc.rm_marker(mark_);
					break;
			}
			
			return true;
		}
		
	public:
		command_t(Enum action, size_t lower_id, size_t upper_id)
			: action_(action), lower_id_(lower_id), upper_id_(upper_id)
		{
		}
		
		command_t(Enum action, size_t lower_id, size_t upper_id, accumulator::const_mark_ref mark)
			: action_(action), lower_id_(lower_id), upper_id_(upper_id), mark_(mark)
		{
		}
		
		Enum action_;
		accumulator::mark_t mark_;
		size_t lower_id_, upper_id_;
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

