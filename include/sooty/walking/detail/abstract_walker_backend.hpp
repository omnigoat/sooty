//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_WALKING_DETAIL_ABSTRACT_WALKER_BACKEND_HPP
#define SOOTY_WALKING_DETAIL_ABSTRACT_WALKER_BACKEND_HPP
//=====================================================================
#include <sooty/walking/detail/context.hpp>
//=====================================================================
namespace sooty {
namespace walking {
namespace detail {
//=====================================================================
	
	// forward declares
	struct abstract_walker_backend;
	typedef boost::shared_ptr<abstract_walker_backend> abstract_walker_backend_ptr;
	
	// the link between nodes (it contains extra data - how_to_traverse)
	//typedef std::pair<abstract_walker_backend_ptr, how_to_traverse::Enum> link_t;
	struct link_t {
		abstract_walker_backend_ptr node;
		how_to_traverse::Enum traversal;
		
		link_t() : node(), traversal(how_to_traverse::stay) {}
		
		link_t(const abstract_walker_backend_ptr& node, how_to_traverse::Enum traversal)
			: node(node), traversal(traversal)
		{
		}
	};
	
	struct abstract_walker_backend
	{
		link_t on_success, on_failure, on_invalid;
		
		virtual abstract_walker_backend_ptr clone() = 0;
		
		
		// some of our walkers really don't mind if the container in the context has
		// no parsemes in it (transformers that will *add* parsemes)
		virtual bool invalid_when_empty() const {
			return true;
		}
		
		virtual bool compare_impl(context_ref context) {
			return true;
		}
		
		virtual bool is_dummy() const { return false; }
	};
	
	inline bool valid_edge(const link_t& e) {
		return e.node;
	}
	
	inline abstract_walker_backend_ptr& node_of(link_t& e) {
		return e.node;
	}
	
	inline const abstract_walker_backend_ptr& node_of(const link_t& e) {
		return e.node;
	}
	
	inline link_t new_edge_from(const abstract_walker_backend_ptr& n, const link_t& e) {
		return link_t(n, e.traversal);
	}
	
	inline bool skippable(const abstract_walker_backend_ptr&) {
		return false;
	}
	
	// this function returns true if lhs was able to be overridden by rhs
	// it returns false if it was impossible
	inline bool overwrote_edge(link_t& lhs, const link_t& rhs)
	{
		assert(lhs.traversal != how_to_traverse::prev_sibling || rhs.traversal != how_to_traverse::prev_sibling);
		
		if (lhs.node->is_dummy()) {
			if (lhs.traversal == how_to_traverse::stay) {
				lhs.traversal = rhs.traversal;
				lhs.node = rhs.node;
				return true;
			}
			else if (lhs.traversal == how_to_traverse::prev_sibling && rhs.traversal == how_to_traverse::next_sibling) {
				lhs.traversal = how_to_traverse::stay;
				lhs.node = rhs.node;
				return true;
			}
			else if (rhs.traversal == how_to_traverse::stay) {
				lhs.node = rhs.node;
				return true;
			}
		}
		
		return false;
	}
	
	inline void assign_edge(link_t& lhs, const link_t& rhs) {

	}
	
//=====================================================================
} // namespace detail
} // namespace walking
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

