


//
// constructors
//
template <typename N>
template <typename... Args>
node_t<N>::node_t(Args... args)
	: value_(args...), terminal_(), bypassable_()
{
}

template <typename N>
node_t<N>::node_t(const node_t<N>& rhs)
	: value_(rhs.value_), terminal_(rhs.terminal_), bypassable_(rhs.bypassable_)
{
}

template <typename N>
node_t<N>::~node_t()
{
	// if we have clones, for each clone, go and remove us from the ancestry
	for (auto const& n : clones_)
	{
		auto i = std::find(n->ancestry_.begin(), n->ancestry_.end(), this);
		ATMA_ASSERT(i != n->ancestry_.end());
		n->ancestry_.erase(i);
	}

	// remove ourselves from our ancestry's clones
	for (auto const& x : ancestry_)
	{
		ATMA_ASSERT(x->clones_.find(this) != x->clones_.end());
		x->clones_.erase(this);
	}
}

template <typename N>
auto node_t<N>::operator = (node_t<N> const& rhs) -> node_t<N>&
{
	type_ = rhs.type_;
	children_ = rhs.children_;
	terminal_ = rhs.terminal_;
	bypassable_ = rhs.bypassable_;
	return *this;
}

template <typename N>
auto node_t<N>::assume(node_t&& rhs) -> node_ptr
{
	// if we have clones, for each clone, go and remove us from the ancestry
	for (auto const& n : clones_)
	{
		auto i = std::find(n->ancestry_.begin(), n->ancestry_.end(), this);
		ATMA_ASSERT(i != n->ancestry_.end());
		n->ancestry_.erase(i);
	}

	// remove ourselves from our ancestry's clones
	for (auto const& x : ancestry_)
	{
		ATMA_ASSERT(x->clones_.find(this) != x->clones_.end());
		x->clones_.erase(this);
	}


	clones_ = std::move(rhs.clones_);
	ancestry_ = std::move(rhs.ancestry_);
	
	rhs.ancestry_.clear();
	rhs.clones_.clear();

	// for each clone, update it so it knows we're its new ancestor
	for (auto const& n : clones_) {
		auto i = std::find(n->ancestry_.begin(), n->ancestry_.end(), &rhs);
		ATMA_ASSERT(i != n->ancestry_.end());
		*i = this;
	}

	// update our ancestors to know we conceptually cloned from them
	for (auto const& x : ancestry_) {
		ATMA_ASSERT(x->clones_.find(&rhs) != x->clones_.end());
		x->clones_.erase(&rhs);
		x->clones_.insert(this);
	}

	return shared_from_this();
}

template <typename N>
auto node_t<N>::clone() -> node_ptr
{
	// clone node
	node_ptr C(new node_t(*this));

	// our clone's ancestry is our ancestry with us at the front
	C->ancestry_.reserve(1 + ancestry_.size());
	C->ancestry_.push_back(this);
	C->ancestry_.insert(C->ancestry_.end(), ancestry_.begin(), ancestry_.end());

	// tell all our clone's ancestors about their new clone
	for (auto const& x : C->ancestry_)
		x->clones_.insert(C.get());

	return C;
}

template <typename N>
auto node_t<N>::add_child(node_ptr const& n) -> node_ptr {
	children_.insert(n);
	return shared_from_this();
}

//
//template <typename N>
//auto node_t<N>::merge(node_ptr const& rhs) -> node_ptr
//{
//	node_ptr result = shared_from_this();
//
//	// neither @lhs nor @rhs actually had any commands
//	// they can be merged if they're not placeholders or if they're placeholders but share ancestry
//	if (node_t::share_ancestry(result, rhs))
//	{
//		children_.insert(rhs->children_.begin(), rhs->children_.end());
//	}
//	// @lhs and @rhs were completely merged together, we can recurse through our children
//	else if (!combined_commands.empty() && new_lhs_commands.empty() && new_rhs_commands.empty())
//	{
//		children_t new_children;
//
//		atma::merge(
//			children_.begin(), children_.end(),
//			rhs->children_.begin(), rhs->children_.end(),
//			std::inserter(new_children, new_children.end()),
//			std::bind(&node_t<N>::merge, std::placeholders::_1, std::placeholders::_2),
//			[&new_children](node_ptr const& n){ new_children.insert(n); },
//			[&new_children](node_ptr const& n){ new_children.insert(n); },
//			merged_ordering_t()
//		);
//
//		children_.swap(new_children);
//
//		// we have children, and one of our two branches was wholly consumed. we
//		// are therefore a terminal.
//		if (!children_.empty())
//			terminal_ = true;
//	}
//	// @rhs was completely merged into @lhs, but has commands left over. merge it into any of our children
//	else if (!combined_commands.empty() && new_lhs_commands.empty() && !new_rhs_commands.empty())
//	{
//		children_t new_children;
//		auto merge_failer = [&new_children](node_ptr const& n){ new_children.insert(n); };
//		atma::merge(
//			children_.begin(), children_.end(),
//			rhs->children_.begin(), rhs->children_.end(),
//			std::inserter(new_children, new_children.end()),
//			std::bind(&node_t<N>::merge, std::placeholders::_1, std::ref(rhs)),
//			merge_failer, merge_failer,
//			merged_ordering_t()
//		);
//
//		children_.swap(new_children);
//	}
//	// no merging took place. we're going to create a parent node and insert both
//	// @lhs and @rhs as children, then replace ourselves with the parent, careful to
//	// avoid cycles
//	else if (combined_commands.empty())
//	{
//		if (type_ == type_t::control) {
//			if (rhs->type_ == type_t::control) {
//				children_.insert(rhs->children_.begin(), rhs->children_.end());
//			}
//			else {
//				children_.insert(rhs);
//			}
//		}
//		else {
//			result = make();
//			result->children_.insert(shared_from_this());
//			result->children_.insert(rhs);
//		}
//	}
//	else {
//		ATMA_ASSERT(false && "yeah probs forgot a use-case");
//	}
//	
//	return result;
//}
//

template <typename C>
auto node_t<C>::equal_or_clone(node_ptr const& lhs, node_ptr const& rhs) -> bool
{
	std::stack<node_t*> nodes;
	nodes.push(lhs.get());
	while (!nodes.empty()) {
		auto x = nodes.top();
		nodes.pop();
		if (x == rhs.get())
			return true;
		for (auto& y : x->clones_)
			nodes.push(y);
	}

	return false;
}

template <typename C>
auto node_t<C>::share_ancestry(node_ptr const& lhs, node_ptr const& rhs) -> bool
{
	return !lhs->ancestry_.empty() && !rhs->ancestry_.empty() && lhs->ancestry_.back() == rhs->ancestry_.back();
}



template <typename C>
auto node_t<C>::ordering_t::operator () (node_ptr const& lhs, node_ptr const& rhs) const -> bool
{
	return lhs->value_ < rhs->value_;
};


template <typename C>
auto node_t<C>::merged_ordering_t::operator () (node_ptr const& lhs, node_ptr const& rhs) const -> bool
{
	// if the two nodes share part of their ancestry
	for (auto& x : lhs->ancestry_) {
		if (std::find(rhs->ancestry_.begin(), rhs->ancestry_.end(), x) != rhs->ancestry_.end())
			return false;
	}

	return lhs->value_ < rhs->value_;
}



//=====================================================================
// append
//=====================================================================
template <typename C>
auto append(C& dest, C const& n) -> C& {
	typedef typename C::value_type ct;
	std::set<ct> visited(n.begin(), n.end());
	append_impl(visited, dest, n);
	return dest;
}

template <typename C>
auto append_backref(C& dest, C const& n) -> C& {
	std::set<typename C::value_type> visited;
	append_impl(visited, dest, n);
	return dest;
}

template <typename C, typename N>
auto append(C& dest, std::shared_ptr<node_t<N>> const& n) -> C& {
	return append(dest, C{n});
}

template <typename C, typename N>
auto append_backref(C& dest, std::shared_ptr<node_t<N>> const& n) -> C& {
	return append_backref(dest, C{n});
}

template <typename C>
auto append_impl(std::set<typename C::value_type>& visited, C& dest, C const& nodes) -> void
{
	typedef typename C::value_type node_ptr;

	// if we are empty (leaf container), then just insert the nodes
	if ( dest.empty() ) {
		dest.insert(nodes.begin(), nodes.end());
	}
	// if all of @dest are bypassable, then we need merge in the nodes
	else if (std::all_of(dest.begin(), dest.end(), [](node_ptr const& y) { return y->bypassable_; })) {
		merge(dest, nodes);
	}
	

	// for each node @x we're inserting into
	for (auto const& x : dest)
	{
		if (visited.find(x) != visited.end())
			continue;
		visited.insert(x);

		// if x is a terminal, then simply merge in all @nodes
		if (x->terminal()) {
			merge(x->children_, nodes);
		}

		append_impl(visited, x->children_, nodes);
	}
}



//=====================================================================
// merge_into_children
//=====================================================================
template <typename C>
auto merge(C& dest, C const& src) -> C&
{
	typedef typename C::value_type::element_type node_t;

	auto srci = src.begin();
	for (auto srci = src.begin(); srci != src.end(); ++srci)
	{
		// this searches for the same *instance*, not elementwise equality
		if (dest.find(*srci) != dest.end()) {
			continue;
		}

		// this searches for elementwise equality, and if not found, inserts
		auto x = std::lower_bound(dest.begin(), dest.end(), *srci, node_t::merged_ordering_t());
		if (x == dest.end() || *x != *srci) {
			dest.insert(*srci);
		}
		// recursively merge children
		else {
			merge((*x)->children_, (*srci)->children_);
		}
	}

	return dest;
}

template <typename C>
auto merge_into_children(std::shared_ptr<node_t<C>>& x, std::shared_ptr<node_t<C>> const& node) -> void
{
	typedef std::shared_ptr<node_t<C>> node_ptr;

	node_t<C>::merged_ordering_t pred;

	auto i = x->children_.begin();
	while (i != x->children_.end() && pred(node, *i))
		++i;

	// we hit the end, or we never found an equal child
	if (i == x->children_.end() || pred(*i, node)) {
		x->children_.insert(node);
	}
	// we have a match! we perform a regular merge from here on out.
	else {
		node_ptr t = *i;
		t = t->merge(node);
		x->children_.erase(i);
		x->children_.insert(t);
	}
}


template <typename C, typename N, typename FN>
void for_each_depth_first(std::set<std::shared_ptr<node_t<N>>>& visited, C const& root, FN fn)
{
	static_assert(typename C::value_type == decltype(visited)::value_type);

	typedef typename node_t<N>::node_ptr node_ptr;

	std::stack<node_ptr> nodes;
	nodes.push(root);
	while (!nodes.empty())
	{
		auto x = nodes.top();
		nodes.pop();
		if (visited.find(x) != visited.end())
			continue;
		visited.insert(x);
		node_t<N>::children_t children = x->children_;
		// call fn, which returns the new acc
		fn(x);

		for (auto const& y : children) {
			nodes.push(y);
		}
	}
}

template <typename C, typename FN>
void for_each_depth_first(C const& root, FN fn)
{
	std::set<typename C::value_type> visited;
	for_each_depth_first(visited, root, fn);
}

//template <typename N, typename acc_tm, typename FN>
//void accumulate_depth_first(node_t<N>::node_ptr const& root, acc_tm acc, FN fn)
//{
//	typedef node_t<N>::node_ptr node_ptr;
//	typedef std::tuple<node_ptr, acc_tm> value_t;
//	std::stack<value_t> nodes;
//	nodes.push(std::make_tuple(root, acc));
//	std::set<node_ptr> visited;
//	while (!nodes.empty())
//	{
//		auto x = nodes.top();
//		auto const& xn = std::get<0>(x);
//		auto const& xa = std::get<1>(x);
//		nodes.pop();
//		if (visited.find(xn) != visited.end())
//			continue;
//		visited.insert(xn);
//
//		typename std::shared_ptr<node_t<C>>::element_type::children_t children = xn->children_;
//			
//		// call fn, which returns the new acc
//		acc_tm combined_acc = fn(xa, xn);
//
//		for (auto const& y : children) {
//			nodes.push( std::make_tuple(y, combined_acc) );
//		}
//	}
//}


template <typename C>
auto interweave_nodes(C& nodes) -> void
{
	for (auto const& x : nodes) {
		append(x->children_, nodes);
	}
}




