#include <sooty/parsing/detail/mark.hpp>
//=====================================================================
#include <sooty/parsing/detail/command.hpp>
//=====================================================================
using sooty::parsing::detail::mark_t;
using sooty::parsing::detail::mark_internal_t;
using sooty::parsing::detail::const_mark_ref;
using sooty::parsing::detail::command_t;

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
	std::vector<command_t*> commands(commands_.begin(), commands_.end());
	
	// without this pointer, it's possible we'd destruct ourselves in the 
	// middle of the following operation.
	mark_t strong_ref = shared_from_this();

	for (std::vector<command_t*>::iterator i = commands.begin(); i != commands.end(); ++i) {
		(*i)->mark = M;
		M->add_command(*i);
	}
}