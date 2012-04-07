/* File: OFTL/lua/dynamic.h
 *
 * About: Version
 *  This is version 2 of the file.
 *
 * About: Purpose
 *  Dynamic class for OFTL Lua interface.
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
    /* Struct: Dynamic
     * This is a "Dynamic", a generic object that can behave as a value,
     * a bit like table, function, etc. It can well represent a value to
     * call methods on:
     *
     * (start code)
     *     lua::Dynamic s = lua.wrap<lua::Dynamic>("foo");
     *     // retvals is a tuple of two types, const char* and int.
     *     // they represent the results of gsub method call on Lua string.
     *     auto retvals = s.method<const char*, int>("gsub", "foo", "bar");
     *     const char *s = types::get<0>(retvals);
     *     int n = types::get<1>(retvals);
     * (end)
     *
     * Inherits <Function>.
     */
    struct Dynamic: Function
    {
        /* Constructor: Dynamic */
        Dynamic(): Function() {}

        /* Constructor: Dynamic
         * See <Object.Object>.
         */
        Dynamic(lua_State *L, int idx): Function()
        {
            lua_pushvalue             (Function::p_state, idx);
            Function::p_ref = luaL_ref(Function::p_state, LUA_REGISTRYINDEX);
        }

        /* Constructor: Dynamic
         * See <Function.Function>.
         */
        template<typename T> Dynamic(const T& o): Function(o) {}

        /* Function: method
         * See <Function.call>. This is the same, except that it performs
         * a method call on the object instead of function call. The name
         * of the method is specified through the first argument.
         */
        template<typename... T, typename... U>
        typename Function::Call_Ret_Type<T...>::type method(
            const char *name, const U&... args
        ) const
        {
            assert(Object::state());

            Object::push();

            lua_pushlstring(
                Object::state(), name, strlen(name)
            );

            lua_gettable (Object::state(), -2);
            lua_pushvalue(Object::state(),  1);
            lua_remove   (Object::state(),  1);

            return Function::Call_Ret_Val<sizeof...(T), T...>::call(
                Object::state(), args...
            );
        }

        /* Function: method
         * Non-returning version of <method>.
         */
        template<typename... T> void method(
            const char *name, const T&... args
        ) const
        {
            assert(Object::state());

            Object::push();

            lua_pushlstring(
                Object::state(), name, strlen(name)
            );

            lua_gettable (Object::state(), -2);
            lua_pushvalue(Object::state(),  1);
            lua_remove   (Object::state(),  1);

            Function::p_push_value(Object::state(), args...);
            lua_call(Object::state(), sizeof...(T), 0);
        }

        /* Function: method
         * See above. Takes <String_Base.String> instead of
         * const char* as the first argument.
         */
        template<typename... T, typename... U>
        typename Function::Call_Ret_Type<T...>::type method(
            const types::String& name, const U&... args
        ) const
        {
            return method<T...>(name.get_buf(), args...);
        }

        /* Function: method
         * See above. Takes <String_Base.String> instead of
         * const char* as the first argument.
         */
        template<typename... T>
        void method(const types::String& name, const T&... args) const
        {
            return method(name.get_buf(), args...);
        }

        /* Operator: []
         * Assuming the value is a table-like value,
         * it retrieves a member as Dynamic.
         */
        template<typename T>
        Dynamic operator[](const T& key) const
        {
            assert(Object::state());

            Object::push();

            stack::push_value(Object::state(), key);
            lua_gettable     (Object::state(),  -2);

            Dynamic ret = stack::get_value<Dynamic>(
                Object::state(), -1
            );

            lua_pop(Object::state(), 2);
            return ret;
        }

        /* Function: length
         * Returns the length of the held value.
         */
        size_t length() const
        {
            assert(Object::state());

            push();
            size_t ret = lua_objlen(Object::state(), -1);

            lua_pop(Object::state(), 1);
            return ret;
        }
    };
} /* end namespace lua */
