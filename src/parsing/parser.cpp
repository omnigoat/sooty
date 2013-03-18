#include <sooty/parsing/parser.hpp>
#include <atma/console.hpp>

using sooty::common::clone_tree;
using sooty::common::append;

using sooty::parsing::parser;
using sooty::parsing::detail::parser_backend_ptr;
using sooty::parsing::detail::parser_backend_t;

parser::parser()
: backend_(detail::parser_backend_t::make_placeholder())
{
}

parser::parser(const parser_backend_ptr& backend)
: backend_(backend)
{
}

auto parser::backend() const -> parser_backend_ptr const& {
	return backend_;
}

auto parser::resolved_backend() const -> parser_backend_ptr const& {
	return resolved_backend_;
}

auto parser::operator >> (parser const& rhs) const -> parser {
	return parser(
		append( clone_tree(backend_), clone_tree(rhs.backend_) )
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
			append(x, detail::parser_backend_t::make()
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
				append(
					clone_tree(backend_),
					detail::parser_backend_t::make()
					 ->push_back_command(detail::command_t(detail::command_t::action_t::combine, 0, 0, 2))
				)
			);
		}
	}

	return parser(p);
}



// @rhs is a tree that may contain a placeholder node that is
// equivalent (might be a clone) of our local @backend_
auto parser::operator = (parser const& rhs) -> parser&
{
	using detail::parser_backend_ptr;
	using detail::parser_backend_t;

	std::stack<std::tuple<parser_backend_ptr, parser_backend_ptr>> nodes;
	nodes.push(std::make_tuple(parser_backend_ptr(), rhs.backend_));
	bool did_work = false;
	while (!nodes.empty()) 
	{
		auto x = nodes.top();
		nodes.pop();
		auto xp = std::get<0>(x);
		auto xn = std::get<1>(x);

		// if this node is us (or a clone of us)
		if (parser_backend_t::equal_or_clone(backend_, xn))
		{
			// create A'  (a1 A' | a2 A' | a3 A')
			parser_backend_ptr A_stroke = parser_backend_t::make_backreference();
			A_stroke->children_ = std::move(xn->children_);
			//A_stroke->append(A_stroke);
			append_backref(A_stroke, A_stroke);

			// rewrite parent				
			if (xp)
			{
				// remove A
				unsigned int rm_count = xp->children_.erase(xn);
				ATMA_ASSERT(rm_count == 1);

				xp->type_ = parser_backend_t::type_t::control;

				// append A' to all B
				parser_backend_t::children_t tmp;
				for (auto& B : xp->children_) {
					//B->append(A_stroke, false);
					parser_backend_ptr b = B;
					tmp.insert( append(b, A_stroke) );
				}
				std::swap(xp->children_, tmp);
			}

			did_work = true;
			break;
		}
		// we need to continue recursing only for "empty" nodes
		else if (xn->commands_.empty()) {
			for (auto const& n : xn->children_)
				nodes.push(std::make_tuple(xn, n));
		}
	}

	resolved_backend_ = rhs.backend_;

	// stay a while and listen:
	//  - placeholder nodes look at their eldest ancestsor to find the
	//    definition they should match.
	//  - the eldest ancestor has a set of every node cloned directly/indirectly
	//    from it - one of these will be a non-placeholder node
	//  - that non-placeholder node will be a @resolved_backend_ somewhere
	//  - we need to manually make @resolved_backend_ a clone of @backend_
	//  - we need to correctly disentangle @resovled_backend_ from ITS clones/ancestors
	//  - we actually have a method to do this, called "assume", which assumes the
	//    clone/ancestor identity of a node
	parser_backend_ptr tmp = common::clone_tree(backend_);
	resolved_backend_->assume( std::move(*tmp) );

	return *this;
}


auto parser::debug_print(int spaces) const -> void
{
	std::set<parser_backend_ptr> visited;
	debug_print_impl(visited, resolved_backend_, spaces);
}




auto parser::debug_print_impl( std::set<parser_backend_ptr>& visited, parser_backend_ptr const& backend, int spaces ) const -> void
{
	if (visited.find(backend) != visited.end()) {
		for (int i = 0; i != spaces; ++i)
			std::cout << " ";
		std::cout << "back-ref " << atma::console::fg_yellow << backend.get() << std::endl << atma::console::reset;
		return;
	}
	visited.insert(backend);

	for (int i = 0; i != spaces; ++i)
		std::cout << " ";
	if (backend->type_ == parser_backend_t::type_t::backreference)
		std::cout << atma::console::foreground_color_t(0xc);
	std::cout << backend.get() << " ";

	switch (backend->type_)
	{
		case parser_backend_t::type_t::actor:
		{
			std::cout << "actor: ";
			for (auto& x : backend->commands_) {
				switch (x.second.action)
				{
				case detail::command_t::action_t::match:
					std::cout << atma::console::fg_dark_green << "match " << x.second.lower_id << atma::console::reset;
					break;

				case detail::command_t::action_t::insert:
					std::cout << "insert";
					break;

				case detail::command_t::action_t::combine:
					std::cout << "combine";
					break;
				}
				std::cout << ", ";
			}
			break;
		}

		case parser_backend_t::type_t::control:
			{
				std::cout << "control";
				break;
			}

		case parser_backend_t::type_t::placeholder:
			{
				std::cout << atma::console::fg_blue << "placeholder " << (backend->ancestry_.empty() ? nullptr : backend->ancestry_.back()) << atma::console::reset;
				break;
			}

		case parser_backend_t::type_t::backreference:
			{
				std::cout << atma::console::fg_red << "backreference" << atma::console::reset;
				break;
			}
	}
	
	
	std::cout << std::endl;
	for (auto& x: backend->children_)
		parser(x).debug_print_impl(visited, x, spaces + 1);
}
