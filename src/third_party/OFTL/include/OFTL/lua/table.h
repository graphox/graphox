/* File: OFTL/lua/table.h
 *
 * About: Version
 *  This is version 2 of the file.
 *
 * About: Purpose
 *  Table class for the OFTL Lua interface.
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
    struct Table;

    template<typename T> struct Table_Item;

    /* Struct: Table_Iterator_Base
     * A forward iterator for <Table>. Inherits into non-paired version
     * <Table_Iterator> and paired version <Table_Pair_Iterator>.
     *
     * Note that Lua table iterators are not full forward iterators,
     * they're missing some features (post-increment, -> operator
     * and true iterator comparisons).
     */
    template<typename T> struct Table_Iterator_Base
    {
        /* Constructor: Table_Iterator_Base
         * Constructs an empty iterator.
         */
        Table_Iterator_Base():
            p_table(NULL), p_status(0), p_refs(NULL) {}

        /* Constructor: Table_Iterator_Base
         * Constructs an iterator from another iterator.
         */
        Table_Iterator_Base(const Table_Iterator_Base& it):
            p_table(it.p_table), p_status(it.p_status), p_refs(it.p_refs)
        {
            ++*p_refs;
        }

        /* Constructor: Table_Iterator_Base
         * Constructs an iterator from a <Table>.
         */
        Table_Iterator_Base(const T *t):
            p_table(t), p_status(0), p_refs(new int(1))
        {
            p_table->push();
            lua_pushnil(p_table->state());
            p_status = lua_next(p_table->state(), -2);
        }

        /* Destructor: Table_Iterator_Base
         * Takes care of proper stack handling when all iterators to the
         * current table disappear.
         */
        ~Table_Iterator_Base()
        {
            if (p_table && p_refs && --(*p_refs) == 0)
            {
                delete p_refs;
                lua_pop(p_table->state(), (p_status == 0) ? 1 : 3);
            }
        }

        /* Operator: ++
         * Moves on to the next item, prefix version.
         */
        Table_Iterator_Base& operator++()
        {
            if (p_status == 0) return *this;

            lua_pop(p_table->state(), 1);
            p_status = lua_next(p_table->state(), -2);

            return *this;
        }

        /* Operator: ++
         * Moves on to the next item, postfix version,
         * same effect as the prefix version.
         */
        Table_Iterator_Base& operator++(int)
        { return operator++(); }

        /* Operator: ==
         * Note that this compares just the iterator status, not the
         * iterator, so it's useful just for checking if the iterator
         * is valid or invalid (against iterator returned by
         * <Table.end> or <Table.pend>).
         */
        friend bool operator==(
            const Table_Iterator_Base& a, const Table_Iterator_Base& b
        )
        { return (a.p_status == b.p_status); }

        /* Operator: !=
         * Note that this compares just the iterator status, not the
         * iterator, so it's useful just for checking if the iterator
         * is valid or invalid (against iterator returned by
         * <Table.end> or <Table.pend>).
         */
        friend bool operator!=(
            const Table_Iterator_Base& a, const Table_Iterator_Base& b
        )
        { return (a.p_status != b.p_status); }

    private:

        const T *p_table;
        int  p_status;
        int *p_refs;

        template<typename U> friend struct Table_Iterator;
        template<typename U> friend struct Table_Pair_Iterator;
    };

    /* Struct: Table_Iterator
     * A forward iterator for <Table>. See <Table_Iterator_Base>.
     * This one works with values only, no keys.
     */
    template<typename T> struct Table_Iterator: Table_Iterator_Base<T>
    {
        /* Typedef: diff_t */
        typedef ptrdiff_t diff_t;
        /* Typedef: val_t */
        typedef Object val_t;
        /* Typedef: ptr_t */
        typedef void ptr_t;
        /* Typedef: ref_t */
        typedef void ref_t;

        Table_Iterator(): Table_Iterator_Base<T>() {}

        Table_Iterator(const Table_Iterator& it):
            Table_Iterator_Base<T>(it) {}

        Table_Iterator(const T *t): Table_Iterator_Base<T>(t) {}

        /* Operator: *
         * Dereferencing Table iterator returns an <Object> which
         * corresponds to the item at the current table index.
         */
        val_t operator*() const
        {
            return stack::get_value<Object>(
                Table_Iterator_Base<T>::p_table->state(), -1
            );
        }
    };


    /* Struct: Table_Pair_Iterator
     * A forward iterator for <Table>. See <Table_Iterator_Base>.
     * This one works with pairs of keys-values (see <Pair>).
     */
    template<typename T> struct Table_Pair_Iterator: Table_Iterator_Base<T>
    {
        /* Typedef: diff_t */
        typedef ptrdiff_t diff_t;
        /* Typedef: val_t */
        typedef types::Pair<Object, Object> val_t;
        /* Typedef: ptr_t */
        typedef void ptr_t;
        /* Typedef: ref_t */
        typedef void ref_t;

        Table_Pair_Iterator(): Table_Iterator_Base<T>() {}

        Table_Pair_Iterator(const Table_Pair_Iterator& it):
            Table_Iterator_Base<T>(it) {}

        Table_Pair_Iterator(const T *t): Table_Iterator_Base<T>(t) {}

        /* Operator: *
         * Dereferencing Table iterator returns a <Pair> of <Object>
         * instances corresponding to the current table key-value pair.
         */
        val_t operator*() const
        {
            return types::Pair<Object, Object>(
                stack::get_value<Object>(
                    Table_Iterator_Base<T>::p_table->state(), -2
                ),
                stack::get_value<Object>(
                    Table_Iterator_Base<T>::p_table->state(), -1
                )
            );
        }
    };

    /* Struct: Table
     * This structure represents the Lua table. You can get
     * its elements  of various types, set them or manipulate
     * metatables. See also <Table_Item>.
     */
    struct Table: Object
    {
        /* Typedef: it
         * An iterator typedef for standard table iterator (loop by values).
         * Useful mostly for arrays. For others (and even for arrays) you'll
         * want to use <pit>.
         */
        typedef Table_Iterator<Table> it;

        /* Typedef: pit
         * An iterator typedef for paired table iterator (key-value pairs).
         * Key-value iterators work also for arrays, keys are numerical
         * and represent the current index.
         */
        typedef Table_Pair_Iterator<Table> pit;

        /* Constructor: Table */
        Table(): Object() {}

        /* Constructor: Table
         * Constructs the table object from any object
         * that has an "object" method.
         */
        template<typename T>
        Table(const T& o): Object(o) {}

        /* Constructor: Table
         * Constructs this instance from the raw lua state pointer and the
         * index of the value on the stack.
         */
        Table(lua_State *L, int idx): Object(L, idx)
        { check_type(L, idx, LUA_TTABLE, "Table"); }

        /* Function: begin
         * Returns an iterator (<it>) to the first element of the table.
         * Useful for arrays. For others, you'll want <pbegin> (and even
         * for arrays).
         */
        Table_Iterator<Table> begin() const
        { return Table_Iterator<Table>(this); }

        /* Function: end
         * Returns an iterator (<it>) to invalid element of the table
         * (so you can end the iteration). See <begin>.
         */
        Table_Iterator<Table> end() const
        { return Table_Iterator<Table>(); }

        /* Function: pbegin
         * Returns a paired iterator (<pit>) to the first element of the
         * table. For arrays, you'll sometimes want to use <begin>. If
         * you need explicit references to array indexes though, this
         * will come into use for arrays as well.
         */
        Table_Pair_Iterator<Table> pbegin() const
        { return Table_Pair_Iterator<Table>(this); }

        /* Function: pend
         * Returns an iterator (<pit>) to invalid element of the table
         * (so you can end the iteration). See <pbegin>.
         */
        Table_Pair_Iterator<Table> pend() const
        { return Table_Pair_Iterator<Table>(); }

        /* Function: get
         * Gets an element of this table or of a contained table,
         * depending on the amount of arguments. The gotten type
         * defaults to <Object>. Example:
         *
         * (start code)
         *     lua::Table tbl = lua.get<lua::Table>("tbl");
         *     // tbl.foo.bar.baz
         *     lua::Object baz = tbl.get<lua::Object>("foo", "bar", "baz");
         *     // tbl.bah
         *     int bah = tbl.get<int>("bah");
         * (end)
         */
        template<typename T = Object, typename... U>
        T get(const U&... args) const
        {
            assert(Object::state());

            push();
            p_push_get(args...);
            T ret = stack::pop_value<T>(Object::state());
            lua_pop(Object::state(), (int)sizeof...(U));

            return ret;
        }

        /* Operator: []
         * Gets a <Table_Item> with a name given by the index.
         */
        template<typename T>
        Table_Item<T> operator[](const T& key) const
        {
            return Table_Item<T>(key, *this);
        }

        /* Function: set
         * Sets the table element. You can also use <[]>, which
         * calls this indirectly via <Table_Item>.
         */
        template<typename T, typename U>
        void set(const T& key, const U& value) const
        {
            if (!Object::state()) return;

            push();

            stack::push_value(Object::state(), key);
            stack::push_value(Object::state(), value);

            lua_settable(Object::state(), -3);
            lua_pop     (Object::state(),  1);
        }

        /* Function: set_metatable
         * Sets a metatable for this table.
         */
        void set_metatable(const Table& metatable) const
        {
            assert(Object::state());
            assert(Object::state() == metatable.state());

            push();
            metatable.push();

            lua_setmetatable(Object::state(), -2);
            lua_pop         (Object::state(),  1);
        }

        /* Function: get_metatable
         * Gets this table's metatable.
         */
        Table get_metatable() const
        {
            assert(Object::state());
            push();

            if (!lua_getmetatable(Object::state(), -1))
                return Table();
            else
            {
                Table ret = stack::pop_value<Table>(Object::state());
                lua_pop(Object::state(), 1);

                return ret;
            }
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

    private:

        template<typename T, typename... U>
        void p_push_get(const T& key, const U&... args) const
        {
            stack::push_value(Object::state(), key);
            lua_gettable     (Object::state(),  -2);

            p_push_get(args...);
        }

        void p_push_get() const {}
    };

    /* Struct: Table_Item
     * Represents a table element. It does not contain any data itself,
     * just a reference to the table it belongs to and the element name
     * (aka table key). You can set the element through it or get its
     * value as any supported type.
     */
    template<typename T> struct Table_Item
    {
        /* Constructor: Table_Item
         * Initializes the table item from a key and a <Table>.
         */
        Table_Item(const T& k, const Table& t):
            p_key(k), p_table(t) {}

        /* Constructor: Table_Item
         * Initializes the table item from another one.
         */
        Table_Item(const Table_Item& i):
            p_key(i.p_key), p_table(i.p_table) {}

        /* Function: set
         * Sets the element value in Lua.
         */
        template<typename U> void set(const U& value) const
        { p_table.set(p_key, value); }

        /* Operator: =
         * See <set>.
         */
        template<typename U> void operator=(const U& value) const
        { set(value); }

        /* Operator: =
         * Explicit version for Table_Items.
         */
        void operator=(const Table_Item& i) const
        { set(i); }

        /* Function: to
         * Returns the data belonging to the element as
         * any supported type.
         */
        template<typename U> U to() const
        { return p_table.get<U>(p_key); }

        /* Function: push
         * Pushes the value on the stack.
         */
        void push() const { p_table.get<Object>(p_key).push(); }

        /* Function: is_nil
         * Returns true if the object is nil, false otherwise.
         */
        bool is_nil() const { return p_table.get<Object>(p_key).is_nil(); }

        /* Function: object
         * Returns the underlying <Object> as const ref.
         */
        Object object() const
        { return p_table.get<Object>(p_key); }

    private:

        const T& p_key;
        const Table& p_table;
    };
} /* end namespace lua */
