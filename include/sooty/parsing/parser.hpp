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
	
	public:
		//parser if_;
		parser() {}
		parser(const detail::parser_backend_ptr& backend) : backend_(backend) {}
		
		parser operator | (const parser& rhs) {
			detail::parser_backend_ptr new_lhs = sooty::common::detail::clone_tree(backend_);
			sooty::common::detail::fold(new_lhs, rhs.backend_);
			return parser(new_lhs);
		}
		
		parser operator >> (const parser& rhs) const {
			detail::parser_backend_ptr new_lhs = sooty::common::detail::clone_tree(backend_);
			sooty::common::detail::append_success(new_lhs, rhs.backend_);
			return parser(new_lhs);
		}
		
		parser operator [] (const parser& rhs) const
		{
			detail::parser_backend_ptr new_lhs = sooty::common::detail::clone_tree(backend_);
			
			detail::parser_backend_ptr dive_head(new detail::parser_backend(detail::parser_backend::type::diver, 0, 0, 0));
			sooty::common::detail::append_success(new_lhs, dive_head);
			
			detail::parser_backend_ptr body = sooty::common::detail::clone_tree(rhs.backend_);
			sooty::common::detail::append_success(new_lhs, body);
			
			detail::parser_backend_ptr surface_tail(new detail::parser_backend(detail::parser_backend::type::surfacer, 0, 0, 0));
			sooty::common::detail::append_success(new_lhs, surface_tail);
			
			return parser(new_lhs);
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
	
	parsemes_t parse(parser& parser, lexing::lexemes::const_iterator begin, lexing::lexemes::const_iterator end);
	
//=====================================================================
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

