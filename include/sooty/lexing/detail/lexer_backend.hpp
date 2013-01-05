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
#include <boost/shared_ptr.hpp>
//=====================================================================
#include <sooty/common/node.hpp>
#include <sooty/lexing/detail/command.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
namespace detail {
//=====================================================================
	
	typedef common::node_t<command_t> lexer_backend_t;
	typedef lexer_backend_t::node_ptr lexer_backend_ptr;
	typedef const lexer_backend_ptr& const_lexer_backend_ptr_ref;
	
//=====================================================================
} // namespace detail
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

