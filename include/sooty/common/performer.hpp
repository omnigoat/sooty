//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_COMMON_PERFORMER_HPP
#define SOOTY_COMMON_PERFORMER_HPP
//=====================================================================
#include <algorithm>
//=====================================================================
namespace sooty {
namespace common {
//=====================================================================
	
	template <typename ExecutorT>
	struct performer_t
	{
		typedef ExecutorT executor_t;
		
		const executor_t& executor() const {
			return executor_;
		}
		
		template <typename StateT, typename InputT, typename NodePTR>
		bool operator()(StateT& state, InputT& input, const NodePTR& node)
		{
			typedef NodePTR::element_type node_t;
		
			typedef StateT state_t;
			typedef state_t& state_ref;
			
			typedef InputT input_t;
			typedef input_t& input_ref;
			
			typedef typename node_t::node_ptr node_ptr;
			typedef typename node_t::command_t command_t;
			typedef typename node_t::commands_t commands_t;
			typedef typename node_t::const_commands_ref const_commands_ref;
			typedef typename node_t::children_t children_t;
			typedef typename node_t::const_children_ref const_children_ref;
			
			children_t first_children;
			first_children.push_back(node);
			
			size_t parent_child_count = 1;
			children_t::const_iterator parent_child_begin = first_children.begin();
			children_t::const_iterator parent_child_iter = first_children.begin();
			children_t::const_iterator parent_child_end = first_children.end();
			
			while (parent_child_iter != parent_child_end)
			{
				node_ptr const& current_node = *parent_child_iter;
				
				const_commands_ref commands = current_node->commands_;
				const_children_ref children = current_node->children_;
				
				// there should not be any failure commands before the first good command
				commands_t::const_iterator i = commands.begin();
				//assert(i == commands.end() || i->first);
				
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
					
					// move onto our sibling
					++parent_child_iter;
					continue;
				}
				else
				{
					// first, inform our siblings that we were chosen, and they were unchosen
					std::for_each(parent_child_begin, parent_child_iter,
						std::bind(&performer_t::perform_unchosen<StateT, InputT, NodePTR>, std::ref(state), std::ref(input), std::placeholders::_1));
					
					++parent_child_iter;
					std::for_each(parent_child_iter, parent_child_end,
						std::bind(&performer_t::perform_unchosen<StateT, InputT, NodePTR>, std::ref(state), std::ref(input), std::placeholders::_1));
					
					// continue with our children
					parent_child_count = children.size();
					parent_child_begin = children.begin();
					parent_child_iter = children.begin();
					parent_child_end = children.end();
				}
			}
			
			return parent_child_count == 0 || parent_child_iter != parent_child_end;
		}
	
	private:
		template <typename StateT, typename InputT, typename NodePTR>
		static void perform_unchosen(StateT& state, InputT& input, const NodePTR& N) {
			typename NodePTR::element_type::commands_t::const_iterator begin = N->unchosen_.begin();
			for ( ; begin != N->unchosen_.end(); ++begin )
				executor_t()(state, input, begin->second);
				
			/*std::for_each(N->unchosen_.begin(), N->unchosen_.end(), 
				boost::bind(executor_t(), boost::ref(state), boost::ref(input), _1));*/
		}
		
	private:
		executor_t executor_;
	};
	
	
//=====================================================================
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

