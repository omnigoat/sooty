//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_WALKING_DETAIL_ID_EQ_WALKER_BACKEND_HPP
#define SOOTY_WALKING_DETAIL_ID_EQ_WALKER_BACKEND_HPP
//=====================================================================
#include <sooty/walking/detail/context.hpp>
#include <sooty/walking/detail/abstract_walker_backend.hpp>
//=====================================================================
namespace sooty {
namespace walking {
namespace detail {
//=====================================================================
	
	struct id_eq_walker_backend : abstract_walker_backend
	{
		parsing::parseme::id_t match_from, match_to;
		
		id_eq_walker_backend(parsing::parseme::id_t match_from, parsing::parseme::id_t match_to)
			: match_from(match_from), match_to(match_to)
		{
		}
		
		bool compare_impl(context_ref context) {
			return match_from <= context.current()->id() && context.current()->id() <= match_to;
		}
		
		abstract_walker_backend_ptr clone() {
			return abstract_walker_backend_ptr(new id_eq_walker_backend(*this));
		}
	};
	
//=====================================================================
} // namespace detail
} // namespace walking
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

