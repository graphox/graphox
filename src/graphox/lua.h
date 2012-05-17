#ifndef GRAPHOX_LUA_H
#define GRAPHOX_LUA_H

extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include "graphox/graphox.h"

namespace graphox
{
	namespace scripting
	{
		namespace lua
		{
			void init();
			void execute(const char *filename);
			//int push_function(const char *name, void *func);
			
			template<typename T, typename X>
				int push_function(T func, X name);

			template<typename T>
				void push_function(T *func, char name)
			{
				push_function<T>(func, (const char *)name);
			}
			
  		}
		using namespace lua;
	}
}

#endif


