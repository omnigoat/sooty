//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_DETAIL_PARSEME_BACKENDS_HPP
#define SOOTY_DETAIL_PARSEME_BACKENDS_HPP
//=====================================================================
#include <vector>
//=====================================================================
#include <boost/weak_ptr.hpp>
//=====================================================================
#include <sooty/parsing/detail/parseme_backend.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
namespace detail {
//=====================================================================
	
	struct parseme_backends_t
	{
		friend struct detail::parseme_backend;
		
	private:
		//detail::parseme_backend_ptr owner;
		parseme_backend_ptr owner;
		typedef std::vector<parseme_backend_ptr> internal_container_t;
		internal_container_t parseme_backends_;
		
	public:
		typedef internal_container_t::iterator iterator;
		typedef internal_container_t::const_iterator const_iterator;

		static parsemes_t empty;
		
		// constructor, don't require valid owner
		parseme_backends_t(const parseme_backend_ptr& owner = parseme_backend_ptr()) : owner(owner) {}
		
		const parseme_backend_ptr& back() const {
			return parseme_backends_.back();
		}
		
		iterator begin();
		iterator end();
		
		void push_back(const parseme_backend_ptr& n) {
			parseme_backends_.push_back(n);
			//n->set_parent(owner);
		}
		
		void clear();
		
		iterator insert(iterator where_, const_parseme_ref n);

		template <typename IT>
		iterator insert(iterator where_, IT begin, IT end)
		{
			// we need to set all parsemes to have this as a new parent
			int new_children = 0;
			for (IT i = begin; i != end; ++i) {
				begin->set_parent(parseme(owner));
				++new_children;
			}
			
			// move [where_, parsemes_.end()) to auxilary container, then put [begin, end)
			// on the end of parsemes_, then stick aux back on again
			internal_container_t aux;
			std::copy(where_, parseme_backends_.end(), std::back_inserter(aux));
			parseme_backends_.erase(where_, parseme_backends_.end());
			parseme_backends_.insert(parseme_backends_.end(), begin, end);
			// remember offset so we can calculate the iterator afterwards
			internal_container_t::size_type result_offset = parseme_backends_.size();
			parseme_backends_.insert(parseme_backends_.end(), aux.begin(), aux.end());
			return parseme_backends_.begin() + result_offset;
		}

		iterator erase(iterator where_);
		iterator erase(iterator from, iterator to);
	};
	
//=====================================================================
} // namespace detail
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

