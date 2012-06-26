//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_WALKING_DETAIL_DUMMY_WALKER_BACKEND_HPP
#define SOOTY_WALKING_DETAIL_DUMMY_WALKER_BACKEND_HPP
//=====================================================================
#include <sooty/walking/detail/context.hpp>
#include <sooty/walking/detail/abstract_walker_backend.hpp>
//=====================================================================
namespace sooty {
namespace walking {
namespace detail {
//=====================================================================
	
	struct dummy_walker_backend : abstract_walker_backend
	{
		bool compare_impl(context_ref context) {
			return true;
		}
		
		abstract_walker_backend_ptr clone() {
			return abstract_walker_backend_ptr(new dummy_walker_backend(*this));
		}
		
		bool is_dummy() const { return true; }
	};
	
//=====================================================================
} // namespace detail
} // namespace walking
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

