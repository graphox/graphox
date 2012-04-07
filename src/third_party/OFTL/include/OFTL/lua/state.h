/* File: OFTL/lua/state.h
 *
 * About: Version
 *  This is version 2 of the file.
 *
 * About: Purpose
 *  The state class for the Lua interface.
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
    /* Enum: Error_Handler
     * Specifies how errors should be handled in <State.do_string> and
     * <State.do_file>. ERROR_NONE makes it not push the traceback.
     * ERROR_TRACEBACK does. ERROR_EXIT throws a Lua error without
     * traceback. ERROR_EXIT_TRACEBACK throws a Lua error with
     * traceback.
     */
    enum Error_Handler
    {
        ERROR_NONE,
        ERROR_TRACEBACK,
        ERROR_EXIT,
        ERROR_EXIT_TRACEBACK
    };

    /* Struct: State
     * The state class. It holds the raw Lua state, the global and
     * registry tables, the traceback and are the thing you interface
     * with in exposed functions.
     */
    struct State
    {
        /* Constructor: State
         * Initializes a state. As this has to create a new Lua raw
         * state, it marks itself as owner of the raw state.
         */
        State(): p_state(luaL_newstate()), p_owner(true)
        {
            p_init();
        }

        /* Constructor: State
         * Initializes a state from existing Lua state, so it's
         * not its owner.
         */
        State(const State& s): p_state(s.p_state), p_owner(false)
        {
            p_init();
        }

        /* Constructor: State
         * Initializes a state from existing raw Lua state, so it's
         * not its owner.
         */
        State(lua_State *L): p_state(L), p_owner(false)
        {
            p_init();
        }

        /* Destructor: State
         * This clears up traceback, registry and global tables and
         * if it's also the owner of the raw state, deinitializes
         * the raw state.
         */
        ~State()
        {
            p_registry .clear();
            p_globals  .clear();
            p_traceback.clear();

            if (p_owner) lua_close(p_state);
        }

        /* Function: state
         * Returns the raw Lua state held by this State.
         */
        lua_State *state() const { return p_state; }

        /* Function: state
         * Retrieves a Lua state from a raw pointer.
         */
        static State from_pointer(lua_State *L)
        {
            lua_getfield(L, LUA_REGISTRYINDEX, "__oftlstate");
            const State& lua = *((State*)lua_touserdata(L, -1));
            lua_pop(L, 1);
            return lua;
        }

        /* Function: set_panic_handler
         * Sets a panic handler. It's a function that takes two arguments,
         * the first one being const reference to State, the second one
         * being the error message. It's called everytime something
         * throws an error.
         */
        void set_panic_handler(void (*fun)(const State&, const char*))
        {
            lua_pushlightuserdata(p_state, (void*)fun);
            lua_setfield(p_state, LUA_REGISTRYINDEX, "__oftlpanic");
        }

        /* Function: error
         * Manually throws an error.
         */
        int error(const char *fmt, ...)
        {
            va_list  ap;
            va_start(ap, fmt);

            luaL_where      (p_state, 1);
            lua_pushvfstring(p_state, fmt, ap);
            va_end(ap);

            lua_concat      (p_state, 2);
            return lua_error(p_state);
        }

        /* Function: open_libs
         * Opens all core Lua libraries. Use open_* to open the
         * specific ones or just use raw Lua functions.
         */
        void open_libs()
        {
            open_base   ();
            open_table  ();
            open_io     ();
            open_os     ();
            open_string ();
            open_math   ();
            open_debug  ();
            open_package();
        }

        /* Function: open_base */
        void open_base()
        {
            lua_pushcfunction(p_state, luaopen_base);
            lua_call         (p_state, 0, 0);
        }

        /* Function: open_table */
        void open_table()
        {
            lua_pushcfunction(p_state, luaopen_table);
            lua_call         (p_state, 0, 0);
        }

        /* Function: open_io */
        void open_io()
        {
            lua_pushcfunction(p_state, luaopen_io);
            lua_call         (p_state, 0, 0);
        }

        /* Function: open_os */
        void open_os()
        {
            lua_pushcfunction(p_state, luaopen_os);
            lua_call         (p_state, 0, 0);
        }

        /* Function: open_string */
        void open_string()
        {
            lua_pushcfunction(p_state, luaopen_string);
            lua_call         (p_state, 0, 0);
        }

        /* Function: open_math */
        void open_math()
        {
            lua_pushcfunction(p_state, luaopen_math);
            lua_call         (p_state, 0, 0);
        }

        /* Function: open_debug
         * This one besides opening the library also
         * initializes the traceback function in the
         * state (which is a reference to debug.traceback)
         */
        void open_debug()
        {
            lua_pushcfunction(p_state, luaopen_debug);
            lua_call         (p_state, 0, 0);

            new (&p_traceback) Function(
                 p_globals.get<Function>("debug", "traceback")
            );
        }

        /* Function: open_package */
        void open_package()
        {
            lua_pushcfunction(p_state, luaopen_package);
            lua_call         (p_state, 0, 0);
        }

        /* Function: globals
         * Returns a const reference to the global table
         * belonging to the current state.
         */
        const Table& globals() const { return p_globals; }

        /* Function: registry
         * Returns a const reference to the registry table
         * belonging to the current state.
         */
        const Table& registry() const { return p_registry; }

        /* Function: load_string
         * Compiles Lua code contained in the given string.
         * Returns a tuple of 3 values of types int (error code,
         * zero on success, otherwise see lua_load), const char*
         * (error message, NULL on success) and Function (on
         * success, the runnable compiled function).
         */
        types::Tuple<int, const char*, Function> load_string(
            const char *code
        ) const
        {
            int err = luaL_loadstring(p_state, code);
            if (err)
            {
                types::Tuple<
                    int, const char*, Function
                > ret = types::make_tuple(
                    err, lua_tostring(p_state, -1), Function()
                );
                lua_pop(p_state, 1);
                return ret;
            }
            else return types::make_tuple<int, const char*, Function>(
                0, NULL, stack::pop_value<Function>(p_state)
            );
        }

        /* Function: load_string */
        types::Tuple<int, const char*, Function> load_string(
            const types::String& code
        ) const
        {
            return load_string(code.get_buf());
        }

        /* Function: load_file
         * Compiles Lua code contained in a file in the given path.
         * Behaves and returns the same as <load_string>.
         */
        types::Tuple<int, const char*, Function> load_file(
            const char *path
        ) const
        {
            int err = luaL_loadfile(p_state, path);
            if (err)
            {
                types::Tuple<
                    int, const char*, Function
                > ret = types::make_tuple(
                    err, lua_tostring(p_state, -1), Function()
                );
                lua_pop(p_state, 1);
                return ret;
            }
            else return types::make_tuple<int, const char*, Function>(
                0, NULL, stack::pop_value<Function>(p_state)
            );
        }

        /* Function: load_file */
        types::Tuple<int, const char*, Function> load_file(
            const types::String& path
        ) const
        {
            return load_file(path.get_buf());
        }

        /* Function: do_string
         * Runs Lua code contained in the given string. See also
         * <lua.Error_Handler>. This function returns a tuple
         * of two values, an integer (being 0 on success, and one
         * of error codes returned by luaL_loadstring or lua_pcall
         * on failure) and a const char*, being NULL on success
         * and error message on failure.
         */
        types::Tuple<int, const char*> do_string(
            const char *code, Error_Handler handler = ERROR_NONE
        ) const
        {
            return p_do_chunk(&luaL_loadstring, code, handler);
        }

        /* Function: do_string */
        types::Tuple<int, const char*> do_string(
            const types::String& code, Error_Handler handler = ERROR_NONE
        ) const
        {
            return do_string(code.get_buf(), handler);
        }

        /* Function: do_file
         * See <do_string>. This behaves and returns the same, but instead
         * of running a piece of code, it runs a file containing Lua code.
         */
        types::Tuple<int, const char*> do_file(
            const char *path, Error_Handler handler = ERROR_NONE
        ) const
        {
            return p_do_chunk(&luaL_loadfile, path, handler);
        }

        /* Function: do_file */
        types::Tuple<int, const char*> do_file(
            const types::String& path, Error_Handler handler = ERROR_NONE
        ) const
        {
            return do_file(path.get_buf(), handler);
        }

        /* Function: do_string
         * Runs Lua code contained in the string, saving the return
         * values into a tuple given by "retvals". Otherwise behaves
         * the same as <do_string> above.
         */
        template<typename... T> types::Tuple<int, const char*> do_string(
            const char *code,
            types::Tuple<T...>& retvals,
            Error_Handler handler = ERROR_NONE
        ) const
        {
            types::Tuple<int, const char*> ret = p_do_chunk(
                &luaL_loadstring, code, handler, sizeof...(T)
            );
            if (types::get<0>(ret)) return ret;

            stack::fill_tuple(p_state, retvals);
            return ret;
        }

        /* Function: do_string */
        template<typename... T> types::Tuple<int, const char*> do_string(
            const types::String& code,
            types::Tuple<T...>& retvals,
            Error_Handler handler = ERROR_NONE
        ) const
        {
            return do_string(code.get_buf(), retvals, handler);
        }

        /* Function: do_file
         * Runs Lua code contained in the file, saving the return
         * values into a tuple given by "retvals". Otherwise behaves
         * the same as <do_file> above.
         */
        template<typename... T> types::Tuple<int, const char*> do_file(
            const char *path,
            types::Tuple<T...>& retvals,
            Error_Handler handler = ERROR_NONE
        ) const
        {
            types::Tuple<int, const char*> ret = p_do_chunk(
                &luaL_loadfile, path, handler, sizeof...(T)
            );
            if (types::get<0>(ret)) return ret;

            stack::fill_tuple(p_state, retvals);
            return ret;
        }

        /* Function: do_file */
        template<typename... T> types::Tuple<int, const char*> do_file(
            const types::String& path,
            types::Tuple<T...>& retvals,
            Error_Handler handler = ERROR_NONE
        ) const
        {
            return do_file(path.get_buf(), retvals, handler);
        }

        /* Function: new_table
         * Creates a new Lua table and returns it. The arguments are
         * optional. They specify the number of pre-initialized array
         * slots and the number of pre-initialized non-array slots.
         */
        Table new_table(size_t narr = 0, size_t nrec = 0) const
        {
            lua_createtable(p_state, narr, nrec);
            return stack::pop_value<Table>(p_state);
        }

        /* Function: register_module
         * Registers a Lua module, given a name and a table with functions.
         * If such module is not yet registered, the table will basically
         * become the module, including possible metatable.
         *
         * If such module is already registered, the table will be appended
         * to it without overriding metatable.
         *
         * If such global variable already exists and it's not a module,
         * it'll fail with name clash error.
         */
        void register_module(const char *name, const Table& t)
        {
            Table loaded = p_registry["_LOADED"];
            if (loaded[name].is_nil())
            {
                if (p_globals[name].is_nil())
                {
                    loaded[name]    = t;
                    p_globals[name] = t;
                }
                else
                    luaL_error(p_state, "Name conflict for module %s.", name);
            }
            else
            {
                Table module = loaded[name];
                for (Table::pit it = t.pbegin(); it != t.pend(); ++it)
                    module[(*it).first] = (*it).second;
            }
        }

        /* Function: register_module
         * See above.
         */
        void register_module(const types::String& name, const Table& t)
        {
            register_module(name.get_buf(), t);
        }

        /* Function: wrap
         * Wraps any given value of type U supported by <stack.push_value>
         * in a container of type T (by default <Object>). Useful for i.e.
         * wrapping real C++ values in the <Dynamic> container and then
         * doing Lua actions on them.
         */
        template<typename T = Object, typename U>
        T wrap(const U& value) const
        {
            stack::push_value(p_state, value);
            return stack::pop_value<T>(p_state);
        }

        /* Function: get
         * Gets something from the global table. This function has the
         * same behavior as <Table.get> (it's done on the global table).
         */
        template<typename T = Object, typename... U>
        T get(const U&... args) const
        {
            return p_globals.get<T>(args...);
        }

        /* Function: set
         * Gives a global variable of the given name the given value.
         * Performs on the global table.
         */
        template<typename T, typename U>
        void set(const T& key, const U& value) const
        {
            p_globals.set(key, value);
        }

        /* Operator: []
         * This operator is for both getting and setting. It returns
         * <Table_Item>, which you can further convert or assign.
         * The returned table item belongs to the global table.
         */
        template<typename T>
        Table_Item<T> operator[](const T& key) const
        {
            return Table_Item<T>(key, p_globals);
        }

        /* Function: pop_stack
         * See <stack.pop_stack>.
         */
        template<typename T>
        types::Vector<T> pop_stack(int n = -1) const
        {
            return stack::pop_stack<T>(p_state, n);
        }

        /* Function: get_stack
         * See <stack.get_stack>.
         */
        template<typename T>
        types::Vector<T> get_stack(int n = -1) const
        {
            return stack::get_stack<T>(p_state, n);
        }

        /* Function: pop_value
         * See <stack.pop_value>.
         */
        template<typename T>
        T pop_value() const
        {
            return stack::pop_value<T>(p_state);
        }

        /* Function: push_value
         * See <stack.push_value>.
         */
        template<typename T>
        void push_value(const T& arg) const
        {
            stack::push_value(p_state, arg);
        }

    private:

        static int p_panic(lua_State *L)
        {
            const char *msg = lua_tostring(L, -1);

            lua_getfield(L, LUA_REGISTRYINDEX, "__oftlpanic");
            void (*fun)(const State&, const char*) =
              (void (*)(const State&, const char*))lua_touserdata(L, -1);

            lua_pop(L, 1);

            if (!fun) fprintf(
                stderr, "OFTL::lua: error in call to the Lua API (%s)\n", msg
            ); else fun(State::from_pointer(L), msg);

            return 0;
        }

        void p_init()
        {
            new (&p_globals) Table(
                Table(p_state, LUA_GLOBALSINDEX)
            );
            new (&p_registry) Table(
                Table(p_state, LUA_REGISTRYINDEX)
            );

            if (p_owner)
            {
                lua_atpanic(p_state, p_panic);

                lua_pushlightuserdata(p_state, (void*)this);
                lua_setfield(p_state, LUA_REGISTRYINDEX, "__oftlstate");
            }
        }

        void p_push_error_handler() const
        {
            if (p_traceback.is_nil()) fprintf(
                stderr, "traceback requires open_debug() or open_libs().\n"
            );

            p_traceback.push();
        }

        template<typename T> types::Tuple<int, const char*> p_do_chunk(
            const T& loader,
            const char *s,
            Error_Handler handler,
            int numrets = 0
        ) const
        {
            if (handler == ERROR_TRACEBACK || handler == ERROR_EXIT_TRACEBACK)
                p_push_error_handler();

            int err = loader(p_state, s);
            if (err)
            {
                types::Tuple<int, const char*> ret = types::make_tuple(
                    err, lua_tostring(p_state, -1)
                );
                lua_pop(p_state, 1);
                return ret;
            }

            err = lua_pcall(
                p_state, 0, numrets,
                (handler == ERROR_TRACEBACK ||
                 handler == ERROR_EXIT_TRACEBACK) ? -2 : 0
            );
            if (err)
            {
                types::Tuple<int, const char*> ret = types::make_tuple(
                    err, lua_tostring(p_state, -1)
                );
                if (handler == ERROR_EXIT || handler == ERROR_EXIT_TRACEBACK)
                    lua_error(p_state);
                else
                    lua_pop(p_state, 1);
                return ret;
            }

            if (handler == ERROR_TRACEBACK || handler == ERROR_EXIT_TRACEBACK)
                lua_remove(p_state, 1);

            return types::make_tuple<int, const char*>(0, NULL);
        }

        lua_State *p_state;

        Table p_globals;
        Table p_registry;

        Function p_traceback;

        bool p_owner;
    };
} /* end namespace lua */
