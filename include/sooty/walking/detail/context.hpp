//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_WALKING_DETAIL_CONTEXT_HPP
#define SOOTY_WALKING_DETAIL_CONTEXT_HPP
//=====================================================================
#include <map>
#include <stack>
//=====================================================================
#include <sooty/parsing/parseme.hpp>
#include <sooty/parsing/parsemes.hpp>
//=====================================================================
namespace sooty {
namespace walking {
namespace detail {
//=====================================================================
	
	// information for travelling between nodes
	struct how_to_traverse {
		enum Enum {
			stay,
			dive,
			surface,
			next_sibling,
			prev_sibling
		};
	};
	
	
	
	struct context_t
	{
		struct frame {
			parsing::parsemes_t* parsemes;
			parsing::parsemes_t::iterator current;
			
			frame(parsing::parsemes_t& parsemes, parsing::parsemes_t::iterator current)
				: parsemes(&parsemes), current(current)
			{
			}
		};
		
		std::stack<frame> frames;
		
		typedef std::map<int, std::pair<parsing::parsemes_t*, int> > rewriter_positions_t;
		mutable rewriter_positions_t rewriter_positions;
		
		typedef std::map<int, parsing::parsemes_t> stored_variables_t;
		stored_variables_t stored_variables;
		
		context_t(parsing::parsemes_t& container) {
			frames.push( frame(container, container.begin()) );
		}
		
		void dive() {
			assert( !invalid() );
			parsing::parsemes_t& nf = container().back().children();
			frames.push( frame(nf, nf.begin()) );
		}
		
		void surface() {
			assert( !frames.empty() );
			frames.pop();
		}
		
		void next() {
			assert( !frames.empty() );
			++frames.top().current;
		}
		
		void traverse(how_to_traverse::Enum how)
		{
			switch (how)
			{
				case how_to_traverse::dive:
					dive();
					break;
					
				case how_to_traverse::surface:
					surface();
					break;
					
				case how_to_traverse::next_sibling:
					next();
					break;
			}
		}
		
		
		
		bool invalid() const {
			return current() == end();
		}
		
		void add_rewriter(int id) {
			std::size_t distance = std::distance(begin(), current());
			rewriter_positions[id] = std::make_pair(&container(), distance);
		}
		
		parsing::parsemes_t::const_iterator begin() const { assert(!frames.empty()); return frames.top().parsemes->begin(); }
		parsing::parsemes_t::const_iterator current() const { assert(!frames.empty()); return frames.top().current; }
		parsing::parsemes_t::const_iterator end() const { assert(!frames.empty()); return frames.top().parsemes->end(); }
		
		parsing::parsemes_t::iterator begin() { assert(!frames.empty()); return frames.top().parsemes->begin(); }
		parsing::parsemes_t::iterator current() { assert(!frames.empty()); return frames.top().current; }
		parsing::parsemes_t::iterator end() { assert(!frames.empty()); return frames.top().parsemes->end(); }
		
		const parsing::parsemes_t& container() const {
			assert(!frames.empty());
			return *frames.top().parsemes;
		}
		
		parsing::parsemes_t& container() {
			assert(!frames.empty());
			return *frames.top().parsemes;
		}
		
		void erase(parsing::parsemes_t::iterator from, parsing::parsemes_t::iterator to) {
			frames.top().current = container().erase(from, to);
		}
		
		void insert(parsing::parseme p) {
			frames.top().current = container().insert(current(), p);
		}
		
		void insert(parsing::parsemes_t::iterator begin, parsing::parsemes_t::iterator end) {
			frames.top().current = container().insert(current(), begin, end);
		}
	};
	
	typedef context_t& context_ref;
	
//=====================================================================
} // namespace detail
} // namespace walking
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

