/* OFTL test: comparison.cpp
 * Tests comparison functors from the functional lib.
 */

#include <assert.h>
#include "OFTL/algorithm.h"

struct test
{
    test(int a): a(a) {}

    friend bool operator==(const test& a, const test& b) { return a.a == b.a; }
    friend bool operator!=(const test& a, const test& b) { return a.a != b.a; }
    friend bool operator<=(const test& a, const test& b) { return a.a <= b.a; }
    friend bool operator>=(const test& a, const test& b) { return a.a >= b.a; }
    friend bool operator< (const test& a, const test& b) { return a.a <  b.a; }
    friend bool operator> (const test& a, const test& b) { return a.a >  b.a; }

    int a;
};

int main()
{
    int a = 5;
    int b = 10;

    const char *d = "b";
    const char *c = "a";

    test e(15);
    test f(20);

    typedef types::Pair<int, int> ip;

    ip g(30, 40);
    ip h(50, 20);

    assert((!functional::Equal        <int, int>()(a, b)));
    assert(( functional::Not_Equal    <int, int>()(a, b)));
    assert((!functional::Greater      <int, int>()(a, b)));
    assert(( functional::Less         <int, int>()(a, b)));
    assert((!functional::Greater_Equal<int, int>()(a, b)));
    assert(( functional::Less_Equal   <int, int>()(a, b)));
    assert(( functional::Greater_Equal<int, int>()(a, a)));
    assert(( functional::Less_Equal   <int, int>()(a, a)));
    assert(( functional::Greater_Equal<int, int>()(b, b)));
    assert(( functional::Less_Equal   <int, int>()(b, b)));

    assert((!functional::Equal        <const char*, const char*>()(c, d)));
    assert(( functional::Not_Equal    <const char*, const char*>()(c, d)));
    assert((!functional::Greater      <const char*, const char*>()(c, d)));
    assert(( functional::Less         <const char*, const char*>()(c, d)));
    assert((!functional::Greater_Equal<const char*, const char*>()(c, d)));
    assert(( functional::Less_Equal   <const char*, const char*>()(c, d)));
    assert(( functional::Greater_Equal<const char*, const char*>()(c, c)));
    assert(( functional::Less_Equal   <const char*, const char*>()(c, c)));
    assert(( functional::Greater_Equal<const char*, const char*>()(d, d)));
    assert(( functional::Less_Equal   <const char*, const char*>()(d, d)));

    assert((!functional::Equal        <test, test>()(e, f)));
    assert(( functional::Not_Equal    <test, test>()(e, f)));
    assert((!functional::Greater      <test, test>()(e, f)));
    assert(( functional::Less         <test, test>()(e, f)));
    assert((!functional::Greater_Equal<test, test>()(e, f)));
    assert(( functional::Less_Equal   <test, test>()(e, f)));
    assert(( functional::Greater_Equal<test, test>()(e, e)));
    assert(( functional::Less_Equal   <test, test>()(e, e)));
    assert(( functional::Greater_Equal<test, test>()(f, f)));
    assert(( functional::Less_Equal   <test, test>()(f, f)));

    assert((!functional::Equal        <ip, ip>()(g, h)));
    assert(( functional::Not_Equal    <ip, ip>()(g, h)));
    assert((!functional::Greater      <ip, ip>()(g, h)));
    assert(( functional::Less         <ip, ip>()(g, h)));
    assert((!functional::Greater_Equal<ip, ip>()(g, h)));
    assert(( functional::Less_Equal   <ip, ip>()(g, h)));
    assert(( functional::Greater_Equal<ip, ip>()(g, g)));
    assert(( functional::Less_Equal   <ip, ip>()(g, g)));
    assert(( functional::Greater_Equal<ip, ip>()(h, h)));
    assert(( functional::Less_Equal   <ip, ip>()(h, h)));

    return 0;
}
