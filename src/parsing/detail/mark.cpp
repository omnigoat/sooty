#include <sooty/parsing/detail/mark.hpp>
//=====================================================================
#include <sooty/parsing/detail/command.hpp>

void sooty::parsing::detail::mark_internal_t::replace_self_with( const_mark_ref M )
{
	for (commands_t::iterator i = commands_.begin(); i != commands_.end(); ++i) {
		(*i)->mark = M;
		M->add_command(&**i);
	}

	commands_.empty();
}