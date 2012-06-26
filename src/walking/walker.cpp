#include <sooty/walking/walker.hpp>
//=====================================================================
#include <sooty/common/detail/clone.hpp>
#include <sooty/common/detail/append_failure.hpp>
#include <sooty/common/detail/append_success.hpp>
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
	// note: the ordering of these statements is not interchangeable
	detail::abstract_walker_backend_ptr new_rhs = common::detail::clone_tree(rhs.backend);
	
	// if we fail, try our next sibling
	sooty::common::detail::append_failure(new_rhs, detail::link_t(new_rhs, detail::how_to_traverse::next_sibling));
	
	// if we succeed, we *must* surface, and the easiest way to do so is to insert a dummy walker
	sooty::common::detail::append_success(new_rhs, detail::link_t(dummy().backend, detail::how_to_traverse::surface));
	
	// finally, clone lhs and append rhs onto it
	detail::abstract_walker_backend_ptr new_lhs = common::detail::clone_tree(this->backend);
	sooty::common::detail::append_success(new_lhs, detail::link_t(new_rhs, detail::how_to_traverse::dive));
	
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

sooty::walking::walker sooty::walking::walker::rewriter::operator[]( const transforming::transformer& rhs ) const
{
	detail::abstract_walker_backend_ptr new_lhs( new detail::position_setter_walker_backend(0) );
	common::detail::append_success(new_lhs, detail::link_t(common::detail::clone_tree(backend), detail::how_to_traverse::stay) );
	common::detail::append_success(new_lhs, detail::link_t(detail::abstract_walker_backend_ptr(new detail::rewrite_walker_backend(0, rhs)), detail::how_to_traverse::stay) );
	return walker(new_lhs);
}
