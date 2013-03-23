#include <sooty/lexing/lexer.hpp>
//=====================================================================
#include <sooty/common/clone.hpp>
#include <atma/unittest/unittest.hpp>
//=====================================================================
using namespace sooty::lexing;
using sooty::common::clone_tree;

lexer_t::lexer_t(detail::lexer_backends_t const& backends)
	: backends_(backends)
{
}

lexer_t::lexer_t(detail::lexer_backend_ptr const& b)
{
	backends_.insert(b);
}

//lexer_t::lexer_t(std::initializer_list<detail::lexer_backend_ptr> const& list)
//	: backends_{list.begin(), list.end()}
//{
//}

lexer_t::lexer_t(lexer_t const& rhs)
	: backends_(rhs.backends_)
{
}


auto lexer_t::operator = (lexer_t const& rhs) -> lexer_t& {
	backends_ = rhs.backends_;
	return *this;
}

auto lexer_t::operator * () const -> lexer_t
{
	detail::lexer_backends_t B = common::clone_nodes(backends_);

	common::interweave_nodes(B);
	for (auto& b : B) { b->set_as_bypassable(); }

	return lexer_t(B);
}

auto lexer_t::operator + () const -> lexer_t {
	// this might be the best line of code I've ever written.
	return *this >> **this;
}

auto lexer_t::operator ! () const -> lexer_t
{
	// for each leaf, let it be a terminator
	detail::lexer_backends_t B = common::clone_nodes(backends_); //clone_tree(backend_);

	/*common::for_each_depth_first(B, [](detail::lexer_backend_ptr const& N) {
		N->set_as_bypassable();
	});*/

	return lexer_t(B);
}


//
//auto lexer_t::operator [] (callback_t const& acc) const -> lexer_t
//{
//	// for each leaf, let it perform this action on success
//	detail::lexer_backend_ptr B = common::clone_nodes(backends_);
//
//	common::append(
//		new_backend,
//		detail::lexer_backend_t::make()
//			->push_back_command( detail::command_t(detail::command_t::action_t::passthrough, 0, 0, false, acc) )
//	);
///*
//	common::for_each_depth_first(new_backend, [&acc](detail::lexer_backend_ptr const& L) {
//		if (L->children_.empty())
//			L->push_back_command( detail::command_t(detail::command_t::action_t::passthrough, 0, 0, false, acc) );
//		if (L->terminal())
//			L->push_back_failure( detail::command_t(detail::command_t::action_t::passthrough, 0, 0, false, acc) );
//	});*/
//
//	return lexer_t(new_backend);
//}

//auto lexer_t::backends() const -> detail::lexer_backends_t const& {
//	return backends_;
//}


auto sooty::lexing::operator >> ( lexer_t const& lhs, lexer_t const& rhs ) -> lexer_t {
	return lexer_t(
		common::append(
			common::clone_nodes(lhs.backends_),
			common::clone_nodes(rhs.backends_)
		)
	);
}


auto sooty::lexing::operator | ( lexer_t const& lhs, lexer_t const& rhs ) -> lexer_t {
	return lexer_t(
		common::merge(
			common::clone_nodes(lhs.backends_),
			common::clone_nodes(rhs.backends_)
		)
	);
}



//auto sooty::lexing::peek(char c) -> lexer_t {
//	return lexer_t(
//		detail::lexer_backend_t::make()
//			->push_back_command( detail::command_t::peek(c, c) )
//	);
//}

auto sooty::lexing::match(char c) -> lexer_t {
	return match(c, c);
}

auto sooty::lexing::match(char from, char to) -> lexer_t {
	using namespace detail;

	return lexer_t(
		//detail::lexer_backend_ptr(new detail::lexer_node_t(detail::lexer_node_t::action_t::match, from, to))
		lexer_backend_t::make(lexer_node_t::action_t::match, from, to)
	);
}

auto sooty::lexing::match(std::string const& str) -> lexer_t {
	
	detail::lexer_backends_t bs;
	for (std::string::const_iterator i = str.begin(); i != str.end(); ++i) {
		bs.insert(detail::lexer_backend_t::make(detail::lexer_node_t::action_t::match, *i, *i));
	}
	
	return lexer_t(bs);
}


auto sooty::lexing::ignore(char c) -> lexer_t {
	return ignore(c, c);
}

auto sooty::lexing::ignore(char from, char to) -> lexer_t {
	using namespace detail;

	return lexer_t(
		lexer_backend_t::make(lexer_node_t::action_t::match, from, to, false)
	);
}



auto sooty::lexing::insert(size_t insert_id, channel_t const& ch, lexer_t const& L) -> lexer_t {
	using namespace detail;

	detail::lexer_backends_t bs;
	bs.insert(detail::lexer_backend_t::make(lexer_node_t::action_t::combine));

	return lexer_t(
		common::append(
			common::clone_nodes(L.backends_),
			bs
		)
	);
}









