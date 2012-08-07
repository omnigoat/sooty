#include <sooty/lexing/lexer.hpp>
//=====================================================================

sooty::lexing::lexer_t sooty::lexing::lexer_t::operator*() const
{
	return lexer_t(
		detail::lexer_backend_t::make_seq_and(true)
			->add_child(backend_->clone())
			->add_self_as_child()
	);
}

sooty::lexing::lexer_t sooty::lexing::lexer_t::operator+() const
{
	return lexer_t(
		backend_->clone()
			->add_child( detail::lexer_backend_t::make_seq_and(true)
				->add_child(backend_->clone())
				->add_self_as_child()
			)
	);
}