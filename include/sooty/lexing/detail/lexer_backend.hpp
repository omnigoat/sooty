//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_DETAIL_LEXER_BACKEND_HPP
#define SOOTY_LEXING_DETAIL_LEXER_BACKEND_HPP
//=====================================================================
#include <set>
#include <map>
//=====================================================================
#include <sooty/common/node.hpp>
#include <sooty/lexing/lexeme.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
namespace detail {
//=====================================================================
	
	struct lexer_node_t : common::node_t<lexer_node_t>
	{
		enum class action_t {
			peek,
			match,
			combine,
			passthrough,
			terminal
		};

		lexer_node_t(action_t const& a)
			: action(a)
		{}

		static node_ptr make_terminal() {
			return node_ptr(new lexer_node_t(action_t::terminal));
		}

		action_t action;
		int from_id, to_id;
		bool should_insert;
		channel_t channel;		
	};

	inline auto operator < (lexer_node_t const& lhs, lexer_node_t const& rhs) -> bool {
		return
		  lhs.action < rhs.action || !(rhs.action < lhs.action) &&
		  lhs.from_id < rhs.from_id || !(rhs.from_id < lhs.from_id) &&
		  lhs.to_id < rhs.to_id
		  ;
	}

	typedef common::node_t<lexer_node_t> lexer_backend_t;
	typedef lexer_backend_t::node_ptr lexer_backend_ptr;
	typedef lexer_backend_t::children_t lexer_backends_t;

//=====================================================================
} // namespace detail
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

