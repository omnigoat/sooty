//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_PARSEMES_HPP
#define SOOTY_PARSING_PARSEMES_HPP
//=====================================================================
#include <vector>
//=====================================================================
#include <sooty/parsing/parseme.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
//=====================================================================
	
	struct parsemes_t
	{
		typedef std::vector<parseme> internal_container_t;
		typedef internal_container_t::iterator iterator;
		typedef internal_container_t::const_iterator const_iterator;

		// constructors
		parsemes_t();

		// accessors
		auto size() const -> size_t;
		auto begin() const -> const_iterator;
		auto end() const -> const_iterator;
		//auto cbegin() const -> const_iterator;
		//auto cend() const -> const_iterator;

		// mutators
		auto begin() -> iterator;
		auto end() -> iterator;
		auto front() -> parseme_ref;
		auto back() -> parseme_ref;
		auto push_back(parseme_ref n) -> void;
		auto pop_back() -> void;
		auto clear() -> void;
		auto insert(iterator where_, const_parseme_ref n) -> iterator;
		auto erase(iterator where_) -> iterator;
		auto erase(iterator from, iterator to) -> iterator;
		
		template <typename IT>
		auto insert(iterator where_, IT begin, IT end) -> void {
			parsemes_.insert(where_, begin, end);
		}
		
		
	private:
		detail::parseme_backend_ptr owner;
		internal_container_t parsemes_;

		friend struct detail::parseme_backend;
		friend struct parseme;
	};

	auto operator << (std::ostream&, parsemes_t const&) -> std::ostream&;
	
//=====================================================================
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

