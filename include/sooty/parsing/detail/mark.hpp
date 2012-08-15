//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_MARK_HPP
#define SOOTY_PARSING_MARK_HPP
//=====================================================================
#include <set>
#include <boost/shared_ptr.hpp>
//=====================================================================
namespace sooty {
namespace parsing {
namespace detail {
//=====================================================================
	
	//=====================================================================
	// forward declares
	//=====================================================================
	struct command_t;
	
	
	//=====================================================================
	// mark
	//=====================================================================
	struct mark_internal_t
	{
		//friend bool operator == (const mark_internal_t&, const mark_internal_t&);
		
		mark_internal_t(size_t id) : id(id) {}
		
		void add_command(command_t* const c) {
			commands_.insert(c);
		}
		
		void remove_command(command_t* const c) {
			commands_.erase(c);
		}
	
	
	public:
		const size_t id;
		
	private:
		std::set<command_t* const> commands_;
	};
	
	bool operator == (const mark_internal_t& lhs, const mark_internal_t& rhs) {
		return lhs.id == rhs.id;
	}
	
	typedef boost::shared_ptr<mark_internal_t> mark_t;
	typedef const mark_t& const_mark_ref;
	
	
	//=====================================================================
	// generation functions
	//=====================================================================
	static inline mark_t generate_mark() {
		static size_t _ = 0;
		return mark_t(new mark_internal_t(++_));
	}
	
	
//=====================================================================
} // namespace detail
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

