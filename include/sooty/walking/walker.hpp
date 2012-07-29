//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_WALKING_WALKER_HPP
#define SOOTY_WALKING_WALKER_HPP
//=====================================================================
#include <sooty/walking/detail/id_eq_walker_backend.hpp>
#include <sooty/walking/detail/start_walker_backend.hpp>
#include <sooty/walking/detail/dummy_walker_backend.hpp>
//=====================================================================
// forward declare
namespace sooty { namespace transforming { struct transformer; } }
//=====================================================================
namespace sooty {
namespace walking {
//=====================================================================
	
	struct walker
	{
		struct rewriter {
			walker operator [](const transforming::transformer& rhs) const;
			rewriter(const detail::abstract_walker_backend_ptr& backend) : backend(backend) {}
		private:
			detail::abstract_walker_backend_ptr backend;
		};
		
		typedef sooty::parsing::parseme::id_t id_t;
		detail::abstract_walker_backend_ptr backend;
		
		rewriter rewrite;
		walker on_match();
		
		walker(const detail::abstract_walker_backend_ptr& backend)
			: backend(backend), rewrite(backend)
		{
		}
		
		walker operator [] (const walker& rhs) const;
		walker operator *  () const;
		walker operator +  () const;
	};
	
	
	walker operator >> (const walker& lhs, const walker& rhs);
	walker operator , (const walker& lhs, const walker& rhs);
	
	inline walker eq(parsing::parseme::id_t match_from, parsing::parseme::id_t match_to) {
		return detail::abstract_walker_backend_ptr(new detail::id_eq_walker_backend(match_from, match_to));
	}

	walker eq(parsing::parseme::id_t id);
	walker fuzzy_eq(parsing::parseme::id_t id);

	inline walker start() {
		return walker( detail::abstract_walker_backend_ptr(new detail::start_walker_backend) );
	}
	
	inline walker dummy() {
		return walker(detail::abstract_walker_backend_ptr(new detail::dummy_walker_backend));
		//return _;
	}
	
	
//=====================================================================
} // namespace walking
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

