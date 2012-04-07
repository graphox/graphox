/* File: OFTL/utils.h
 *
 * About: Version
 *  This is version 2 of the file.
 *
 * About: Purpose
 *  Various generic stuff (like typedefs).
 *
 * About: Author
 *  Daniel "q66" Kolesa <quaker66@gmail.com>
 *  Bits taken from the Cube 2 source code (zlib).
 *
 * About: License
 *  This file is licensed under MIT. See COPYING.txt for more information.
 */

#ifndef OFTL_UTILS_H
#define OFTL_UTILS_H

#ifdef NULL
#undef NULL
#endif

/* Define: NULL
 * Set to 0.
 */
#define NULL 0

/* Package: utils */
namespace utils
{
    /* Typedef: uint
     * Defined as unsigned int.
     */
    typedef unsigned int uint;

    /* Typedef: ushort
     * Defined as unsigned short.
     */
    typedef unsigned short ushort;

    /* Typedef: ulong
     * Defined as unsigned long.
     */
    typedef unsigned long ulong;

    /* Typedef: uchar
     * Defined as unsigned char.
     */
    typedef unsigned char uchar;

    /* Typedef: llong
     * Defined as signed long long int.
     */
    typedef signed long long int llong;

    /* Typedef: ullong
     * Defined as unsigned long long int.
     */
    typedef unsigned long long int ullong;

    template<typename T> struct RR
    { typedef T t; };
    template<typename T> struct RR<T&>
    { typedef T t; };
    template<typename T> struct RR<T&&>
    { typedef T t; };

    /* Function: forward
     * Implements perfect forwarding, such as in a function template, when
     * v is a function argument which needs to be passed as an argument to
     * another function as-is: if v is an rvalue, an rvalue is forwarded,
     * if it's an lvalue, an lvalue is forwarded.
     */
    template<typename T>
    inline T&& forward(typename RR<T>::t& v)
    { return (T&&)v; }

    /* Function: forward
     * See above.
     */
    template<typename T>
    inline T&& forward(typename RR<T>::t&& v)
    { return (T&&)v; }

    /* Function: move
     * Converts a given value into rvalue reference.
     */
    template<typename T>
    inline typename RR<T>::t&& move(T&& v)
    { return (typename RR<T>::t&&)v; }

} /* end namespace utils */

using utils::uint;
using utils::ushort;
using utils::ulong;
using utils::uchar;
using utils::llong;
using utils::ullong;

#endif
