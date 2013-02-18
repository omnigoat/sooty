//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_COMMON_PERFORMER_HPP
#define SOOTY_COMMON_PERFORMER_HPP
//=====================================================================
#include <algorithm>
#include <atma/assert.hpp>
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
			typedef typename node_t::children_t children_t;
			
			children_t first_children;
			first_children.insert(node);
			
			size_t parent_child_count = 1;
			children_t::const_iterator parent_child_begin = first_children.begin();
			children_t::const_iterator parent_child_iter = first_children.begin();
			children_t::const_iterator parent_child_end = first_children.end();
			
			while (parent_child_iter != parent_child_end)
			{
				node_ptr const& current_node = *parent_child_iter;
				commands_t const& commands = current_node->commands_;
				children_t const& children = current_node->children_;
				bool success = true;

				if (current_node->type() == node_t::type_t::placeholder)
				{
					ATMA_ASSERT(!current_node->ancestry_.empty());

					std::vector<node_t*> clones(
						current_node->ancestry_.back()->clones_.begin(),
						current_node->ancestry_.back()->clones_.end()
					);

					ATMA_ASSERT( std::partition(clones.begin(), clones.end(), [](node_t* x) -> bool {
						return x->type_ == node_t::type_t::control;
					}) == clones.begin() + 1);

					success = this->operator ()(state, input, (*clones.begin())->shared_from_this());
				}
				else
				{
					
				
					// there should not be any failure commands before the first good command
					commands_t::const_iterator i = commands.begin();
				
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

					success = i == commands.end();
				}
					
				

				// if we failed, perform all failures
				if (!success)
				{
					/*commands_t::const_iterator next_command
						= std::find_if(i + 1, commands.end(), node_t::is_command);
					
					commands_t::const_reverse_iterator failure =
						std::find_if(commands_t::const_reverse_iterator(next_command), commands.rend(), node_t::is_failure);
					
					while (failure != first_failure) {
						executor_(state, input, failure->second);
						failure = std::find_if(failure, first_failure, node_t::is_failure);
					}*/
					
					// move onto our sibling
					++parent_child_iter;
					continue;
				}
				else
				{
					++parent_child_iter;
					
					
					// continue with our children
					parent_child_count = children.size();
					parent_child_begin = children.begin();
					parent_child_iter = children.begin();
					parent_child_end = children.end();
				}
			}
			
			// HEY  JONATHAN
			// FIX THIS, THIS WILL NEVER RETURN GOOD RESULTS
			return true;
		}
	
	private:
		template <typename StateT, typename InputT, typename NodePTR>
		static void perform_unchosen(StateT& state, InputT& input, const NodePTR& N) {
			//typename NodePTR::element_type::commands_t::const_iterator begin = N->unchosen_.begin();
			//for ( ; begin != N->unchosen_.end(); ++begin )
				//executor_t()(state, input, begin->second);
				
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

