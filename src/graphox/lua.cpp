
#ifndef GRAPHOX_LUA_CPP
#define GRAPHOX_LUA_CPP

#define CORE_TABLE_NAME "CAPI"

#include "graphox/graphox_internal.h"

const char *version = "100";

namespace graphox
{
	namespace scripting
	{
		using namespace lua;

		namespace lua
		{
			oftl_lua::State state;
			oftl_lua::Table core_table;
			int vars_table;
			
			lua_State *get_state()
			{
				return state.state();
			}
			
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
				
				core_table = state.new_table();
		    }
		    
		    void finalize_bindings()
		    {
		    	state.register_module(CORE_TABLE_NAME, core_table);
		    	
		    	lua_State *L = get_state();

				lua_newtable(L);
    			vars_table = lua_gettop(L);
		    	
		    	bind_var<const char *, true>(L, vars_table, "version", version);
		    	
				//lua_settable(L, -3);
				lua_setglobal(L, "vars");

		    }
		    
		    template<typename T>
		    	struct type{};
		    
		    int to(lua_State * L, int index, type<int>*)
		    {
		    	return luaL_checkint(L, index);
		    }
		    
		    unsigned int to(lua_State * L, int index, type<unsigned int>*)
		    {
		    	return static_cast<unsigned int>(luaL_checknumber(L, index));
		    }
		    
		    long to(lua_State * L, int index, type<long>*)
		    {
		    	return static_cast<long>(luaL_checklong(L, index));
		    }
		    
		    unsigned long to(lua_State * L, int index, type<unsigned long>*)
		    {
		    	return static_cast<unsigned long>(luaL_checknumber(L, index));
		    }

			lua_Number to(lua_State * L, int index, type<lua_Number>*)
			{
				return luaL_checknumber(L, index);
			}
/*
			bool to(lua_State * L, int index, bool type)
			{
				luaL_checkany(L, index);
		
				if(lua_type(L, index) == LUA_TNUMBER && lua_tointeger(L, index) == 0)
					return false;
		
				return static_cast<bool>(lua_toboolean(L, index));
			}*/

			const char * to(lua_State * L, int index, type<const char *>*)
			{
				return luaL_checkstring(L, index);
			}
			
			class nil_type {};
			nil_type nil;
			
			void push(lua_State  * L, nil_type)
			{
				lua_pushnil(L);
			}

			void push(lua_State  * L, bool value)
			{
				lua_pushboolean(L, static_cast<int>(value));
			}

			void push(lua_State  * L, int value)
			{
				lua_pushinteger(L, value);
			}

			void push(lua_State  * L, lua_Number value)
			{
				lua_pushnumber(L, value);
			}

			void push(lua_State  * L, lua_CFunction value)
			{
				lua_pushcfunction(L, value);
			}

			void push(lua_State  * L, const char * value)
			{
				lua_pushstring(L, value);
			}

			void push(lua_State  * L, const char * value, size_t length)
			{
				lua_pushlstring(L, value, length);
			}

			/*void push(lua_State  * L, const types::String & value)
			{
				lua_pushlstring(L, value.data(), value.length());
			}*/
			
			template<typename T, bool READ_ONLY, bool WRITE_ONLY>
				static int variable_accessor(lua_State * L)
			{
				T * var = reinterpret_cast<T *>(lua_touserdata(L, lua_upvalueindex(1)));
				if(lua_gettop(L) > 0) // Set variable
				{
					if(READ_ONLY) luaL_error(L, "variable is read-only");
					*var = to(L, 1, new type<T>);
					return 0;
				}
				else // Get variable
				{
					if(WRITE_ONLY) luaL_error(L, "variable is write-only");
					push(L, *var);
					return 1;
				}
			}
			
			template<typename T, bool READ_ONLY = false, bool WRITE_ONLY = false>
				static void bind_var(lua_State * L, int table, const char * name, T & var)
			{
				lua_pushstring(L, name);
				lua_pushlightuserdata(L, &var);
				lua_pushstring(L, name);
				lua_pushcclosure(L, variable_accessor<T, READ_ONLY, WRITE_ONLY>, 2);
				lua_settable(L, table);
			}		
		    
			template<typename T, typename X>
				void push_function(T function, X name)
		    {

				state[name] = &function;
		    	
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
		    
		    void frame()
		    {
		    	state.get<oftl_lua::Function>("event", "frame")();
		    }
		    
		    void connect(int cn)
		    {
		    	state.get<oftl_lua::Function>("event", "connect")(cn);
		    }
		}
	}
}

#endif
