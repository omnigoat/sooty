//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_WALKING_DETAIL_START_WALKER_BACKEND_HPP
#define SOOTY_WALKING_DETAIL_START_WALKER_BACKEND_HPP
//=====================================================================
#include <sooty/walking/detail/context.hpp>
//=====================================================================
namespace sooty {
namespace walking {
namespace detail {
//=====================================================================
	
	struct start_walker_backend : abstract_walker_backend
	{
		bool compare_impl(context_ref context) {
			return context.current() == context.begin();
		}
		
		abstract_walker_backend_ptr clone() {
			return abstract_walker_backend_ptr(new start_walker_backend);
		}
	};
	
//=====================================================================
} // namespace detail
} // namespace walking
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

