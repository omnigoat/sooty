#include <sooty/parsing/detail/parseme_backend.hpp>
//=====================================================================
#include <boost/lexical_cast.hpp>
//=====================================================================
using sooty::parsing::detail::parseme_backend;

parseme_backend::parseme_backend()
	: id_(), lexeme_(), parent_()
{	
}

parseme_backend::parseme_backend( id_t id, const lexing::lexeme_t* lexeme )
	: id_(id), lexeme_(lexeme), parent_()
{
}

parseme_backend::parseme_backend( parseme_backend_ptr parent, id_t id )
	: id_(id), lexeme_(), parent_(parent)
{	
}

parseme_backend::parseme_backend( parseme_backend_ptr parent, id_t id, const lexing::lexeme_t* lexeme )
	: id_(id), lexeme_(lexeme), parent_(parent)
{	
}

auto parseme_backend::id() const -> parseme_backend::id_t {
	return id_;
}

auto parseme_backend::position() const -> sooty::lexing::position_t {
	return lexeme_ ? lexeme_->position() : sooty::lexing::position_t();
}

auto parseme_backend::lexeme() const -> sooty::lexing::lexeme_t const* {
	return lexeme_;
}

auto parseme_backend::text() const -> std::string {
	return lexeme_ ? lexeme_->text() : std::string();
}

auto parseme_backend::integer() const -> int {
	return lexeme_ ? boost::lexical_cast<int>(lexeme_->text()) : 0;
}

auto parseme_backend::parent() const -> sooty::parsing::detail::parseme_backend_ptr {
	return parent_;
}

auto parseme_backend::set_parent( parseme_backend_ptr parent ) -> void {
	parent_ = parent;
}

auto parseme_backend::children() -> sooty::parsing::parsemes_t& {
	return children_;
}

auto parseme_backend::children() const -> sooty::parsing::parsemes_t const& {
	return children_;
}
