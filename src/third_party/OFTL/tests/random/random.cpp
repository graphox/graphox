/* OFTL test: random.cpp
 * Tests the random number generation facilities.
 */

#include <assert.h>
#include <time.h>
#include <math.h>
#include "OFTL/random.h"

int main()
{
    pseudorandom::MT gen(time(0));

    int a, b;
    for (int i = 0; i < 100; ++i)
        if (gen.get(100) == gen.get(100))
            ++b;
        else
            ++a;

    assert(a != b);

    for (int i = 0; i < 100; ++i)
    {
        float x = gen.get(100.0f);
        assert(x != floor(x));
    }

    return 0;
}
