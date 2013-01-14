#include <sooty/parsing/parser.hpp>
//=====================================================================
using sooty::parsing::parser;
using sooty::parsing::detail::parser_backend_t;
using sooty::parsing::detail::parser_backend_ptr;
//=====================================================================

void remove_left_recursive_of(parser_backend_ptr& n, parser_backend_ptr& ancestor, parser_backend_ptr& parent)
{
	parser_backend_t::children_t alphas;
	
	if (parent) {
		// trailing alphas
		auto& parent_children = parent->children();
		auto i = std::find(parent_children.begin(), parent_children.end(), ancestor);
		ATMA_ASSERT(i != parent_children.end());
		alphas.assign(i + 1, parent_children.end());
	}

	// create A'
	auto a_new = parser_backend_t::make(parser_backend_t::type_t::and_);
	a_new->add_children(alphas.begin(), alphas.end());
	a_new->add_self_as_child();

	// remove A from parent
	if (parent) {
		parent->remove_child(ancestor);

		// get or-node
		parser_backend_ptr or_node = parent->parent();
		if (or_node) {
			for (auto& x : or_node->children()) {
				if (x != parent) {
					ATMA_ASSERT(x->is_and_node());
					x->add_child(a_new);
				}
			}
		}
	}
	else {
		ancestor = a_new;
	}
}





parser::parser()
: backend_(new detail::parser_backend_t(detail::parser_backend_t::type_t::leaf)), assigned_(false)
{
}

parser::parser(const parser_backend_ptr& backend)
: backend_(backend), assigned_(true)
{
}

auto parser::backend() const -> parser_backend_ptr const& {
	return backend_;
}

auto parser::operator >> (parser const& rhs) const -> parser
{
	if (backend_->is_and_node()) {
		return parser(
			common::clone_tree(backend_)->add_child(
				common::clone_tree(rhs.backend())
			)
		);
	}

	return parser(
		detail::parser_backend_t::make(detail::parser_backend_t::type_t::and_)
			->add_child(common::clone_tree(backend_))
			->add_child(common::clone_tree(rhs.backend_))
	);
}
		
auto parser::operator | (parser const& rhs) const -> parser
{
	if (backend_->is_or_node()) {
		return parser(
			common::clone_tree(backend_)->add_child(
				common::clone_tree(rhs.backend())
			)
		);
	}

	return parser (
		detail::parser_backend_t::make(detail::parser_backend_t::type_t::or_)
			->add_child(common::clone_tree(backend_))
			->add_child(common::clone_tree(rhs.backend_))
	);
}

auto parser::operator [] (const parser& rhs) const -> parser
{
	//detail::mark_t mark = detail::generate_mark();
	//		
	//return parser (
	//	detail::parser_backend_t::make(detail::parser_backend_t::type_t::or_)
	//		->push_back_command(detail::command_t::make_add_marker(mark))
	//		->push_back_failure(detail::command_t::make_rm_marker(mark))
	//		->append(common::clone_tree(rhs.backend_))
	//		->append(common::clone_tree(backend_))
	//		//->append(rhs.backend_)
	//		//->append(backend_)
	//		->append(detail::parser_backend_t::make()
	//			->push_back_command(detail::command_t(detail::command_t::combine, 0, 0, mark))
	//		)
	//);
	return rhs;
}

auto parser::operator = (parser const& rhs) -> parser&
{
	//typedef node_t<Command> node_t;
	//typedef typename node_t::node_ptr node_ptr;
	using detail::parser_backend_t;
	using detail::parser_backend_ptr;

	// get set of nodes that have been cloned from us
	parser_backend_ptr rhs_backend = rhs.backend();

	// as we go "down" the tree, build a set of cloned nodes. we'll remove entries when we go back up
	std::set<parser_backend_ptr> visited;
	std::map<parser_backend_ptr, std::set<parser_backend_ptr>> cloned_nodes;

	// wrap all instances of ourselves in the existing tree with a dummy node
	for_all_depth_first_with_parent
	(
		rhs_backend,

		// build cloned nodes, maintain list of visited nodes
		[&](parser_backend_ptr const& parent, parser_backend_ptr const& x) -> bool
		{
			if (visited.find(x) != visited.end()) return false;
			visited.insert(x);

			std::stack<parser_backend_t*> nodes;
			nodes.push(x.get());
			while (!nodes.empty()) {
				auto const& n = nodes.top();
				nodes.pop();
				parser_backend_t::for_all_cloned_nodes_of(n, [&nodes](parser_backend_t* clonee){nodes.push(clonee);});
				cloned_nodes[x].insert(n->shared_from_this());
			}
			return true;
		},

		[&](parser_backend_ptr& parent, parser_backend_ptr& n)
		{
			// for each node, go through anscestors and find any left-recursion
			parser_backend_ptr ancestor = parent;
			parser_backend_ptr child = n;
			while (ancestor)
			{
				// if we're not the first node of an and-node, we are not left-recursive
				if (ancestor->is_and_node() && ancestor->children().front() != child)
					break;

				// oh, we're left-recursive
				if (ancestor == n) {
					remove_left_recursive_of(n, ancestor, ancestor->parent());
				}

				child = ancestor;
				ancestor = child->parent();
			}
		}
	);


	*backend_ = *rhs.backend();
	return *this;
}



