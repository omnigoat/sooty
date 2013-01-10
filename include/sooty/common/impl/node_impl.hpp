#include <atma/algorithm.hpp>
//=====================================================================
namespace sooty {
namespace common {
//=====================================================================

template <typename Command>
std::map<node_t<Command>*, std::set<node_t<Command>*>> node_t<Command>::cloned_nodes_;

template <typename Command>
std::map<node_t<Command>*, node_t<Command>*> node_t<Command>::cloner_node_;

// constructors
template <typename Command>
node_t<Command>::node_t(const node_t<Command>& rhs)
: is_terminal(rhs.is_terminal), commands_(rhs.commands_), unchosen_(rhs.unchosen_)
{	
}

template <typename Command>
node_t<Command>::node_t(bool is_terminal)
: is_terminal(false)
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
auto node_t<Command>::operator = (node_t<Command> const& rhs) -> node_t<Command>&
{
	children_t tmp = children_;

	std::set<node_ptr> visited;
	node_ptr n = shared_from_this();
	std::stack<node_ptr> nodes;
	nodes.push(n);
	while (!nodes.empty()) {
		node_ptr n = nodes.top();
		nodes.pop();
		for (auto& x : n->children_) {
			nodes.push(x->shared_from_this());
		}
		visited.insert(n);
	}

	children_ = rhs.children_;
	commands_ = rhs.commands_;

	for (auto& x : tmp) {
		append_impl(visited, x);
	}
	
	if (cloned_nodes_.find(this) != cloned_nodes_.end()) {
		for (auto& x : cloned_nodes_[this]) {
			*x = rhs;
		}
	}

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
auto node_t<Command>::add_child(const_node_ptr_ref n) -> node_ptr {
	children_.insert(n);
	return shared_from_this();
}

template <typename Command>
auto node_t<Command>::add_self_as_child() -> node_ptr {
	return add_child(shared_from_this());
}

template <typename Command>
auto node_t<Command>::append(const_node_ptr_ref node) -> node_ptr
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
		children_.insert(node);
	}
	else {
		std::for_each(children_.begin(), children_.end(), std::bind(&node_t::append_impl, std::placeholders::_1, std::ref(visited), std::ref(node)));
	}
			
	return shared_from_this();
}

template <typename Command>
auto node_t<Command>::merge(const_node_ptr_ref rhs) -> node_ptr
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
	
	// we merged all the commands @this has, so recurse into the children
	if (!combined_commands.empty() && !new_rhs_commands.empty() && new_lhs_commands.empty())
	{
		rhs->commands_.swap(new_rhs_commands);
				
		children_t new_children;
		children_t::iterator i = children_.begin();
		for (; i != children_.end(); ++i)
		{
			node_ptr n = (*i)->merge(rhs);

			// we successfully merged, so there's no need to process further.
			// however, we need to insert remaining (untouched) children.
			if (n) {
				new_children.insert(n);
				std::copy(++i, children_.end(), std::inserter(new_children, new_children.end()));
				break;
			}

			new_children.insert(*i);
		}
		children_.swap(new_children);
				
		// the problem of merging has been pushed down one level. we're done!
		if (new_children.empty())
			return shared_from_this();
	}
	
	node_ptr result = node_t::make();
	result->commands_.swap(combined_commands);

	// we merged absolutely everything, so maybe we can merge our children?
	if (!result->commands_.empty() && new_lhs_commands.empty() && new_rhs_commands.empty()
	  && !children_.empty() && !rhs->children_.empty())
	{
		auto x = children_.begin();
		auto y = rhs->children_.begin();

		auto on_fail_merge = [&result](const_node_ptr_ref n) {result->children_.insert(n);};

		atma::merge(
			children_.begin(), children_.end(),
			rhs->children_.begin(), rhs->children_.end(),
			std::inserter(result->children_, result->children_.end()),
			std::bind(&node_t<Command>::merge, std::placeholders::_1, std::placeholders::_2),
			on_fail_merge, on_fail_merge
		);

		return result;
	}

	result->children_.insert(shared_from_this());
	result->children_.insert(rhs);
	
	return result;
}


template <typename Command>
auto node_t<Command>::merge_commands(commands_ref combined, commands_ref new_lhs, commands_ref new_rhs, commands_ref lhs, commands_ref rhs) -> void
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
} // namespace common
} // namespace sooty
//=====================================================================
