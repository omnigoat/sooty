//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_LEXER_HPP
#define SOOTY_LEXING_LEXER_HPP
//=====================================================================
#include <sooty/common/detail/fold.hpp>
#include <sooty/common/detail/append_success.hpp>
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
		friend lexer_t match(char, char, bool);
		friend lexer_t match(const std::string&);
		friend lexer_t insert(size_t, const_lexer_ref);
		
		
		friend lexer_t operator >> (const_lexer_ref, const_lexer_ref);
		friend lexer_t operator | (const_lexer_ref, const_lexer_ref);
		
		detail::const_lexer_backend_ptr_ref backend() const { return backend_; }
		
		lexer_t operator * () const;
		lexer_t operator + () const;
		
	private:
		lexer_t(detail::const_lexer_backend_ptr_ref backend)
			: backend_(backend)
		{
		}
		
		detail::lexer_backend_ptr backend_;
	};
	
	// operators
	lexer_t operator >> (const_lexer_ref, const_lexer_ref);
	lexer_t operator |  (const_lexer_ref, const_lexer_ref);
	
	
	lexer_t match(char from, char to, bool should_insert = true);
	
	inline lexer_t match(char c, bool should_insert = true) {
		return match(c, c, should_insert);
	}
	
	inline lexer_t match(const std::string& str) {
		detail::lexer_backend_ptr backend = detail::lexer_backend_t::make();
		for (std::string::const_iterator i = str.begin(); i != str.end(); ++i) {
			backend->push_back_command( detail::command_t::match(*i, *i, true) );
		}
		
		return lexer_t(backend);
	}
	
	
	lexer_t insert(size_t insert_id, const_lexer_ref L);


//=====================================================================
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

