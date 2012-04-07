/* OFTL test: sort.cpp
 * Tests function of insertion sort and the quicksort/insertion sort hybrid.
 */

#include <assert.h>
#include "OFTL/algorithm.h"

int main()
{
    int a[10] = { 4, 6, 3, 150, 43, 24, 26, 139, 124, 111 };
    algorithm::insertion_sort(a, a + 10);

    assert(a[0] == 3);
    assert(a[1] == 4);
    assert(a[2] == 6);
    assert(a[3] == 24);
    assert(a[4] == 26);
    assert(a[5] == 43);
    assert(a[6] == 111);
    assert(a[7] == 124);
    assert(a[8] == 139);
    assert(a[9] == 150);

    algorithm::insertion_sort(a, a + 10, functional::Greater<int, int>());

    assert(a[9] == 3);
    assert(a[8] == 4);
    assert(a[7] == 6);
    assert(a[6] == 24);
    assert(a[5] == 26);
    assert(a[4] == 43);
    assert(a[3] == 111);
    assert(a[2] == 124);
    assert(a[1] == 139);
    assert(a[0] == 150);

    const char *b[10] = { "a", "d", "b", "e", "c", "h", "f", "g", "k", "j" };
    algorithm::sort(b, b + 10);

    assert(!strcmp(b[0], "a"));
    assert(!strcmp(b[1], "b"));
    assert(!strcmp(b[2], "c"));
    assert(!strcmp(b[3], "d"));
    assert(!strcmp(b[4], "e"));
    assert(!strcmp(b[5], "f"));
    assert(!strcmp(b[6], "g"));
    assert(!strcmp(b[7], "h"));
    assert(!strcmp(b[8], "j"));
    assert(!strcmp(b[9], "k"));

    algorithm::sort(b, b + 10, functional::Greater<
        const char*, const char*
    >());

    assert(!strcmp(b[9], "a"));
    assert(!strcmp(b[8], "b"));
    assert(!strcmp(b[7], "c"));
    assert(!strcmp(b[6], "d"));
    assert(!strcmp(b[5], "e"));
    assert(!strcmp(b[4], "f"));
    assert(!strcmp(b[3], "g"));
    assert(!strcmp(b[2], "h"));
    assert(!strcmp(b[1], "j"));
    assert(!strcmp(b[0], "k"));

    return 0;
}
