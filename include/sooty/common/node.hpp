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
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
//=====================================================================
namespace sooty {
namespace common {
//=====================================================================
	
	//=====================================================================	
	// forward declares
	//=====================================================================
	template <typename Command, typename Orders> struct node_t;
	

	
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
			nothing,
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
	template <typename Command, typename Orders = no_orders>
	struct node_t : boost::enable_shared_from_this<node_t<Command, Orders> >
	{
		//typedef node_t<Command, Orders> node_t;
		typedef boost::shared_ptr<node_t> node_ptr;
		typedef const node_ptr& const_node_ptr_ref;
		typedef Command command_t;
		
		typedef Orders orders_t;
		typedef orders_t& orders_ref;
		
		typedef std::deque< std::pair<bool, command_t> > commands_t;
		typedef const commands_t& const_commands_ref;
		typedef commands_t& commands_ref;
		
		typedef std::vector<node_ptr> children_t;
		typedef const children_t& const_children_ref;
		
		
		node_ptr add_child(const_node_ptr_ref n) {
			children_.push_back(n);
			return shared_from_this();
		}
		
		node_ptr push_back_command(const command_t& command) {
			commands_.push_back( std::make_pair(true, command) );
			return shared_from_this();
		}
		
		node_ptr push_back_command_ex(bool good, const command_t& command) {
			commands_.push_back( std::make_pair(good, command) );
			return shared_from_this();
		}
		
		
		node_ptr clone() const {
			node_ptr C(new node_t(combination_));
			C->children_.insert(C->children_.end(), children_.begin(), children_.end());
			C->commands_.insert(C->commands_.end(), commands_.begin(), commands_.end());
			return C;
		}
		
		
		bool perform(orders_ref orders) const
		{
			commands_t::const_iterator i = commands_.begin();
			commands_t::const_reverse_iterator first_failure = commands_.rend();
			
			for ( ; i != commands_.end(); ++i) {
				if (!i->first) {
					if (first_failure == commands_.rend())
						first_failure = commands_t::const_reverse_iterator(i);
					continue;
				}
				
				if (!i->second(orders))
					break;
			}
			
			// if we failed, perform all failures
			if (i != commands_.end()) {
				// first, move forward to the first non-failure command following i
				commands_t::const_reverse_iterator failure =
					commands_t::const_reverse_iterator(
						std::find_if(i, commands_.end(), detail::is_not_failure<command_t>() )
					);
				
				for ( ; failure != first_failure; ++failure) {
					if (failure->first)
						continue;
					
					failure->second(orders);
				}
				
				return false;
			}
			
			switch (combination_) {
				case combination_t::one:
					return do_one(orders);
				
				case combination_t::all:
					return do_all(orders);
			}
			
			return true;
		}
		
		
		
		
		
		
		static node_ptr make(const combination_t::Enum& combination = combination_t::nothing) {
			return node_ptr(new node_t(combination));
		}
		
		static node_ptr seq_and(const_node_ptr_ref lhs, const_node_ptr_ref rhs) {
			if (lhs->combination_ == combination_t::nothing) {
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
			node_ptr result = make(combination_t::one),
					 lhs = o_lhs->clone(),
					 rhs = o_rhs->clone()
					 ;
			
			if (lhs->combination_ == combination_t::one) {
				merge_commands_into(result->commands_, lhs->commands_, rhs->commands_);
				
				if (rhs->combination_ == combination_t::nothing) {
					if (lhs->commands_.empty()) {
						result->children_.swap(lhs->children_);
						result->children_.push_back(rhs);
					}
					else {
						result->children_.push_back(lhs);
						result->children_.push_back(rhs);
					}
				}
				else if (rhs->combination_ == combination_t::one) {
					if (lhs->commands_.empty() && rhs->commands_.empty()) {
						result->children_.swap(lhs->children_);
						result->children_.insert(result->children_.end(), rhs->children_.begin(), rhs->children_.end());
					}
					else if (lhs->commands_.empty()) {
						result->children_.swap(lhs->children_);
						result->children_.push_back(rhs);
					}
					else if (rhs->commands_.empty()) {
						result->children_.swap(rhs->children_);
						result->children_.push_back(lhs);
					}
					else {
						result->children_.push_back(lhs);
						result->children_.push_back(rhs);
					}
				}
				
				return result;
			}
			else if (rhs->combination_ == combination_t::one) {
				return node_t::one(o_rhs, o_lhs);
			}
			else {
				merge_commands_into(result->commands_, lhs->commands_, rhs->commands_);
				
				result->add_child(lhs);
				result->add_child(rhs);
			}
			
			return result;
		}
		
		
		
		
	private:
		node_t(const combination_t::Enum& combination)
			: combination_(combination)
		{
		}
		
		bool do_all(orders_ref orders) const {
			for (children_t::const_iterator i = children_.begin(); i != children_.end(); ++i) {
				if ( !(*i)->perform(orders) )
					return false;
			}
			
			return true;
		}
		
		bool do_one(orders_ref orders) const {
			children_t::const_iterator performer = children_.begin();
			for ( ; performer != children_.end(); ++performer) {
				if ( (*performer)->perform(orders) )
					return true;
			}
			
			return false;
		}
		
		
		static void merge_commands_into(commands_ref output, commands_ref lhs, commands_ref rhs)
		{
			commands_t::iterator
			  lhsi = lhs.begin(),
			  rhsi = rhs.begin()
			  ;
			
			while (lhsi != lhs.end() && rhsi != rhs.end() && lhsi->first == rhsi->first && lhsi->second == rhsi->second) {
				++lhsi;
				++rhsi;
			}
			
			if (&output != &lhs) {
				output.assign(lhs.begin(), lhsi);
				lhs.erase(lhs.begin(), lhsi);
			}
			
			rhs.erase(rhs.begin(), rhsi);
		}

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

