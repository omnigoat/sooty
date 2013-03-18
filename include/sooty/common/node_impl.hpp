


//
// constructors
//
template <typename Command>
node_t<Command>::node_t(type_t type)
	: type_(type), terminal_(), fallible_()
{
}

template <typename Command>
node_t<Command>::node_t(const node_t<Command>& rhs)
	: type_(rhs.type_), commands_(rhs.commands_), terminal_(rhs.terminal_), fallible_(rhs.fallible_)
{
}

template <typename Command>
node_t<Command>::~node_t()
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

template <typename Command>
auto node_t<Command>::operator = (node_t<Command> const& rhs) -> node_t<Command>&
{
	type_ = rhs.type_;
	commands_ = rhs.commands_;
	children_ = rhs.children_;
	terminal_ = rhs.terminal_;
	fallible_ = rhs.fallible_;
	return *this;
}

template <typename Command>
auto node_t<Command>::assume(node_t&& rhs) -> node_ptr
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

template <typename Command>
auto node_t<Command>::clone() -> node_ptr
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

template <typename Command>
auto node_t<Command>::add_child(node_ptr const& n) -> node_ptr {
	children_.insert(n);
	return shared_from_this();
}

template <typename Command>
auto node_t<Command>::add_self_as_child() -> node_ptr {
	return add_child(shared_from_this());
}

template <typename Command>
auto node_t<Command>::append_self() -> node_ptr {
	return append(shared_from_this());
}

// adding commands
template <typename Command>
auto node_t<Command>::push_back_command(const command_t& command) -> node_ptr {
	ATMA_ASSERT(type_ != type_t::placeholder);
	type_ = type_t::actor;
	commands_.push_back( std::make_pair(true, command) );
	return shared_from_this();
}
		
template <typename Command>
auto node_t<Command>::push_back_failure(const command_t& command) -> node_ptr {
	ATMA_ASSERT(type_ != type_t::placeholder);
	type_ = type_t::actor;
	commands_.push_back( std::make_pair(false, command) );
	return shared_from_this();
}

template <typename Command>
auto node_t<Command>::push_back_action(bool good, const command_t& command) -> node_ptr {
	ATMA_ASSERT(type_ != type_t::placeholder);
	type_ = type_t::actor;
	commands_.push_back( std::make_pair(good, command) );
	return shared_from_this();
}

template <typename Command>
auto node_t<Command>::merge(node_ptr const& rhs) -> node_ptr
{
	commands_t combined_commands;
	commands_t new_lhs_commands, new_rhs_commands;
			
	// perform a merge
	merge_commands(
		combined_commands,
		new_lhs_commands,
		new_rhs_commands,
		commands_,
		rhs->commands_
	);
	
	node_ptr result = shared_from_this();

	// neither @lhs nor @rhs actually had any commands
	// they can be merged if they're not placeholders or if they're placeholders but share ancestry
	if (combined_commands.empty() && new_lhs_commands.empty() && new_rhs_commands.empty() &&
	  ((type_ == type_t::control && rhs->type_ == type_t::control) ||
	  node_t::share_ancestry(result, rhs)))
	{
		children_.insert(rhs->children_.begin(), rhs->children_.end());
	}
	// @lhs and @rhs were completely merged together, we can recurse through our children
	else if (!combined_commands.empty() && new_lhs_commands.empty() && new_rhs_commands.empty())
	{
		commands_.swap(combined_commands);
		children_t new_children;

		atma::merge(
			children_.begin(), children_.end(),
			rhs->children_.begin(), rhs->children_.end(),
			std::inserter(new_children, new_children.end()),
			std::bind(&node_t<Command>::merge, std::placeholders::_1, std::placeholders::_2),
			[&new_children](node_ptr const& n){ new_children.insert(n); },
			[&new_children](node_ptr const& n){ new_children.insert(n); },
			merged_ordering_t()
		);

		children_.swap(new_children);

		// we have children, and one of our two branches was wholly consumed. we
		// are therefore a terminal.
		if (!children_.empty())
			terminal_ = true;
	}
	// @rhs was completely merged into @lhs, but has commands left over. merge it into any of our children
	else if (!combined_commands.empty() && new_lhs_commands.empty() && !new_rhs_commands.empty())
	{
		commands_.swap(combined_commands);
		rhs->commands_.swap(new_rhs_commands);
		
		children_t new_children;
		auto merge_failer = [&new_children](node_ptr const& n){ new_children.insert(n); };
		atma::merge(
			children_.begin(), children_.end(),
			rhs->children_.begin(), rhs->children_.end(),
			std::inserter(new_children, new_children.end()),
			std::bind(&node_t<Command>::merge, std::placeholders::_1, std::ref(rhs)),
			merge_failer, merge_failer,
			merged_ordering_t()
		);

		children_.swap(new_children);
	}
	// no merging took place. we're going to create a parent node and insert both
	// @lhs and @rhs as children, then replace ourselves with the parent, careful to
	// avoid cycles
	else if (combined_commands.empty())
	{
		if (type_ == type_t::control) {
			if (rhs->type_ == type_t::control) {
				children_.insert(rhs->children_.begin(), rhs->children_.end());
			}
			else {
				children_.insert(rhs);
			}
		}
		else {
			result = make();
			result->children_.insert(shared_from_this());
			result->children_.insert(rhs);
		}
	}
	else {
		ATMA_ASSERT(false && "yeah probs forgot a use-case");
	}
	
	return result;
}


template <typename Command>
auto node_t<Command>::merge_commands(commands_t& combined, commands_t& new_lhs, commands_t& new_rhs, commands_t& lhs, commands_t& rhs) -> void
{
	commands_t::iterator
		lhsi = lhs.begin(),
		rhsi = rhs.begin()
		;
			
	while (lhsi != lhs.end() && rhsi != rhs.end())
	{
		if (lhsi == lhs.end() || rhsi == rhs.end())
			break;
				
		// merging commands
		bool success = false;
		command_t merged_command = merged(lhsi->second, rhsi->second, success);
				
		// success!
		if (success) {
			combined.push_back( std::make_pair(lhsi->first, merged_command) );
			++lhsi;
			++rhsi;
		}
		// sentinels will merge together, but if only one of lhs or rhs is a sentinel,
		// then we prepend it to the combined commands
		else if (lhsi->second.is_sentinel())
			combined.push_back(*lhsi++);
		else if (rhsi->second.is_sentinel())
			combined.push_back(*rhsi++);
		// failure-commands are totes fine to be prepended too
		else if (!lhsi->first)
			combined.push_back(*lhsi++);
		else if (!rhsi->first)
			combined.push_back(*rhsi++);
		// anything else and we're done
		else
			break;
	}
			
	new_lhs.assign(lhsi, lhs.end());
	new_rhs.assign(rhsi, rhs.end());
}

template <typename Command>
auto node_t<Command>::clone_command(const std::pair<bool, command_t>& C) -> std::pair<bool, command_t> {
	return std::make_pair(C.first, C.second.clone());
}


//=====================================================================
// append
//=====================================================================
template <typename node_ptr_tm>
auto sooty::common::append(node_ptr_tm& x, node_ptr_tm const& node) -> node_ptr_tm&
{
	std::set<node_ptr_tm> visited;
	visited.insert(node);
	append_impl(visited, x, node);
	return x;
}

template <typename node_ptr_tm>
auto sooty::common::append_backref(node_ptr_tm& x, node_ptr_tm const& node) -> node_ptr_tm&
{
	std::set<node_ptr_tm> visited;
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

	if (x->children_.empty()) {
		x->children_.insert(node);
	}
	else
	{
		// if we're a terminal node, then we need to merge @node into our children
		if (x->terminal()) {
			merge_into_children(x, node);
		}

		// if we're fallible
		if (x->fallible_)
		{
			// create control @t to replace @x with
			node_ptr t = node_t<C>::make();
			t->add_child(x);
			t->add_child(node);
			x->fallible_ = false;

			// remove backreferences to ourself
			auto tmp_visited = visited;
			tmp_visited.erase(x);
			for_each_depth_first(tmp_visited, x, [&t, &x](node_ptr const& c) {
				if (c->children_.find(x) != c->children_.end()) {
					c->children_.erase(x);
					c->children_.insert(t);
				}
			});
			
			visited.erase(x);
			x = t;
		}
		
		node_t<C>::children_t tmp = x->children_;
		node_t<C>::children_t new_children;
		for (auto& y : tmp) {
			node_ptr n = y;
			append_impl(visited, n, node);
			new_children.insert(n);
		}
		std::swap(x->children_, new_children);
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












