#ifndef GRAPHOX_LUA_H
#define GRAPHOX_LUA_H

extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include "graphox/graphox.h"

#ifdef HAS_OFTL
	#include "OFTL/lua.h"
	
	namespace lua {};
	
	namespace graphox
	{
		namespace oftl_lua
		{
			using namespace lua;
		}
	}
#endif

namespace graphox
{
	namespace scripting
	{
		namespace lua
		{
			#ifdef HAS_OFTL
				extern oftl_lua::State state;
				extern oftl_lua::Table core_table;
			#endif
			
			template<typename T, bool READ_ONLY = false, bool WRITE_ONLY = false>
				static void bind_var(lua_State * L, int table, const char * name, T & var);
			
			void finalize_bindings();
			
			void init();
			void execute(const char *filename);
			lua_State *get_state();
			int get_bind_table();
			
			void frame();
			void connect(int);
			
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


