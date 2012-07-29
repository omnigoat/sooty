//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_ACCUMULATOR_HPP
#define SOOTY_PARSING_ACCUMULATOR_HPP
//=====================================================================
#include <map>
#include <vector>
#include <stack>
#include <boost/shared_ptr.hpp>
//=====================================================================
#include <sooty/parsing/parsemes.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
//=====================================================================
	
	struct accumulator 
	{
		typedef boost::shared_ptr<size_t> mark_t;
		
		struct frame {
			parseme parent;
			parsemes_t* container;
			
			frame(const_parseme_ref parent, parsemes_t* container)
				: parent(parent), container(container)
			{
			}
		};
		
		void insert(parseme_ref p) {
			parsemes.push_back(p);
		}
		
		void insert(size_t id, const lexing::lexeme_t* lexeme = NULL) {
			parsemes.push_back(
				parseme(parseme(), id, lexeme)
			);
		}
		
		void insert_all(parsemes_t& these) {
			parsemes.insert(
				parsemes.end(),
				these.begin(),
				these.end()
			);
		}
		
		const parsemes_t& container() const {
			return parsemes;
		}
		
		void add_marker(const mark_t& id) {
			markers[id].push(parsemes.size());
		}
		
		void rm_marker(const mark_t& id) {
			markers_t::iterator m = markers.find(id);
			m->second.pop();
			if (m->second.empty())
				markers.erase(id);
		}
		
		void delete_at(const mark_t& id) {
			parsemes.erase(parsemes.begin() + markers[id].top());
		}
		
		parseme at_marker(const mark_t& id) {
			return *(parsemes.begin() + markers[id].top());
		}
		
		void merge_into(const mark_t& marker_id) {
			parseme parent = parsemes.back();
			parent.children().insert(
				parent.children().end(),
				parsemes.begin() + markers[marker_id].top(),
				parsemes.end() - 1
			);
			parsemes.erase(parsemes.begin() + markers[marker_id].top(), parsemes.end() - 1);
		}
		
		static mark_t generate_marker() {
			static size_t _ = 0;
			return mark_t(new size_t(++_));
		}
		
	private:
		parsemes_t parsemes;
		typedef std::map<mark_t, std::stack<size_t> > markers_t;
		markers_t markers;
	};
	
//=====================================================================
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

