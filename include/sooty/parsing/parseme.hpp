//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_PARSEME_HPP
#define SOOTY_PARSING_PARSEME_HPP
//=====================================================================
#include <sooty/lexing/lexeme.hpp>
//=====================================================================
#include <vector>
#include <memory>
//=====================================================================
namespace sooty {
namespace parsing {
//=====================================================================
	
	//=====================================================================
	// forward declares
	//=====================================================================
	namespace detail {
		struct parseme_backend;
		typedef std::shared_ptr<parseme_backend> parseme_backend_ptr;
	}
	
	struct parseme;
	typedef parseme& parseme_ref;
	typedef const parseme& const_parseme_ref;
	struct parsemes_t;
	
	
	//=====================================================================
	//
	//  parseme
	//  ---------
	//    A parseme is a node in our abstract syntax tree. It is effectively
	//    a glorified smart-pointer, and should be treated as such. It
	//    points to parseme_backend, containing all the actual data, and has
	//    several accessors and mutators.
	//
	//=====================================================================
	struct parseme
	{
		typedef size_t id_t;
		typedef lexing::position_t position_t;

		// constructors
		parseme();
		parseme(id_t id);
		parseme(const_parseme_ref parent, id_t id, const lexing::lexeme_t* lexeme);
		
		// accessors
		auto id() const -> id_t;
		auto position() const -> position_t;
		auto text() const -> std::string;
		auto integer() const -> int;
		auto lexeme() const -> lexing::lexeme_t const*;
		auto parent() const -> parseme;
		auto children() const -> parsemes_t const&;
		
		// mutators
		auto children() -> parsemes_t&;
		auto set_parent(const_parseme_ref parent) -> void;
		
	private:
		detail::parseme_backend_ptr backend_;
		parseme(detail::parseme_backend_ptr backend);

		friend struct parsemes_t;
	};

	auto operator << (std::ostream&, parseme const&) -> std::ostream&;
	
//=====================================================================
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

