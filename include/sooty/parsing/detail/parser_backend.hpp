//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_DETAIL_PARSER_BACKEND_HPP
#define SOOTY_PARSING_DETAIL_PARSER_BACKEND_HPP
//=====================================================================
#include <sooty/parsing/detail/command.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
namespace detail {
//=====================================================================

	typedef common::node_t<command_t> parser_backend_t;
	typedef std::shared_ptr<parser_backend_t> parser_backend_ptr;
	typedef const parser_backend_ptr& const_parser_backend_ptr_ref;
	
	
//=====================================================================
} // namespace detail
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

