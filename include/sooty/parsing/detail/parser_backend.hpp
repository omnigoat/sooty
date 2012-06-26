//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_PARSER_BACKEND_HPP
#define SOOTY_PARSING_PARSER_BACKEND_HPP
//=====================================================================
#include <boost/shared_ptr.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
namespace detail {
//=====================================================================
	
	struct parser_backend;
	typedef boost::shared_ptr<parser_backend> parser_backend_ptr;

	struct parser_backend
	{
		struct type {
			enum Enum {
				matcher,
				diver,
				surfacer
			};
		};
		
		//var type as in { matcher, diver, surfacer }
		type::Enum type_;
		size_t match_from, match_to;
		size_t to_insert;
		parser_backend_ptr on_success, on_failure, on_invalid;
		
		parser_backend(type::Enum type_, size_t match_from, size_t match_to, size_t to_insert)
			: type_(type_), match_from(match_from), match_to(match_to), to_insert(to_insert)
		{
		}
		
		parser_backend_ptr clone() { return parser_backend_ptr(new parser_backend(*this)); }
	};

	bool equivalent(const parser_backend_ptr& lhs, const parser_backend_ptr& rhs);
	bool partially_equivalent(const parser_backend_ptr& lhs, const parser_backend_ptr& rhs);
	bool should_prepend(const parser_backend_ptr& P);
	
	inline bool valid_edge(const parser_backend_ptr& e) {
		return e;
	}
	
	inline parser_backend_ptr& node_of(parser_backend_ptr& e) {
		return e;
	}
	
	inline const parser_backend_ptr& node_of(const parser_backend_ptr& e) {
		return e;
	}
	
	inline const parser_backend_ptr& new_edge_from(const parser_backend_ptr& n, const parser_backend_ptr& e) {
		return e;
	}
	
	inline bool overwrote_edge(parser_backend_ptr& lhs, const parser_backend_ptr& rhs) {
		return false;
	}
	
//=====================================================================
} // namespace detail
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

