#include <sooty/transforming/transformer.hpp>
//=====================================================================
#include <sooty/common/detail/clone.hpp>
#include <sooty/common/detail/append_success.hpp>
//=====================================================================
#include <sooty/transforming/detail/layout.hpp>
//=====================================================================
using namespace sooty::transforming;



transformer sooty::transforming::transformer::operator [] (const sooty::transforming::transformer &rhs) const
{
	detail::abstract_transformer_backend_ptr new_rhs = sooty::common::detail::clone_tree(rhs.backend);
	sooty::common::detail::append_success(new_rhs, detail::link_t(dummy().backend, detail::how_to_traverse::surface));
	detail::abstract_transformer_backend_ptr new_lhs = sooty::common::detail::clone_tree(this->backend);
	sooty::common::detail::append_success(new_lhs, detail::link_t(new_rhs, detail::how_to_traverse::dive));
	return transformer(new_lhs);
}

sooty::transforming::transformer sooty::transforming::transformer::operator , (const transformer& rhs) const
{
	detail::abstract_transformer_backend_ptr new_lhs = sooty::common::detail::clone_tree(this->backend);
	sooty::common::detail::append_success(new_lhs, detail::link_t(rhs.backend, detail::how_to_traverse::next_sibling));
	return transformer(new_lhs);
}

