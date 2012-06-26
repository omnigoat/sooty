//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_TRANSFORMING_DETAIL_MK_TRANSFORMER_BACKEND_HPP
#define SOOTY_TRANSFORMING_DETAIL_MK_TRANSFORMER_BACKEND_HPP
//=====================================================================
#include <sooty/transforming/detail/abstract_transformer_backend.hpp>
//=====================================================================
namespace sooty {
namespace transforming {	
namespace detail {
//=====================================================================
	
	struct mk_transformer_backend : abstract_transformer_backend
	{
		int id;
		
		mk_transformer_backend(int id) : id(id)
		{
		}

		bool perform(context_ref context)
		{
			context.insert(parsing::parseme(id));
			return true;
		}
		
		abstract_transformer_backend_ptr clone() {
			return abstract_transformer_backend_ptr(new mk_transformer_backend(*this));
		}
	};
	
//=====================================================================
} // namespace detail
} // namespace transforming
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

