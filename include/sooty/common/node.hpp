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
	
		//typedef node_t<Command, Orders> node_t;
		typedef boost::shared_ptr<node_t> node_ptr;
		typedef node_ptr& node_ptr_ref;
		typedef const node_ptr& const_node_ptr_ref;
		typedef Command command_t;
		
		typedef std::deque< std::pair<bool, command_t> > commands_t;
		typedef const commands_t& const_commands_ref;
		typedef commands_t& commands_ref;
		
		typedef std::vector<node_ptr> children_t;
		typedef const children_t& const_children_ref;
		
		
		// cloning!
		node_ptr clone() const {
			// purposefully doesn't clone next_. use detail::clone_tree for that
			node_ptr C(new node_t(is_terminal));
			C->children_.insert(C->children_.end(), children_.begin(), children_.end());
			C->commands_.insert(C->commands_.end(), commands_.begin(), commands_.end());
			return C;
		}
		
		
		// adding children
		node_ptr add_child(const_node_ptr_ref n) {
			children_.push_back(n);
			return shared_from_this();
		}
		
		node_ptr add_self_as_child() {
			children_.push_back( shared_from_this() );
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
				children_.push_back(node);
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
			
			// perform a non-mutative merge
			merge_commands(combined_commands, new_lhs_commands, new_rhs_commands, commands_, rhs->commands_);
			
			// at least something was merged. recurse! :D
			if (!combined_commands.empty()) {
				rhs->commands_.swap(new_rhs_commands);
				
				children_t::iterator child =
					std::find_if(
						children_.begin(),
						children_.end(),
						boost::bind(&node_t::merge, _1, boost::ref(rhs))
					);
				
				// the problem of merging has been pushed down one level. we're done!
				if (child != children_.end())
					return shared_from_this();
			}
			
			// we now mutate lhs, because there's *still* stuff left, we're going to join it.
			// the following code optimizes the join by combining one-nodes.
			commands_.swap(new_lhs_commands);
			
			node_ptr result = make();
			result->commands_.swap(combined_commands);
			
			if (commands_.empty())
				result->children_.swap(children_);
			else
				result->children_.push_back(shared_from_this());
			
			if (rhs->commands_.empty())
				result->children_.insert(result->children_.end(), rhs->children_.begin(), rhs->children_.end());
			else
				result->children_.push_back(rhs);
			
			
			*this = *result;
			
			return shared_from_this();
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
		
		static void merge_commands(commands_ref combined, commands_ref new_lhs, commands_ref new_rhs, const_commands_ref lhs, const_commands_ref rhs)
		{
			commands_t::const_iterator
			  lhsi = lhs.begin(),
			  rhsi = rhs.begin()
			  ;
			
			while (lhsi != lhs.end() && rhsi != rhs.end() && lhsi->first == rhsi->first && lhsi->second == rhsi->second) {
				++lhsi;
				++rhsi;
			}
			
			combined.assign(lhs.begin(), lhsi);
			new_lhs.assign(lhsi, lhs.end());
			new_rhs.assign(rhsi, rhs.end());
		}
		
		static typename commands_t::iterator intersection_of(commands_ref lhs, commands_ref rhs)
		{
			commands_t::iterator
			  lhsi = lhs.begin(),
			  rhsi = rhs.begin()
			  ;
			
			while (lhsi != lhs.end() && rhsi != rhs.end() && *lhsi == *rhsi) {
				++lhsi;
				++rhsi;
			}
			
			return lhsi;
		}
		
	public:
		bool is_terminal;
		
	private:
		commands_t commands_;
		
		children_t children_;
	};

//=====================================================================
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

