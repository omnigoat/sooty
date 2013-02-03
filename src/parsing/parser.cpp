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

namespace sooty { namespace parsing {
	unsigned int insert_count(parser_backend_ptr const& head)
	{
		unsigned int result = 0;

		std::stack<parser_backend_ptr> nodes;
		nodes.push(head);
		while (!nodes.empty()) {
			parser_backend_ptr x = nodes.top();
			nodes.pop();
			if (x->commands_.empty())
				++result;
			else if (x->commands_.front().second.action != detail::command_t::action_t::combine &&
			  x->commands_.front().second.insert_id != 0)
				++result;
			
			for (auto const& y : x->children_) {
				nodes.push(y);
			}
		}

		return result;
	}
} }

auto parser::operator [] (const parser& rhs) const -> parser
{
	//detail::mark_t mark = detail::generate_mark();
	unsigned int inserts = insert_count(rhs.backend_);

	return parser (
		detail::parser_backend_t::make()
			->append(common::clone_tree(rhs.backend_))
			->append(common::clone_tree(backend_))

			// count how many inserts happen in rhs, and reduce by that many
			->append(detail::parser_backend_t::make()
				->push_back_command(detail::command_t(detail::command_t::action_t::combine, 0, 0, inserts))
			)
	);
}

auto parser::operator = (parser const& rhs) -> parser&
{
	// yay!
	//backend_ = common::remove_left_recursion(rhs, shared_from_this());

	return *this;
}



