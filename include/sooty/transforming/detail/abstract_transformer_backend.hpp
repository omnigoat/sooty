//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_TRANSFORMING_DETAIL_ABSTRACT_TRANSFORMER_BACKEND_HPP
#define SOOTY_TRANSFORMING_DETAIL_ABSTRACT_TRANSFORMER_BACKEND_HPP	
//=====================================================================
#include <sooty/walking/detail/context.hpp>
//=====================================================================
namespace sooty {
namespace transforming {	
namespace detail {
//=====================================================================
	
	// the context is repurposed from the walking context
	typedef sooty::walking::detail::context_t context_t;
	typedef sooty::walking::detail::context_ref context_ref;
	
	// forward declares
	struct abstract_transformer_backend;
	typedef std::shared_ptr<abstract_transformer_backend> abstract_transformer_backend_ptr;
	
	typedef walking::detail::how_to_traverse how_to_traverse;
	
	// the link between nodes (it contains extra data - how_to_traverse)
	typedef std::pair<abstract_transformer_backend_ptr, how_to_traverse::Enum> link_t;
	
	struct abstract_transformer_backend
	{
		link_t on_success, on_failure, on_invalid;
		
		virtual abstract_transformer_backend_ptr clone() = 0;
		
		virtual bool perform(walking::detail::context_ref context) {
			return true;
		}
	};
	
	inline bool valid_edge(const link_t& e) {
		return e.first;
	}
	
	inline abstract_transformer_backend_ptr& node_of(link_t& e) {
		return e.first;
	}
	
	inline const abstract_transformer_backend_ptr& node_of(const link_t& e) {
		return e.first;
	}
	
	inline link_t new_edge_from(const abstract_transformer_backend_ptr& n, const link_t& e) {
		return link_t(n, e.second);
	}
	
	inline bool overwrote_edge(link_t& lhs, const link_t& rhs) {
		return false;
	}
	
	inline bool should_prepend(const abstract_transformer_backend_ptr&) {
		return false;
	}
	
	
	//=====================================================================
	// dummy transformer needs no great implementation
	//=====================================================================
	struct dummy_transformer_backend : abstract_transformer_backend {
		bool compare_impl(context_ref context) { return true; }
		abstract_transformer_backend_ptr clone() { return abstract_transformer_backend_ptr(new dummy_transformer_backend(*this)); }
	};
	
//=====================================================================
} // namespace detail
} // namespace transforming
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

