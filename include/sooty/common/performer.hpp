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
		
		template <typename state_tm, typename input_tm, typename node_ptr_tm>
		bool operator()(state_tm& state, input_tm& input, const node_ptr_tm& node)
		{
			typedef input_tm input_t;
			typedef input_t& input_ref;
			
			typedef node_ptr_tm::element_type node_t;
			typedef typename node_t::node_ptr node_ptr;
			typedef typename node_t::command_t command_t;
			typedef typename node_t::commands_t commands_t;
			typedef typename node_t::children_t children_t;
			
			children_t first_children;
			first_children.insert(node);
			
			node_ptr parent = node_ptr();
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
					
				

				if (success)
				{
					// continue with our children
					parent = *parent_child_iter;
					parent_child_begin = children.begin();
					parent_child_iter = children.begin();
					parent_child_end = children.end();
				}
				else
				{
					// move onto our sibling
					++parent_child_iter;
				}
			}
			
			// we succeeded if we didn't exhaust our child-options, or if we're a backreference,
			// or if we didn't even have children, or we are a terminal node
			return
				parent_child_iter != parent_child_end ||
				parent->type() == node_t::type_t::backreference ||
				std::distance(parent_child_begin, parent_child_end) == 0 ||
				parent->terminal()
				;
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

