#include <sooty/parsing/parser.hpp>

using sooty::parsing::parser;
using sooty::parsing::detail::parser_backend_ptr;

parser::parser()
: backend_(new detail::parser_backend_t), assigned_(false)
{
}

parser::parser(const parser_backend_ptr& backend)
: backend_(backend), assigned_(true)
{
}

auto parser::backend() const -> parser_backend_ptr const& {
	return backend_;
}

auto parser::operator >> (parser const& rhs) const -> parser {
	return parser(
		common::clone_tree(backend_)->append( common::clone_tree(rhs.backend_) )
	);
}
		
auto parser::operator | (parser const& rhs) const -> parser {
	return parser (
		common::clone_tree(backend_)->merge( common::clone_tree(rhs.backend_) )
	);
}

auto parser::operator [] (const parser& rhs) const -> parser
{
	detail::mark_t mark = detail::generate_mark();
			
	return parser (
		detail::parser_backend_t::make()
			->push_back_command(detail::command_t::make_add_marker(mark))
			->push_back_failure(detail::command_t::make_rm_marker(mark))
			->append(common::clone_tree(rhs.backend_))
			->append(common::clone_tree(backend_))
			->append(detail::parser_backend_t::make()
				->push_back_command(detail::command_t(detail::command_t::combine, 0, 0, mark))
			)
	);
}

auto parser::operator = (parser const& rhs) -> parser&
{
	*backend_ = *rhs.backend();
	return *this;
}



