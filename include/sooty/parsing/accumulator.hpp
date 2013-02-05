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
//=====================================================================
#include <sooty/lexing/lexemes.hpp>
//=====================================================================
#include <sooty/parsing/parsemes.hpp>
#include <sooty/parsing/parseme.hpp>
#include <sooty/parsing/detail/mark.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
//=====================================================================
	
	struct accumulator 
	{
		typedef detail::mark_t mark_t;
		typedef detail::const_mark_ref const_mark_ref;
		
		struct frame {
			parseme parent;
			parsemes_t* container;
			
			frame(const_parseme_ref parent, parsemes_t* container)
				: parent(parent), container(container)
			{
			}
		};
		
		accumulator() {
			//parseme_stack.push( frame(parseme(), &parsemes) );
		}
		
		void insert(parseme_ref p) {
			parsemes.push_back(p);
			//frames.top().container.push_back(p);
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
		
		void add_marker(const mark_t& mark) {
			markers[mark->id].first.push(parsemes.size());
		}
		
		void add_marker(const mark_t& mark, const lexing::lexemes_t::const_iterator& iter) {
			markers[mark->id].second = iter;
			markers[mark->id].first.push(parsemes.size());
		}
		
		void rm_marker(const mark_t& mark) {
			markers_t::iterator m = markers.find(mark->id);
			if (m != markers.end())
				m->second.first.pop();
			if (m->second.first.empty())
				markers.erase(mark->id);
		}
		
		void unmerge(const mark_t& mark) {
			parsemes_t insertables;
			parsemes_t::iterator i = parsemes.end() - 1;
			for ( ; i != parsemes.begin() + markers[mark->id].first.top() - 1; --i) {
				parsemes_t& children = i->children();
				if (children.size() > 0) {
					insertables.insert(insertables.begin(), children.begin(), children.end());
				}
				else
					break;
			}
		}
		
		void delete_at(const mark_t& mark) {
			parsemes.erase(parsemes.begin() + markers[mark->id].first.top());
		}
		
		void delete_all_at(const mark_t& mark) {
			parsemes.erase(parsemes.begin() + markers[mark->id].first.top(), parsemes.end());
		}
		
		void pop() {
			parsemes.pop_back();
		}
		
		parseme back() {
			return parsemes.back();
		}
		
		void promote_children()
		{
			if (parsemes.size() == 0)
				return;
			ATMA_ASSERT(parsemes.back().children().size() > 0);
			
			parsemes.insert(parsemes.end() - 1,
				parsemes.back().children().begin(),
				parsemes.back().children().end()
			);
			parsemes.erase(parsemes.end() - 1);
		}
		
		parseme at_marker(const mark_t& mark) {
			return *(parsemes.begin() + markers[mark->id].first.top());
		}
		
		lexing::lexemes_t::const_iterator iter_marker(const mark_t& mark) {
			return markers[mark->id].second;
		}
		
		void merge_into(unsigned int count)
		{
			ATMA_ASSERT(parsemes.size() >= 1 + count);

			parseme parent = parsemes.back();
			parent.children().insert(
				parent.children().end(),
				parsemes.end() - 1 - count,
				parsemes.end() - 1
			);

			parsemes.erase(parsemes.end() - 1 - count, parsemes.end() - 1);
		}
		
		/*static mark_t combined_mark(const mark_t& lhs, const mark_t& rhs) {
			typedef std::map< std::pair<mark_t, mark_t>, mark_t > map_t;
			static map_t _;
			
			map_t::iterator i;
			if ((i = _.find(std::make_pair(lhs, rhs))) == _.end() &&
			    (i = _.find(std::make_pair(rhs, lhs))) == _.end()
			   )
				return _[std::make_pair(lhs, rhs)] = accumulator::generate_marker();
			else
				return i->second;
		}*/
		
	private:
		parsemes_t parsemes;
		typedef std::map<size_t, std::pair<std::stack<size_t>, lexing::lexemes_t::const_iterator> > markers_t;
		markers_t markers;
	};
	
	typedef accumulator& accumulator_ref;
	
//=====================================================================
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

