#include <atma/algorithm.hpp>
#include <atma/assert.hpp>
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
: type_(rhs.type_), commands_(rhs.commands_), unchosen_(rhs.unchosen_)
{	
}

template <typename Command>
node_t<Command>::node_t(type_t type)
: type_(type)
{
}

template <typename Command>
sooty::common::node_t<Command>::node_t( type_t type, std::initializer_list<node_ptr> children )
: type_(type), children_(children.begin(), children.end())
{
	std::sort(children_.begin(), children_.end(), ordering_t());
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
auto node_t<Command>::operator = (node_t<Command>& rhs) -> node_t<Command>&
{
	ATMA_ASSERT_MSG(false, "not yet implemented");
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
auto node_t<Command>::add_child(node_ptr& n) -> node_ptr
{
	detail::link(shared_from_this(), n);
	return shared_from_this();
}

template <typename Command>
template <typename IT>
auto node_t<Command>::add_children(IT begin, IT end) -> node_ptr
{
	for (auto i = begin; i != end; ++i)
		add_child(*i);

	return shared_from_this();
}

template <typename Command>
auto node_t<Command>::add_self_as_child() -> node_ptr {
	return add_child(shared_from_this());
}

template <typename Command>
auto node_t<Command>::remove_child(node_ptr& n) -> node_ptr {
	detail::unlink(shared_from_this(), n);
	return shared_from_this();
}

template <typename Command>
auto node_t<Command>::append(node_ptr& node) -> node_ptr
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
		detail::link(shared_from_this(), node);
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
				new_children.push_back(n);
				std::copy(++i, children_.end(), std::inserter(new_children, new_children.end()));
				break;
			}

			new_children.push_back(*i);
		}
		children_.swap(new_children);
				
		// the problem of merging has been pushed down one level. we're done!
		if (new_children.empty()) {
			std::sort(children_.begin(), children_.end(), ordering_t());
			return shared_from_this();
		}
	}
	
	node_ptr result = node_t::make(type_t::or_);
	result->commands_.swap(combined_commands);

	// we merged absolutely everything, so maybe we can merge our children?
	if (!result->commands_.empty() && new_lhs_commands.empty() && new_rhs_commands.empty()
	  && !children_.empty() && !rhs->children_.empty())
	{
		// merge requires sorted vectors
		std::sort(result->children_.begin(), result->children_.end(), ordering_t());

		atma::merge(
			children_.begin(), children_.end(),
			rhs->children_.begin(), rhs->children_.end(),
			std::back_inserter(result->children_),
			std::bind(&node_t<Command>::merge, std::placeholders::_1, std::placeholders::_2),
			std::bind(&node_t<Command>::add_child, std::ref(result), std::placeholders::_1),
			std::bind(&node_t<Command>::add_child, std::ref(result), std::placeholders::_1)
		);

		return result;
	}

	result->children_.push_back(shared_from_this());
	result->children_.push_back(rhs);
	
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
