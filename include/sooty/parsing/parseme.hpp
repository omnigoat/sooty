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
	
	//=====================================================================
	// forward declares
	//=====================================================================
	namespace detail {
		struct parseme_backend;
		typedef boost::shared_ptr<parseme_backend> parseme_backend_ptr;
	}
	
	struct parseme;
	typedef parseme& parseme_ref;
	typedef const parseme& const_parseme_ref;
	
	
	
	
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
		const lexing::lexeme_t* lexeme() const;
		parsemes_t& children();
	};
	
//=====================================================================
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

