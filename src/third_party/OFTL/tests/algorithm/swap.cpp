/* OFTL test: swap.cpp
 * Tests function of the swap and iter_swap functions.
 */

#include <assert.h>
#include "OFTL/algorithm.h"

bool is_int(int  i) { return true;  }
bool is_int(long i) { return false; }

bool is_long(long i) { return true;  }
bool is_long(int  i) { return false; }

int main()
{
    int a = 5;
    int b = 10;
    algorithm::swap(a, b);

    assert(a == 10);
    assert(b == 5);

    int  *c = new int(5);
    long *d = new long(10);
    algorithm::iter_swap(c, d);

    assert(*c == 10);
    assert(*d == 5);

    assert(is_int(*c));
    assert(!is_long(*c));

    assert(is_long(*d));
    assert(!is_int(*d));

    delete c;
    delete d;

    return 0;
}
