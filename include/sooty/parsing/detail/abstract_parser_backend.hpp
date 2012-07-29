//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_ABSTRACT_PARSER_BACKEND_HPP
#define SOOTY_PARSING_ABSTRACT_PARSER_BACKEND_HPP
//=====================================================================
#include <boost/shared_ptr.hpp>
//=====================================================================
#include <sooty/lexing/lexemes.hpp>
#include <sooty/parsing/accumulator.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
namespace detail {
//=====================================================================
	
	struct abstract_parser_backend;
	typedef boost::shared_ptr<abstract_parser_backend> abstract_parser_backend_ptr;

	struct abstract_parser_backend
	{
		virtual abstract_parser_backend_ptr clone() = 0;
		
		virtual bool applies(accumulator&, lexing::lexemes::const_iterator&, lexing::lexemes::const_iterator&) = 0;
	};
	
	namespace parsers {
		
		// handles all matching/inserting
		struct match : abstract_parser_backend
		{
			size_t match_from, match_to, insert_id;
			bool match, insert;
			
			bool applies(accumulator& acc, lexing::lexemes::const_iterator& begin, lexing::lexemes::const_iterator& end) {
				if (match && begin == end || (*begin < match_from) || (match_to < *begin))
					return false;
				
				if (insert)
					acc.insert(insert_id, match ? &*begin : NULL);
				++begin;
				return true;
			}
		};
		
		
		// subparser
		struct subparser : abstract_parser_backend {
			bool applies(accumulator& acc, lexing::lexemes::const_iterator& begin, lexing::lexemes::const_iterator& end) {
				
			}
		};
		
		struct set_marker : abstract_parser_backend {
			size_t mark_id;
			
			bool applies(accumulator& acc, lexing::lexemes::const_iterator&, lexing::lexemes::const_iterator&) {
				acc.set_marker(mark_id);
				return true;
			}
		};
		
		struct merge : abstract_parser_backend {
			size_t from_mark, to_mark;
			
			bool applies(accumulator& acc, lexing::lexemes::const_iterator&, lexing::lexemes::const_iterator&) {
				acc.merge_into(from_mark);
				return true;
			}
		};
		
	} // namespace parsers
	
//=====================================================================
} // namespace detail
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

