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
#include <boost/shared_ptr.hpp>
//=====================================================================
#include <sooty/lexing/lexemes.hpp>
//=====================================================================
#include <sooty/parsing/parsemes.hpp>
#include <sooty/parsing/parseme.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
//=====================================================================
	
	struct accumulator 
	{
		typedef boost::shared_ptr<size_t> mark_t;
		typedef const mark_t& const_mark_ref;
		
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
		
		void add_marker(const mark_t& id) {
			markers[*id].first.push(parsemes.size());
		}
		
		void add_marker(const mark_t& id, const lexing::lexemes_t::const_iterator& iter) {
			markers[*id].second = iter;
			markers[*id].first.push(parsemes.size());
		}
		
		void rm_marker(const mark_t& id) {
			markers_t::iterator m = markers.find(*id);
			if (m != markers.end())
				m->second.first.pop();
			if (m->second.first.empty())
				markers.erase(*id);
		}
		
		void unmerge(const mark_t& id) {
			parsemes_t insertables;
			parsemes_t::iterator i = parsemes.end() - 1;
			for ( ; i != parsemes.begin() + markers[*id].first.top() - 1; --i) {
				parsemes_t& children = i->children();
				if (children.size() > 0) {
					insertables.insert(insertables.begin(), children.begin(), children.end());
				}
				else
					break;
			}
		}
		
		void delete_at(const mark_t& id) {
			parsemes.erase(parsemes.begin() + markers[*id].first.top());
		}
		
		void delete_all_at(const mark_t& id) {
			parsemes.erase(parsemes.begin() + markers[*id].first.top(), parsemes.end());
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
			assert(parsemes.back().children().size() > 0);
			
			parsemes.insert(parsemes.end() - 1,
				parsemes.back().children().begin(),
				parsemes.back().children().end()
			);
			parsemes.erase(parsemes.end() - 1);
		}
		
		parseme at_marker(const mark_t& id) {
			return *(parsemes.begin() + markers[*id].first.top());
		}
		
		lexing::lexemes_t::const_iterator iter_marker(const mark_t& id) {
			return markers[*id].second;
		}
		
		void merge_into(const mark_t& id) {
			parseme parent = parsemes.back();
			parent.children().insert(
				parent.children().end(),
				parsemes.begin() + markers[*id].first.top(),
				parsemes.end() - 1
			);
			parsemes.erase(parsemes.begin() + markers[*id].first.top(), parsemes.end() - 1);
		}
		
		static mark_t generate_marker() {
			static size_t _ = 0;
			return mark_t(new size_t(++_));
		}
		
		static mark_t mapped_marker(const mark_t& m) {
			static std::map<size_t, mark_t> _;
			if (_.find(*m) == _.end())
				_[*m] = accumulator::generate_marker();
			
			return _[*m];
		}
		
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

