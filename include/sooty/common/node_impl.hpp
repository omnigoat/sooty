


//
// constructors
//
template <typename N>
node_t<N>::node_t()
	: type_(type), terminal_(), bypassable_()
{
}

template <typename N>
node_t<N>::node_t(const node_t<N>& rhs)
	: terminal_(rhs.terminal_), bypassable_(rhs.bypassable_)
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
	node_ptr C(new N(static_cast<N&>(*this)));

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
	return lhs.get() < rhs.get();
};


template <typename C>
auto node_t<C>::merged_ordering_t::operator () (node_ptr const& lhs, node_ptr const& rhs) const -> bool
{
	// if the two nodes share part of their ancestry
	for (auto& x : lhs->ancestry_) {
		if (std::find(rhs->ancestry_.begin(), rhs->ancestry_.end(), x) != rhs->ancestry_.end())
			return false;
	}

	return lhs.get() < rhs.get();
}



//=====================================================================
// append
//=====================================================================
template <typename C>
auto sooty::common::append(std::shared_ptr<node_t<C>>& x, std::shared_ptr<node_t<C>> const& node) -> std::shared_ptr<node_t<C>>&
{
	std::set<std::shared_ptr<node_t<C>>> visited;
	visited.insert(node);
	append_impl(visited, x, node);
	return x;
}

template <typename C>
auto sooty::common::append_backref(std::shared_ptr<node_t<C>>& x, std::shared_ptr<node_t<C>> const& node) -> std::shared_ptr<node_t<C>>&
{
	std::set<std::shared_ptr<node_t<C>>> visited;
	append_impl(visited, x, node);
	return x;
}

template <typename C>
auto sooty::common::append_impl(std::set<std::shared_ptr<node_t<C>>>& visited, std::shared_ptr<node_t<C>>& x, std::shared_ptr<node_t<C>> const& node) -> void
{
	typedef std::shared_ptr<node_t<C>> node_ptr;

	if (visited.find(x) != visited.end())
		return;

	visited.insert(x);

	if (x->children_.empty())
	{
		// append control node's children, not them themselves
		if (node->type_ == node_t<C>::type_t::control) {
			for (auto const& y : node->children_) {
				y->terminal_ = node->bypassable_;
				x->children_.insert(y);
			}

			x->terminal_ = x->terminal_ || node->bypassable_;
		}
		// otherwise, append the node itself
		else {
			x->children_.insert(node);
			// also, append a bypassable node's children in addition to itself
			if (node->bypassable_) {
				for (auto const& y : node->children_) {
					x->children_.insert(y);
				}
			}
		}
	}
	else
	{
		// if we're a terminal node, or any of our children are
		// a bypassable node, then we need to merge @node into our children
		if (x->terminal() || std::any_of(x->children_.begin(), x->children_.end(), [](node_ptr const& y) { return y->bypassable_; })) {
			merge_into_children(x, node);
			x->terminal_ = false;
		}
		
		// 
		// 
		// 
		node_t<C>::children_t old_children = x->children_;
		node_t<C>::children_t new_children;
		for (auto const& y : old_children)
		{
			node_ptr n = y;
			append_impl(visited, n, node);
			new_children.insert(n);
		}
		x->children_ = std::move(new_children);
	}
}



//=====================================================================
// merge_into_children
//=====================================================================
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


template <typename node_ptr_tm, typename FN>
void for_each_depth_first(std::set<node_ptr_tm>& visited, node_ptr_tm const& root, FN fn)
{
	std::stack<node_ptr_tm> nodes;
	nodes.push(root);
	while (!nodes.empty())
	{
		auto x = nodes.top();
		nodes.pop();
		if (visited.find(x) != visited.end())
			continue;
		visited.insert(x);
		typename node_ptr_tm::element_type::children_t children = x->children_;
		// call fn, which returns the new acc
		fn(x);

		for (auto const& y : children) {
			nodes.push(y);
		}
	}
}

template <typename node_ptr_tm, typename FN>
void for_each_depth_first(node_ptr_tm const& root, FN fn)
{
	std::set<node_ptr_tm> visited;
	for_each_depth_first(visited, root, fn);
}

template <typename C, typename acc_tm, typename FN>
void accumulate_depth_first(std::shared_ptr<node_t<C>> const& root, acc_tm acc, FN fn)
{
	typedef std::tuple<std::shared_ptr<node_t<C>>, acc_tm> value_t;
	std::stack<value_t> nodes;
	nodes.push(std::make_tuple(root, acc));
	std::set<std::shared_ptr<node_t<C>>> visited;
	while (!nodes.empty())
	{
		auto x = nodes.top();
		auto const& xn = std::get<0>(x);
		auto const& xa = std::get<1>(x);
		nodes.pop();
		if (visited.find(xn) != visited.end())
			continue;
		visited.insert(xn);

		typename std::shared_ptr<node_t<C>>::element_type::children_t children = xn->children_;
			
		// call fn, which returns the new acc
		acc_tm combined_acc = fn(xa, xn);

		for (auto const& y : children) {
			nodes.push( std::make_tuple(y, combined_acc) );
		}
	}
}







