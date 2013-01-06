#include <sooty/parsing/detail/mark.hpp>
//=====================================================================
#include <sooty/parsing/detail/command.hpp>
//=====================================================================
using sooty::parsing::detail::mark_internal_t;
using sooty::parsing::detail::const_mark_ref;


mark_internal_t::mark_internal_t(size_t id)
: id(id)
{
}

auto mark_internal_t::add_command(command_t* const c) -> void {
	commands_.insert(c);
}

auto mark_internal_t::remove_command(command_t* const c) -> void {
	commands_.erase(c);
}

auto mark_internal_t::replace_self_with( const_mark_ref M ) -> void {
	for (commands_t::iterator i = commands_.begin(); i != commands_.end(); ++i) {
		(*i)->mark = M;
		M->add_command(&**i);
	}

	commands_.empty();
}