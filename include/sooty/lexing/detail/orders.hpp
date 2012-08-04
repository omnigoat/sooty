//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_DETAIL_ORDERS_HPP
#define SOOTY_LEXING_DETAIL_ORDERS_HPP
//=====================================================================
#include <set>
#include <map>
//=====================================================================
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
//=====================================================================
#include <sooty/common/node.hpp>
#include <sooty/lexing/detail/accumulator.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
namespace detail {
//=====================================================================
	
	struct orders_t
	{
		typedef std::istreambuf_iterator<char> iterator_t;
		accumulator_t accumulator;
		
		iterator_t begin;
		iterator_t end;
		
		orders_t(lexemes_ref lexemes, const iterator_t& begin, const iterator_t& end)
			: accumulator(lexemes), begin(begin), end(end)
		{
		}
	};
	
	
//=====================================================================
} // namespace detail
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

