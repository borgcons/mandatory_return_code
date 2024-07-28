#include"MandatoryReturncode.hpp"

using namespace MandatoryReturnValue;

ReturnValue<int> returningFunction()
{ENTRY
	return 1;
}

// I just added this for testing the throwing function
#include <stdexcept>

struct any_exception : std::runtime_error
{
	any_exception () : std::runtime_error("some kind of exception") {}
};

ReturnValue<int> throwingFunction()
{ENTRY
	throw any_exception();
	return 1;
}

#include <exception>

// Howard Hinnant sent this to help me see what was going on
static_assert( std::is_destructible<ReturnValue<int>>{}, "");
static_assert(!std::is_default_constructible<ReturnValue<int>>{}, "");
static_assert(!std::is_copy_constructible<ReturnValue<int>>{}, "");
static_assert(!std::is_copy_assignable<ReturnValue<int>>{}, "");
static_assert(std::is_move_constructible<ReturnValue<int>>{}, ""); 
static_assert(!std::is_move_assignable<ReturnValue<int>>{}, "");

int main()
{
	// Tested on Wandbox
	// gcc 5.4.0 - 9.0.0
	// C/C++ Optimizing Compiler Version 19.00.23506 for x64 did not file test 8 and test 12
	// clang 3.8.0 - 7.0.0 did not like test 8 and test 12

	try
	{
		std::cout << "Test 1 - Proper usage " << std::endl;
		
		int result = returningFunction();
		
		std::cout << "PASS" << std::endl;
	}
	catch (...)
	{
		std::cout << "FAIL" << std::endl;
	}

	try
	{
		std::cout << "Test 2 - Also Proper Usage" << std::endl;
		
		if (returningFunction()) 
		{ 
		}
		
		std::cout << "PASS" << std::endl;
	}
	catch (...)
	{
		std::cout << "FAIL" << std::endl;
	}

	try
	{
		std::cout << "Test 3 - Expect Exception" << std::endl;
		returningFunction(); 
		
		std::cout << "FAIL" << std::endl;
	}
	catch (...)
	{
		std::cout << "PASS" << std::endl;
	}

	try
	{
		std::cout << "Test 4 - Clumsy usage to ignore the return code" << std::endl;
		
		!returningFunction();
		
		std::cout << "PASS" << std::endl;
	}
	catch (...)
	{
		std::cout << "FAIL" << std::endl;
	}

	try
	{
		std::cout << "Test 5 - Another clumsy usage to ignore the return code" << std::endl;
		
		(int)returningFunction();
		
		std::cout << "PASS" << std::endl;
	}
	catch (...)
	{
		std::cout << "FAIL" << std::endl;
	}

	try
	{
		std::cout << "Test 6 - Yet another clumsy usage to ignore the return code" << std::endl;
		
		int dummy = returningFunction();
		
		std::cout << "PASS" << std::endl;
	}
	catch (...)
	{
		std::cout << "FAIL" << std::endl;
	}

	try
	{
		std::cout << "Test 7 - Proper usage to ignore the ErrorCode" << std::endl;
		
		ignore_cast(returningFunction());
		
		std::cout << "PASS" << std::endl;
	}
	catch (...)
	{
		std::cout << "FAIL" << std::endl;
	}

	try
	{
		std::cout << "Test 8 - A clumsy usage to ignore the ErrorCode" << std::endl;
		
		(ignore)returningFunction();
		
		std::cout << "PASS" << std::endl;
	}
	catch (...)
	{
		std::cout << "FAIL" << std::endl;
	}

	try
	{
		std::cout << "Test 9 - Can't stop this and has little use" << std::endl;
		
		{
			ReturnValue<int> ec = 5;
		}	
		
		std::cout << "FAIL" << std::endl;
	}
	catch (...) 
	{
		std::cout << "PASS" << std::endl;
	}

	try
	{
		std::cout << "Test 10 - Handle a throwing function " << std::endl;
		
		throwingFunction(); 
		
		std::cout << "FAIL" << std::endl;
	}
	catch (...)
	{
		std::cout << "PASS" << std::endl;
	}

	try
	{
		std::cout << "Test 11 - Handle a throwing function with proper usage" << std::endl;
		
		int ec = throwingFunction(); 
		
		std::cout << "FAIL" << std::endl;
	}
	catch (...)
	{
		std::cout << "PASS" << std::endl;
	}

	try
	{
		std::cout << "Test 12 - Handle a throwing function with another proper usage" << std::endl;
		
		ignore_cast(throwingFunction()); 
		
		std::cout << "FAIL" << std::endl;
	}
	catch (...)
	{
		std::cout << "PASS" << std::endl;
	}

	try
	{
		std::cout << "Test 13 - Handle a throwing function with yet another proper usage" << std::endl;
		
		if (throwingFunction()) 
		{ 
		}
		
		std::cout << "FAIL" << std::endl;
	}
	catch (...)
	{
		std::cout << "PASS" << std::endl;
	}

	try 
	{
		std::cout << "Test 14 - Handle returning and throwing function with proper usage" << std::endl;
		
		int ec1 = returningFunction();
		int ec2 = throwingFunction();

		if (ec1 && ec2) 
		{
		}
		
		std::cout << "FAIL" << std::endl;
	}
	catch (...) 
	{
		std::cout << "PASS" << std::endl;
	}

	try 
	{
		std::cout << "Test 15 - Handle returning and throwing function improper usage causes terminate" << std::endl;

        std::set_terminate([]()
        {
            std::cout << "PASS" << std::endl;
            std::exit(0);
        });

		ReturnValue<int> ec1 = returningFunction();

		// When this throws ec1 has not used its value so
		// this will result in terminate being called
		ReturnValue<int> ec2 = throwingFunction();

		if (ec1 && ec2) 
		{
		}
		
		std::cout << "FAIL" << std::endl;
	}
	catch (...) 
	{
		std::cout << "FAIL" << std::endl;
	}

	return 0;
}
