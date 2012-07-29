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
#include <sooty/parsing/detail/parser_backend.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
//=====================================================================
	
		
	struct parser
	{
	// gotta make this private somehow
		detail::parser_backend_ptr backend_;
		
		static size_t mark_guid() {
			static size_t _ = 0;
			return ++_;
		}
		
	public:
		parser() : backend_( detail::parser_backend::create(detail::parser_backend::type::superparser) ) {}
		parser(const detail::parser_backend_ptr& backend) : backend_(backend) {}
		
		parser& operator = (const parser& rhs) {
			//backend_ = detail::parser_backend::create(detail::parser_backend::type::superparser);
			backend_->subparser = rhs.backend_;
			return *this;
		}
		
		parser operator | (const parser& rhs) {
			detail::parser_backend_ptr new_lhs = sooty::common::detail::clone_tree(backend_);
			sooty::common::detail::fold(new_lhs, sooty::common::detail::clone_tree(rhs.backend_));
			return parser(new_lhs);
		}
		
		parser operator >> (const parser& rhs) const {
			detail::parser_backend_ptr new_lhs = sooty::common::detail::clone_tree(backend_);
			sooty::common::detail::append_success(new_lhs, sooty::common::detail::clone_tree(rhs.backend_));
			return parser(new_lhs);
		}
		
		parser operator [] (const parser& rhs) const
		{
			using namespace detail;
			
			parser_backend_ptr head, tail;
			size_t mark = parser::mark_guid();
			
			// the first thing we do is push the stack, unless our parent is a matcher,
			// in which case the very first thing we do is match (but not record), and
			// save the inserting for later
			if (backend_->type_ == parser_backend::type::matcher) {
				head = parser_backend::create(parser_backend::type::matcher, backend_->match_from, 0);
				tail = parser_backend::create(parser_backend::type::inserter, 0, backend_->to_insert);
				
				parser_backend_ptr sm = parser_backend::create(parser_backend::type::set_marker);
				sm->mark = mark;
				sooty::common::detail::append_success(head, sm);
			}
			else if (backend_->type_ == parser_backend::type::inserter) {
				head = parser_backend::create(parser_backend::type::set_marker);
				head->mark = mark;
				tail = sooty::common::detail::clone_tree(backend_);
			}
			else {
				assert(false);
			}
			
			// next we match our body like normal
			detail::parser_backend_ptr body = sooty::common::detail::clone_tree(rhs.backend_);
			sooty::common::detail::append_success(head, body);
			
			// if successful, we perform our insertion
			sooty::common::detail::append_success(head, tail);
			
			// now merge
			parser_backend_ptr mi = parser_backend::create(parser_backend::type::merge_into);
			mi->mark = mark;
			sooty::common::detail::append_success(head, mi);
			
			return parser(head);
		}
		
		parser if_(const parser& rhs) const
		{
			detail::parser_backend_ptr new_lhs = sooty::common::detail::clone_tree(backend_);
			detail::parser_backend_ptr new_rhs = sooty::common::detail::clone_tree(rhs.backend_);
			
			sooty::common::detail::append_success(new_rhs, new_lhs);
			return parser(new_rhs);
		}
		
	};
	
	
	inline parser match(size_t id) {
		return parser( detail::parser_backend_ptr(new detail::parser_backend(detail::parser_backend::type::matcher, id, id, id)) );
	}
	
	inline parser match_insert(size_t id, size_t nid) {
		return parser( detail::parser_backend_ptr(new detail::parser_backend(detail::parser_backend::type::matcher, id, id, nid)) );
	}

	inline parser discard(size_t id) {
		return parser( detail::parser_backend_ptr(new detail::parser_backend(detail::parser_backend::type::matcher, id, id, 0)) );
	}
	
	inline parser insert(size_t id) {
		return parser( detail::parser_backend_ptr(new detail::parser_backend(detail::parser_backend::type::inserter, 0, 0, id)) );
	}
	
	parsemes_t parse(parser& parser, lexing::lexemes::const_iterator& begin, lexing::lexemes::const_iterator& end);
	
//=====================================================================
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

