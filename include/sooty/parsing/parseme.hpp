//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_PARSEME_HPP
#define SOOTY_PARSING_PARSEME_HPP
//=====================================================================
#include <vector>
//=====================================================================
#include <sooty/lexing/lexeme.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
//=====================================================================
	
	// forward declare
	namespace detail {
		struct parseme_backend;
		typedef boost::shared_ptr<parseme_backend> parseme_backend_ptr;
	}
	
	// forward declare us
	struct parseme;
	typedef parseme* parseme_ptr;
	typedef const parseme* const_parseme_ptr;
	typedef parseme& parseme_ref;
	typedef const parseme& const_parseme_ref;
	//typedef std::vector<parseme> parsemes_t;

	struct parseme
	{
		friend struct parsemes_t;
		
		typedef size_t id_t;
		
	private:
		detail::parseme_backend_ptr backend_;
		parseme(detail::parseme_backend_ptr backend);
		
	public:
		parseme();
		parseme(id_t id);
		parseme(const_parseme_ref parent, id_t id, const lexing::lexeme_t* lexeme);
		
		id_t id() const;
		lexing::position_t position() const;
		
		std::string text() const;
		int integer() const;
		
		parseme parent();
		void set_parent(const_parseme_ref parent);
		
		parsemes_t& children();
	};
	
//=====================================================================
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

