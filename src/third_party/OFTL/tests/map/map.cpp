/* OFTL test: map.cpp
 * Tests features of maps.
 */

#include <assert.h>
#include "OFTL/map.h"

struct Test
{
    Test(int i = 0):
        i(i) {}

    int i;
};

int main()
{
    typedef types::Map<int, Test> tmap;

    tmap map;

    map.insert(5,  Test(10));
    map.insert(25, Test(50));
    map.insert(10, Test(20));
    map.insert(20, Test(40));
    map.insert(15, Test(30));
    map[35] = Test(70);
    map[45] = Test(90);
    map[30] = Test(60);
    map[40] = Test(80);

    assert(map.length() == 9);

    int i = 0;
    for (tmap::cit it = map.begin(); it != map.end(); ++it)
    {
        i += 5;
        assert((*it).first == i);
    }
    i += 5;

    for (tmap::crit it = map.rbegin(); it != map.rend(); ++it)
    {
        i -= 5;
        assert((*it).first == i);
    }

    map.erase(10);
    map.erase(25);
    assert(map.length() == 7);

    tmap::cit it = map.find(10);
    assert(it == map.end());

    it = map.find(30);
    assert(it != map.end());
    assert((*it).second.i == 60);

    /* auto default insertion */
    assert(map[150].i == 0);

    map.clear();
    assert(map.length() == 0);

    return 0;
}
