
#ifndef GRAPHOX_SCRIPTING_CPP
#define GRAPHOX_SCRIPTING_CPP

#include "graphox/scripting.h"

namespace graphox
{
	namespace scripting
	{
		void error_info::print(const char *message)
		{
			printf("\ton %s:%i : %s\n", file, line, message);
		}
			
		void error_info::print()
		{
			printf("\ton %s:%i\n", file, line);
		}
		
		error_info *Exception::get_error_info()
		{
			return script_info;
		}
				
		void Exception::print()
		{
			printf("ERROR %i: %s\n", error_number, error_message);
			script_info->print();
		}
				
		void print(const char *message)
		{
			printf("%s", message);
		}
		
		void init()
		{
			graphox::scripting::lua::init();
		};
		
		void execute(const char *file)
		{
			graphox::scripting::lua::execute(file);
		};
	}
}
#endif
