/* File: OFTL/lua/stack.h
 *
 * About: Version
 *  This is version 2 of the file.
 *
 * About: Purpose
 *  Internal Lua stack library.
 *
 * About: Author
 *  Daniel "q66" Kolesa <quaker66@gmail.com>
 *
 * About: License
 *  This file is licensed under MIT. See COPYING.txt for more information.
 */

/* Package: lua */
namespace lua
{
    /* Package: stack */
    namespace stack
    {
        template<typename T, typename... U>
        inline int lua_native_wrap(lua_State *L);

        /* Function: push_value
         * Defined for many types, see below.
         */
        template<typename T>
        inline void push_value(lua_State *L, const T& value)
        { value.push(L); }

        /* Function: push_value */
        inline void push_value(lua_State *L, const Object& value)
        { value.push(); }

        /* Function: push_value */
        inline void push_value(lua_State *L, const Dynamic& value)
        { value.push(); }

        /* Function: push_value */
        inline void push_value(lua_State *L, const Function& value)
        { value.push(); }

        /* Function: push_value */
        inline void push_value(lua_State *L, const Table& value)
        { value.push(); }

        /* Function: push_value */
        template<typename T>
        inline void push_value(lua_State *L, const Table_Item<T>& value)
        { value.push(); }

        /* Function: push_value */
        inline void push_value(lua_State *L, const Nil& value)
        { lua_pushnil(L); }

        /* Function: push_value */
        inline void push_value(lua_State *L, bool value)
        { lua_pushboolean(L, value); }

        /* Function: push_value */
        inline void push_value(lua_State *L, char value)
        { lua_pushlstring(L, &value, 1); }

        /* Function: push_value */
        inline void push_value(lua_State *L, uchar value)
        { lua_pushlstring(L, (char*)&value, 1); }

        /* Function: push_value */
        inline void push_value(lua_State *L, short value)
        { lua_pushinteger(L, value); }

        /* Function: push_value */
        inline void push_value(lua_State *L, int value)
        { lua_pushinteger(L, value); }

        /* Function: push_value */
        inline void push_value(lua_State *L, long value)
        { lua_pushinteger(L, value); }

        /* Function: push_value */
        inline void push_value(lua_State *L, llong value)
        { lua_pushinteger(L, value); }

        /* Function: push_value */
        inline void push_value(lua_State *L, ushort value)
        { lua_pushinteger(L, value); }

        /* Function: push_value */
        inline void push_value(lua_State *L, uint value)
        { lua_pushinteger(L, value); }

        /* Function: push_value */
        inline void push_value(lua_State *L, ulong value)
        { lua_pushinteger(L, value); }

        /* Function: push_value */
        inline void push_value(lua_State *L, ullong value)
        { lua_pushinteger(L, value); }

        /* Function: push_value */
        inline void push_value(lua_State *L, float value)
        { lua_pushnumber(L, value); }

        /* Function: push_value */
        inline void push_value(lua_State *L, double value)
        { lua_pushnumber(L, value); }

        /* Function: push_value */
        inline void push_value(lua_State *L, long double value)
        { lua_pushnumber(L, value); }

        /* Function: push_value */
        inline void push_value(lua_State *L, const char *value)
        { lua_pushstring(L, value); }

        /* Function: push_value */
        inline void push_value(lua_State *L, char *value)
        { lua_pushstring(L, value); }

        /* Function: push_value */
        inline void push_value(lua_State *L, const uchar *value)
        { lua_pushstring(L, (const char*)value); }

        /* Function: push_value */
        inline void push_value(lua_State *L, const types::String& value)
        { lua_pushstring(L, value.get_buf()); }

        /* Function: push_value */
        template<typename T>
        inline void push_value(lua_State *L, const types::Vector<T>& v)
        {
            lua_createtable(L, v.length(), 0);
            for (size_t i = 0; i < v.length(); ++i)
            {
                push_value  (L, (int)(i + 1));
                push_value  (L, v[i]);
                lua_settable(L, -3);
            }
        }

#ifdef OFTL_MAP_H
        /* Function: push_value
         * Support for the <Map> container is enabled conditionally.
         * If the header is included, map is supported (note that
         * you have to include it BEFORE lua.h).
         */
        template<typename T, typename U>
        inline void push_value(lua_State *L, const types::Map<T, U>& v)
        {
            lua_createtable(L, 0, v.length());
            for (
                typename types::Map<T, U>::cit it = v.begin();
                it != v.end();
                ++it
            )
            {
                push_value  (L, it->first);
                push_value  (L, it->second);
                lua_settable(L, -3);
            }
        }
#endif
#ifdef OFTL_HASHMAP_H
        /* Function: push_value
         * Support for the <Hash_Map> container is enabled conditionally.
         * If the header is included, hashmap is supported (note that
         * you have to include it BEFORE lua.h).
         */
        template<typename T, typename U>
        inline void push_value(lua_State *L, const types::Hash_Map<T, U>& v)
        {
            lua_createtable(L, 0, v.length());
            for (
                typename types::Hash_Map<T, U>::cit it = v.begin();
                it != v.end();
                ++it
            )
            {
                push_value  (L, it->first);
                push_value  (L, it->second);
                lua_settable(L, -3);
            }
        }
#endif
        /* Function: push_value */
        inline void push_value(lua_State *L, lua_CFunction value)
        { lua_pushcfunction(L, value); }

        /* Function: push_value */
        template<typename T, typename... U>
        inline void push_value(lua_State *L, T (*fun)(U...))
        {
            lua_pushlightuserdata(L, (void*)fun);
            lua_pushcclosure(L, lua_native_wrap<T, U...>, 1);
        }

        template<size_t i, size_t l>
        struct Lua_Tuple_Push
        {
            template<typename... T>
            static void push(lua_State *L, const types::Tuple<T...>& t)
            {
                push_value(L, types::get<i>(t));
                return Lua_Tuple_Push<i + 1, l>::push(L, t);
            }
        };

        template<size_t l>
        struct Lua_Tuple_Push<l, l>
        {
            template<typename... T>
            static void push(lua_State *L, const types::Tuple<T...>& t) {}
        };

        /* Function: push_value */
        template<typename... T>
        inline void push_value(lua_State *L, const types::Tuple<T...>& t)
        {
            return Lua_Tuple_Push<0, sizeof...(T)>::push(L, t);
        }

        /* Variable: Typeof
         * You can use this to get native Lua types corresponding to
         * appropriate C++ type. Defined for:
         *
         *  bool - LUA_TBOOLEAN
         *  Nil - LUA_TNIL
         *  Object - LUA_TUSERDATA
         *  Dynamic - LUA_TUSERDATA
         *  Function - LUA_TFUNCTION
         *  Table - LUA_TTABLE
         *  String - LUA_TSTRING
         *  char - LUA_TSTRING
         *  const char* - LUA_TSTRING
         *  char* - LUA_TSTRING
         *  uchar - LUA_TSTRING
         *  const uchar* - LUA_TSTRING
         *  uchar* - LUA_TSTRING
         *  short - LUA_TNUMBER
         *  int - LUA_TNUMBER
         *  long - LUA_TNUMBER
         *  llong - LUA_TNUMBER
         *  ushort - LUA_TNUMBER
         *  uint - LUA_TNUMBER
         *  ulong - LUA_TNUMBER
         *  ullong - LUA_TNUMBER
         *  float - LUA_TNUMBER
         *  double - LUA_TNUMBER
         *  long double - LUA_TNUMBER
         *  lua_CFunction - LUA_TFUNCTION
         *  T* - LUA_TUSERDATA
         *
         * Usage:
         *
         * (start code)
         *     // will be LUA_TNUMBER
         *     int type = stack::Typeof<5>::value;
         * (end)
         */
        template<typename T>
        struct Typeof { enum { value = LUA_TNONE }; };

        template<>
        struct Typeof<bool> { enum { value = LUA_TBOOLEAN }; };

        template<>
        struct Typeof<Nil> { enum { value = LUA_TNIL }; };

        template<>
        struct Typeof<Object> { enum { value = LUA_TUSERDATA }; };

        template<>
        struct Typeof<Dynamic> { enum { value = LUA_TUSERDATA }; };

        template<>
        struct Typeof<Function> { enum { value = LUA_TFUNCTION }; };

        template<>
        struct Typeof<Table> { enum { value = LUA_TTABLE }; };

        template<>
        struct Typeof<types::String> { enum { value = LUA_TSTRING }; };

        template<>
        struct Typeof<char> { enum { value = LUA_TSTRING }; };

        template<>
        struct Typeof<const char*> { enum { value = LUA_TSTRING }; };

        template<>
        struct Typeof<char*> { enum { value = LUA_TSTRING }; };

        template<>
        struct Typeof<uchar> { enum { value = LUA_TSTRING }; };

        template<>
        struct Typeof<const uchar*> { enum { value = LUA_TSTRING }; };

        template<>
        struct Typeof<uchar*> { enum { value = LUA_TSTRING }; };

        template<>
        struct Typeof<short> { enum { value = LUA_TNUMBER }; };

        template<>
        struct Typeof<int> { enum { value = LUA_TNUMBER }; };

        template<>
        struct Typeof<long> { enum { value = LUA_TNUMBER }; };

        template<>
        struct Typeof<llong> { enum { value = LUA_TNUMBER }; };

        template<>
        struct Typeof<ushort> { enum { value = LUA_TNUMBER }; };

        template<>
        struct Typeof<uint> { enum { value = LUA_TNUMBER }; };

        template<>
        struct Typeof<ulong> { enum { value = LUA_TNUMBER }; };

        template<>
        struct Typeof<ullong> { enum { value = LUA_TNUMBER }; };

        template<>
        struct Typeof<float> { enum { value = LUA_TNUMBER }; };

        template<>
        struct Typeof<double> { enum { value = LUA_TNUMBER }; };

        template<>
        struct Typeof<long double> { enum { value = LUA_TNUMBER }; };

        template<>
        struct Typeof<lua_CFunction> { enum { value = LUA_TFUNCTION }; };

        template<typename T>
        struct Typeof<T*> { enum { value = LUA_TUSERDATA }; };

        template<typename T>
        inline void assert_type(lua_State *L, int idx)
        {
            int atype = lua_type(L, idx);

            /* allow nil (or invalid), as default value; do not allow
             * incompatible types though in order to prevent issues.
             */
            if (atype == LUA_TNIL || atype == LUA_TNONE) return;

            int etype = Typeof<T>::value;

            /* ditto */
            if (etype == LUA_TNIL || etype == LUA_TNONE) return;

            /* allow conversions between strings and numbers */
            if (
                (atype == LUA_TNUMBER && etype == LUA_TSTRING) ||
                (etype == LUA_TNUMBER && atype == LUA_TSTRING)
            ) return;

            if (atype != etype) luaL_error(
                L, "expected %s, got %s", lua_typename(L, etype),
                luaL_typename(L, idx)
            );
        }

        /* Function: get_value
         * Gets a value of type specified by template parameter T
         * from the stack held by state L on index idx.
         *
         * Has specializations for various types. T can be either
         * generic object that can be constructed from L and idx
         * or one of these:
         *
         *  T - generic object with (L, idx) constructor
         *  Object - <Object>
         *  Dynamic - <Dynamic>
         *  Function - <Function>
         *  Table - <Table>
         *  Nil - <lua.Nil>
         *  Boolean - bool
         *  String - const char*, const uchar*, char*, uchar*
         *  String - <String_Base.String>
         *  Character - char, uchar
         *  Integral number - short, int, long, llong,
         *  ushort, uint, ulong, ullong
         *  Floating point number - float
         *  Floating point number - double, long double
         *  C function - lua_CFunction
         *  Raw state - lua_State*.
         */
        template<typename T>
        inline T get_value(lua_State *L, int idx)
        {
            assert_type<T>(L, idx);
            return T(L, idx);
        }

        template<>
        inline Object get_value(lua_State *L, int idx)
        {
            return Object(L, idx);
        }

        template<>
        inline Dynamic get_value(lua_State *L, int idx)
        {
            return Dynamic(L, idx);
        }

        template<>
        inline Nil get_value(lua_State *L, int idx)
        {
            assert_type<Nil>(L, idx);
            return nil;
        }

        template<>
        inline bool get_value(lua_State *L, int idx)
        {
            assert_type<bool>(L, idx);
            return lua_toboolean(L, idx);
        }

        template<>
        inline const char *get_value(lua_State *L, int idx)
        {
            assert_type<const char*>(L, idx);
            return lua_tostring(L, idx);
        }

        template<>
        inline char *get_value(lua_State *L, int idx)
        {
            assert_type<char*>(L, idx);
            return (char*)lua_tostring(L, idx);
        }

        template<>
        inline const uchar *get_value(lua_State *L, int idx)
        {
            assert_type<const uchar*>(L, idx);
            return (const uchar*)lua_tostring(L, idx);
        }

        template<>
        inline uchar *get_value(lua_State *L, int idx)
        {
            assert_type<uchar*>(L, idx);
            return (uchar*)lua_tostring(L, idx);
        }

        template<>
        inline types::String get_value(lua_State *L, int idx)
        {
            assert_type<types::String>(L, idx);
            return lua_tostring(L, idx);
        }

        template<>
        inline char get_value(lua_State *L, int idx)
        {
            assert_type<char>(L, idx);
            return *lua_tostring(L, idx);
        }

        template<>
        inline uchar get_value(lua_State *L, int idx)
        {
            assert_type<uchar>(L, idx);
            return (uchar)*lua_tostring(L, idx);
        }

        template<>
        inline short get_value(lua_State *L, int idx)
        {
            assert_type<short>(L, idx);
            return lua_tointeger(L, idx);
        }

        template<>
        inline int get_value(lua_State *L, int idx)
        {
            assert_type<int>(L, idx);
            return lua_tointeger(L, idx);
        }

        template<>
        inline long get_value(lua_State *L, int idx)
        {
            assert_type<long>(L, idx);
            return lua_tointeger(L, idx);
        }

        template<>
        inline llong get_value(lua_State *L, int idx)
        {
            assert_type<llong>(L, idx);
            return lua_tointeger(L, idx);
        }

        template<>
        inline ushort get_value(lua_State *L, int idx)
        {
            assert_type<ushort>(L, idx);
            return lua_tointeger(L, idx);
        }

        template<>
        inline uint get_value(lua_State *L, int idx)
        {
            assert_type<uint>(L, idx);
            return lua_tointeger(L, idx);
        }

        template<>
        inline ulong get_value(lua_State *L, int idx)
        {
            assert_type<ulong>(L, idx);
            return lua_tointeger(L, idx);
        }

        template<>
        inline ullong get_value(lua_State *L, int idx)
        {
            assert_type<ullong>(L, idx);
            return lua_tointeger(L, idx);
        }

        template<>
        inline float get_value(lua_State *L, int idx)
        {
            assert_type<float>(L, idx);
            return lua_tonumber(L, idx);
        }

        template<>
        inline double get_value(lua_State *L, int idx)
        {
            assert_type<double>(L, idx);
            return lua_tonumber(L, idx);
        }

        template<>
        inline long double get_value(lua_State *L, int idx)
        {
            assert_type<long double>(L, idx);
            return lua_tonumber(L, idx);
        }

        template<>
        inline lua_CFunction get_value(lua_State *L, int idx)
        {
            assert_type<lua_CFunction>(L, idx);
            return lua_tocfunction(L, idx);
        }

        template<>
        inline lua_State *get_value(lua_State *L, int idx)
        {
            return L;
        }

        /* Function: pop_value
         * Gets a value from the stack using <get_value> and pops it out.
         */
        template<typename T> inline T pop_value(lua_State *L)
        {
            T ret = get_value<T>(L, -1);
            lua_pop(L, 1);
            return ret;
        }

        /* Function: get_stack
         * Gets contents of the whole stack and returns them in a <Vector>.
         * The type held by the vector is specified by the template parameter.
         * You can specify additional argument n specifying how many items
         * to get from the stack; by default it's -1 and that means it gets
         * all the items.
         */
        template<typename T>
        inline types::Vector<T> get_stack(lua_State *L, int n)
        {
            if (!n) return types::Vector<T>();
            int top = lua_gettop(L);

            types::Vector<T> ret;
            ret.reserve(top);

            if (n < 0) for (int i = 1; i <= top; ++i)
                ret.push_back(get_value<T>(L, i));
            else for (int i = (top - n + 1); i <= top; ++i)
                ret.push_back(get_value<T>(L, i));

            return ret;
        }

        /* Function: pop_stack
         * Calls <get_stack>, pops out required amount of items from the
         * stack and returns the vector.
         */
        template<typename T>
        inline types::Vector<T> pop_stack(lua_State *L, int n)
        {
            types::Vector<T> ret = get_stack<T>(L, n);
            if (n)
            {
                if (n < 0) lua_settop(L, 0);
                else lua_pop(L, n);
            }
            return ret;
        }

        template<typename T>
        inline void get_arg(lua_State *L, int idx, volatile T *ptr)
        {
            ptr->~T();
            new ((void*)ptr) T(get_value<T>(L, idx));
        }

        template<typename T>
        inline void get_arg(lua_State *L, int idx, types::Vector<T> *ptr)
        {
            if (idx > lua_gettop(L)) return;

            ptr->push_back(get_value<T>(L, idx));
            get_arg(L, idx + 1, ptr);
        }

        template<size_t i, typename... T>
        struct Lua_Tuple_Get
        {
            static void fill(lua_State *L, types::Tuple<T...>& t)
            {
                get_arg(
                    L,
                    (sizeof...(T) - i + 1),
                    &types::get<(sizeof...(T) - i)>(t)
                );
                Lua_Tuple_Get<i - 1, T...>::fill(L, t);
            }
        };

        template<typename... T>
        struct Lua_Tuple_Get<0, T...>
        {
            static void fill(lua_State *L, types::Tuple<T...>& t) {}
        };

        template<typename T, typename... U>
        struct Lua_Tuple_Call
        {
            static int call(lua_State *L, T (*f)(U...), types::Tuple<U...>& t)
            {
                T ret = types::unpack(f, t);
                push_value(L, ret);
                return 1;
            }
        };

        template<typename... T, typename... U>
        struct Lua_Tuple_Call<types::Tuple<T...>, U...>
        {
            static int call(
                lua_State *L,
                types::Tuple<T...> (*f)(U...),
                types::Tuple<U...>& t
            )
            {
                types::Tuple<T...> ret = types::unpack(f, t);
                push_value(L, ret);
                return (sizeof...(T));
            }
        };

        template<typename... T>
        struct Lua_Tuple_Call<void, T...>
        {
            static int call(
                lua_State *L, void (*f)(T...), types::Tuple<T...>& t
            )
            {
                types::unpack(f, t);
                return 0;
            }
        };

        /* Function: lua_native_wrap
         * Serves as a wrapper for C++ functions callable from
         * Lua. Expands arguments properly through a tuple.
         */
        template<typename T, typename... U>
        inline int lua_native_wrap(lua_State *L)
        {
            typedef T (*fun_t)(U...);
            fun_t f = (fun_t)lua_touserdata(L, lua_upvalueindex(1));

            types::Tuple<U...> args;
            Lua_Tuple_Get<sizeof...(U), U...>::fill(L, args);

            return Lua_Tuple_Call<T, U...>::call(L, f, args);
        }

        template<typename... T>
        inline void fill_tuple(lua_State *L, types::Tuple<T...>& tuple)
        {
            Lua_Tuple_Get<sizeof...(T), T...>::fill(L, tuple);
        }
    } /* end namespace stack */
} /* end namespace lua */
