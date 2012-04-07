/* OFTL test: hashmap.cpp
 * Tests features of hash maps.
 */

#include <assert.h>
#include "OFTL/hashmap.h"

int main()
{
    typedef types::Hash_Map<const char*, int> cmap;
    typedef types::Hash_Map<types::String, int> smap;

    cmap foo;
    foo.insert("abcdef", 10);
    foo.insert("ghijkl", 15);
    foo.insert("mnopqr", 20);
    foo.insert("stuvwx", 25);
    foo.insert("yzabcd", 30);
    foo.insert("foobar", 35);
    foo.insert("qwertz", 40);

    assert(foo.length() == 7);

    foo.erase("yzabcd");
    assert(foo.length() == 6);

    foo.erase("fedcba");
    assert(foo.length() == 6);

    int i = 0;
    for (cmap::cit it = foo.begin(); it != foo.end(); ++it)
        ++i;

    assert(i == 6);

    smap bar;
    bar.insert("abcdef", 10);
    bar.insert("ghijkl", 15);
    bar.insert("mnopqr", 20);
    bar.insert("stuvwx", 25);
    bar.insert("yzabcd", 30);
    bar.insert("foobar", 35);
    bar.insert("qwertz", 40);

    assert(bar.length() == 7);

    bar.erase("yzabcd");
    assert(bar.length() == 6);

    bar.erase("fedcba");
    assert(bar.length() == 6);

    assert(foo["abcdef"] == bar["abcdef"]);
    assert(foo["ghijkl"] == bar["ghijkl"]);
    assert(foo["mnopqr"] == bar["mnopqr"]);
    assert(foo["stuvwx"] == bar["stuvwx"]);
    assert(foo["yzabcd"] == bar["yzabcd"]);
    assert(foo["foobar"] == bar["foobar"]);
    assert(foo["qwertz"] == bar["qwertz"]);

    assert(foo.find("mnopqr") != foo.end());
    assert(foo.find("arghzp") == foo.end());

    foo.clear();
    assert(foo.length() == 0);

    return 0;
}
