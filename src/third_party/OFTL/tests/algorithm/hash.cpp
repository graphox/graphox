/* OFTL test: hash.cpp
 * Tests function of the has function.
 */

#include <assert.h>
#include "OFTL/algorithm.h"

int main()
{
    const char *a = "foobar";
    const char *b = "qwertz";

    uint h1 = algorithm::hash(a) % 65535;
    uint h2 = algorithm::hash(b) % 65535;

    assert(h1 == 24227);
    assert(h2 == 50826);

    assert(algorithm::hash(5)     == 5);
    assert(algorithm::hash(65537) == 65537);

    types::Pair<const char*, int> c("foobar", 5);
    types::Pair<const char*, int> d("qwertz", 6);

    uint h3 = algorithm::hash(c) % 65535;
    uint h4 = algorithm::hash(d) % 65535;

    assert(h1 == h3);
    assert(h2 == h4);

    return 0;
}
