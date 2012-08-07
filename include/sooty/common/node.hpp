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
	// combination
	//=====================================================================
	struct combination_t {
		enum Enum {
			all,
			one
		};
	};
	
	

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
		
		combination_t::Enum combination() const {
			return combination_;
		}
		
		node_ptr add_child(const_node_ptr_ref n) {
			children_.push_back(n);
			return shared_from_this();
		}
		
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
		
		node_ptr clone() const {
			node_ptr C(new node_t(combination_));
			C->children_.insert(C->children_.end(), children_.begin(), children_.end());
			C->commands_.insert(C->commands_.end(), commands_.begin(), commands_.end());
			C->success = success;
			return C;
		}
		
		node_ptr on_success(const_node_ptr_ref S) {
			node_ptr new_lhs = this->clone();
			assert(!success);
			new_lhs->success = S;
			return new_lhs;
		}
		
		
		static node_ptr make(bool is_terminal = false) {
			return make_seq_and(is_terminal);
		}
		
		static node_ptr make_seq_and(bool is_terminal = false) {
			return node_ptr(new node_t(combination_t::all, is_terminal));
		}
		
		static node_ptr make_logical_or(bool is_terminal = false) {
			return node_ptr(new node_t(combination_t::one, is_terminal));
		}
		
		
		static node_ptr seq_and(const_node_ptr_ref lhs, const_node_ptr_ref rhs) {
			if (lhs->combination_ == combination_t::all) {
				node_ptr new_lhs = lhs->clone();
				
				if (new_lhs->commands_.back().second.is_terminal()) {
					new_lhs->commands_.pop_back();
				}
				
				new_lhs->commands_.insert(
					new_lhs->commands_.end(),
					rhs->commands_.begin(),
					rhs->commands_.end()
				);
				
				return new_lhs;
			}
			else {
				lhs->success = rhs;
			}
			
			return node_ptr();
		}
		
		static node_ptr one(const_node_ptr_ref o_lhs, const_node_ptr_ref o_rhs)
		{
			node_ptr lhs = o_lhs->clone(),
					 rhs = o_rhs->clone()
					 ;
			
			merge(lhs, rhs);
			
			return lhs;
		}
		
		// returns true if merging occurred
		static bool merge(node_ptr_ref lhs, const_node_ptr_ref rhs)
		{
			commands_t combined_commands;
			commands_t new_lhs_commands, new_rhs_commands;
			
			// perform a non-mutative merge
			merge_commands(combined_commands, new_lhs_commands, new_rhs_commands, lhs->commands_, rhs->commands_);
			
			if (combined_commands.empty()) {
				return false;
			}
			// at least something was merged. recurse! :D
			else {
				rhs->commands_.swap(new_rhs_commands);
				
				children_t::iterator child =
					std::find_if(
						lhs->children_.begin(),
						lhs->children_.end(),
						boost::bind(&node_t::merge, _1, boost::ref(rhs))
					);
				
				// the problem of merging has been pushed down one level. we're done!
				if (child != lhs->children_.end())
					return true;
			}
			
			// we now mutate lhs, because there's *still* stuff left, we're going to join it.
			// the following code optimizes the join by combining one-nodes.
			lhs->commands_.swap(new_lhs_commands);
			
			node_ptr result = make_logical_or();
			result->commands_.swap(combined_commands);
			
			if (lhs->combination_ == combination_t::one && lhs->commands_.empty())
				result->children_.swap(lhs->children_);
			else
				result->children_.push_back(lhs);
			
			if (rhs->combination_ == combination_t::one && rhs->commands_.empty())
				result->children_.insert(result->children_.end(), rhs->children_.begin(), rhs->children_.end());
			else
				result->children_.push_back(rhs);
			
			
			lhs = result;
			
			return true;
		}
		
		
		
		static bool is_failure(const std::pair<bool, command_t>& C) {
			return !C.first;
		}
		
		static bool is_command(const std::pair<bool, command_t>& C) {
			return C.first;
		}
		
	private:
		node_t(const combination_t::Enum& combination, bool is_terminal = false)
			: combination_(combination), is_terminal(is_terminal)
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
		node_ptr success, failure;
		
		combination_t::Enum combination_;
		
		commands_t commands_;
		
		children_t children_;
	};

//=====================================================================
} // namespace common
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

