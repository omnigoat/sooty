//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_COMMON_DETAIL_APPEND_FAILURE_HPP
#define SOOTY_COMMON_DETAIL_APPEND_FAILURE_HPP
//=====================================================================
#include <set>
//=====================================================================
#include <sooty/common/detail/layout.hpp>
//=====================================================================
namespace sooty {
namespace common {
namespace detail {
//=====================================================================
	
	template <typename Node, typename Edge>
	inline void append_failure_impl(std::set<Node>& visited, const Node& lhs, const Edge& rhs)
	{
		// skippable nodes never fail
		if (skippable(lhs))
			if (valid_edge(lhs->on_success))
				return append_failure_impl(visited, node_of(lhs->on_success), rhs);
			else
				return;
		
		if (visited.find(lhs) != visited.end())
			return;
		visited.insert(lhs);
		
		if (valid_edge(lhs->on_failure)) {
			if (!overwrote_edge(lhs->on_failure, rhs))
				append_failure_impl(visited, node_of(lhs->on_failure), rhs);
		}
		else
			lhs->on_failure = rhs;
		
		if (valid_edge(lhs->on_success) && !overwrote_edge(lhs->on_success, rhs))
			append_failure_impl(visited, node_of(lhs->on_success), rhs);
		
		if (valid_edge(lhs->on_invalid) && !overwrote_edge(lhs->on_invalid, rhs))
			append_failure_impl(visited, node_of(lhs->on_invalid), rhs);
	}
	
	template <typename Node, typename Edge>
	inline void append_failure(const Node& lhs, const Edge& rhs) {
		std::set<Node> visited;
		append_failure_impl(visited, lhs, rhs);
	}

//=====================================================================
} // namespace detail
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

