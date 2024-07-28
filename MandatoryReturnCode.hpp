#include <iostream>

// Found this on StackOverflow but made it a class and used std::cout and std::endl;
// https://stackoverflow.com/questions/2281739/automatically-adding-enter-exit-function-logs-to-a-project

//#define SHOW_DETAIL 1

#if SHOW_DETAIL
class EntryRaiiObject 
{
public:
	EntryRaiiObject(const char *f) : f_(f) { std::cout << "Entered into " << f_ << std::endl; }
	~EntryRaiiObject() { std::cout << "Exited from " << f_ << std::endl; }
private:
	const char * f_;
};

#define ENTRY EntryRaiiObject obj ## __LINE__ (__PRETTY_FUNCTION__);
#else
#define ENTRY
#endif

#include <stdexcept>
#include <tuple>

namespace MandatoryReturnValue
{
	// From Eric Niebler write this once and put it somewhere you can reuse it
	template<typename A, typename B>
	using disable_if_same_or_derived =
	typename std::enable_if<
	!std::is_base_of<A,typename
	std::remove_reference<B>::type
	>::value
	>::type;

	template <typename T>
	class ReturnValue;

    struct ignore {};
    
	template<typename T>
	ignore ignore_cast(ReturnValue<T>&& code)
	{
		code.shouldThrow_ = false;
		return ignore();
	}

	struct mandatory_return_value : public std::runtime_error
	{
		mandatory_return_value() : std::runtime_error("Mandatory return value ignored") {};
	};

	template <class T>
	class ReturnValue
	{
		mutable bool	shouldThrow_;
		T 				returnValue_;

	public: 
		// user-defined constructor - receive the returnValue and arm the exception
		ReturnValue(const T & value) : shouldThrow_(true), returnValue_(value)
		{ENTRY
		}

		// copy constructor
		ReturnValue(ReturnValue &) = delete;

		// copy assignment 
		ReturnValue & operator= ( ReturnValue & ) = delete;

		// move assignment
		ReturnValue & operator= ( ReturnValue && ) = delete;

		// move constructor
		template<typename U, typename X =
		disable_if_same_or_derived<ReturnValue,U>>
		ReturnValue( ReturnValue<U> && rhs ) : shouldThrow_(rhs.shouldThrow_), returnValue_(rhs.returnValue_) 
		{ENTRY
			rhs.shouldThrow_ = false;
		}

		// conversion operator to T disarms the exception
		operator T() 
		{ENTRY
			shouldThrow_ = false;
			return returnValue_;
		}

		// conversion operator to ignore disarms the exception

		operator ignore() 
		{ENTRY
			shouldThrow_ = false;
			return ignore();
		}

		// destructor
		~ReturnValue() noexcept(false) 
		{ENTRY
			if (shouldThrow_)
			{
				throw mandatory_return_value();
			}
		}

	    friend ignore ignore_cast<>(ReturnValue<T> &&);

	};
};
