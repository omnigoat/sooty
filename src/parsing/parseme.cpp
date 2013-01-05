#include <sooty/parsing/parseme.hpp>
//=====================================================================
#include <sooty/parsing/detail/parseme_backend.hpp>
//=====================================================================
using sooty::parsing::parseme;
using sooty::parsing::const_parseme_ref;
using sooty::parsing::detail::parseme_backend;
using sooty::parsing::detail::parseme_backend_ptr;

parseme::parseme()
{
}

parseme::parseme(parseme_backend_ptr backend)
: backend_(backend)
{
}

parseme::parseme(id_t id)
: backend_(new parseme_backend(id))
{
	backend_->children_.owner = backend_;
}


parseme::parseme( const_parseme_ref parent, id_t id, const sooty::lexing::lexeme_t* lexeme )
: backend_(new parseme_backend(parent.backend_, id, lexeme))
{
	backend_->children_.owner = backend_;
}

// accessors
auto parseme::id() const -> parseme::id_t {
	return backend_->id();
}

auto parseme::position() const -> position_t {
	return backend_->position();
}

auto parseme::text() const -> std::string {
	return backend_->text();
}

auto parseme::integer() const -> int {
	return backend_->integer();
}

auto parseme::parent() const -> parseme {
	return parseme(backend_->parent());
}

auto parseme::children() const -> const sooty::parsing::parsemes_t& {
	return backend_->children();
}

auto parseme::lexeme() const -> sooty::lexing::lexeme_t const* {
	return backend_->lexeme();
}

// mutators
auto parseme::set_parent(const_parseme_ref parent) -> void {
	backend_->set_parent(parent.backend_);
}

auto parseme::children() -> sooty::parsing::parsemes_t& {
	return backend_->children();
}
