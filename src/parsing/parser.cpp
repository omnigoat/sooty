#include <sooty/parsing/parser.hpp>

using sooty::parsing::parser;
using sooty::parsing::detail::parser_backend_ptr;
using sooty::parsing::detail::parser_backend_t;

parser::parser()
: backend_(detail::parser_backend_t::make_placeholder()), assigned_(false)
{
}

parser::parser(const parser_backend_ptr& backend)
: backend_(backend), assigned_(true)
{
}

auto parser::backend() const -> parser_backend_ptr const& {
	return backend_;
}

auto parser::operator >> (parser const& rhs) const -> parser {
	return parser(
		common::clone_tree(backend_)->append( common::clone_tree(rhs.backend_) )
	);
}

auto parser::operator , (parser const& rhs) const -> parser {
	return *this >> rhs;
}
		
auto parser::operator | (parser const& rhs) const -> parser {
	return parser (
		common::clone_tree(backend_)->merge( common::clone_tree(rhs.backend_) )
	);
}

namespace sooty { namespace parsing {

	using detail::parser_backend_ptr;
	using detail::parser_backend_t;

	parser_backend_ptr remove_left_recursion(parser_backend_ptr const& root, detail::parser_backend_ptr const& niq)
	{
		std::stack<std::tuple<parser_backend_ptr, parser_backend_ptr>> nodes;
		nodes.push(std::make_pair(parser_backend_ptr(), root));
		while (!nodes.empty()) 
		{
			auto x = nodes.top();
			nodes.pop();
			auto xp = std::get<0>(x);
			auto xn = std::get<1>(x);

			// if this node is us (or a clone of us)
			if (parser_backend_t::equal_or_clone(niq, xn))
			{
				// create A'  (a1 A' | a2 A' | a3 A')
				parser_backend_ptr A_stroke = parser_backend_t::make();
				A_stroke->children_ = xn->children_;
				A_stroke->append(A_stroke);

				// rewrite parent
				if (xp)
				{
					unsigned int rm_count = xp->children_.erase(xn);
					ATMA_ASSERT(rm_count == 1);

					for (auto& B : xp->children_) {
						B->append(A_stroke);
					}
				}
			}
			// we need to continue recursing only for "empty" nodes
			else if (xn->commands_.empty()) {
				for (auto const& n : xn->children_)
					nodes.push(std::make_tuple(xn, n));
			}
		}

		// sometimes we end up with control nodes at the front with just one child. they are
		// useless to us and increase nosie. so skip them!
		parser_backend_ptr n = root;
		while (n->type() == parser_backend_t::type_t::control && n->children_.size() == 1)
			n = *n->children_.begin();

		return n;
	}
} }

auto parser::operator [] (const parser& rhs) const -> parser
{
	detail::parser_backend_ptr p = common::clone_tree(rhs.backend_);

	//
	//
	//    JONATHAN, LISTEN
	//
	//  we need to append the insert+combine in a smarter way, as currently
	//  it appends to nothing if there's backreferences. so find them, and
	//  make it another alternative. consider chronological ordering children?
	//
	//
	using detail::parser_backend_ptr;

	std::map<parser_backend_ptr, unsigned int> visited;
	std::stack<parser_backend_ptr> nodes;
	std::map<parser_backend_ptr, unsigned int> inserts;
	nodes.push(p);
	inserts[p] = 0;
	while (!nodes.empty())
	{
		auto x = nodes.top();
		nodes.pop();

		if (++visited[x] > 1)
			continue;

		unsigned int ii = inserts[x];
		detail::parser_backend_t::children_t children = x->children_;

		// count number of insertions
		if (x->commands_.empty()) {
			++ii;
		}
		else if (x->commands_.front().second.insert_id != 0) {
			if (x->commands_.front().second.action != detail::command_t::action_t::combine) {
				++ii;
			}
		}

		// if this is a leaf node, append @this' backend, and the combine
		if (x->children_.empty()) {
			x->add_child( common::clone_tree(backend_) );
			x->append(detail::parser_backend_t::make()
				->push_back_command(detail::command_t(detail::command_t::action_t::combine, 0, 0, ii))
			);
		}

		for (auto y : children) {
			inserts[y] = ii;
			nodes.push(y);
		}
	}

	for (auto const& x : visited) {
		if (x.second > 1) {
			x.first->add_child(
				common::clone_tree(backend_)->append(detail::parser_backend_t::make()
					->push_back_command(detail::command_t(detail::command_t::action_t::combine, 0, 0, 2))
				)
			);
		}
	}

	//common::accumulate_depth_first(p, 0, [&](unsigned int inserts, detail::parser_backend_ptr const& x) -> unsigned int {
	//	if (x->commands_.empty()) {
	//		++inserts;
	//	}
	//	else if (x->commands_.front().second.insert_id != 0) {
	//		if (x->commands_.front().second.action != detail::command_t::action_t::combine) {
	//			++inserts;
	//		}
	//	}

	//	// if this is a leaf node, append @this' backend, and the combine
	//	if (x->children_.empty()) {
	//		x->add_child( common::clone_tree(backend_) );
	//		x->append(detail::parser_backend_t::make()
	//			->push_back_command(detail::command_t(detail::command_t::action_t::combine, 0, 0, inserts))
	//		);
	//	}

	//	return inserts;
	//});

	return parser(p);
}



// @rhs is a tree that may contain a placeholder node that is
// equivalent (might be a clone) of our local @backend_
auto parser::operator = (parser const& rhs) -> parser&
{
	// yay!
	detail::parser_backend_ptr hold = backend_;
	
	backend_ = remove_left_recursion(rhs.backend_, backend_);

	std::vector<parser_backend_t*> clones(hold->clones_.begin(), hold->clones_.end());

	// for each clone of our @backend_ (which must be a hanging placeholder node)
	// go and change it to be like us
	for (auto& x : hold->clones_)
	{
		ATMA_ASSERT(x->type_ == parser_backend_t::type_t::placeholder);

		x->type_ = parser_backend_t::type_t::control;
		parser_backend_t::children_t x_children = x->children_;
		x->children_.clear();
		x->children_.insert(common::clone_tree(backend_));

		for (auto const& y : x_children) {
			x->append(y, false);
		}
	}
	




	return *this;
}



