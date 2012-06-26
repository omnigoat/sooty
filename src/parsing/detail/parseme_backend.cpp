#include <sooty/parsing/detail/parseme_backend.hpp>
//=====================================================================
#include <boost/lexical_cast.hpp>
//=====================================================================


sooty::parsing::detail::parseme_backend::parseme_backend()
	: id_(), lexeme_(), parent_()
{	
}

sooty::parsing::detail::parseme_backend::parseme_backend( id_t id )
	: id_(id), lexeme_(), parent_()
{	
}

sooty::parsing::detail::parseme_backend::parseme_backend( parseme_backend_ptr parent, id_t id )
	: id_(id), lexeme_(), parent_(parent)
{	
}

sooty::parsing::detail::parseme_backend::parseme_backend( parseme_backend_ptr parent, id_t id, const lexing::lexeme_t* lexeme )
	: id_(id), lexeme_(lexeme), parent_(parent)
{	
}
sooty::parsing::detail::parseme_backend::id_t sooty::parsing::detail::parseme_backend::id() const
{
	return id_;
}

sooty::lexing::position_t sooty::parsing::detail::parseme_backend::position() const
{
	if (lexeme_)
		return lexeme_->position();
	else
		return lexing::position_t();
}

std::string sooty::parsing::detail::parseme_backend::text() const
{
	if (lexeme_)
		return lexeme_->text();
	else
		return std::string();
}

int sooty::parsing::detail::parseme_backend::integer() const
{
	if (lexeme_)
		return boost::lexical_cast<int>( this->text() );
	else
		return 0;
}

sooty::parsing::detail::parseme_backend_ptr sooty::parsing::detail::parseme_backend::parent() const
{
	return parent_;
}

void sooty::parsing::detail::parseme_backend::set_parent( parseme_backend_ptr parent )
{
	parent_ = parent;
}


sooty::parsing::parsemes_t& sooty::parsing::detail::parseme_backend::children()
{
	return children_;
}
