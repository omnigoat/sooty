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
		friend struct parseme;
		typedef size_t id_t;
		
	private:
		id_t id_;
		// parsemes created directly from lexems point to the lexeme
		const lexing::lexeme_t* lexeme_;
		parsemes_t children_;
		parseme_backend_ptr parent_;
		
		// private constructors! :O
		// only allowed to be created by friends (parseme)
		parseme_backend();
		parseme_backend(id_t id, const lexing::lexeme_t* lexeme = NULL);
		parseme_backend(parseme_backend_ptr parent, id_t id);
		parseme_backend(parseme_backend_ptr parent, id_t id, const lexing::lexeme_t* lexeme);
		
	public:
		id_t id() const;
		lexing::position_t position() const;
		const lexing::lexeme_t* lexeme() const;
		
		std::string text() const;
		int integer() const;
		
		parseme_backend_ptr parent() const;
		void set_parent(parseme_backend_ptr parent);
		
		parsemes_t& children();
		const parsemes_t& children() const;
	};
	
	typedef std::vector<parseme_backend_ptr> parseme_backends_t;
	
//=====================================================================
} // namespace detail
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

