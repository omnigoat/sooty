//=====================================================================
//
//  fold
//  ------
//    Give two parse-trees, attempt to merge them together as tightly
//    as possible.
//
//=====================================================================
#ifndef SOOTY_COMMON_DETAIL_FOLD_HPP
#define SOOTY_COMMON_DETAIL_FOLD_HPP
//=====================================================================
#include <set>
//=====================================================================
#include <sooty/common/detail/clone.hpp>
#include <sooty/common/detail/append_failure.hpp>
//=====================================================================
namespace sooty {
namespace common {
namespace detail {
//=====================================================================

	template <typename T>
	inline void fold_impl(std::set<T>& visited_nodes, T& lhs, T& rhs)
	{
		if (!lhs || !rhs)
			return;
		
		if (visited_nodes.find(lhs) != visited_nodes.end())
			return;
		visited_nodes.insert(lhs);
		
		if ( should_prepend(lhs) ) {
			fold_impl(visited_nodes, lhs->on_success, rhs);
		}
		// if this rhs node needs to be prepended, we manually add it to lhs->on_success
		else if ( should_prepend(rhs) ) {
			if (lhs->on_success) {
				visited_nodes.erase(lhs);
				T saved_lhs = lhs;
				lhs = rhs;
				T saved_rhs_s = rhs->on_success;
				lhs->on_success = saved_lhs;
				
				fold_impl(visited_nodes, lhs->on_success, saved_rhs_s);
			}
			else {
				lhs->on_success = rhs;
			}
		}
		// if lhs is partially equivalent to rhs, then we need to try rhs
		// if it fails, but try the one /after/ rhs if it succeeds
		else if ( equivalent(lhs, rhs) )
		{
			if (lhs->on_success)
				fold_impl(visited_nodes, lhs->on_success, rhs->on_success);
			else {
				lhs->on_success = rhs->on_success;
				if (lhs->on_failure) {
					append_failure(lhs->on_success, lhs->on_failure);
					lhs->on_failure = T();
				}
			}
		}
		//else if ( partially_equivalent(lhs, rhs) )
		//{
		//	if (lhs->on_failure)
		//		append_failure(lhs->on_failure, rhs);
		//	else
		//		lhs->on_failure = rhs;
		//	
		//	if (lhs->on_success)
		//		fold_impl(visited_nodes, lhs->on_success, rhs->on_success);
		//	else
		//		lhs->on_success = rhs->on_success;
		//}
		//// rhs is partially equivalent to lhs, which means that we have to
		//// test rhs first, *then* lhs.
		//else if ( partially_equivalent(rhs, lhs) )
		//{
		//	T lhs_copy = clone_tree(lhs);
		//	*lhs = *rhs;
		//	visited_nodes.erase(lhs);
		//	fold_impl(visited_nodes, lhs, lhs_copy);
		//}
		// we have found a point of difference
		else {
			if (lhs->on_failure)
				append_success_impl(visited_nodes, lhs->on_failure, rhs);
			else
				lhs->on_failure = rhs;
		}
	}

	template <typename T>
	inline void fold(T& lhs, const T& rhs) {
		std::set<T> visited_nodes;
		fold_impl<T>(visited_nodes, lhs, *const_cast<T*>(&rhs));
	}

//=====================================================================
} // namespace detail
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

