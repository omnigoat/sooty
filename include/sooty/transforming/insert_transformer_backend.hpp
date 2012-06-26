//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_TRANSFORMING_DETAIL_INSERT_TRANSFORMER_BACKEND_HPP
#define SOOTY_TRANSFORMING_DETAIL_INSERT_TRANSFORMER_BACKEND_HPP
//=====================================================================
#include <sooty/transforming/detail/abstract_transformer_backend.hpp>
//=====================================================================
namespace sooty {
namespace transforming {	
namespace detail {
//=====================================================================
	
	struct insert_transformer_backend : abstract_transformer_backend
	{
		int id;
		
		insert_transformer_backend(int id) : id(id)
		{
		}

		bool perform(context_ref context)
		{
			context.insert(context.stored_variables[id].front());
			return true;
		}
		
		abstract_transformer_backend_ptr clone() {
			return abstract_transformer_backend_ptr(new insert_transformer_backend(*this));
		}
	};
	
	inline abstract_transformer_backend_ptr insert_backend(int i) {
		return abstract_transformer_backend_ptr(new insert_transformer_backend(i));
	}
	
//=====================================================================
} // namespace detail
} // namespace transforming
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

