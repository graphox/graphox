/* OFTL test: string.cpp
 * Tests the strings.
 */

#include <assert.h>
#include "OFTL/string.h"

using namespace types;

int main()
{
    String a = "hello ";
    a += "world";
    assert(a == "hello world");

    a = "goodbye world";
    assert(a == "goodbye world");

    a = a.substr(a.find("w"), 5);
    assert(a == "world");

    String b = "a string";
    a = b;
    assert(a == b);
    assert(a == "a string");

    a = 'x';
    assert(a == "x");

    a = "hello world";
    assert(a.length() == 11);

    b = "";
    assert(b.is_empty());

    for (String::cit it = a.begin(); it != a.end(); ++it)
        b += *it;
    assert(b == "hello world");

    a = "";

    for (String::crit it = b.rbegin(); it != b.rend(); ++it)
        a += *it;
    assert(a == "dlrow olleh");

    assert(a.find ("o") == 3);
    assert(a.rfind("o") == 6);

    a = "foobar";
    b = "barfoo";

    assert(a > b);

    assert(a[3] == 'b');
    assert(!strcmp(&a[3], "bar"));

    a.format("%s %i %.3f", "foo", 5, 3.14f);
    assert(a == "foo 5 3.140");

    a = "This string will be erased soon.";

    a.erase(4, 7);
    assert(a == "This will be erased soon.");

    a.erase(a.begin() + 2);
    assert(a == "Ths will be erased soon.");

    a.erase(a.begin() + 3, a.begin() + 7);
    assert(a == "Ths be erased soon.");

    a.erase(0, a.length());
    assert(a == "");

    a = "foo";

    a.erase(a.begin(), a.end() - 1);
    assert(a == "");

    a = "foo";

    a.erase(a.begin() - 2, a.end() + 3);
    assert(a == "");

    a = "foo";

    a.erase(a.begin() - 10, a.begin() - 1);
    a.erase(a.end(), a.end() + 2);
    assert(a == "foo");

    return 0;
}
