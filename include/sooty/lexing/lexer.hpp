//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_LEXER_HPP
#define SOOTY_LEXING_LEXER_HPP
//=====================================================================
#include <sooty/lexing/detail/lexer_backend.hpp>
#include <sooty/lexing/detail/command.hpp>
//=====================================================================
#include <initializer_list>
//=====================================================================
namespace sooty {
namespace lexing {
//=====================================================================
	
	struct lexer_t
	{
		typedef detail::lexer_node_t::callback_t callback_t;

		lexer_t(lexer_t const&);

		auto operator = (lexer_t const& rhs) -> lexer_t&;

		auto operator !  () const -> lexer_t;
		auto operator *  () const -> lexer_t;
		auto operator +  () const -> lexer_t;
		auto operator [] (callback_t const&) const -> lexer_t;
		
		//auto backend() const -> detail::const_lexer_backend_ptr_ref;
		
		
		friend auto operator >> (lexer_t const&, lexer_t const&) -> lexer_t;
		friend auto operator | (lexer_t const&, lexer_t const&) -> lexer_t;

		friend auto peek(char) -> lexer_t;
		friend auto match(char) -> lexer_t;
		friend auto match(char, char) -> lexer_t;
		friend auto ignore(char) -> lexer_t;
		friend auto ignore(char, char) -> lexer_t;
		
		friend auto match(std::string const&) -> lexer_t;
		friend auto insert(size_t, channel_t const&, lexer_t const&) -> lexer_t;
		
	private:
		lexer_t(detail::lexer_backends_t const&);
		lexer_t(detail::lexer_backend_ptr const&);
		//lexer_t(std::initializer_list<detail::lexer_backend_ptr> const& list);
		
		detail::lexer_backends_t backends_;
	};

//=====================================================================
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

