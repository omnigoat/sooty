//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_DETAIL_ACCUMULATOR_HPP
#define SOOTY_LEXING_DETAIL_ACCUMULATOR_HPP
//=====================================================================
#include <vector>
//=====================================================================
#include <sooty/lexing/lexemes.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
namespace detail {
//=====================================================================
	
	struct accumulator_t
	{
		accumulator_t(lexemes_ref lexemes)
			: lexemes_(lexemes)
		{
		}
		
	private:
		lexemes_ref lexemes_;
	};
	
	typedef const accumulator_t& const_accumulator_ref;
	typedef accumulator_t& accumulator_ref;
	
//=====================================================================
} // namespace detail
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

