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

