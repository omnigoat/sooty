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
#include <cassert>
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
	void fold_fail_impl(std::set<T>& visited_nodes, T& lhs, T& rhs)
	{
		if (visited_nodes.find(lhs) != visited_nodes.end())
			return;
		visited_nodes.insert(lhs);
		
		if ( equivalent_in_failure(lhs, rhs) )
		{
			if (valid_edge(lhs->on_success))
				fold_impl(visited_nodes, lhs->on_success, rhs->on_success);
			else
				lhs->on_success = rhs->on_success;
			
			if (valid_edge(lhs->on_failure))
				fold_fail_impl(visited_nodes, lhs->on_failure, rhs->on_failure);
			else
				lhs->on_failure = rhs->on_failure;
		}
		else {
			if (valid_edge(lhs->on_failure))
				fold_fail_impl(visited_nodes, lhs->on_failure, rhs);
			else
				lhs->on_failure = rhs;
		}
	}
	
	template <typename T>
	void fold_impl(std::set<T>& visited_nodes, T& lhs, T& rhs)
	{
		assert(lhs && rhs);
		
		if ( skippable(lhs) ) {
			lhs = lhs->on_success;
			if (!lhs) return;
		}
		if ( skippable(rhs) ) {
			rhs = rhs->on_success;
			if (!rhs) return;
		}
		
		if (visited_nodes.find(lhs) != visited_nodes.end())
			return;
		visited_nodes.insert(lhs);
		
		
		if ( should_prepend(lhs) ) {
			fold_impl(visited_nodes, lhs->on_success, rhs);
		}
		else if (should_prepend(rhs))
		{
			visited_nodes.erase(lhs);
			T saved_lhs = lhs;
			T rhs_os = rhs->on_success;
			lhs = rhs;
			lhs->on_success = saved_lhs;
			
			if (valid_edge(lhs->on_success))
				fold_impl(visited_nodes, lhs->on_success, rhs_os);
			else
				lhs->on_success = rhs_os;
		}
		// equivalent may overwrite lhs with rhs, but leaves the edges intact
		else if ( equivalent(lhs, rhs) )
		{
			if (valid_edge(rhs->on_success)) {
				if (valid_edge(lhs->on_success)) {
					fold_impl(visited_nodes, lhs->on_success, rhs->on_success);
				}
				else
					lhs->on_success = rhs->on_success;
			}
			
			if (valid_edge(rhs->on_failure))
				if (valid_edge(lhs->on_failure))
					fold_impl(visited_nodes, lhs->on_failure, rhs->on_failure);
				else
					lhs->on_failure = rhs->on_failure;
		}
		// point of contention
		else {
			/*if (valid_edge(lhs->on_failure))
				fold_fail_impl(visited_nodes, lhs->on_failure, rhs);
			else
				lhs->on_failure = rhs;
				
			if (valid_edge(rhs->on_failure))
				if (valid_edge(lhs->on_success))
					fold_impl(visited_nodes, lhs->on_success, rhs->on_failure);
				else
					lhs->on_success = rhs->on_failure;*/
			if (valid_edge(lhs->on_failure))
				append_failure_impl(visited_nodes, lhs->on_failure, rhs);
			else
				lhs->on_failure = rhs;
			
			if (valid_edge(rhs->on_failure))
				if (valid_edge(lhs->on_success))
					append_success_impl(visited_nodes, lhs, rhs->on_failure);
				else
					lhs->on_success = rhs->on_failure;
		}
	}

	template <typename T>
	T fold(T& lhs, T& rhs) {
		std::set<T> visited_nodes;
		fold_impl<T>(visited_nodes, lhs, rhs);
		return lhs;
	}

//=====================================================================
} // namespace detail
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

