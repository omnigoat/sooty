#include <sooty/parsing/detail/abstract_parser_backend.hpp>
//=====================================================================
#include <sooty/parsing/parser.hpp>
#include <sooty/common/detail/append_success.hpp>

//=====================================================================
// nop
//=====================================================================
bool sooty::parsing::detail::parsers::nop::applies( accumulator&, lexing::lexemes_t::const_iterator&, lexing::lexemes_t::const_iterator& )
{
	return true;
}

//=====================================================================
// superparser
//=====================================================================
bool sooty::parsing::detail::parsers::superparser::applies( accumulator& acc, lexing::lexemes_t::const_iterator& begin, lexing::lexemes_t::const_iterator& end )
{
	return detail::parse(acc, nop, begin, end);
}

int sooty::parsing::detail::parsers::superparser::debug( std::set<detail::abstract_parser_backend_ptr>& visited_nodes, int padding )
{
	for (int i = 0; i != padding; ++i) {
		std::cout << " ";
	}
	
	std::cout << "superparser" << std::endl;
	sooty::parsing::debug_impl(visited_nodes, nop, padding + 2);
	return padding;
}

void sooty::parsing::detail::parsers::superparser::assign_subparser( const abstract_parser_backend_ptr& p )
{
	sooty::common::detail::append_success(nop, p);
}


//=====================================================================
// match
//=====================================================================
bool sooty::parsing::detail::parsers::match::applies( accumulator& acc, lexing::lexemes_t::const_iterator& begin, lexing::lexemes_t::const_iterator& end )
{
	if (begin == end || (begin->id() < match_from) || (match_to < begin->id()))
		return false;

	if (mark.is_initialized()) {
		acc.add_marker(mark.get(), begin);
		acc.insert(0, &*begin);
	}

	++begin;
	return true;
}

int sooty::parsing::detail::parsers::match::debug( std::set<detail::abstract_parser_backend_ptr>&, int padding )
{
	for (int i = 0; i != padding; ++i) {
		std::cout << " ";
	}

	std::cout << "match " << match_from << ":" << match_to;
	if (mark.is_initialized())
		std::cout << "  ->  " << *mark.get();
	std::cout << std::endl;
	return padding;
}


//=====================================================================
// insert
//=====================================================================
bool sooty::parsing::detail::parsers::insert::applies( accumulator& acc, lexing::lexemes_t::const_iterator&, lexing::lexemes_t::const_iterator& )
{
	if (lexeme_mark_id.is_initialized()) {
		parseme m = acc.at_marker(lexeme_mark_id.get());
		acc.delete_at(lexeme_mark_id.get());
		acc.insert(insert_id, m.lexeme());
		acc.rm_marker(lexeme_mark_id.get());
	}
	else {
		acc.insert(insert_id);
	}

	return true;
}


//=====================================================================
// add-marker
//=====================================================================
bool sooty::parsing::detail::parsers::add_marker::applies( accumulator& acc, lexing::lexemes_t::const_iterator& begin, lexing::lexemes_t::const_iterator& )
{
	acc.add_marker(mark_id, begin);
	return true;
}


//=====================================================================
// rm-marker
//=====================================================================
bool sooty::parsing::detail::parsers::rm_marker::applies( accumulator& acc, lexing::lexemes_t::const_iterator& begin, lexing::lexemes_t::const_iterator& )
{
	//begin = acc.iter_marker(mark_id);
	acc.rm_marker(mark_id);
	return false;
}


//=====================================================================
// merge
//=====================================================================
bool sooty::parsing::detail::parsers::merge::applies( accumulator& acc, lexing::lexemes_t::const_iterator&, lexing::lexemes_t::const_iterator& )
{
	acc.merge_into(from_mark);
	acc.rm_marker(from_mark);
	return true;
}


//=====================================================================
// rewind
//=====================================================================
bool sooty::parsing::detail::parsers::rewind::applies( accumulator& acc, lexing::lexemes_t::const_iterator& begin, lexing::lexemes_t::const_iterator& )
{
	parseme parent = acc.back();
	acc.pop();
	acc.promote_children();
	acc.add_marker(mark);
	acc.insert_all(parent.children());
	begin = acc.iter_marker(mark);
	return false;
}


bool sooty::parsing::detail::skippable( const abstract_parser_backend_ptr& P )
{
	return
		(
			boost::shared_dynamic_cast<parsers::terminal>(P) &&
			P->on_success
		)
		||
		boost::shared_dynamic_cast<parsers::nop>(P)
		;
}




//
//
///
bool sooty::parsing::detail::equivalent( abstract_parser_backend_ptr& lhs, const abstract_parser_backend_ptr& rhs )
{
	// superparsers
	boost::shared_ptr<parsers::superparser> lhs_sp = boost::shared_dynamic_cast<parsers::superparser>(lhs);
	if (lhs_sp) {
		boost::shared_ptr<parsers::superparser> rhs_sp
			= boost::shared_dynamic_cast<parsers::superparser>(rhs);
		
		return rhs_sp && lhs_sp->nop == rhs_sp->nop;
	}
	
	// matchers
	boost::shared_ptr<parsers::match> lhs_mp = boost::shared_dynamic_cast<parsers::match>(lhs);
	if (lhs_mp) {
		boost::shared_ptr<parsers::match> rhs_mp
			= boost::shared_dynamic_cast<parsers::match>(rhs);
		
		bool g = rhs_mp && lhs_mp->match_from == rhs_mp->match_from &&
			lhs_mp->match_to == rhs_mp->match_to;
		
		if (g) {
			accumulator::mark_t lm = lhs_mp->mark.get();
			accumulator::mark_t rm = rhs_mp->mark.get();
			
			*rhs_mp->mark.get() = *lhs_mp->mark.get();
		}
		
		return g;
	}
	
	// inserters
	boost::shared_ptr<parsers::insert> lhs_ip = boost::shared_dynamic_cast<parsers::insert>(lhs);
	if (lhs_ip) {
		boost::shared_ptr<parsers::insert> rhs_ip
			= boost::shared_dynamic_cast<parsers::insert>(rhs);
		
		bool g = rhs_ip && lhs_ip->insert_id == rhs_ip->insert_id;
		
		if (g) {
			accumulator::mark_t lm = lhs_ip->lexeme_mark_id.get();
			accumulator::mark_t rm = rhs_ip->lexeme_mark_id.get();
			
			assert(*rhs_ip->lexeme_mark_id.get() == *lhs_ip->lexeme_mark_id.get());
		}
		
		return g;
	}
	
	
	// terminals
	{
		boost::shared_ptr<parsers::terminal> lhsc = boost::shared_dynamic_cast<parsers::terminal>(lhs);
		if (lhsc && lhsc->state)
		{
			abstract_parser_backend_ptr on_success = lhs->on_success, on_failure = lhs->on_failure;
		
			// if lhs and rhs are both terminals, overwrite lhs with rhs->on_success (if it exists)
			// otherwise, just overwrite lhs with rhs
			boost::shared_ptr<parsers::terminal> rhsc = boost::shared_dynamic_cast<parsers::terminal>(rhs);
			if (rhsc && rhs->on_success)
				lhs = rhs->on_success->clone();
			else
				lhs = rhs->clone();

			lhs->on_success = on_success;
			lhs->on_failure = on_failure;
			return true;
		}
	}
	
	return false;
	
	
	// add-marker
	boost::shared_ptr<parsers::add_marker> lhs_amp = boost::shared_dynamic_cast<parsers::add_marker>(lhs);
	if (lhs_amp) {
		boost::shared_ptr<parsers::add_marker> rhs_ip
			= boost::shared_dynamic_cast<parsers::add_marker>(rhs);
		
		if (rhs_ip) {
			// give *both* nodes new a shared new marker
			*rhs_ip->mark_id.get() = *lhs_amp->mark_id.get() = *accumulator::generate_marker();
		}
		
		return true;
	}
	
	return false;
}

bool sooty::parsing::detail::overwrote_edge( abstract_parser_backend_ptr& lhs, const abstract_parser_backend_ptr& rhs )
{
	return false;
}

bool sooty::parsing::detail::should_prepend( const abstract_parser_backend_ptr& P )
{
	return
		boost::shared_dynamic_cast<parsers::add_marker>(P) || 
		boost::shared_dynamic_cast<parsers::rm_marker>(P)
		;
}

bool sooty::parsing::detail::equivalent_in_failure( abstract_parser_backend_ptr& lhs, abstract_parser_backend_ptr& rhs )
{
	return false;
	
	if (boost::shared_dynamic_cast<parsers::rm_marker>(lhs)) {
		abstract_parser_backend_ptr on_success = lhs->on_success;
		abstract_parser_backend_ptr on_failure = lhs->on_failure;
		lhs = rhs->clone();
		lhs->on_success = on_success;
		lhs->on_failure = on_failure;
		return true;
	}
	
	return false;
}

bool sooty::parsing::detail::parsers::terminal::applies( accumulator& acc, lexing::lexemes_t::const_iterator&, lexing::lexemes_t::const_iterator& )
{
	return state;
}