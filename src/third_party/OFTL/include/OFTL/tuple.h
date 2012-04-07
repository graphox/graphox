/* File: OFTL/tuple.h
 *
 * About: Version
 *  This is version 1 of the file.
 *
 * About: Purpose
 *  Tuple class header.
 *
 * About: Author
 *  Daniel "q66" Kolesa <quaker66@gmail.com>
 *  Inspired by libstdc++ tuple implementation.
 *
 * About: License
 *  This file is licensed under MIT. See COPYING.txt for more information.
 */

#ifndef OFTL_TUPLE_H
#define OFTL_TUPLE_H

#include "OFTL/utils.h"
#include "OFTL/pair.h"

/* Package: types */
namespace types
{
    template<typename T> struct Add_Const_Ref
    { typedef const T& type; };

    template<typename T> struct Add_Const_Ref<T&>
    { typedef T& type; };

    template<typename T> struct Add_Ref
    { typedef T& type; };

    template<typename T> struct Add_Ref<T&>
    { typedef T& type; };

    template<size_t i, typename... T>
    struct Tuple_Base;

    template<size_t i>
    struct Tuple_Base<i> {};

    template<size_t i, typename T, typename... U>
    struct Tuple_Base<i, T, U...>: Tuple_Base<i + 1, U...>
    {
        typedef Tuple_Base<i + 1, U...> next;

        Tuple_Base(): next(), p_data() {}

        Tuple_Base(const Tuple_Base& v):
            next(v.rest()), p_data(v.data()) {}

        template<typename... V>
        Tuple_Base(const Tuple_Base<i, V...>& v):
            next(v.rest()), p_data(v.data()) {}

        explicit Tuple_Base(
            typename Add_Const_Ref<T>::type data,
            typename Add_Const_Ref<U>::type... rest
        ): next(rest...), p_data(data) {}

        Tuple_Base& operator=(const Tuple_Base& v)
        {
            data() = v.data();
            rest() = v.rest();
            return *this;
        }

        template<typename... V>
        Tuple_Base& operator=(const Tuple_Base<i, V...>& v)
        {
            data() = v.data();
            rest() = v.rest();
            return *this;
        }

              T& data()       { return p_data; }
        const T& data() const { return p_data; }

              next& rest()       { return *this; }
        const next& rest() const { return *this; }

    private:

        T p_data;
    };

    /* Struct: Tuple
     * Tuples represent compile-time lists. They can hold multiple
     * values of different types. You can't append to them, they
     * are initialized statically.
     *
     * Example:
     * (start code)
     *     types::Tuple<int, String, float> t = make_tuple(5, "foo", 4.3f);
     *     printf(
     *         "%i %s %f\n",
     *         types::get<0>(t),
     *         types::get<1>(t),
     *         types::get<2>(t)
     *     );
     * (end)
     *
     * You can construct tuples, you can assign them and you can compare them.
     * You can also set their members using references, get their members and
     * unpack them as function arguments (see <unpack>). You can as well get
     * their size.
     */
    template<typename... T>
    struct Tuple: Tuple_Base<0, T...>
    {
        typedef Tuple_Base<0, T...> base;

        /* Constructor: Tuple */
        Tuple(): base() {}

        /* Constructor: Tuple */
        Tuple(const Tuple& v): base(v) {}

        /* Constructor: Tuple */
        template<typename... U>
        Tuple(const Tuple<U...>& v): base(v) {}

        /* Constructor: Tuple */
        explicit Tuple(typename Add_Const_Ref<T>::type... args):
            base(args...) {}

        /* Operator: = */
        template<typename... U>
        Tuple& operator=(const Tuple<U...>& v)
        {
            ((base&)*this) = v;
            return *this;
        }

        /* Operator: = */
        Tuple& operator=(const Tuple& v)
        {
            ((base&)*this) = v;
            return *this;
        }
    };

    template<> struct Tuple<> {};

    template<typename T, typename U>
    struct Tuple<T, U>: Tuple_Base<0, T, U>
    {
        typedef Tuple_Base<0, T, U> base;

        Tuple(): base() {}

        Tuple(const Tuple& v): base(v) {}

        template<typename V, typename X>
        Tuple(const Tuple<V, X>& v): base(v) {}

        explicit Tuple(
            typename Add_Const_Ref<T>::type a,
            typename Add_Const_Ref<U>::type b
        ): base(a, b) {}

        template<typename V, typename X>
        Tuple(const types::Pair<V, X>& v): base(Tuple_Base<
            0,
            typename Add_Const_Ref<V>::type,
            typename Add_Const_Ref<X>::type
        >(v.first, v.second)) {}

        template<typename V, typename X>
        Tuple& operator=(const Tuple<V, X>& v)
        {
            ((base&)*this) = v;
            return *this;
        }

        Tuple& operator=(const Tuple& v)
        {
            ((base&)*this) = v;
            return *this;
        }

        template<typename V, typename X>
        Tuple& operator=(const types::Pair<V, X>& v)
        {
            this->data() = v.first;
            this->rest().data() = v.second;
            return *this;
        }
    };

    template<size_t i, typename T> struct Tuple_Element;
    template<size_t i, typename T, typename... U>
    struct Tuple_Element<i,     Tuple<T, U...> >:
           Tuple_Element<i - 1, Tuple<   U...> > {};

    template<typename T, typename... U>
    struct Tuple_Element<0, Tuple<T, U...> >
    {
        typedef T type;
    };

    /* Variable: Tuple_Size
     * Using this, you can retrieve tuple size. Usage:
     * (start code)
     *     types::Tuple<int, float, int, const char*> t;
     *     size_t size = types::Tuple_Size<t>::value;
     * (end)
     */
    template<typename T> struct Tuple_Size;
    template<typename... T> struct Tuple_Size<Tuple<T...> >
    { enum { value = sizeof...(T) }; };

    template<size_t i, typename T, typename... U>
    inline typename Add_Ref<T>::type get_helper(Tuple_Base<i, T, U...>& v)
    { return v.data(); }

    template<size_t i, typename T, typename... U>
    inline typename Add_Const_Ref<T>::type get_helper(
        const Tuple_Base<i, T, U...>& v
    ) { return v.data(); }

    /* Function: get
     * Gets a tuple element. Usage:
     * (start code)
     *     types::Tuple<int, float> t;
     *     int value = types::get<0>(t);
     * (end)
     * This returns a mutable reference.
     */
    template<size_t i, typename... T>
    inline typename Add_Ref<
        typename Tuple_Element<i, Tuple<T...> >::type
    >::type get(Tuple<T...>& v) { return get_helper<i>(v); }

    /* Function: get
     * Same as above, but it returns a const reference.
     */
    template<size_t i, typename... T>
    inline typename Add_Const_Ref<
        typename Tuple_Element<i, Tuple<T...> >::type
    >::type get(const Tuple<T...>& v) { return get_helper<i>(v); }

    template<size_t i>
    struct Tuple_Unpacker
    {
        template<typename T, typename... U, typename... V, typename... X>
        static T unpack(T (*fun)(U...), const Tuple<V...>& t, const X&... args)
        {
            return Tuple_Unpacker<i - 1>::unpack(
                fun, t, get<i - 1>(t), args...
            );
        }
    };

    template<>
    struct Tuple_Unpacker<0>
    {
        template<typename T, typename... U, typename... V, typename... X>
        static T unpack(T (*fun)(U...), const Tuple<V...>& t, const X&... args)
        {
            return fun(args...);
        }
    };

    template<size_t eq_sz, size_t i, size_t j, typename T, typename U>
    struct Tuple_Compare;

    template<size_t i, size_t j, typename T, typename U>
    struct Tuple_Compare<0, i, j, T, U>
    {
        static bool equals(const T& a, const U& b)
        {
            return (
                (get<i>(a) == get<i>(b)) &&
                Tuple_Compare<0, i + 1, j, T, U>::equals(a, b)
            );
        }

        static bool less(const T& a, const U& b)
        {
            return (
                ((get<i>(a) < get<i>(b)) ||
                !(get<i>(b) < get<i>(a))) &&
                Tuple_Compare<0, i + 1, j, T, U>::less(a, b)
            );
        }
    };

    template<size_t i, typename T, typename U>
    struct Tuple_Compare<0, i, i, T, U>
    {
        static bool equals(const T& a, const U& b)
        { return true; }

        static bool less(const T& a, const U& b)
        { return false; }
    };

    /* Operator: == */
    template<typename... T, typename... U>
    inline bool operator==(const Tuple<T...>& a, const Tuple<U...>& b)
    {
        typedef Tuple<T...> tt;
        typedef Tuple<U...> ut;
        return (Tuple_Compare<
            (Tuple_Size<tt>::value - Tuple_Size<ut>::value), 0,
             Tuple_Size<tt>::value, tt, ut
        >::equals(a, b));
    }

    /* Operator: < */
    template<typename... T, typename... U>
    inline bool operator<(const Tuple<T...>& a, const Tuple<U...>& b)
    {
        typedef Tuple<T...> tt;
        typedef Tuple<U...> ut;
        return (Tuple_Compare<
            (Tuple_Size<tt>::value - Tuple_Size<ut>::value), 0,
             Tuple_Size<tt>::value, tt, ut
        >::less(a, b));
    }

    /* Operator: != */
    template<typename... T, typename... U>
    inline bool operator!=(const Tuple<T...>& a, const Tuple<U...>& b)
    { return !(a == b); }

    /* Operator: > */
    template<typename... T, typename... U>
    inline bool operator>(const Tuple<T...>& a, const Tuple<U...>& b)
    { return (b < a); }

    /* Operator: <= */
    template<typename... T, typename... U>
    inline bool operator<=(const Tuple<T...>& a, const Tuple<U...>& b)
    { return !(b < a); }

    /* Operator: >= */
    template<typename... T, typename... U>
    inline bool operator>=(const Tuple<T...>& a, const Tuple<U...>& b)
    { return !(a < b); }

    /* Function: make_tuple
     * Constructs a tuple from given arguments.
     */
    template<typename... T>
    inline Tuple<T...> make_tuple(T... args)
    {
        return Tuple<T...>(args...);
    }

    /* Function: tie
     * Creates a tuple of lvalue references to its arguments.
     */
    template<typename... T>
    inline Tuple<T&...> tie(T&... args)
    {
        return Tuple<T&...>(args...);
    }

    /* Function: unpack
     * Unpacks a tuple as function arguments. Example:
     * (start code)
     *     void fun(int a, float b, const char *c)
     *     {
     *         printf("%i %f %s\n", a, b, c); 
     *     }
     *     auto t = types::make_tuple(5, 6.78f, "foo");
     *     types::unpack(&fun, t);
     * (end)
     */
    template<typename T, typename... U, typename... V>
    T unpack(T (*fun)(U...), const Tuple<V...>& t)
    {
        return Tuple_Unpacker<sizeof...(V)>::unpack(fun, t);
    }
} /* end namespace types */

#endif
