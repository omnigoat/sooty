//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_PARSER_HPP
#define SOOTY_PARSING_PARSER_HPP
//=====================================================================
#include <sooty/common/clone.hpp>
#include <sooty/common/detail/fold.hpp>
//=====================================================================
#include <sooty/lexing/lexemes.hpp>
//=====================================================================
#include <sooty/parsing/parsemes.hpp>
#include <sooty/parsing/detail/command.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
//=====================================================================
	
	
	struct parser
	{
		/*parser()
			: backend_()
		{
		}
		*/
		parser(const detail::parser_backend_ptr& backend) : backend_(backend) {}
	
		parser operator >> (const parser& rhs) const
		{
			return parser(
				common::clone_tree(backend_)->append( common::clone_tree(rhs.backend_) )
			);
		}
		
		parser operator | (const parser& rhs) const {
			return parser (
				common::clone_tree(backend_)->merge( common::clone_tree(rhs.backend_) )
			);
		}
	
	private:
		detail::parser_backend_ptr backend_;
	};
	
	inline parser match(size_t id) {
		accumulator::mark_t mark = accumulator::generate_marker();
		using detail::command_t;
		
		return parser(
			detail::parser_backend_t::make()
				->push_back_command( command_t::make_add_marker(mark) )
				->push_back_failure( command_t::make_rm_marker(mark) )
				->push_back_command( command_t::make_match(id, id) )
				->push_back_command( command_t::make_insert(0, mark) )
				->push_back_command( command_t::make_rm_marker(mark) )
		);
	}
	
	
	
//=====================================================================
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

