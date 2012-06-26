//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_WALKING_DETAIL_POSITION_SETTER_WALKER_BACKEND_HPP
#define SOOTY_WALKING_DETAIL_POSITION_SETTER_WALKER_BACKEND_HPP
//=====================================================================
#include <sooty/walking/detail/context.hpp>
//=====================================================================
namespace sooty {
namespace walking {
namespace detail {
//=====================================================================
	
	struct position_setter_walker_backend : abstract_walker_backend
	{
		int id;
		
		position_setter_walker_backend(int id) : id(id) {
		}
		
		bool compare_impl(context_ref context) {
			context.add_rewriter(id);
			return true;
		}
		
		abstract_walker_backend_ptr clone() {
			return abstract_walker_backend_ptr(new position_setter_walker_backend(*this));
		}
	};
	
//=====================================================================
} // namespace detail
} // namespace walking
} // namespace sooty
//=====================================================================
#endif
//=====================================================================


