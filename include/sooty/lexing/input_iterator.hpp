//=====================================================================
//
//
//
//=====================================================================
#ifndef SOOTY_LEXING_INPUT_ITERATOR_HPP
#define SOOTY_LEXING_INPUT_ITERATOR_HPP
//=====================================================================
#include <string>
#include <iterator>
//=====================================================================
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/type_traits.hpp>
//=====================================================================
namespace sooty {
namespace lexing {
//=====================================================================
	
	//=====================================================================
	//
	//=====================================================================
	namespace detail
	{
		struct iterator_impl_base
		{
			virtual void increment() = 0;
			virtual char dereference() = 0;
			virtual bool equality(iterator_impl_base*) const = 0;
			virtual iterator_impl_base* clone() const = 0;
			virtual std::string stringize(iterator_impl_base*) const = 0;
			virtual ~iterator_impl_base() {}
		};
		
		template <typename T>
		struct iterator_impl : iterator_impl_base
		{
			T t;
			
			iterator_impl(const T& t)
				: t(t)
			{
			}
			
			void increment()
			{
				t.operator ++();
			}
			
			char dereference()
			{
				return *t;
			}
			
			bool equality(iterator_impl_base* other) const
			{
				iterator_impl<T> * them = static_cast<iterator_impl<T>*>(other);
				return t == them->t;
			}
			
			iterator_impl_base* clone() const
			{
				return new iterator_impl<T>(t);
			}
			
			std::string stringize(iterator_impl_base* rhs) const
			{
				iterator_impl<T> * them = static_cast<iterator_impl<T>*>(rhs);
				return std::string(t, them->t);
			}
		};
	}
	
	//=====================================================================
	//
	//=====================================================================
	class input_iterator
	{
		friend std::string make_string(const input_iterator&, const input_iterator&);
		
		boost::scoped_ptr<detail::iterator_impl_base> backend;
		
	public:
		//=====================================================================
		// constructors
		//=====================================================================
		input_iterator()
		{
		}
		
		template <typename T>
		input_iterator(const T& t)
			: backend(new detail::iterator_impl<T>(t))
		{
		}
		
		input_iterator(const input_iterator& rhs)
		{
			if (rhs.backend)
				this->backend.reset(rhs.backend->clone());
		}
		
		
		//=====================================================================
		// operators
		//=====================================================================
		input_iterator& operator = (const input_iterator& rhs)
		{
			backend.reset( rhs.backend->clone() );
			return *this;
		}
		
		input_iterator& operator ++() {
			backend->increment();
			return *this;
		}

		input_iterator  operator ++(int) {
			input_iterator L(*this);
			++*this;
			return L;
		}
		
		char operator * () const {
			return backend->dereference();
		}
		
		bool operator == (const input_iterator& rhs) const {
			return backend->equality(rhs.backend.get());
		}
		
		bool operator != (const input_iterator& rhs) const {
			return !operator == (rhs);
		}
	};
	
	
	
	//inline std::ostream& operator << (std::ostream& lhs, const input_iterator& rhs)
	//{
	//	return lhs;
	//}
	
	
	
//=====================================================================
} // namespace lexing
} // namespace sooty
//=====================================================================
namespace std {
	template <> struct iterator_traits<::sooty::lexing::input_iterator>
	{
		typedef std::forward_iterator_tag iterator_category;
		typedef char value_type;
		typedef std::ptrdiff_t difference_type;
		typedef char* pointer;
		typedef char& reference;
	};
}
//=====================================================================
#endif
//=====================================================================

