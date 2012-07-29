//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_COMMON_DETAIL_APPEND_INVALID_HPP
#define SOOTY_COMMON_DETAIL_APPEND_INVALID_HPP
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
	Node& append_invalid_impl(std::set<Node>& visited, Node& lhs, const Edge& rhs)
	{
		if ( std::find(visited.begin(), visited.end(), lhs) == visited.end() )
		{
			visited.insert(lhs);
			
			if (valid_edge(lhs->on_invalid)) {
				if (!overwrote_edge(lhs->on_invalid, rhs))
					append_invalid_impl<Node, Edge>(visited, node_of(lhs->on_invalid), rhs);
			}
			else
				lhs->on_invalid = rhs;
			
			if (valid_edge(lhs->on_failure) && !overwrote_edge(lhs->on_failure, rhs))
				append_invalid_impl<Node, Edge>(visited, node_of(lhs->on_failure), rhs);
			
			if (valid_edge(lhs->on_success) && !overwrote_edge(lhs->on_success, rhs))
				append_invalid_impl<Node, Edge>(visited, node_of(lhs->on_success), rhs);
		}
		
		return lhs;
	}

	template <typename Node, typename Edge>
	inline void append_invalid( Node& lhs, const Edge& rhs ) {
		std::set<Node> visited;
		append_invalid_impl<Node, Edge>(visited, lhs, rhs);
	}


//=====================================================================
} // namespace detail
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

