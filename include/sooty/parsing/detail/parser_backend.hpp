//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_PARSER_BACKEND_HPP
#define SOOTY_PARSING_PARSER_BACKEND_HPP
//=====================================================================
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
namespace detail {
//=====================================================================
	
	struct parser_backend;
	typedef boost::shared_ptr<parser_backend> parser_backend_ptr;

	struct parser_backend
		: boost::enable_shared_from_this<parser_backend>
	{
		struct type {
			enum Enum {
				matcher,
				inserter,
				superparser,
				set_marker,
				merge_into
			};
		};
		
		type::Enum type_;
		size_t match_from, match_to;
		size_t to_insert;
		parser_backend_ptr subparser;
		parser_backend_ptr on_success, on_failure, on_invalid;
		size_t mark;
		
		parser_backend(type::Enum type_, size_t match_from, size_t match_to, size_t to_insert)
			: type_(type_), match_from(match_from), match_to(match_to), to_insert(to_insert)
		{
		}
		
		parser_backend_ptr clone() {
			return parser_backend_ptr(new parser_backend(*this));
		}
		
		static parser_backend_ptr create(type::Enum t) {
			return parser_backend_ptr(new parser_backend(t, 0, 0, 0));
		}
		
		static parser_backend_ptr create(type::Enum t, size_t match, size_t insert) {
			return parser_backend_ptr(new parser_backend(t, match, match, insert));
		}
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
	
	inline void clone_passalong(parser_backend_ptr& lhs, const parser_backend_ptr& rhs) {
		if (rhs->type_ == parser_backend::type::superparser)
			lhs->subparser = rhs->subparser;
	}
	
//=====================================================================
} // namespace detail
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

