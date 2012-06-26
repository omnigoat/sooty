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
	void append_invalid_impl(std::set<Node>& visited, const Node& lhs, const Edge& rhs)
	{
		if ( std::find(visited.begin(), visited.end(), lhs) == visited.end() )
		{
			visited.insert(lhs);
			
			if ( valid<Node, Edge>(lhs->on_invalid) )
				append_invalid_impl(visited, node_of<Node, Edge>(lhs->on_invalid), rhs);
			else
				lhs->on_invalid = rhs;
			
			
			if ( valid<Node, Edge>(lhs->on_success) )
				append_invalid_impl(visited, node_of<Node, Edge>(lhs->on_failure), rhs);
			
			if ( valid<Node, Edge>(lhs->on_failure) )
				append_invalid_impl(visited, node_of<Node, Edge>(lhs->on_failure), rhs);
		}
	}

	template <typename Node, typename Edge>
	inline void append_invalid( const Node& lhs, const Edge& rhs ) {
		std::set<Node> visited;
		append_invalid_impl(visited, lhs, rhs);
	}


//=====================================================================
} // namespace detail
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

