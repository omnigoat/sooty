//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_COMMON_DETAIL_APPEND_SUCCESS_HPP
#define SOOTY_COMMON_DETAIL_APPEND_SUCCESS_HPP
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
	void append_success_impl(std::set<Node>& visited, Node& lhs, const Edge& rhs)
	{
		if ( std::find(visited.begin(), visited.end(), lhs) == visited.end() )
		{
			visited.insert(lhs);
			
			if (valid_edge(lhs->on_success)) {
				if (!overwrote_edge(lhs->on_success, rhs))
					append_success_impl<Node, Edge>(visited, node_of(lhs->on_success), rhs);
			}
			else
				lhs->on_success = rhs;
			
			if (valid_edge(lhs->on_failure) && !overwrote_edge(lhs->on_failure, rhs))
				append_success_impl<Node, Edge>(visited, node_of(lhs->on_failure), rhs);
			
			if (valid_edge(lhs->on_invalid) && !overwrote_edge(lhs->on_invalid, rhs))
				append_success_impl<Node, Edge>(visited, node_of(lhs->on_invalid), rhs);
		}
	}

	template <typename Node, typename Edge>
	inline void append_success( Node& lhs, const Edge& rhs ) {
		std::set<Node> visited;
		append_success_impl(visited, lhs, rhs);
	}


//=====================================================================
} // namespace detail
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

