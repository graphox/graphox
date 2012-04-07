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
  		}
	}
}

#endif


