#include <sooty/lexing/lexer.hpp>
//=====================================================================

sooty::lexing::lexer_t sooty::lexing::lexer_t::operator*() const
{
	detail::lexer_backend_ptr master_and = detail::lexer_backend_t::make_seq_and(true);
	
	
	master_and
		->add_child(backend_->clone())
		->add_child(master_and);
	
	return lexer_t(master_and);
}