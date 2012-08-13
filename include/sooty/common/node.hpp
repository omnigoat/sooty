//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_COMMON_NODE_HPP
#define SOOTY_COMMON_NODE_HPP
//=====================================================================
#include <vector>
#include <deque>
//=====================================================================
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <sooty/common/detail/clone_impl.hpp>
//=====================================================================
namespace sooty {
namespace common {
//=====================================================================
	
	//=====================================================================	
	// forward declares
	//=====================================================================
	template <typename Command> struct node_t;

	
	//=====================================================================
	// predicates
	//=====================================================================
	namespace detail {
		template <typename Command>
		struct is_failure {
			bool operator () (const std::pair<bool, Command>& p) {
				return !p.first;
			}
		};
		
		template <typename Command>
		struct is_not_failure {
			bool operator () (const std::pair<bool, Command>& p) {
				return p.first;
			}
		};
	}
	
	
	//=====================================================================
	// no_orders
	//=====================================================================
	struct no_orders {};
	
	
	
	
	//=====================================================================
	// node
	//=====================================================================
	template <typename Command>
	struct node_t : boost::enable_shared_from_this<node_t<Command> >
	{
		template <typename ExecutorT>
		friend struct performer_t;
		template <typename NodePtr>
		friend NodePtr detail::clone_tree_impl(std::map<NodePtr, NodePtr>& visited_nodes, const NodePtr& clonee);
		
		typedef boost::shared_ptr<node_t> node_ptr;
		typedef node_ptr& node_ptr_ref;
		typedef const node_ptr& const_node_ptr_ref;
		typedef Command command_t;
		
		typedef std::deque< std::pair<bool, command_t> > commands_t;
		typedef const commands_t& const_commands_ref;
		typedef commands_t& commands_ref;
		
		// sort nodes descending by number of commands
		struct ordering_t {
			bool operator () (const_node_ptr_ref lhs, const_node_ptr_ref rhs) const {
				return rhs->commands_.size() < lhs->commands_.size();
			};
		};
		
		typedef std::multiset<node_ptr, ordering_t> children_t;
		typedef const children_t& const_children_ref;
		
		
		// cloning!
		node_ptr clone() const {
			// purposefully doesn't clone next_. use detail::clone_tree for that
			node_ptr C(new node_t(*this));
			
			return C;
		}
		
		
		// adding children
		node_ptr add_child(const_node_ptr_ref n) {
			children_.insert(n);
			return shared_from_this();
		}
		
		node_ptr add_self_as_child() {
			children_.insert( shared_from_this() );
			return shared_from_this();
		}
		
		node_ptr append_self() {
			return append(shared_from_this());
		}
		
		// adding commands
		node_ptr push_back_command(const command_t& command) {
			commands_.push_back( std::make_pair(true, command) );
			return shared_from_this();
		}
		
		node_ptr push_back_failure(const command_t& command) {
			commands_.push_back( std::make_pair(false, command) );
			return shared_from_this();
		}
		
		node_ptr push_back_action(bool good, const command_t& command) {
			commands_.push_back( std::make_pair(good, command) );
			return shared_from_this();
		}
		
		
		// appending 
		node_ptr append(const_node_ptr_ref node)
		{
			std::set<node_ptr> visited;
			return append_impl(visited, node);
		}
		
		node_ptr append_impl(std::set<node_ptr>& visited, node_ptr node) {
			if (visited.find(shared_from_this()) != visited.end())
				return shared_from_this();
			
			visited.insert(shared_from_this());
			
			if (children_.empty()) {
				children_.insert(node);
			}
			else {
				std::for_each(children_.begin(), children_.end(), boost::bind(&node_t::append_impl, _1, boost::ref(visited), boost::ref(node)));
			}
			
			return shared_from_this();
		}
		
		
		// returns true if merging occurred
		node_ptr merge(const_node_ptr_ref rhs)
		{
			commands_t combined_commands;
			commands_t new_lhs_commands, new_rhs_commands;
			commands_t lhs_unchosen, rhs_unchosen;
			
			// perform a merge
			merge_commands(
				combined_commands,
				new_lhs_commands,
				new_rhs_commands,
				lhs_unchosen,
				rhs_unchosen,
				commands_,
				rhs->commands_
			);
			
			// we merged all the commands @this has, so recurse into the children
			if (!combined_commands.empty() && new_lhs_commands.empty()) {
				rhs->commands_.swap(new_rhs_commands);
				
				children_t::iterator i = children_.begin();
				for (; i != children_.end(); ++i) {
					node_ptr n = (*i)->merge(rhs);
					if (n) {
						*i = n;
						break;
					}
				}
				
				// the problem of merging has been pushed down one level. we're done!
				if (i != children_.end())
					return shared_from_this();
			}
			
			// we now mutate lhs, because there's *still* stuff left, we're going to join it.
			// the following code optimizes the join by combining one-nodes.
			commands_.swap(new_lhs_commands);
			this->unchosen_.swap(lhs_unchosen);
			
			rhs->commands_.swap(new_rhs_commands);
			rhs->unchosen_.swap(rhs_unchosen);
			
			node_ptr result = make();
			result->commands_.swap(combined_commands);
			
			if (commands_.empty())
				result->children_.swap(children_);
			else
				result->children_.insert(shared_from_this()->clone());
			
			if (rhs->commands_.empty())
				result->children_.insert(rhs->children_.begin(), rhs->children_.end());
			else
				result->children_.insert(rhs);
			
			
			return result;
		}
		
		
		
		static node_ptr make(bool is_terminal = false) {
			return node_ptr(new node_t(is_terminal));
		}
		
		
		
		
		
		
		
		static bool is_failure(const std::pair<bool, command_t>& C) {
			return !C.first;
		}
		
		static bool is_command(const std::pair<bool, command_t>& C) {
			return C.first;
		}
		
	private:
		node_t(bool is_terminal = false)
			: is_terminal(false)
		{
		}
		
		static void merge_commands(commands_ref combined, commands_ref new_lhs, commands_ref new_rhs,
			commands_ref lhs_unchosen, commands_ref rhs_unchosen, const_commands_ref lhs, const_commands_ref rhs)
		{
			commands_t::const_iterator
			  lhsi = lhs.begin(),
			  rhsi = rhs.begin()
			  ;
			
			while (lhsi != lhs.end() && rhsi != rhs.end())
			{
				// push back lhs sentinels
				while (lhsi->second.is_sentinel() && lhsi != lhs.end())
					combined.push_back(*lhsi++);
				
				// push back rhs sentinels
				while (rhsi->second.is_sentinel() && rhsi != rhs.end())
					combined.push_back(*rhsi++);
				
				if (lhsi == lhs.end() || rhsi == rhs.end() || lhsi->first != rhsi->first)
					break;
				
				// if these are commands
				if (lhsi->first) {
					if (lhsi->second == rhsi->second)
						combined.push_back(*lhsi);
					else
						break;
				}
				// a recourse
				else {
					lhs_unchosen.push_back(*lhsi);
					rhs_unchosen.push_back(*rhsi);
				}
				
				++lhsi;
				++rhsi;
			}
			
			new_lhs.assign(lhsi, lhs.end());
			new_rhs.assign(rhsi, rhs.end());
		}
		
	public:
		bool is_terminal;
		
	private:
		commands_t commands_;
		commands_t unchosen_;
		
		children_t children_;
	};
	

//=====================================================================
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

