#include <sooty/parsing/detail/parser_backend.hpp>
//=====================================================================

bool sooty::parsing::detail::equivalent(const parser_backend_ptr& lhs, const parser_backend_ptr& rhs) {
	return false;
	return lhs->type_ == rhs->type_ && lhs->type_ == parser_backend::type::matcher &&
		lhs->match_from == rhs->match_from &&
		lhs->match_to == rhs->match_to &&
		lhs->to_insert == rhs->to_insert
		;
}

bool sooty::parsing::detail::partially_equivalent(const parser_backend_ptr& lhs, const parser_backend_ptr& rhs) {
	return false;
	return lhs->type_ == rhs->type_ &&
		lhs->match_from >= rhs->match_from &&
		lhs->match_to <= rhs->match_to &&
		lhs->to_insert == rhs->to_insert
		;
}

bool sooty::parsing::detail::should_prepend(const parser_backend_ptr& P) {
	return false;
}