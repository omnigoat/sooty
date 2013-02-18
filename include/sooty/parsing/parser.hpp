//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_PARSER_HPP
#define SOOTY_PARSING_PARSER_HPP
//=====================================================================
#include <sooty/common/clone.hpp>
//=====================================================================
#include <sooty/lexing/lexemes.hpp>
//=====================================================================
#include <sooty/parsing/parsemes.hpp>
#include <sooty/parsing/detail/command.hpp>
#include <sooty/parsing/detail/parser_backend.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
//=====================================================================
	
	
	struct parser
	{
		parser();
		parser(const detail::parser_backend_ptr& backend);
		
		// accessors
		auto backend() const -> detail::parser_backend_ptr const&;
		auto resolved_backend() const -> detail::parser_backend_ptr const&;

		// operators
		auto operator >> (parser const&) const -> parser;
		auto operator ,  (parser const&) const -> parser;
		auto operator |  (parser const&) const -> parser;
		auto operator [] (parser const&) const -> parser;

		auto operator = (parser const&) -> parser&;
		
		void debug_print(int spaces = 0) const;

	private:
		void debug_print_impl(std::set<detail::parser_backend_ptr>& visited, detail::parser_backend_ptr const&, int spaces) const;

		detail::parser_backend_ptr resolved_backend_;
		detail::parser_backend_ptr backend_;
	};
	
	inline parser match(size_t id, bool insert = true) {
		using detail::command_t;
		
		return parser(
			detail::parser_backend_t::make()
				->push_back_command( command_t(command_t::action_t::match, id, id, insert ? id : 0) )
		);
	}
	
	inline parser insert(size_t id) {
		return parser(
			detail::parser_backend_t::make()
				->push_back_command(detail::command_t::make_insert(id))
		);
	}
	
	
	
//=====================================================================
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

