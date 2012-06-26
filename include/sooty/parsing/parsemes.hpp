//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_PARSEMES_HPP
#define SOOTY_PARSING_PARSEMES_HPP
//=====================================================================
#include <vector>
//=====================================================================
#include <boost/mpl/bool.hpp>
#include <boost/weak_ptr.hpp>
//=====================================================================
#include <sooty/parsing/parseme.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
//=====================================================================
	
	struct parsemes_t
	{
		friend struct detail::parseme_backend;
		friend struct parseme;
		
	private:
		detail::parseme_backend_ptr owner;
		typedef std::vector<parseme> internal_container_t;
		internal_container_t parsemes_;
		
	public:
		typedef internal_container_t::iterator iterator;
		typedef internal_container_t::const_iterator const_iterator;

		static parsemes_t empty;
		
		parsemes_t();
		
		parseme_ref front();
		parseme_ref back();
		
		iterator begin();
		iterator end();
		
		void push_back(parseme_ref n);
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
			std::copy(where_, parsemes_.end(), std::back_inserter(aux));
			parsemes_.erase(where_, parsemes_.end());
			parsemes_.insert(parsemes_.end(), begin, end);
			// remember offset so we can calculate the iterator afterwards
			internal_container_t::size_type result_offset = parsemes_.size();
			parsemes_.insert(parsemes_.end(), aux.begin(), aux.end());
			return parsemes_.begin() + result_offset;
		}

		iterator erase(iterator where_);
		iterator erase(iterator from, iterator to);
	};
	
//=====================================================================
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

