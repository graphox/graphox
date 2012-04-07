/* OFTL test: hashset.cpp
 * Tests features of hash sets.
 */

#include <assert.h>
#include "OFTL/hashset.h"

int main()
{
    typedef types::Hash_Set<const char*> cset;
    typedef types::Hash_Set<types::String> sset;

    cset foo;
    foo.insert("abcdef");
    foo.insert("ghijkl");
    foo.insert("mnopqr");
    foo.insert("stuvwx");
    foo.insert("yzabcd");
    foo.insert("foobar");
    foo.insert("qwertz");

    assert(foo.length() == 7);

    foo.erase("yzabcd");
    assert(foo.length() == 6);

    foo.erase("fedcba");
    assert(foo.length() == 6);

    int i = 0;
    for (cset::cit it = foo.begin(); it != foo.end(); ++it)
        ++i;

    assert(i == 6);

    sset bar;
    bar.insert("abcdef");
    bar.insert("ghijkl");
    bar.insert("mnopqr");
    bar.insert("stuvwx");
    bar.insert("yzabcd");
    bar.insert("foobar");
    bar.insert("qwertz");

    assert(bar.length() == 7);

    bar.erase("yzabcd");
    assert(bar.length() == 6);

    bar.erase("fedcba");
    assert(bar.length() == 6);

    assert(*bar.find("abcdef") == *foo.find("abcdef"));
    assert(*bar.find("ghijkl") == *foo.find("ghijkl"));
    assert(*bar.find("mnopqr") == *foo.find("mnopqr"));
    assert(*bar.find("stuvwx") == *foo.find("stuvwx"));
    assert(*bar.find("foobar") == *foo.find("foobar"));
    assert(*bar.find("qwertz") == *foo.find("qwertz"));

    return 0;
}
