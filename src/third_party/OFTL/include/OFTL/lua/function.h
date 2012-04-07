/* File: OFTL/lua/function.h
 *
 * About: Version
 *  This is version 2 of the file.
 *
 * About: Purpose
 *  Function class for the OFTL Lua interface.
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
    /* Struct: Function
     * This represents a Lua function. Example:
     *
     * (start code)
     *     lua::Function print = lua["print"];
     *     print("hello world");
     * (end)
     *
     * Inherits <Object>.
     */
    struct Function: Object
    {
        template<size_t i, typename... T>
        struct TP_Get
        {
            template<typename U>
            static void get_arg(lua_State *L, int idx, U *ptr)
            {
                ptr->~U();
                new (ptr) U(stack::get_value<U>(L, idx));
            }

            static void fill(lua_State *L, types::Tuple<T...>& t)
            {
                get_arg(
                    L,
                    (sizeof...(T) - i + 1),
                    &types::get<(sizeof...(T) - i)>(t)
                );
                TP_Get<i - 1, T...>::fill(L, t);
            }
        };

        template<typename... T>
        struct TP_Get<0, T...>
        {
            static void fill(lua_State *L, types::Tuple<T...>& t) {}
        };

        template<typename... T> struct Call_Ret_Type;
        template<typename    T> struct Call_Ret_Type<T>
        { typedef T type; };

        template<typename T, typename... U> struct Call_Ret_Type<T, U...>
        { typedef types::Tuple<T, U...> type; };

        template<size_t i, typename... T>
        struct Call_Ret_Val
        {
            template<typename... U>
            static types::Tuple<T...> call(lua_State *L, const U&... args)
            {
                p_push_value(L, args...);

                lua_call(L, sizeof...(U), sizeof...(T));

                types::Tuple<T...> ret;
                TP_Get<sizeof...(T), T...>::fill(L, ret);
                lua_pop(L, (int)sizeof...(T));
                return ret;
            }
        };

        template<typename T>
        struct Call_Ret_Val<1, T>
        {
            template<typename... U>
            static T call(lua_State *L, const U&... args)
            {
                p_push_value(L, args...);

                lua_call(L, sizeof...(U), 1);
                T ret = stack::get_value<T>(L, -1);
                lua_pop(L, 1);
                return ret;
            }
        };

        /* Constructor: Function */
        Function(): Object() {}

        /* Constructor: Function
         * See <Object.Object>.
         */
        Function(lua_State *L, int idx): Object(L, idx)
        { check_type(L, idx, LUA_TFUNCTION, "Function"); }

        /* Constructor: Function
         * See <Object.Object>.
         */
        template<typename T> Function(const T& o): Object(o) {}

        /* Operator: ()
         * Calls the function with given arguments. Returns
         * nothing. If you want to get return values, use
         * <call>.
         */
        template<typename... T>
        void operator()(const T&... args) const
        {
            assert(Object::state());

            Object::push();

            p_push_value(Object::state(), args...);
            lua_call    (Object::state(), sizeof...(T), 0);
        }

        /* Function: call
         * Calls the function with given arguments. Returns
         * either one value (if you pass one template argument) or
         * a tuple of values (if you pass multiple ones). Template
         * arguments basically specify types of return values.
         */
        template<typename... T, typename... U>
        typename Call_Ret_Type<T...>::type call(const U&... args) const
        {
            assert(Object::state());

            Object::push();

            return Call_Ret_Val<sizeof...(T), T...>::call(
                Object::state(), args...
            );
        }

        /* Function: call
         * See <()>.
         */
        template<typename... T>
        void call(const T&... args) const
        {
            return operator()(args...);
        }

    private:

        template<typename T, typename... U>
        static void p_push_value(lua_State *L, const T& v, const U&... args)
        {
            stack::push_value(L, v);
            p_push_value(L, args...);
        }

        static void p_push_value(lua_State *L) {}
    };
} /* end namespace lua */
