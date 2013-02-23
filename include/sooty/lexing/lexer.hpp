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
namespace sooty {
namespace lexing {
//=====================================================================
	
	struct lexer_t;
	typedef const lexer_t& const_lexer_ref;
	
	struct lexer_t
	{
		lexer_t(lexer_t const&);

		auto operator = (lexer_t const& rhs) -> lexer_t&;

		auto operator * () const -> lexer_t;
		auto operator + () const -> lexer_t;

		auto backend() const -> detail::const_lexer_backend_ptr_ref;
		
		
		friend auto match(char, char, bool) -> lexer_t;
		friend auto match(const std::string& -> lexer_t);
		friend auto insert(size_t, const_lexer_ref) -> lexer_t;

		friend auto operator >> (const_lexer_ref, const_lexer_ref) -> lexer_t;
		friend auto operator | (const_lexer_ref, const_lexer_ref) -> lexer_t;
		
	private:
		lexer_t(detail::const_lexer_backend_ptr_ref);
		
		detail::lexer_backend_ptr backend_;
	};
	
	
	auto match(char from, char to, bool should_insert = true) -> lexer_t;
	auto match(char c, bool should_insert = true) -> lexer_t;
	auto match(std::string const& str) -> lexer_t;

	
	lexer_t insert(size_t insert_id, const_lexer_ref L);


//=====================================================================
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

