
#ifndef GRAPHOX_LUA_CPP
#define GRAPHOX_LUA_CPP

#include "graphox/lua.h"
#include "OFTL/lua.h"

namespace oftl_lua
{
	using namespace lua;
}

namespace graphox
{
	namespace scripting
	{
		using namespace lua;

		namespace lua
		{
			oftl_lua::State state;
			bool initialised = false;
			
		    void panic(const oftl_lua::State& s, const char *msg)
		    {
		    	puts("ERROR: ");
		    	puts(msg);
		    }
		    
		    void init()
			{
				const char *homedir = "./";
				
				if (initialised)
					return;
				else
					initialised  = true;
				
				state.set_panic_handler(&panic);
				
				state.open_base();
				state.open_table();
				state.open_string();
				state.open_math();
				state.open_package();
				state.open_debug();
				
				/*
				lua_State *L  = state.state();
				
				Table loaded  = state.registry()["_LOADED"];
				Table package = loaded["package"];

				/ * home directory paths * /
				lua_pushfstring(
				    L, ";%sdata%c?%cinit.lua",
					homedir, PATHDIV, PATHDIV
				);
				lua_pushfstring(
				    L, ";%sdata%clibrary%c?%cinit.lua",
				    homedir, PATHDIV, PATHDIV, PATHDIV
				);

				/ * root paths * /
				lua_pushliteral(L, ";./data/library/core/?.lua");
				lua_pushliteral(L, ";./data/library/core/?/init.lua");
				lua_pushliteral(L, ";./data/?/init.lua");
				lua_pushliteral(L, ";./data/library/?/init.lua");

				lua_concat(L,  6);
				Object str(L, -1);
				lua_pop   (L,  1);
				package["path"] = str;

				/ * restrict package * /
				Table pkg = state.new_table(0, 1);
				pkg  ["seeall" ] = package["seeall"];
				state["package"] = pkg;

				setup_binds();*/
				
				oftl_lua::Table api_all = state.new_table();
				//api_all["panic"] = &panic;
				
				state.register_module("CAPI", api_all);
		    }
		    
		    void execute(const char *filename)
		    {
		    	auto error = state.do_file(filename, oftl_lua::ERROR_TRACEBACK);
		    	
		    	// !0 -> error occured
		    	if(types::get<0>(error))
		    	{
		    		puts("ERROR:");
		    		puts(types::get<1>(error));
		    	}
		    }
		}
	}
}

#endif
