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
		
		template <typename state_tm, typename input_tm, typename C>
		bool operator()(state_tm& state, input_tm& input, C const& nodes)
		{
			typedef input_tm input_t;
			typedef input_t& input_ref;
			
			typedef C::value_type node_ptr;
			typedef node_ptr::element_type node_t;
			typedef typename node_t::children_t children_t;
			
			
			node_ptr parent = node_ptr();
			children_t::const_iterator parent_child_begin = nodes.begin();
			children_t::const_iterator parent_child_iter = nodes.begin();
			children_t::const_iterator parent_child_end = nodes.end();
			
			while (parent_child_iter != parent_child_end)
			{
				node_ptr const& current_node = *parent_child_iter;
				children_t const& children = current_node->children_;
				bool success = true;

				#if 0
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
				#endif
				{
					success = executor_(state, input, current_node->value_);
				}
					
				
				// if we succeeded, continue with one of our children (hopefully)
				if (success) {
					parent = *parent_child_iter;
					parent_child_begin = children.begin();
					parent_child_iter = parent_child_begin;
					parent_child_end = children.end();
				}
				// if we failed, try one of our remaining siblings
				else {
					++parent_child_iter;
				}
			}
			
			// we succeeded if we didn't exhaust our child-options, or if we're a backreference,
			// or if we didn't even have children, or we are a terminal node
			return
			  parent_child_iter != parent_child_end ||
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

