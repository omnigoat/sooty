//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_COMMON_PERFORMER_HPP
#define SOOTY_COMMON_PERFORMER_HPP
//=====================================================================
#include <boost/bind.hpp>
//=====================================================================
namespace sooty {
namespace common {
//=====================================================================
	
	template <typename ExecutorT>
	struct performer_t
	{
		typedef ExecutorT executor_t;
		
		template <typename StateT, typename InputT, typename NodeT>
		bool operator()(StateT& state, InputT& input, const NodeT& node) {
			return perform_node(state, input, node);
		}
	
	private:
		template <typename StateT, typename InputT, typename NodePTR>
		bool perform_node(StateT& state, InputT& input, const NodePTR& node)
		{
			typedef NodePTR::value_type node_t;
		
			typedef StateT state_t;
			typedef state_t& state_ref;
			
			typedef InputT input_t;
			typedef input_t& input_ref;
			
			typedef typename node_t::node_ptr node_ptr;
			typedef typename node_t::const_node_ptr_ref const_node_ptr_ref;
			typedef typename node_t::command_t command_t;
			typedef typename node_t::commands_t commands_t;
			typedef typename node_t::const_commands_ref const_commands_ref;
			typedef typename node_t::children_t children_t;
			typedef typename node_t::const_children_ref const_children_ref;
			
			
			
			node_ptr current_node = node;
			
			while (current_node)
			{
				const_commands_ref commands = current_node->commands_;
				const_children_ref children = current_node->children_;
				
				// there should not be any failure commands before the first good command
				commands_t::const_iterator i = commands.begin();
				assert(i == commands.end() || i->first);
				
				commands_t::const_iterator ff
					= std::find_if(commands.begin(), commands.end(), node_t::is_failure);
				
				commands_t::const_reverse_iterator first_failure
					= (ff == commands.end()) ? commands.rend() : commands_t::const_reverse_iterator(ff);
				
				
				// perform all commands until we fail one
				while (i != commands.end())
				{
					if (!executor_(state, input, i->second))
						break;
					
					i = std::find_if(i + 1, commands.end(), node_t::is_command);
				}
				
				// if we failed, perform all failures
				if (i != commands.end())
				{
					commands_t::const_iterator next_command
						= std::find_if(i + 1, commands.end(), node_t::is_command);
					
					commands_t::const_reverse_iterator failure =
						std::find_if(commands_t::const_reverse_iterator(next_command), commands.rend(), node_t::is_failure);
					
					while (failure != first_failure) {
						executor_(state, input, failure->second);
						failure = std::find_if(failure, first_failure, node_t::is_failure);
					}
					
					return false;
				}
				
				switch (current_node->combination_) {
					case combination_t::one: {
						children_t::const_iterator child =
							std::find_if(children.begin(), children.end(),
								boost::bind(&performer_t::perform_node<StateT, InputT, NodePTR>, this, boost::ref(state), boost::ref(input), _1));
						
						if (child == children.end() && !current_node->is_terminal) {
							//current_node = current_node->failure;
							return false;
						}
						else {
							current_node = current_node->success;
						}
						break;
					}
						
					case combination_t::all: {
						children_t::const_iterator child = 
							std::find_if(children.begin(), children.end(),
								!boost::bind(&performer_t::perform_node<StateT, InputT, NodePTR>, this, boost::ref(state), boost::ref(input), _1));
						
						if (child != children.end() && !current_node->is_terminal) {
							//current_node = current_node->failure;
							return false;
						}
						else {
							current_node = current_node->success;
						}
						break;
					}
					
					default:
						current_node = current_node->success;
						break;
				}
			}
			
			return true;
		}
		
	private:
		executor_t executor_;
		
		//std::stack< boost::tuple<node_ptr, commands_t::const_reverse_iterator, commands_t::const_iterator> > node_stack_;
	};
	
	
//=====================================================================
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

