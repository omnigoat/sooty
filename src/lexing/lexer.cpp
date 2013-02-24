#include <sooty/lexing/lexer.hpp>
//=====================================================================
#include <sooty/common/clone.hpp>
//=====================================================================
using namespace sooty::lexing;
using sooty::common::clone_tree;

lexer_t::lexer_t(lexer_t const& rhs)
	: backend_(rhs.backend_)
{
}

lexer_t::lexer_t(detail::const_lexer_backend_ptr_ref backend)
: backend_(backend)
{
}

auto lexer_t::operator = (lexer_t const& rhs) -> lexer_t& {
	backend_ = rhs.backend_;
	return *this;
}

auto lexer_t::operator * () const -> lexer_t
{
	detail::lexer_backend_ptr C = detail::lexer_backend_t::make();
	detail::lexer_backend_ptr B = clone_tree(backend_);

	// minor optimisation, if our head node of B is a control node,
	// we can simply append itself to itself. otherwise, we have to
	// append a new control node and make that the head node (so we can
	// make it a terminable node)
	if (B->type() == detail::lexer_backend_t::type_t::control) {
		B->append(B, false);
	}
	else {
		C->append(B, false);
		B->append(C);
		B = C;
	}
	
	B->set_as_terminator();

	return lexer_t(B);
}

auto lexer_t::operator + () const -> lexer_t {
	// this might be the best line of code I've ever written.
	return *this >> **this;
}

auto lexer_t::backend() const -> detail::lexer_backend_ptr const& {
	return backend_;
}



auto sooty::lexing::operator >> ( const_lexer_ref lhs, const_lexer_ref rhs ) -> lexer_t {
	return lexer_t( clone_tree(lhs.backend())->append(clone_tree(rhs.backend())) );
}

auto sooty::lexing::operator | ( const_lexer_ref lhs, const_lexer_ref rhs ) -> lexer_t {
	return lexer_t( clone_tree(lhs.backend())->merge(clone_tree(rhs.backend())) );
}

auto sooty::lexing::match(char from, char to, bool should_insert) -> lexer_t {
	return lexer_t(
		detail::lexer_backend_t::make()
			->push_back_command( detail::command_t::match(from, to, should_insert) )
	);
}





auto sooty::lexing::match(char c, bool should_insert) -> lexer_t {
	return match(c, c, should_insert);
}


auto sooty::lexing::match(std::string const& str) -> lexer_t {
	detail::lexer_backend_ptr backend = detail::lexer_backend_t::make();
	for (std::string::const_iterator i = str.begin(); i != str.end(); ++i) {
		backend->push_back_command( detail::command_t::match(*i, *i, true) );
	}
	return lexer_t(backend);
}


auto sooty::lexing::insert(size_t insert_id, const_lexer_ref L) -> lexer_t {
	return lexer_t(
		clone_tree(L.backend())->append(
			detail::lexer_backend_t::make()
				->push_back_command(detail::command_t::combine(insert_id))
		)
	);
}
