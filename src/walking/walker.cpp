#include <sooty/walking/walker.hpp>
//=====================================================================
#include <sooty/common/detail/clone.hpp>
#include <sooty/common/detail/append_failure.hpp>
#include <sooty/common/detail/append_success.hpp>
#include <sooty/common/detail/append_invalid.hpp>
//=====================================================================
#include <sooty/walking/detail/layout.hpp>
#include <sooty/walking/detail/context.hpp>
#include <sooty/walking/detail/position_setter_walker_backend.hpp>
#include <sooty/walking/detail/rewriter_walker_backend.hpp>
//=====================================================================

using namespace sooty::walking;

// match rhs *fuzzily*
//
//  a node 'a' with sequential children 'b' and 'c':
//   a[b >> c] 
//
//     a[b >> c]         - MATCH
//     a[f >> b >> c]    - MATCH
//     a[b >> f >> c]    - NO MATCH
//     a[c >> b]         - NO MATCH
//
walker walker::operator[]( const walker& rhs ) const
{
	using namespace common::detail;
	using namespace detail;
	
	// note: the ordering of these statements is not interchangeable
	detail::abstract_walker_backend_ptr new_rhs = common::detail::clone_tree(rhs.backend);
	
	// if we fail, try our next sibling
	append_failure(new_rhs, detail::link_t(new_rhs, detail::how_to_traverse::next_sibling));
	
	// if we're ever invalid, surface but make sure we know we've failed
	abstract_walker_backend_ptr dummy_1 = dummy().backend;
	append_invalid(new_rhs, link_t(dummy_1, how_to_traverse::surface));
	append_success(dummy_1, link_t(abstract_walker_backend_ptr(new failer_walker_backend), how_to_traverse::stay));
	
	// if we succeed, we *must* surface
	append_success(new_rhs, detail::link_t(dummy().backend, detail::how_to_traverse::surface));
	
	// finally, clone lhs and append rhs onto it
	detail::abstract_walker_backend_ptr new_lhs = clone_tree(backend);
	append_success(new_lhs, detail::link_t(new_rhs, detail::how_to_traverse::dive));
	
	return walker(new_lhs);
}


sooty::walking::walker sooty::walking::walker::operator*() const
{
	detail::abstract_walker_backend_ptr new_lhs = common::detail::clone_tree(this->backend);
	
	// if we succeed, try again!
	sooty::common::detail::append_success(new_lhs, detail::link_t(new_lhs, detail::how_to_traverse::next_sibling));
	// if we fail, it doesn't matter - append a dummy and stay where we are. STAY.
	sooty::common::detail::append_failure(new_lhs, detail::link_t(walking::dummy().backend, detail::how_to_traverse::prev_sibling));
	
	return walker(new_lhs);
}

sooty::walking::walker sooty::walking::walker::operator + () const
{
	return *this >> *(*this);
}

sooty::walking::walker sooty::walking::walker::on_match()
{
	detail::abstract_walker_backend_ptr new_lhs = sooty::common::detail::clone_tree(backend);
	common::detail::append_success(new_lhs, detail::link_t(
		detail::abstract_walker_backend_ptr(new detail::debug_walker_backend),
		detail::how_to_traverse::stay
	));
	return walker(new_lhs);
}



walker sooty::walking::operator >> (const walker& lhs, const walker& rhs)
{
	if ( boost::shared_dynamic_cast<detail::start_walker_backend>(lhs.backend)) {
		detail::abstract_walker_backend_ptr new_lhs = sooty::common::detail::clone_tree(lhs.backend);
		sooty::common::detail::append_success(new_lhs, detail::link_t(rhs.backend, detail::how_to_traverse::stay));
		return walker(new_lhs);
	}
	else {
		detail::abstract_walker_backend_ptr new_lhs = sooty::common::detail::clone_tree(lhs.backend);
		sooty::common::detail::append_success(new_lhs, detail::link_t(rhs.backend, detail::how_to_traverse::next_sibling));
		return walker(new_lhs);
	}
}

walker sooty::walking::operator , (const walker& lhs, const walker& rhs)
{
	return operator >> (lhs, rhs);
}

sooty::walking::walker sooty::walking::eq( parsing::parseme::id_t id )
{
	return walker( detail::abstract_walker_backend_ptr(new detail::id_eq_walker_backend(id, id)) );
}

sooty::walking::walker sooty::walking::fuzzy_eq( parsing::parseme::id_t id )
{
	using namespace detail;
	using namespace common::detail;
	
	detail::abstract_walker_backend_ptr head(new id_eq_walker_backend(id, id));
	detail::abstract_walker_backend_ptr dummy_1 = dummy().backend;
	detail::abstract_walker_backend_ptr dummy_2 = dummy().backend;
	detail::abstract_walker_backend_ptr dummy_3 = dummy().backend;
	
	dummy_3->on_success = link_t(head, how_to_traverse::stay);
	dummy_3->on_invalid = link_t(dummy_1, how_to_traverse::stay);
	
	dummy_2->on_success = link_t(dummy_3, how_to_traverse::next_sibling);
	
	dummy_1->on_success = link_t(head, how_to_traverse::stay);
	dummy_1->on_invalid = link_t(dummy_2, how_to_traverse::surface);
	
	head->on_failure = link_t(dummy_1, how_to_traverse::dive);
	
	/*
	id_eq {0}
		fail: dive
			invalid: surface - {1}
				success: next_sibling
					invalid: {1}
					success: {0}
			success: {0}
		success:*/
				
	return walker(head);
}

sooty::walking::walker sooty::walking::walker::rewriter::operator[]( const transforming::transformer& rhs ) const
{
	detail::abstract_walker_backend_ptr new_lhs( new detail::position_setter_walker_backend(0) );
	common::detail::append_success(new_lhs, detail::link_t(common::detail::clone_tree(backend), detail::how_to_traverse::stay) );
	common::detail::append_success(new_lhs, detail::link_t(detail::abstract_walker_backend_ptr(new detail::rewrite_walker_backend(0, rhs)), detail::how_to_traverse::stay) );
	return walker(new_lhs);
}
