#include <sooty/parsing/parseme.hpp>
//=====================================================================
#include <sooty/parsing/detail/parseme_backend.hpp>
//=====================================================================

sooty::parsing::parseme::parseme()
{
}

sooty::parsing::parseme::parseme(detail::parseme_backend_ptr backend)
	: backend_(backend)
{
}


sooty::parsing::parseme::parseme(id_t id)
	: backend_(new detail::parseme_backend(id))
{
	backend_->children_.owner = backend_;
}


sooty::parsing::parseme::parseme( const_parseme_ref parent, id_t id, const lexing::lexeme_t* lexeme )
	: backend_(new detail::parseme_backend(parent.backend_, id, lexeme))
{
	backend_->children_.owner = backend_;
}

sooty::parsing::parseme::id_t sooty::parsing::parseme::id() const {
	return backend_->id();
}

sooty::lexing::position_t sooty::parsing::parseme::position() const
{
	return backend_->position();
}

std::string sooty::parsing::parseme::text() const
{
	return backend_->text();
}

int sooty::parsing::parseme::integer() const
{
	return backend_->integer();
}

sooty::parsing::parseme sooty::parsing::parseme::parent()
{
	return parseme(backend_->parent());
}

void sooty::parsing::parseme::set_parent( const_parseme_ref parent )
{
	backend_->set_parent(parent.backend_);
}


sooty::parsing::parsemes_t& sooty::parsing::parseme::children()
{
	return backend_->children();
}

const sooty::lexing::lexeme_t* sooty::parsing::parseme::lexeme() const
{
	return backend_->lexeme();
}