
#ifndef GRAPHOX_EXCEPTION_H
#define GRAPHOX_EXCEPTION_H

#include "graphox.h"

namespace graphox
{
	class Exception
	{
		public:
			Exception(int error_num, const char *message) :error_number(error_num), error_message(message) {}
			Exception() {};
		
			int get_error_number();
			const char *get_message();
			void print();
		
		protected:
			int error_number;
			const char *error_message;
		
	};
}


#endif
