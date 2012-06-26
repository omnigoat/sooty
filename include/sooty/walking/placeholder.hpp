//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_WALKING_PLACEHOLDER_HPP
#define SOOTY_WALKING_PLACEHOLDER_HPP
//=====================================================================
#include <sooty/walking/detail/abstract_walker_backend.hpp>
#include <sooty/walking/walker.hpp>
//=====================================================================
namespace sooty {
namespace walking {
//=====================================================================
		

	template <int I>
	struct var_assign : sooty::walking::detail::abstract_walker_backend
	{
		bool compare_impl(walking::detail::context_ref context) {
			context.stored_variables[I].clear();
			context.stored_variables[I].push_back(*context.current());
			return true;
		}

		sooty::walking::detail::abstract_walker_backend_ptr clone() {
			return sooty::walking::detail::abstract_walker_backend_ptr(new var_assign(*this));
		}
	};


	template <int I>
	struct placeholder {
		
		placeholder() : recurse(true)
		{
		}
		
		walking::walker operator = (const walking::walker& rhs) {
			walking::detail::abstract_walker_backend_ptr new_rhs = common::detail::clone_tree(rhs.backend);
			
			common::detail::append_success(
				new_rhs, 
				walking::detail::link_t(
					walking::detail::abstract_walker_backend_ptr(new var_assign<I>),
					walking::detail::how_to_traverse::stay
				)
			);
			
			return walking::walker(new_rhs);
		}
		
	private:
		bool recurse;
	};
	
//=====================================================================
} // namespace walking
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

