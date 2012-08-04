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
#include <boost/function.hpp>
//=====================================================================
#include <sooty/common/node.hpp>
#include <sooty/lexing/detail/command.hpp>
#include <sooty/lexing/detail/orders.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
namespace detail {
//=====================================================================
	
	typedef common::node_t<command_t, orders_t> lexer_backend_t;
	typedef typename lexer_backend_t::node_ptr lexer_backend_ptr;
	
//=====================================================================
} // namespace detail
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

