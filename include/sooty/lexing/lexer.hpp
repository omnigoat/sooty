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
#include <sooty/lexing/detail/mark.hpp>
#include <sooty/lexing/detail/command.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
//=====================================================================
	
	struct lexer_t;
	typedef const lexer_t& const_lexer_ref;
	
	struct lexer_t
	{
		friend lexer_t match(char, char);
		friend lexer_t insert(size_t, const_lexer_ref);
		friend lexer_t string_(const std::string&);
		
		friend lexer_t operator >> (const_lexer_ref, const_lexer_ref);
		friend lexer_t operator | (const_lexer_ref, const_lexer_ref);
		
		detail::const_lexer_backend_ptr_ref backend() const { return backend_; }
		
		lexer_t operator * () const;
		
	private:
		lexer_t(detail::const_lexer_backend_ptr_ref backend)
			: backend_(backend)
		{
		}
		
		detail::lexer_backend_ptr backend_;
	};
	
	
	
	
	inline lexer_t operator >> (const_lexer_ref lhs, const_lexer_ref rhs) {
		return lexer_t(
			detail::lexer_backend_t::seq_and(lhs.backend(), rhs.backend())
		);
	}
	
	inline lexer_t operator | (const_lexer_ref lhs, const_lexer_ref rhs) {
		return lexer_t(
			detail::lexer_backend_t::one(lhs.backend(), rhs.backend())
		);
	}
	
	
	
	
	inline lexer_t match(char from, char to) {
		using detail::command_t;
		
		detail::mark_t mark = detail::generate_mark();
		
		return lexer_t(
			detail::lexer_backend_t::make()
				->push_back_command( command_t::match(from, to) )
				->push_back_command( command_t::terminal() )
		);
	}
	
	inline lexer_t match(char c) {
		return match(c, c);
	}
	
	inline lexer_t string_(const std::string& str) {
		detail::lexer_backend_ptr backend = detail::lexer_backend_t::make();
		for (std::string::const_iterator i = str.begin(); i != str.end(); ++i) {
			backend->push_back_command( detail::command_t::match(*i, *i) );
		}
		backend->push_back_command( detail::command_t::terminal() );
		
		return lexer_t(backend);
	}
	
	inline lexer_t insert(size_t insert_id, const_lexer_ref L) {
		return lexer_t(
			detail::lexer_backend_t::seq_and(
				L.backend(),
				detail::lexer_backend_t::make()->push_back_command(detail::command_t::combine(insert_id))
			)
		);
	}
	

//=====================================================================
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

