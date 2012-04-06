
#ifndef GRAPHOX_EXCEPTION_CPP
#define GRAPHOX_EXCEPTION_CPP

#include "exception.h"

namespace graphox
{

	int Exception::get_error_number()
	{
		return error_number;
	}
			
	const char *Exception::get_message()
	{
		return error_message;
	}
			
	void Exception::print()
	{
		printf("ERROR %i: %s", error_number, error_message);
	}
}	

#endif
