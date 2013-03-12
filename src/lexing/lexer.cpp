#include <sooty/lexing/lexer.hpp>
//=====================================================================
#include <sooty/common/clone.hpp>
#include <atma/unittest/unittest.hpp>
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
	//detail::lexer_backend_ptr C = detail::lexer_backend_t::make();
	detail::lexer_backend_ptr B = clone_tree(backend_);

	common::append(B, B);
	B->set_as_terminator();

	return lexer_t(B);
}

auto lexer_t::operator + () const -> lexer_t {
	// this might be the best line of code I've ever written.
	return *this >> **this;
}

auto lexer_t::operator ! () const -> lexer_t
{
	// for each leaf, let it be a terminator
	detail::lexer_backend_ptr new_backend = clone_tree(backend_);

	common::for_each_depth_first(new_backend, [](detail::lexer_backend_ptr const& L) {
		L->set_as_terminator();
	});

	return lexer_t(new_backend);
}

auto lexer_t::operator [] (callback_t const& acc) const -> lexer_t
{
	// for each leaf, let it perform this action on success
	detail::lexer_backend_ptr new_backend = clone_tree(backend_);

	common::for_each_depth_first(new_backend, [&acc](detail::lexer_backend_ptr const& L) {
		if (L->children_.empty())
			L->push_back_command( detail::command_t(detail::command_t::action_t::passthrough, 0, 0, false, acc) );
		if (L->terminal())
			L->push_back_failure( detail::command_t(detail::command_t::action_t::passthrough, 0, 0, false, acc) );
	});

	return lexer_t(new_backend);
}

auto lexer_t::backend() const -> detail::lexer_backend_ptr const& {
	return backend_;
}


auto sooty::lexing::operator >> ( lexer_t const& lhs, lexer_t const& rhs ) -> lexer_t {
	return lexer_t( common::append(clone_tree(lhs.backend()), clone_tree(rhs.backend())) );
}


auto sooty::lexing::operator | ( lexer_t const& lhs, lexer_t const& rhs ) -> lexer_t {
	return lexer_t( clone_tree(lhs.backend())->merge(clone_tree(rhs.backend())) );
}



auto sooty::lexing::peek(char c) -> lexer_t {
	return lexer_t(
		detail::lexer_backend_t::make()
			->push_back_command( detail::command_t::peek(c, c) )
	);
}

auto sooty::lexing::match(char c) -> lexer_t {
	return match(c, c);
}

auto sooty::lexing::match(char from, char to) -> lexer_t {
	return lexer_t(
		detail::lexer_backend_t::make()
			->push_back_command( detail::command_t::match(from, to, true) )
	);
}

auto sooty::lexing::match(std::string const& str) -> lexer_t {
	detail::lexer_backend_ptr backend = detail::lexer_backend_t::make();
	for (std::string::const_iterator i = str.begin(); i != str.end(); ++i) {
		backend->push_back_command( detail::command_t::match(*i, *i, true) );
	}
	return lexer_t(backend);
}


auto sooty::lexing::ignore(char c) -> lexer_t {
	return ignore(c, c);
}

auto sooty::lexing::ignore(char from, char to) -> lexer_t {
	return lexer_t(
		detail::lexer_backend_t::make()
			->push_back_command( detail::command_t::match(from, to, false) )
	);
}



auto sooty::lexing::insert(size_t insert_id, channel_t const& ch, lexer_t const& L) -> lexer_t {
	return lexer_t(
		append(
			clone_tree(L.backend()),
			detail::lexer_backend_t::make()
				->push_back_command(detail::command_t::combine(insert_id, ch))
		)
	);
}


//ATMA_SUITE(lexing)
//{
//	ATMA_TEST(nodes)
//	{
//		lexer L = match('a') >> match('b');
//	}
//}
//












