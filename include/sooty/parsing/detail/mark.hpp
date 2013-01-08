//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_PARSING_MARK_HPP
#define SOOTY_PARSING_MARK_HPP
//=====================================================================
#include <set>
#include <map>
#include <memory>
//=====================================================================
namespace sooty {
namespace parsing {
namespace detail {
//=====================================================================
	
	//=====================================================================
	// forward declares
	//=====================================================================
	struct command_t;
	
	struct mark_internal_t;
	typedef std::shared_ptr<mark_internal_t> mark_t;
	typedef mark_t const& const_mark_ref;
	

	//=====================================================================
	// mark
	//=====================================================================
	struct mark_internal_t
	{
		mark_internal_t(size_t id); 
		
		auto add_command(command_t* const c) -> void;
		auto remove_command(command_t* const c) -> void;
		auto replace_self_with(const_mark_ref M) -> void;
	
	public:
		const size_t id;
		
	private:
		typedef std::set<command_t* const> commands_t;
		commands_t commands_;
	};
	
	inline bool operator == (const mark_internal_t& lhs, const mark_internal_t& rhs) {
		return lhs.id == rhs.id;
	}
	
		
	//=====================================================================
	// generation functions
	//=====================================================================
	static inline mark_t generate_mark() {
		static size_t _ = 0;
		return mark_t(new mark_internal_t(++_));
	}
	
	static inline mark_t mapped_mark(const_mark_ref mark) {
		static std::map<mark_t, mark_t> _;
		if (_.find(mark) == _.end())
			_[mark] = generate_mark();
		
		return _[mark];
	}
	
	
//=====================================================================
} // namespace detail
} // namespace parsing
} // namespace sooty
//=====================================================================
#endif
//=====================================================================

