/* File: OFTL/map.h
 *
 * About: Version
 *  This is version 2 of the file.
 *
 * About: Purpose
 *  Map class header.
 *
 * About: Author
 *  Daniel "q66" Kolesa <quaker66@gmail.com>
 *
 * About: License
 *  This file is licensed under MIT. See COPYING.txt for more information.
 */

#ifndef OFTL_MAP_H
#define OFTL_MAP_H

#include "OFTL/utils.h"
#include "OFTL/algorithm.h"
#include "OFTL/functional.h"
#include "OFTL/pair.h"
#include "OFTL/set.h"

/* Package: types */
namespace types
{
    /* Struct: Map
     * See <Set>. This is the same thing, but it makes use of <Pair> to
     * create a key-value associative container, not key-key (value-value).
     * The interface is designed in that way you don't actually access
     * or modify pairs much, only when iterating.
     *
     * (start code)
     *     typedef types::Map<const char*, int> mymap;
     *     mymap test;
     *     // these two ways are equivalent in function, the
     *     // second one is better if the element doesn't exist.
     *     test["foo"] = 5;
     *     test.insert("bar", 10);
     * 
     *     // searching
     *     printf("%i\n", test["bar"]);
     *     // this will not fail - it'll insert an empty value,
     *     // so it'll print just 0 (default value for an int)
     *     printf("%i\n", test["baz"]);
     *
     *     // iteration
     *     for (mymap::cit it = test.begin(); it != test.end(); ++it)
     *         printf("%s - %i\n", (*n).first, (*n).second);
     *
     *     // reverse iteration
     *     for (mymap::crit it = test.rbegin(); it != test.rend(); ++it)
     *         printf("%s - %i\n", n->first, n->second);
     *
     *     // erasing, will delete the node
     *     test.erase("baz");
     *
     *     // retrieving the tree length
     *     printf("%i\n", test.length());
     *
     *     // will clear the node - also done
     *     // automatically in the destructor
     *     test.clear();
     * (end)
     */
    template<typename T, typename U> struct Map: Set<Pair<T, U> >
    {
        typedef Set<Pair<T, U> > base;
        typedef Set_Node<Pair<T, U> > node;

        /* Typedef: it
         * An iterator typedef for standard, non-const iterator.
         */
        typedef Set_Iterator<Pair<T, U> > it;

        /* Typedef: cit
         * An iterator typedef for const iterator.
         */
        typedef Set_Const_Iterator<Pair<T, U> > cit;

        /* Typedef: rit
         * Reverse iterator typedef, a <Reverse> < <it> >.
         */
        typedef iterators::Reverse_Iterator<it> rit;

        /* Typedef: crit
         * Const reverse iterator typedef, a <Reverse> < <cit> >.
         */
        typedef iterators::Reverse_Iterator<cit> crit;

        /* Function: insert
         * Inserts a new node into the tree with key and data members given
         * by the arguments.
         *
         * You can also use the <[]> operator with assignment, both ways are
         * equivalent in function, this is however better on non-existent keys
         * because it doesn't have to insert first and then assign, instead it
         * creates with the given data directly.
         */
        U& insert(const T& key, const U& data)
        {
            return base::p_insert(
                base::p_root, Pair<T, U>(key, data)
            )->p_data.second;
        }

        /* Function: insert
         * A variant of insert that accepts a <pair> of key and data instead
         * of key and data separately.
         */
        U& insert(const Pair<T, U>& data)
        {
            return base::p_insert(base::p_root, data)->p_data.second;
        }

        /* Function: erase
         * Erases a node with a given key from the tree.
         */
        void erase(const T& key) { delete base::p_erase(base::p_root, key); }

        /* Function: find
         * Returns an iterator to a node that belongs to a given key. There is
         * also a const version that returns a const iterator (non-modifiable).
         */
        it find(const T& key) { return it(p_find(base::p_root, key)); }

        /* Function: find
         * Const version of <find>. The result cannot be modified.
         */
        cit find(const T& key) const { return cit(p_find(base::p_root, key)); }

        /* Operator: []
         * See <find>. If you assign a non-existant key, it'll get created
         * first, because this has to return the data, not a node
         * (see <insert>).
         *
         * (start code)
         *     tree[key] = value;
         *     printf("%s\n", tree[key]);
         * (end)
         */
        U& operator[](const T& key)
        {
            return p_find(base::p_root, key, true)->p_data.second;
        }

    protected:

        node *p_find(node *nd, const T& key, bool do_insert = false)
        {
            if (nd == base::p_nil)
            {
                if (do_insert)
                    return base::p_insert(base::p_root, Pair<T, U>(key, U()));
                else
                    return base::p_nil;
            }

            if (!functional::Equal<T, T>()(key, nd->p_data.first))
                return p_find((
                    (functional::Less<T, T>()(key, nd->p_data.first))
                        ? nd->p_left
                        : nd->p_right
                ), key, do_insert);

            return nd;
        }
    };
} /* end namespace types */

#endif
