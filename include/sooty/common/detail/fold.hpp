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
	void fold_impl(std::set<T>& visited_nodes, T& lhs, T& rhs)
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
			visited_nodes.erase(lhs);
			T saved_lhs = lhs;
			T saved_rhs = rhs;
			T& prepended_node = lhs;
			prepended_node = rhs;
			
			prepended_node->on_success = saved_lhs;
			append_failure(prepended_node->on_success, saved_rhs->on_failure);
			prepended_node->on_failure = saved_rhs;
			append_success(prepended_node->on_failure, saved_lhs->on_success);
			
			//fold_impl(visited_nodes, lhs->on_success, saved_rhs->on_success);
			//fold_impl(visited_nodes, lhs->on_failure, saved_rhs->on_failure);
		}
		// equivalent nodes are simply skipped side-by-side. well, unless there is further
		// nodes attached to rhs->on_success. then, 
		else if ( equivalent(lhs, rhs) )
		{
			if (valid_edge(lhs->on_success))
				fold_impl(visited_nodes, lhs->on_success, rhs->on_success);
			else
				lhs->on_success = rhs->on_success;
		}
		else {
			append_failure(lhs, rhs);
			append_success(lhs, rhs->on_failure);
		}
	}

	template <typename T>
	void fold(T& lhs, const T& rhs) {
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

