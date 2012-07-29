//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_PARSER_HPP
#define SOOTY_PARSING_PARSER_HPP
//=====================================================================
#include <sooty/common/detail/append_success.hpp>
#include <sooty/common/detail/fold.hpp>
//=====================================================================
#include <sooty/lexing/lexemes.hpp>
//=====================================================================
#include <sooty/parsing/parsemes.hpp>
//#include <sooty/parsing/detail/parser_backend.hpp>
#include <sooty/parsing/detail/abstract_parser_backend.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
//=====================================================================
	
		
	struct parser
	{
	// gotta make this private somehow
		//detail::parser_backend_ptr backend_;
		detail::abstract_parser_backend_ptr backend_;
		
	public:
		parser()
			: backend_( detail::parsers::superparser::create() )
		{
		}
		
		parser(const detail::abstract_parser_backend_ptr& backend) : backend_(backend) {}
		
		parser& operator = (const parser& rhs) {
			boost::shared_ptr<detail::parsers::superparser>
				sp = boost::shared_polymorphic_downcast<detail::parsers::superparser>(backend_);
			assert(sp);
			sp->assign_subparser(rhs.backend_);
			return *this;
		}
		
		parser operator | (const parser& rhs) {
			detail::abstract_parser_backend_ptr new_lhs = sooty::common::detail::clone_tree(backend_);
			sooty::common::detail::fold(new_lhs, sooty::common::detail::clone_tree(rhs.backend_));
			return parser(new_lhs);
		}
		
		parser operator >> (const parser& rhs) const {
			detail::abstract_parser_backend_ptr new_lhs = sooty::common::detail::clone_tree(backend_);
			sooty::common::detail::append_success(new_lhs, sooty::common::detail::clone_tree(rhs.backend_));
			return parser(new_lhs);
		}
		
		parser operator [] (const parser& rhs) const
		{
			using namespace detail;
			using namespace sooty::common::detail;
			
			abstract_parser_backend_ptr head, tail;
			accumulator::mark_t mark = accumulator::generate_marker();
			
			// the first thing we do is push the stack, unless our parent is a matcher,
			// in which case the very first thing we do is match (but not record), and
			// save the inserting for later
			if (boost::shared_polymorphic_downcast<parsers::insert>(backend_)) {
				head = parsers::add_marker::create(mark);
				tail = clone_tree(backend_);
			}
			else {
				assert(false);
			}
			
			// next we match our body like normal
			abstract_parser_backend_ptr body = clone_tree(rhs.backend_);
			append_success(head, body);
			
			// if successful, we perform our insertion
			append_success(head, tail);
			
			// now merge
			abstract_parser_backend_ptr mi = parsers::merge::create(mark);
			append_success(head, mi);
			
			// at any point 'till now, if we fail, remove our marker
			append_failure(head, parsers::rm_marker::create(mark));
			
			
			return parser(head);
		}
		
		//parser if_(const parser& rhs) const
		//{
		//	using namespace detail;
		//	using namespace sooty::common::detail;
		//	
		//	detail::parser_backend_ptr new_lhs = sooty::common::detail::clone_tree(backend_);
		//	detail::parser_backend_ptr new_rhs = sooty::common::detail::clone_tree(rhs.backend_);
		//	
		//	sooty::common::detail::append_success(new_rhs, new_lhs);
		//	return parser(new_rhs);
		//}
		
	};
	
	// unlike our backend parser, match inserts a parseme by default. that is, it is
	// two backends together -> match+insert
	inline parser match(size_t id) {
		accumulator::mark_t mark = accumulator::generate_marker();
		using common::detail::append_success;
		
		return parser(
			sooty::common::detail::append_success(
				detail::parsers::match::create(id, id, mark),
				detail::parsers::insert::create(id, mark)
			)
		
			/*sooty::common::detail::append_success(
				detail::parsers::add_marker::create(mark),
				append_success(
					detail::parsers::match::create(id, id),
					append_success(
						detail::parsers::insert::create(id, mark),
						detail::parsers::rm_marker::create(mark)
					)
				)
			)*/
		);
	}
	
	inline parser match_insert(size_t match_id, size_t insert_id) {
		accumulator::mark_t mark = accumulator::generate_marker();
		
		return parser(
			sooty::common::detail::append_success(
				detail::parsers::match::create(match_id, match_id, mark),
				detail::parsers::insert::create(insert_id, mark)
			)
		);
	}

	inline parser discard(size_t match_id) {
		return parser(
			detail::parsers::match::create(match_id, match_id)
		);
	}
	
	inline parser insert(size_t insert_id) {
		return parser(
			detail::parsers::insert::create(insert_id)
		);
	}
	
	parsemes_t parse(parser& parser, lexing::lexemes::const_iterator& begin, lexing::lexemes::const_iterator& end);
	
	namespace detail {
		void parse(accumulator&, const abstract_parser_backend_ptr&, 
			lexing::lexemes::const_iterator&, lexing::lexemes::const_iterator&);
	}
	
	inline void debug_impl(std::set<detail::abstract_parser_backend_ptr>& visited_nodes,
		detail::abstract_parser_backend_ptr p, int padding)
	{
		if (visited_nodes.find(p) != visited_nodes.end())
			return;
		visited_nodes.insert(p);
		
		p->debug(visited_nodes, padding);
		if (p->on_success) {
			std::cout << "+";
			debug_impl(visited_nodes, p->on_success, padding + 2);
			
		}
		
		if (p->on_failure) {
			std::cout << "-";
			debug_impl(visited_nodes, p->on_failure, padding + 2);
		}
	}
	
	inline void debug(detail::abstract_parser_backend_ptr p, int padding = 0) {
		std::set<detail::abstract_parser_backend_ptr> visited_nodes;
		debug_impl(visited_nodes, p, padding);
	}
	
	
	
//=====================================================================
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

