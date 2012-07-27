//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_LEXER_HPP
#define SOOTY_LEXING_LEXER_HPP
//=====================================================================
#include <sooty/common/detail/fold.hpp>
//=====================================================================
#include <sooty/lexing/detail/base_lexer.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
//=====================================================================
	
	typedef detail::lex_results_t lex_results_t;
	typedef detail::semantic_action semantic_action;
	
	struct lexer
	{
		detail::base_lexer_ptr base_lexer;
		
		lexer(const detail::base_lexer_ptr& base_lexer) : base_lexer(base_lexer)
		{
		}
		
		lexer operator >> (const lexer& rhs) const {
			detail::base_lexer_ptr new_lhs = clone_tree(this->base_lexer);
			detail::append_success(new_lhs, rhs.base_lexer);
			return lexer(new_lhs);
		}
		
		lexer operator | (const lexer& rhs) const {
			detail::base_lexer_ptr new_lhs = clone_tree(this->base_lexer);
			//detail::fold(new_lhs, rhs.base_lexer);
			sooty::common::detail::fold(new_lhs, rhs.base_lexer);
			return lexer(new_lhs);
		}
		
		lexer operator * () const {
			detail::base_lexer_ptr new_lhs = clone_tree(this->base_lexer);
			detail::zero_or_more(new_lhs);
			return lexer(new_lhs);
		}
		
		lexer operator + () const {
			return *this >> **this;
		}
		
		lexer operator [](semantic_action action) const {
			static char _ = 0;
			detail::base_lexer_ptr marker(new detail::base_lexer(detail::base_lexer::lexer_type::marker, _, _));
			detail::base_lexer_ptr actor(new detail::base_lexer(detail::base_lexer::lexer_type::actor, _, _));
			actor->action = action;
			++_;
			append_success(marker, clone_tree(this->base_lexer));
			append_success(marker, actor);
			return lexer(marker);
		}
	};

	inline lexer char_(char c) {
		return lexer(detail::base_lexer_ptr(new detail::base_lexer(detail::base_lexer::lexer_type::matcher, c, c)));
	}

	inline lexer in_range(char from, char to) {
		return lexer(detail::base_lexer_ptr(new detail::base_lexer(detail::base_lexer::lexer_type::matcher, from, to)));
	}

	inline lexer string_(const std::string& str) {
		detail::base_lexer_ptr previous_lexer;
		
		for (std::string::const_iterator i = str.begin(); i != str.end(); ++i) {
			if (previous_lexer)
				append_success(previous_lexer, char_(*i).base_lexer);
			else
				previous_lexer = char_(*i).base_lexer;
		}
		return lexer(previous_lexer);
	}
	
	namespace detail {
		lex_results_t lex_impl(const lexer& lexer, input_iterator begin, input_iterator end);
	}
	
	template <typename T>
	inline lex_results_t lex(const lexer& start, const T& begin, const T& end) {
		return detail::lex_impl(start, input_iterator(begin), input_iterator(end));
	}
	
	template <>
	inline lex_results_t lex(const lexer& start, const input_iterator& begin, const input_iterator& end) {
		return detail::lex_impl(start, begin, end);
	}
	
	

//=====================================================================
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

