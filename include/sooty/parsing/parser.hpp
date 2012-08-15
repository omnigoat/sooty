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
	
		const detail::parser_backend_ptr& backend() const { return backend_; }
		
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
		
		parser operator [] (const parser& rhs) const
		{
			detail::mark_t mark = detail::generate_mark();
			
			return parser (
				detail::parser_backend_t::make()
					->push_back_command(detail::command_t::make_add_marker(mark))
					->append(common::clone_tree(rhs.backend_))
					->append(common::clone_tree(backend_))
					->append(detail::parser_backend_t::make()
						->push_back_command(detail::command_t(detail::command_t::combine, 0, 0, mark))
					)
			);
		}
	
	private:
		detail::parser_backend_ptr backend_;
	};
	
	inline parser match(size_t id) {
		using detail::command_t;
		
		return parser(
			detail::parser_backend_t::make()
				->push_back_command( command_t(command_t::match, id, id, id) )
		);
	}
	
	inline parser insert(size_t id) {
		return parser(
			detail::parser_backend_t::make()
				->push_back_command(detail::command_t::make_insert(id))
		);
	}
	
	
	
//=====================================================================
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

