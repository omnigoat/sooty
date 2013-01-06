//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_DETAIL_PARSEME_BACKEND_HPP
#define SOOTY_PARSING_DETAIL_PARSEME_BACKEND_HPP
//=====================================================================
#include <vector>
//=====================================================================
#include <boost/enable_shared_from_this.hpp>
//=====================================================================
#include <sooty/lexing/lexeme.hpp>
//=====================================================================
#include <sooty/parsing/parsemes.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
namespace detail {
//=====================================================================
	
	struct parseme_backend
	: boost::enable_shared_from_this<parseme_backend>
	{
		typedef size_t id_t;
		
		// accessors
		auto id() const -> id_t;
		auto position() const -> lexing::position_t;
		auto lexeme() const -> lexing::lexeme_t const*;
		auto text() const -> std::string;
		auto integer() const -> int;
		auto parent() const -> parseme_backend_ptr;
		auto children() const -> parsemes_t const&;

		// mutators
		auto children() -> parsemes_t&;
		auto set_parent(parseme_backend_ptr parent) -> void;
		
	private:
		parseme_backend();
		parseme_backend(id_t id, const lexing::lexeme_t* lexeme = NULL);
		parseme_backend(parseme_backend_ptr parent, id_t id);
		parseme_backend(parseme_backend_ptr parent, id_t id, const lexing::lexeme_t* lexeme);

		id_t id_;
		const lexing::lexeme_t* lexeme_;
		parsemes_t children_;
		parseme_backend_ptr parent_;

		friend struct parseme;
	};
	
	typedef std::vector<parseme_backend_ptr> parseme_backends_t;
	
//=====================================================================
} // namespace detail
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

