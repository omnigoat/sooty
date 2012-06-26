#include <sooty/transforming/transform.hpp>
//=====================================================================

bool sooty::transforming::transform(walking::detail::context_ref context, const transformer& with)
{
	detail::abstract_transformer_backend_ptr current_transformer = with.backend;
		
	while (current_transformer)
	{
		// I don't think there's a viable invalid state for transforming??
		/*if (context.invalid()) {
			if (current_transformer->on_invalid.first) {
				traverse(context, current_transformer->on_invalid.second);
				current_transformer = current_transformer->on_invalid.first;
			}
			else {
				return false;
			}
		}
		else*/ if ( current_transformer->perform(context) )
		{
			context.traverse(current_transformer->on_success.second);
			current_transformer = current_transformer->on_success.first;
		}
		else if (current_transformer->on_failure.first)
		{
			context.traverse(current_transformer->on_failure.second);
			current_transformer = current_transformer->on_failure.first;
		}
		else {
			return false;
		}
	}
	
	return true;
}
