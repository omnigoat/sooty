//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_ABSTRACT_PARSER_BACKEND_HPP
#define SOOTY_PARSING_ABSTRACT_PARSER_BACKEND_HPP
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
	//=====================================================================
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
	
	
	inline parser_backend_ptr match(size_t match_from, size_t match_to) {
		accumulator:: mark_t mark = accumulator::generate_marker();
		
		return parser_backend_t::make()
			->push_back_command( command_t(command_t::add_marker, 0, 0, mark) )
			->push_back_failure( command_t(command_t::rm_marker, 0, 0, mark) )
			->push_back_command( command_t(command_t::match, match_from, match_to) )
			->push_back_command( command_t(command_t::insert, 0, 0, mark) )
			->push_back_command( command_t(command_t::rm_marker, 0, 0, mark) )
			->push_back_command( command_t(command_t::terminal, 0, 0) )
			;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	//=====================================================================
	//=====================================================================
	struct abstract_parser_backend;
	typedef boost::shared_ptr<abstract_parser_backend> abstract_parser_backend_ptr;
	
	//=====================================================================
	//=====================================================================
	struct abstract_parser_backend
	{
		abstract_parser_backend_ptr on_success, on_failure, on_invalid;
		
		
		virtual abstract_parser_backend_ptr clone() = 0;
		virtual int debug(std::set<detail::abstract_parser_backend_ptr>&, int p) {return p;}
		virtual bool applies(accumulator&, lexing::lexemes_t::const_iterator&, lexing::lexemes_t::const_iterator&) = 0;
	};
	
	
	bool equivalent(abstract_parser_backend_ptr&, const abstract_parser_backend_ptr&);
	bool equivalent_in_failure(abstract_parser_backend_ptr&, abstract_parser_backend_ptr&);
	
	inline bool partially_equivalent(const abstract_parser_backend_ptr& lhs, const abstract_parser_backend_ptr& rhs) {
		return false;
	}
	
	bool should_prepend(const abstract_parser_backend_ptr& P);
	
	bool skippable(const abstract_parser_backend_ptr& P);
	
	inline bool valid_edge(const abstract_parser_backend_ptr& e) {
		return e;
	}
	
	inline abstract_parser_backend_ptr& node_of(abstract_parser_backend_ptr& e) {
		return e;
	}
	
	inline const abstract_parser_backend_ptr& node_of(const abstract_parser_backend_ptr& e) {
		return e;
	}
	
	inline const abstract_parser_backend_ptr& new_edge_from(const abstract_parser_backend_ptr& n, const abstract_parser_backend_ptr& e) {
		return e;
	}
	
	bool overwrote_edge(abstract_parser_backend_ptr& lhs, const abstract_parser_backend_ptr& rhs);

	
	namespace parsers {
	
		// nopper
		struct nop : abstract_parser_backend {
			bool applies(accumulator&, lexing::lexemes_t::const_iterator&, lexing::lexemes_t::const_iterator&);
			int debug(std::set<detail::abstract_parser_backend_ptr>&, int padding) {
				for (int i = 0; i != padding; ++i) {
					std::cout << " ";
				}
				std::cout << "nop" << std::endl;
				return padding;
			}
			
			abstract_parser_backend_ptr clone() { return nop::create(); }
			static abstract_parser_backend_ptr create() { return abstract_parser_backend_ptr(new nop); }
			
		};
		
		// superparser
		struct superparser : abstract_parser_backend {
			// without storing the original subparser, it could simply be garbage removed
			abstract_parser_backend_ptr nop;
			
			superparser()
				: nop(parsers::nop::create())
			{
			}
			
			superparser(const abstract_parser_backend_ptr& n) 
				: nop(n)
			{
			}
			
			bool applies(accumulator& acc, lexing::lexemes_t::const_iterator& begin, lexing::lexemes_t::const_iterator& end);
			
			int debug(std::set<detail::abstract_parser_backend_ptr>&, int padding);
			
			void assign_subparser(const abstract_parser_backend_ptr& p);
			
			abstract_parser_backend_ptr clone() {
				return superparser::create(nop);
			}
			
			static abstract_parser_backend_ptr create() {
				return abstract_parser_backend_ptr(new superparser);
			}
			
			static abstract_parser_backend_ptr create(const abstract_parser_backend_ptr& nop) {
				return abstract_parser_backend_ptr(new superparser(nop));
			}
		};
		
		
		// handles all matching/inserting
		struct match : abstract_parser_backend
		{
			size_t match_from, match_to;
			boost::optional<accumulator::mark_t> mark;
			
			match(size_t match_from, size_t match_to)
				: match_from(match_from), match_to(match_to)
			{
			}
			
			match(size_t match_from, size_t match_to, const boost::optional<accumulator::mark_t>& mark)
				: match_from(match_from), match_to(match_to), mark(mark)
			{
			}
			
			bool applies(accumulator& acc, lexing::lexemes_t::const_iterator& begin, lexing::lexemes_t::const_iterator& end);
			
			int debug(std::set<detail::abstract_parser_backend_ptr>&, int padding);
			
			abstract_parser_backend_ptr clone() {
				//if (mark.is_initialized())
					//return match::create(match_from, match_to, accumulator::mapped_marker(mark.get()));
				//else
					return match::create(match_from, match_to, mark);
			}
			
			static abstract_parser_backend_ptr create(size_t match_from, size_t match_to) {
				return abstract_parser_backend_ptr(new match(match_from, match_to));
			}
			
			static abstract_parser_backend_ptr create(size_t match_from, size_t match_to, const boost::optional<accumulator::mark_t>& mark) {
				return abstract_parser_backend_ptr(new match(match_from, match_to, mark));
			}
		};
		
		struct insert : abstract_parser_backend
		{
			size_t insert_id;
			boost::optional<accumulator::mark_t> lexeme_mark_id;
			
			insert(size_t insert_id)
				: insert_id(insert_id)
			{
			}
			
			insert(size_t insert_id, const boost::optional<accumulator::mark_t>& mark)
				: insert_id(insert_id), lexeme_mark_id(mark)
			{
			}
			
			bool applies(accumulator& acc, lexing::lexemes_t::const_iterator&, lexing::lexemes_t::const_iterator&);
			
			int debug(std::set<detail::abstract_parser_backend_ptr>&, int padding) {
				for (int i = 0; i != padding; ++i) {
					std::cout << " ";
				}
				
				std::cout << "insert " << insert_id;
				if (lexeme_mark_id.is_initialized())
					std::cout << "  ->  " << *lexeme_mark_id.get();
				std::cout << std::endl;
				return padding;
			}
			
			abstract_parser_backend_ptr clone() {
				//if (lexeme_mark_id.is_initialized())
				//	return insert::create(insert_id, accumulator::mapped_marker(lexeme_mark_id.get()));
				//else
					return insert::create(insert_id, lexeme_mark_id);
			}
			
			static abstract_parser_backend_ptr create(size_t id) {
				return abstract_parser_backend_ptr(new insert(id));
			}
			
			static abstract_parser_backend_ptr create(size_t id, const boost::optional<accumulator::mark_t>& mark) {
				return abstract_parser_backend_ptr(new insert(id, mark));
			}
		};
		
		
		

		
		
		
		struct add_marker : abstract_parser_backend {
			accumulator::mark_t mark_id;
			
			add_marker(const accumulator::mark_t& mark_id) : mark_id(mark_id) {}
			
			bool applies(accumulator& acc, lexing::lexemes_t::const_iterator&, lexing::lexemes_t::const_iterator&);
			
			int debug(std::set<detail::abstract_parser_backend_ptr>&, int padding) {
				for (int i = 0; i != padding; ++i) {
					std::cout << " ";
				}
				
				std::cout << "add-marker: " << *mark_id << std::endl;
				return padding;
			}
			
			abstract_parser_backend_ptr clone() {
				return add_marker::create(mark_id);
			}
			
			static abstract_parser_backend_ptr create(const accumulator::mark_t& mark) {
				return abstract_parser_backend_ptr(new add_marker(mark));
			}
		};
		
		
		struct rm_marker : abstract_parser_backend {
			accumulator::mark_t mark_id;
			
			rm_marker(const accumulator::mark_t& mark_id) : mark_id(mark_id) {}
			
			bool applies(accumulator& acc, lexing::lexemes_t::const_iterator&, lexing::lexemes_t::const_iterator&);
			
			int debug(std::set<detail::abstract_parser_backend_ptr>&, int padding) {
				for (int i = 0; i != padding; ++i) {
					std::cout << " ";
				}
				
				std::cout << "rm-marker: " << *mark_id << std::endl;
				return padding;
			}
			
			abstract_parser_backend_ptr clone() {
				return rm_marker::create(mark_id);
			}
			
			static abstract_parser_backend_ptr create(const accumulator::mark_t& mark) {
				return abstract_parser_backend_ptr(new rm_marker(mark));
			}
		};
		
		struct rewind : abstract_parser_backend {
			accumulator::mark_t mark;
			
			rewind(const accumulator::mark_t& mark)
				: mark(mark)
			{
			}
			
			bool applies(accumulator& acc, lexing::lexemes_t::const_iterator&, lexing::lexemes_t::const_iterator&);
			
			abstract_parser_backend_ptr clone() {
				return rewind::create(mark);
			}
			
			static abstract_parser_backend_ptr create(const accumulator::mark_t& mark) {
				return abstract_parser_backend_ptr(new rewind(mark));
			}
		};
		
		struct merge : abstract_parser_backend {
			accumulator::mark_t from_mark, to_mark;
			
			merge(const accumulator::mark_t& m) : from_mark(m) {}
			
			bool applies(accumulator& acc, lexing::lexemes_t::const_iterator&, lexing::lexemes_t::const_iterator&);
			
			int debug(std::set<detail::abstract_parser_backend_ptr>&, int padding) {
				for (int i = 0; i != padding; ++i) {
					std::cout << " ";
				}
				
				std::cout << "merge: " << *from_mark << std::endl;
				return padding;
			}
			
			abstract_parser_backend_ptr clone() {
				return merge::create(from_mark);
			}
			
			static abstract_parser_backend_ptr create(const accumulator::mark_t& mark) {
				return abstract_parser_backend_ptr(new merge(mark));
			}
		};
		
		struct terminal : abstract_parser_backend {
			bool state;
			
			terminal(bool state) : state(state) {}
			
			bool applies(accumulator& acc, lexing::lexemes_t::const_iterator&, lexing::lexemes_t::const_iterator&);
			
			int debug(std::set<detail::abstract_parser_backend_ptr>&, int padding) {
				for (int i = 0; i != padding; ++i) {
					std::cout << " ";
				}
				
				std::cout << "terminal: " << state << std::endl;
				return padding;
			}
			
			abstract_parser_backend_ptr clone() {
				return terminal::create(state);
			}
			
			static abstract_parser_backend_ptr create(bool state = true) {
				return abstract_parser_backend_ptr(new terminal(state));
			}
		};
		
	} // namespace parsers
	
//=====================================================================
} // namespace detail
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

