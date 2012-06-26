//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_WALKING_DETAIL_REWRITER_WALKER_BACKEND_HPP
#define SOOTY_WALKING_DETAIL_REWRITER_WALKER_BACKEND_HPP
//=====================================================================
#include <sooty/walking/detail/context.hpp>
//=====================================================================
#include <sooty/transforming/transformer.hpp>
#include <sooty/transforming/transform.hpp>
//=====================================================================
namespace sooty {
namespace walking {
namespace detail {
//=====================================================================
	
	struct rewrite_walker_backend : abstract_walker_backend
	{
		int id;
		transforming::transformer t;
		
		rewrite_walker_backend(int id, const transforming::transformer& t) : id(id), t(t)
		{
		}
		
		bool compare_impl(context_ref context)
		{
			
			context_t::rewriter_positions_t::const_iterator i = context.rewriter_positions.find(id);
			if (i == context.rewriter_positions.end())
				return false;
			
			if (&context.container() != i->second.first)
				return false;
			
			context.erase(context.begin() + i->second.second, context.current() + 1);
			
			return transforming::transform(context, t);
		}
		
		abstract_walker_backend_ptr clone() {
			return abstract_walker_backend_ptr(new rewrite_walker_backend(*this));
		}
	};
	
//=====================================================================
} // namespace detail
} // namespace walking
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

