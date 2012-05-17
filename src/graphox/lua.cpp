
#ifndef GRAPHOX_LUA_CPP
#define GRAPHOX_LUA_CPP

#include "graphox/graphox.h"
#include "OFTL/lua.h"

namespace graphox
{

	namespace oftl_lua
	{
		using namespace lua;
	}

	namespace scripting
	{
		using namespace lua;

		namespace lua
		{
			oftl_lua::State state;
			lua_State *L = state.state();
			oftl_lua::Table core_table = state.new_table();
			
			bool initialised = false;
			
		    void panic(const oftl_lua::State& s, const char *msg)
		    {
		    	puts("ERROR: ");
		    	puts(msg);
		    }
		    
		    void init()
			{
			
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
				
				lua::module::open_luaproxy(L);
				
				state.register_module("CAPI", core_table);
		    }
	    
		    /*void push_function(oftl_lua::Table table, const char *name, void *func_)
		    {
		    	//oftl_lua::Function *func = func_;
		    	//table[name] = func;	    
		    }*/
		    
			template<typename T, typename X>
				void push_function(T function, X name)
		    {

				core_table[name] = &function;
		    	
		    	return 0;
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
