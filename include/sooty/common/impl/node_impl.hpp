

template <typename Command>
std::map<node_t<Command>*, std::set<node_t<Command>*>> node_t<Command>::cloned_nodes_;

template <typename Command>
std::map<node_t<Command>*, node_t<Command>*> node_t<Command>::cloner_node_;



// constructors
template <typename Command>
node_t<Command>::node_t()
{
}

template <typename Command>
node_t<Command>::node_t(const node_t<Command>& rhs)
: commands_(rhs.commands_), unchosen_(rhs.unchosen_)
{	
}

template <typename Command>
node_t<Command>::node_t(node_t<Command>&& rhs)
: commands_(std::move(rhs.commands_)), children_(std::move(rhs.children_))
{
}

template <typename Command>
node_t<Command>::~node_t()
{
	if (cloner_node_.find(this) != cloner_node_.end()) {
		cloned_nodes_[cloner_node_[this]].erase(this);
	}

	// tell all our children we no longer exist
	if (cloned_nodes_.find(this) != cloned_nodes_.end()) {
		for (auto& x : cloned_nodes_[this]) {
			cloner_node_.erase(x);
		}
	}

	cloned_nodes_.erase(this);
}

template <typename Command>
auto node_t<Command>::operator = (node_t<Command> const& rhs) -> node_t<Command>& {
	children_t tmp = children_;

	children_ = rhs.children_;
	commands_ = rhs.commands_;
	
	for (auto& x : tmp) {
		append(x);
	}

	if (cloned_nodes_.find(this) != cloned_nodes_.end()) {
		for (auto& x : cloned_nodes_[this]) {
			*x = rhs;
		}
	}

	return *this;
}

template <typename Command>
auto node_t<Command>::operator = (node_t<Command>&& rhs) -> node_t<Command>& {
	commands_.swap(rhs.commands_);
	children_.swap(rhs.children_);
	return *this;
}

template <typename Command>
auto node_t<Command>::clone() -> node_ptr {
	node_ptr C(new node_t(*this));
	cloned_nodes_[this].insert(C.get());
	cloner_node_[C.get()] = this;
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
auto node_t<Command>::append(node_ptr const& node) -> node_ptr
{
	std::set<node_ptr> visited;
	return append_impl(visited, node);
}

template <typename Command>
auto node_t<Command>::append_self() -> node_ptr {
	return append(shared_from_this());
}

// adding commands
template <typename Command>
auto node_t<Command>::push_back_command(const command_t& command) -> node_ptr {
	commands_.push_back( std::make_pair(true, command) );
	return shared_from_this();
}
		
template <typename Command>
auto node_t<Command>::push_back_failure(const command_t& command) -> node_ptr {
	commands_.push_back( std::make_pair(false, command) );
	return shared_from_this();
}

template <typename Command>
auto node_t<Command>::push_back_action(bool good, const command_t& command) -> node_ptr {
	commands_.push_back( std::make_pair(good, command) );
	return shared_from_this();
}

template <typename Command>
auto node_t<Command>::append_impl(std::set<node_ptr>& visited, node_ptr node) -> node_ptr
{
	if (visited.find(shared_from_this()) != visited.end())
		return shared_from_this();
			
	visited.insert(shared_from_this());
			
	if (children_.empty()) {
		//if (!node->commands_.empty() && node->children_.empty())
			//commands_.insert(commands_.end(), node->commands_.begin(), node->commands_.end());
		//else 
			children_.insert(node);
	}
	else {
		std::for_each(children_.begin(), children_.end(), std::bind(&node_t::append_impl, std::placeholders::_1, std::ref(visited), std::ref(node)));
	}
			
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
	
	// @lhs and @rhs were completely merged together, we can recurse through our children
	if (!combined_commands.empty() && new_lhs_commands.empty() && new_rhs_commands.empty())
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
			ordering_t()
		);

		children_.swap(new_children);
	}
	// @rhs was completely merged into @lhs, but has commands left over. merge it into any of our children
	else if (!combined_commands.empty() && new_lhs_commands.empty() && !new_rhs_commands.empty())
	{
		commands_.swap(combined_commands);
		rhs->commands_.swap(new_rhs_commands);
		
		children_t new_children;
		atma::merge(
			children_.begin(), children_.end(),
			rhs->children_.begin(), rhs->children_.end(),
			std::inserter(new_children, new_children.end()),
			std::bind(&node_t<Command>::merge, std::placeholders::_1, std::ref(rhs)),
			[&new_children](node_ptr const& n){ new_children.insert(n); },
			[&new_children](node_ptr const& n){ new_children.insert(n); },
			ordering_t()
		);

		children_.swap(new_children);
	}
	// no merging took place. we're going to create a parent node and insert both
	// @lhs and @rhs as children, then replace ourselves with the parent, careful to
	// avoid cycles
	else if (combined_commands.empty())
	{
		// create a clone of us with our commands and children
		node_ptr sub_lhs = make();
		sub_lhs->commands_.swap(commands_);
		sub_lhs->children_.swap(children_);
		children_.insert(sub_lhs);
		children_.insert(rhs);
	}
	else {
		ATMA_ASSERT(false && "yeah probs forgot a use-case");
	}

	return shared_from_this();
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
