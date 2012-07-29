//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_ACCUMULATOR_HPP
#define SOOTY_PARSING_ACCUMULATOR_HPP
//=====================================================================
#include <stack>
#include <list>
#include <boost/shared_ptr.hpp>
//=====================================================================
#include <sooty/parsing/parsemes.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
//=====================================================================
	
	struct accumulator 
	{
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
		
		void set_marker(size_t id) {
			markers[id] = parsemes.size();
		}
		
		void rm_marker(size_t id) {
			markers.erase(id);
		}
		
		void merge_into(size_t marker_id) {
			parseme parent = parsemes.back();
			parent.children().insert(
				parent.children().end(),
				parsemes.begin() + markers[marker_id],
				parsemes.end() - 1
			);
			parsemes.erase(parsemes.begin() + markers[marker_id], parsemes.end() - 1);
		}
		
	private:
		parsemes_t parsemes;
		std::map<size_t, size_t> markers;
	};
	
//=====================================================================
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

