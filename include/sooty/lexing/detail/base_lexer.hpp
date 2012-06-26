//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_DETAIL_BASE_LEXER_HPP
#define SOOTY_LEXING_DETAIL_BASE_LEXER_HPP
//=====================================================================
#include <set>
#include <map>
//=====================================================================
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
//=====================================================================
#include <sooty/lexing/input_iterator.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
namespace detail {
//=====================================================================
	
	struct lex_results_t
	{
		bool pass;
		input_iterator begin, end;
		
		lex_results_t(bool pass, const input_iterator& begin, const input_iterator& end) : pass(pass), begin(begin), end(end) {}
		
		lex_results_t& operator += (const lex_results_t& rhs) {
			this->pass = rhs.pass;
			this->end = rhs.end;
			return *this;
		}
		
		static lex_results_t success(const input_iterator& begin, const input_iterator& end) { return lex_results_t(true, begin, end); }
		static lex_results_t failure(const input_iterator& iter) { return lex_results_t(false, iter, iter); }
	};

	
	struct base_lexer;
	typedef boost::shared_ptr<base_lexer> base_lexer_ptr;
	typedef boost::function< void(lex_results_t&) > semantic_action;

	

	struct base_lexer
	{
		struct lexer_type {
			enum Enum {
				matcher,
				terminal,
				negator,
				marker,
				actor
			};
		};
		
		lexer_type::Enum type;
		char from, to;
		base_lexer_ptr on_success, on_failure;
		semantic_action action;
		
		static base_lexer_ptr terminal() {
			return base_lexer_ptr(new base_lexer(lexer_type::terminal, '\0', '\0'));
		}
		
		base_lexer(lexer_type::Enum type, char from, char to);
		base_lexer(lexer_type::Enum type, char from, char to, const base_lexer_ptr& on_success, const base_lexer_ptr& on_failure);
	};
	
	
	
	//=====================================================================
	// cloning
	//=====================================================================
	base_lexer_ptr clone_tree(const base_lexer_ptr& root);
	base_lexer_ptr clone_tree_impl(const base_lexer_ptr& root, std::map<base_lexer_ptr, base_lexer_ptr>& visited_nodes);


	//=====================================================================
	// equivalence
	//=====================================================================
	inline bool equivalent(const base_lexer_ptr& lhs, const base_lexer_ptr& rhs) {
		return lhs->type == rhs->type && lhs->from == rhs->from && lhs->to == rhs->to;
	}

	// lhs would match something that rhs could match
	// rhs doesn't necessarily accept as limited an input as lhs, and may
	// match things outside of lhs' purview
	inline bool partially_equivalent(const base_lexer_ptr& lhs, const base_lexer_ptr& rhs) {
		return lhs->type == rhs->type && lhs->from >= rhs->from && lhs->to <= rhs->to;
	}

	inline bool should_prepend(const base_lexer_ptr& L) {
		return L->type == base_lexer::lexer_type::marker;
	}

	//=====================================================================
	// combinatorics
	//=====================================================================
	void append_success(const base_lexer_ptr& lhs, const base_lexer_ptr& rhs);
	void append_success_impl(std::set<base_lexer_ptr>& visited, const base_lexer_ptr& lhs, const base_lexer_ptr& rhs);

	void append_failure(const base_lexer_ptr& lhs, const base_lexer_ptr& rhs);
	void append_failure_impl(std::set<base_lexer_ptr>& visited, const base_lexer_ptr& lhs, const base_lexer_ptr& rhs);

	void fold(const base_lexer_ptr& lhs, const base_lexer_ptr& rhs);
	void fold_impl(std::set<base_lexer_ptr> visited_nodes, const base_lexer_ptr& lhs, const base_lexer_ptr& rhs);

	inline void zero_or_more(const base_lexer_ptr& lhs) {
		append_success(lhs, lhs);
		append_failure(lhs, base_lexer::terminal());
	}
	
//=====================================================================
} // namespace detail
} // namespace lexing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

