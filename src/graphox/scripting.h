
#ifndef GRAPHOX_SCRIPTING_H
#define GRAPHOX_SCRIPTING_H

#include "graphox/graphox.h"

namespace graphox
{
	namespace scripting
	{
		struct error_info
		{
			int line;
			const char *file;
			
			error_info(int line_, const char *file_) : line(line_), file(file_) {};
			
			//dummy
			error_info() {};
			
			void print(const char *message);
			void print();
		};
		
		class Exception : graphox::Exception
		{
			public:
				Exception(int error_num, const char *message, error_info *info) :error_number(error_num), error_message(message), script_info(info) {};
			
				error_info *get_error_info();
				void print();

			protected:
				error_info* script_info;		
				
				//why is this needed?
				int error_number;
				const char *error_message;
				
		};
		
		void print(const char *message);
		
		void init();
		void execute(const char *name);
	}
}
#endif
