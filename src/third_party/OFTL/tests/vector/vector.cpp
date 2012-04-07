/* OFTL test: vector.cpp
 * Tests the vectors.
 */

#include <assert.h>
#include "OFTL/vector.h"

using namespace types;

struct Foo
{
    Foo(int i = 0):
        i(i) {}

    int i;
};

bool foo_sort(const Foo& a, const Foo& b)
{ return (a.i < b.i); }

int main()
{
    typedef Vector<Foo> vfoo;

    vfoo bar;
    bar.push_back(Foo(5));
    bar.push_back(Foo(10));
    bar.push_back(Foo(8));
    bar.push_back(Foo(24));
    bar.push_back(Foo(16));
    bar.push_back(Foo(28));

    assert(bar[0].i == 5);
    assert(bar[1].i == 10);
    assert(bar[2].i == 8);
    assert(bar[3].i == 24);
    assert(bar[4].i == 16);
    assert(bar[5].i == 28);

    algorithm::sort(bar.begin(), bar.end(), foo_sort);

    assert(bar[0].i == 5);
    assert(bar[1].i == 8);
    assert(bar[2].i == 10);
    assert(bar[3].i == 16);
    assert(bar[4].i == 24);
    assert(bar[5].i == 28);

    assert(bar.at(3).i == 16);

    bar.pop_back();
    bar.pop_back();

    int i = 0;
    for (vfoo::cit it = bar.begin(); it != bar.end(); ++it)
        ++i;
    assert(i == 4);

    assert((*bar.rbegin()).i == 16);
    assert((*bar.begin ()).i == 5);

    assert((*(bar.rend() - 1)).i == 5);
    assert((*(bar.end () - 1)).i == 16);

    assert(bar.length  () == 4);
    /* vector min capacity is 8, then it doubles */
    assert(bar.capacity() == 8);

    bar.push_back(Foo(5));
    bar.push_back(Foo(10));
    bar.push_back(Foo(8));
    bar.push_back(Foo(24));
    bar.push_back(Foo(16));
    bar.push_back(Foo(28));
    bar.push_back(Foo(5));
    bar.push_back(Foo(10));
    bar.push_back(Foo(8));
    bar.push_back(Foo(24));
    bar.push_back(Foo(16));
    bar.push_back(Foo(28));

    assert(bar.length  () == 16);
    assert(bar.capacity() == 16);

    bar.push_back(Foo(5));
    bar.push_back(Foo(10));
    bar.push_back(Foo(8));
    bar.push_back(Foo(24));
    bar.push_back(Foo(16));
    bar.push_back(Foo(28));

    assert(bar.length  () == 22);
    assert(bar.capacity() == 32);

    bar.clear();

    assert(bar.length  () == 0);
    assert(bar.capacity() == 0);

    return 0;
}
