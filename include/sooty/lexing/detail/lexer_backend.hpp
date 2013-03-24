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
#include <sooty/lexing/detail/accumulator.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
namespace detail {
//=====================================================================
	
	struct lexer_node_t
	{
		typedef std::function< void(detail::accumulator_t&) > callback_t;

		enum class action_t {
			eof,
			peek,
			match,
			combine,
			callback,
			terminal
		};

		
		lexer_node_t(action_t const& a, int from_id = 0, int to_id = 0, bool should_insert = true)
			: action(a), from_id(from_id), to_id(to_id), should_insert(should_insert)
		{}
		
		lexer_node_t(action_t const& a, callback_t const& c)
			: action(a), from_id(), to_id(), should_insert(), callback(c)
		{}

		auto with_callback(callback_t const& c) -> lexer_node_t& {
			callback = c;
			return *this;
		}

		action_t action;
		int from_id, to_id;
		bool should_insert;
		channel_t channel;
		callback_t callback;
	};

	inline auto operator < (lexer_node_t const& lhs, lexer_node_t const& rhs) -> bool {
		return
		  lhs.action < rhs.action || !(rhs.action < lhs.action) &&
		  lhs.from_id < rhs.from_id || !(rhs.action < lhs.action) && !(rhs.from_id < lhs.from_id) &&
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

