//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_TRANSFORMING_TRANSFORMER_HPP
#define SOOTY_TRANSFORMING_TRANSFORMER_HPP
//=====================================================================
#include <sooty/transforming/detail/abstract_transformer_backend.hpp>
#include <sooty/transforming/detail/mk_transformer_backend.hpp>
#include <sooty/transforming/insert_transformer_backend.hpp>
//=====================================================================
#include <sooty/walking/placeholder.hpp>
//=====================================================================
namespace sooty {
namespace transforming {
//=====================================================================
	
	struct transformer
	{
		typedef sooty::parsing::parseme::id_t id_t;
		detail::abstract_transformer_backend_ptr backend;
		
		transformer(const detail::abstract_transformer_backend_ptr& backend)
			: backend(backend)
		{
		}
		
		transformer operator [] ( const transformer& rhs ) const;
		transformer operator ,  (const transformer& rhs) const;
		
		
		static int guid() {
			static int i = 0;
			return i++;
		}
	};
	
	//=====================================================================
	// operators
	//=====================================================================
	template <int I>
	transformer operator , (const transformer& lhs, const walking::placeholder<I>& rhs) {
		detail::abstract_transformer_backend_ptr new_lhs = sooty::common::detail::clone_tree(lhs.backend);
		sooty::common::detail::append_success(new_lhs, detail::link_t(detail::insert_backend(I), detail::how_to_traverse::next_sibling));
		return transformer(new_lhs);
	}
	
	
	
	inline transformer mk(parsing::parseme::id_t id) {
		return transformer( detail::abstract_transformer_backend_ptr(new detail::mk_transformer_backend(id)) );
	}
	
	inline transformer dummy() {
		return transformer( detail::abstract_transformer_backend_ptr(new detail::dummy_transformer_backend) );
	}

//=====================================================================
} // namespace transforming
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

