/* OFTL test: min_max_clamp.cpp
 * Tests function of the min, max and clamp functions.
 */

#include <assert.h>
#include "OFTL/algorithm.h"

int main()
{
    int a = 10;
    int b = 15;
    int c = 30;

    assert(algorithm::min(a, b) == a);
    assert(algorithm::max(a, b) == b);
    assert(algorithm::clamp(a, b, c) == b);

    float d = 3.15;
    float e = 3.17;
    float f = 4.36;

    assert(algorithm::min(d, e) == d);
    assert(algorithm::max(d, e) == e);
    assert(algorithm::clamp(d, e, f) == e);

    return 0;
}
