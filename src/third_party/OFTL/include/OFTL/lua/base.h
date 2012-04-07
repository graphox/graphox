/* File: OFTL/lua/base.h
 *
 * About: Version
 *  This is version 2 of the file.
 *
 * About: Purpose
 *  Basic Lua object definition, Lua types and nil.
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
    struct State;
    struct Object;
    struct Dynamic;
    struct Function;
    struct Table;
    template<typename T>
    struct Table_Item;

    /* Enum: Type
     * Enumeration defining core Lua types. Returned by <Object.type>.
     *
     * Values:
     *
     *  TYPE_STRING
     *  TYPE_NUMBER
     *  TYPE_TABLE
     *  TYPE_NIL
     *  TYPE_BOOLEAN
     *  TYPE_FUNCTION
     *  TYPE_USERDATA
     *  TYPE_LIGHTUSERDATA
     *  TYPE_THREAD
     *
     * Please refer to the Lua documentation for more information.
     */
    enum Type
    {
        TYPE_STRING        = LUA_TSTRING,
        TYPE_NUMBER        = LUA_TNUMBER,
        TYPE_TABLE         = LUA_TTABLE,
        TYPE_NIL           = LUA_TNIL,
        TYPE_BOOLEAN       = LUA_TBOOLEAN,
        TYPE_FUNCTION      = LUA_TFUNCTION,
        TYPE_USERDATA      = LUA_TUSERDATA,
        TYPE_LIGHTUSERDATA = LUA_TLIGHTUSERDATA,
        TYPE_THREAD        = LUA_TTHREAD
    };

    /* Variable: Nil
     * An empty structure you don't usually make instances of, the
     * only (singleton) instance is <nil>.
     */
    struct Nil {};

    /* Variable: nil
     * The "nil" value that can be assigned as table member etc.
     * It represents an instance of an empty struct, basically.
     */
    static Nil nil;

    namespace stack
    {
        template<typename T>
        inline void push_value(lua_State *L, const T& value);
        inline void push_value(lua_State *L, const Object& value);
        inline void push_value(lua_State *L, const Dynamic& value);
        inline void push_value(lua_State *L, const Function& value);
        inline void push_value(lua_State *L, const Table& value);
        template<typename T>
        inline void push_value(lua_State *L, const Table_Item<T>& value);
        inline void push_value(lua_State *L, const Nil& value);
        inline void push_value(lua_State *L, bool value);
        inline void push_value(lua_State *L, char value);
        inline void push_value(lua_State *L, uchar value);
        inline void push_value(lua_State *L, short value);
        inline void push_value(lua_State *L, int value);
        inline void push_value(lua_State *L, long value);
        inline void push_value(lua_State *L, llong value);
        inline void push_value(lua_State *L, ushort value);
        inline void push_value(lua_State *L, uint value);
        inline void push_value(lua_State *L, ulong value);
        inline void push_value(lua_State *L, ullong value);
        inline void push_value(lua_State *L, float value);
        inline void push_value(lua_State *L, double value);
        inline void push_value(lua_State *L, long double value);
        inline void push_value(lua_State *L, const char *value);
        inline void push_value(lua_State *L, const uchar *value);
        inline void push_value(lua_State *L, char *value);
        inline void push_value(lua_State *L, uchar *value);
        inline void push_value(lua_State *L, const types::String& value);
        inline void push_value(lua_State *L, lua_CFunction value);

        template<typename T>
        inline void push_value(lua_State *L, const types::Vector<T>& v);
        template<typename... T>
        inline void push_value(lua_State *L, const types::Tuple<T...>& t);
#ifdef OFTL_MAP_H
        template<typename T, typename U>
        inline void push_value(lua_State *L, const types::Map<T, U>& v);
#endif
#ifdef OFTL_HASHMAP_H
        template<typename T, typename U>
        inline void push_value(lua_State *L, const types::Hash_Map<T, U>& v);
#endif
        template<typename T, typename... U>
        inline void push_value(lua_State *L, T (*fun)(U...));

        template<typename T> inline T get_value(lua_State *L, int idx);
        template<> inline Object get_value(lua_State *L, int idx);
        template<> inline Dynamic get_value(lua_State *L, int idx);
        template<> inline Nil get_value(lua_State *L, int idx);
        template<> inline bool get_value(lua_State *L, int idx);
        template<> inline const char *get_value(lua_State *L, int idx);
        template<> inline char *get_value(lua_State *L, int idx);
        template<> inline const uchar *get_value(lua_State *L, int idx);
        template<> inline uchar *get_value(lua_State *L, int idx);
        template<> inline types::String get_value(lua_State *L, int idx);
        template<> inline char get_value(lua_State *L, int idx);
        template<> inline uchar get_value(lua_State *L, int idx);
        template<> inline short get_value(lua_State *L, int idx);
        template<> inline int get_value(lua_State *L, int idx);
        template<> inline long get_value(lua_State *L, int idx);
        template<> inline llong get_value(lua_State *L, int idx);
        template<> inline ushort get_value(lua_State *L, int idx);
        template<> inline uint get_value(lua_State *L, int idx);
        template<> inline ulong get_value(lua_State *L, int idx);
        template<> inline ullong get_value(lua_State *L, int idx);
        template<> inline float get_value(lua_State *L, int idx);
        template<> inline double get_value(lua_State *L, int idx);
        template<> inline long double get_value(lua_State *L, int idx);
        template<> inline lua_CFunction get_value(lua_State *L, int idx);
        template<> inline lua_State *get_value(lua_State *L, int idx);

        template<typename T> inline T pop_value(lua_State *L);

        template<typename T>
        inline types::Vector<T> pop_stack(lua_State *L, int n = -1);

        template<typename T>
        inline types::Vector<T> get_stack(lua_State *L, int n = -1);

        template<typename... T>
        inline void fill_tuple(lua_State *L, types::Tuple<T...>& tuple);
    }

    /* Struct: Object
     * This is an object, a container holding a reference to any
     * Lua type. The type is unreferenced when the destructor is
     * called.
     *
     * You can also retrieve actual C++ values, the raw Lua state
     * and others using this.
     */
    struct Object
    {
        /* Constructor: Object */
        Object(): p_ref(LUA_REFNIL), p_state(NULL) {}

        /* Constructor: Object
         * Constructs the object from raw Lua state and
         * the index of the item on the stack.
         */
        Object(lua_State *state, int idx):
            p_ref(LUA_REFNIL), p_state(state)
        {
            lua_pushvalue   (p_state, idx);
            p_ref = luaL_ref(p_state, LUA_REGISTRYINDEX);
        }

        /* Constructor: Object
         * Constructs the object from an object of any type that
         * contains a method "object" returning Object.
         */
        template<typename T>
        Object(const T& o)
        {
            const Object& obj = o.object();
            if (&obj == this || !obj.p_state)
            {
                p_state = NULL;
                p_ref   = LUA_REFNIL;
                return;
            }

            p_state = o.object().p_state;
            lua_rawgeti(p_state, LUA_REGISTRYINDEX, o.object().p_ref);
            p_ref = luaL_ref(p_state, LUA_REGISTRYINDEX);
        }

        /* Constructor: Object
         * Copy constructor. Constructs an Object from another Object.
         */
        Object(const Object& o)
        {
            if (&o == this || !o.p_state)
            {
                p_state = NULL;
                p_ref   = LUA_REFNIL;
                return;
            }

            p_state = o.p_state;
            lua_rawgeti(p_state, LUA_REGISTRYINDEX, o.p_ref);
            p_ref = luaL_ref(p_state, LUA_REGISTRYINDEX);
        }

        /* Operator: =
         * See the appropriate constructor. Returns *this.
         */
        template<typename T>
        Object& operator=(const T& o)
        {
            const Object& obj = o.object();

            if (&obj == this || !obj.p_state)
            {
                p_state = NULL;
                p_ref   = LUA_REFNIL;
                return *this;
            }

            p_state = o.object().p_state;
            lua_rawgeti(p_state, LUA_REGISTRYINDEX, o.object().p_ref);
            p_ref = luaL_ref(p_state, LUA_REGISTRYINDEX);

            return *this;
        }

        /* Operator: =
         * See the appropriate constructor. Returns *this.
         */
        Object& operator=(const Object& o)
        {
            if (&o == this || !o.p_state)
            {
                p_state = NULL;
                p_ref   = LUA_REFNIL;
                return *this;
            }

            p_state = o.p_state;
            lua_rawgeti(p_state, LUA_REGISTRYINDEX, o.p_ref);
            p_ref = luaL_ref(p_state, LUA_REGISTRYINDEX);

            return *this;
        }

        /* Destructor: Object
         * Unreferences the held reference.
         */
        ~Object()
        {
            if (p_ref != LUA_REFNIL)
                luaL_unref(p_state, LUA_REGISTRYINDEX, p_ref);
        }

        /* Function: push
         * Pushes the held object onto the stack.
         */
        void push() const
        {
            assert(p_state);
            lua_rawgeti(p_state, LUA_REGISTRYINDEX, p_ref);
        }

        /* Function: state
         * Returns the raw Lua state for the object.
         */
        lua_State *state() const { return p_state; }

        /* Function: clear
         * Unreferences the held reference and NULLs the
         * state pointer. Useful to get rid of objects
         * before the end of their lifetime.
         */
        void clear()
        {
            if (p_state && p_ref != LUA_REFNIL)
                luaL_unref(p_state, LUA_REGISTRYINDEX, p_ref);

            p_ref   = LUA_REFNIL;
            p_state = NULL;
        }

        /* Function: type
         * See <lua.Type>. Returns the type of the value held by this object.
         */
        Type type() const
        {
            assert(p_state);
            push();

            Type t = (Type)lua_type(p_state, -1);
            lua_pop(p_state, 1);

            return t;
        }

        /* Function: type_name
         * See <type>. Returns the type name.
         */
        types::String type_name() const
        {
            assert(p_state);
            push();

            const char *str = luaL_typename(p_state, -1);
            lua_pop(p_state, 1);

            return str;
        }

        /* Function: is_nil
         * Returns true if the object is nil, false otherwise.
         */
        bool is_nil() const
        {
            return (p_ref == LUA_REFNIL);
        }

        /* Function: to
         * Returns the value of the object converted into the
         * given type.
         */
        template<typename T> T to() const
        {
            assert(p_state);
            push();
            return stack::pop_value<T>(p_state);
        }

        /* Function: equals
         * Checks for value equality with another object.
         */
        template<typename T> bool equals(const T& o) const
        {
            assert(p_state);
            push();
            stack::push_value(p_state, o);

            bool eq = lua_equal(p_state, -1, -2);
            lua_pop(p_state, 2);

            return eq;
        }

        /* Operator: ==
         * See <equals>.
         */
        template<typename T> bool operator==(const T& o) const
        { return equals(o); }

        /* Function: object
         * Returns this.
         */
        const Object& object() const { return *this; }

    protected:

        int        p_ref;
        lua_State *p_state;
    };

    /* Function: check_type
     * Checks the type of the value at the given position on the
     * stack and fails if it isn't the expected one. Sends an
     * error to Lua in that case.
     */
    inline void check_type(
        lua_State *L, int idx, int expected_type,
        const char *expected_name
    )
    {
        int t = lua_type(L, idx);

        /* allow nil or invalid values, see
         * assert_type in stack namespace */
        if (t == LUA_TNIL || t == LUA_TNONE) return;

        if (t != expected_type) luaL_error(
            L, "attempt to create %s with %s",
            expected_name, lua_typename(L, t)
        );
    }
} /* end namespace lua */
