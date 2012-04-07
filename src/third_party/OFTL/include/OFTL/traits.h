/* File: OFTL/traits.h
 *
 * About: Version
 *  This is version 2 of the file.
 *
 * About: Purpose
 *  Basic type traits.
 *
 * About: Author
 *  Daniel "q66" Kolesa <quaker66@gmail.com>
 *
 * About: License
 *  This file is licensed under MIT. See COPYING.txt for more information.
 */

#ifndef OFTL_TRAITS_H
#define OFTL_TRAITS_H

/* Package: traits */
namespace traits
{
    /* Variable: Integral_Constant
     * Compile time integral constant.
     */
    template<typename T, T v>
    struct Integral_Constant
    {
        static const T value = v;

        typedef T value_type;
        typedef Integral_Constant<T, v> type;
    };

    /* Variable: True_Type
     * Boolean (true) version of <Integral_Constant>.
     */
    typedef Integral_Constant<bool, true> True_Type;

    /* Variable: False_Type
     * Boolean (false) version of <Integral_Constant>.
     */
    typedef Integral_Constant<bool, false> False_Type;

    template<typename T, T v>
    const T Integral_Constant<T, v>::value;

    /* Variable: Is_Integral
     * Version of <Bool_Type> for integral types.
     *
     * Specializations:
     *  bool
     *  char
     *  unsigned char
     *  signed char
     *  short
     *  unsigned short
     *  int
     *  unsigned int
     *  long
     *  unsigned long
     */
    template<typename T> struct Is_Integral: False_Type {};

    /* Variable: Is_Floating_Point
     * Version of <Bool_Type> for floating point types.
     *
     * Specializations:
     *  float
     *  double
     *  long double
     */
    template<typename T> struct Is_Floating_Point: False_Type {};

    /* Variable: Is_Pointer
     * Version of <Bool_Type> for pointer types.
     *
     * Only true specialization here is for T*.
     */
    template<typename T> struct Is_Pointer: False_Type {};

    /* the specializations */
    template<>           struct Is_Integral<bool              > : True_Type {};
    template<>           struct Is_Integral<char              > : True_Type {};
    template<>           struct Is_Integral<unsigned char     > : True_Type {};
    template<>           struct Is_Integral<signed char       > : True_Type {};
    template<>           struct Is_Integral<short             > : True_Type {};
    template<>           struct Is_Integral<unsigned short    > : True_Type {};
    template<>           struct Is_Integral<int               > : True_Type {};
    template<>           struct Is_Integral<unsigned int      > : True_Type {};
    template<>           struct Is_Integral<long              > : True_Type {};
    template<>           struct Is_Integral<unsigned long     > : True_Type {};
    template<>           struct Is_Integral<long long         > : True_Type {};
    template<>           struct Is_Integral<unsigned long long> : True_Type {};
    template<>           struct Is_Floating_Point<float       > : True_Type {};
    template<>           struct Is_Floating_Point<double      > : True_Type {};
    template<>           struct Is_Floating_Point<long double > : True_Type {};
    template<typename T> struct Is_Pointer<T*                 > : True_Type {};

    /* Variable: Is_POD
     * Version of <Integral_Constant> for POD types. POD type is every integral,
     * floating point or pointer type. Non-POD types are defined, like
     * various structs / classes.
     */
    template<typename T> struct Is_POD: Integral_Constant<bool, (
        Is_Integral      <T>::value ||
        Is_Floating_Point<T>::value ||
        Is_Pointer       <T>::value
    )> {};

    /* Variable: Is_Equal
     * Checks whether two types are equal. This is a definition for
     * the case when they are different, so its "value" is 0.
     */
    template<typename, typename> struct Is_Equal { enum { value = 0 }; };

    /* Variable: Is_Equal
     * Version for when they are equal, so the "value" is 1.
     */
    template<typename T> struct Is_Equal<T, T> { enum { value = 1 }; };

    template<typename T> struct Is_Empty_Impl
    {
    private:
        template<typename  > struct v1 {};
        template<typename U> struct v2 { U v; };

    public:
        enum { value = (sizeof(v1<T>) == sizeof(v2<T>)) };
    };

    /* Variable: Is_Empty
     * Checks whether a given structure is empty.
     * Version of <Integral_Constant>.
     */
    template<typename T> struct Is_Empty:
        Integral_Constant<bool, Is_Empty_Impl<T>::value> {};

    /* Variable: Remove_Reference
     * Makes non-reference type from type. Get the
     * non-reference type using the "type" member of this.
     */
    template<typename T> struct Remove_Reference
    { typedef T type; };

    /* Variable: Remove_Reference
     * See above.
     */
    template<typename T> struct Remove_Reference<T&>
    { typedef T type; };

    /* Variable: Remove_Reference
     * See above.
     */
    template<typename T> struct Remove_Reference<T&&>
    { typedef T type; };
} /* end namespace traits */

#endif
