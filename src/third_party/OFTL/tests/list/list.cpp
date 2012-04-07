/* OFTL test: map.cpp
 * Tests features of maps.
 */

#include <assert.h>
#include "OFTL/list.h"

struct Test
{
    Test(int i = 0):
        i(i) {}

    int i;
};

int main()
{
    typedef types::List<Test> tlist;

    tlist list;

    list.push_back(Test(10));
    list.push_back(Test(20));
    list.push_back(Test(30));
    list.push_back(Test(40));
    list.push_back(Test(50));
    list.push_front(Test(60));
    list.push_front(Test(70));
    list.push_front(Test(80));
    list.push_front(Test(90));
    list.push_front(Test(100));

    assert(list.length() == 10);

    tlist::cit it = list.begin();
    assert(it->i == 100);

    ++it; assert(it->i == 90);
    ++it; assert(it->i == 80);
    ++it; assert(it->i == 70);
    ++it; assert(it->i == 60);
    ++it; assert(it->i == 10);
    ++it; assert(it->i == 20);
    ++it; assert(it->i == 30);
    ++it; assert(it->i == 40);
    ++it; assert(it->i == 50);

    tlist::crit rit = list.rbegin();
    assert(rit->i == 50);

    ++rit; assert(rit->i == 40);

    list.pop_back ();
    list.pop_back ();
    list.pop_front();
    list.pop_front();

    assert(list.length() == 6);

    it = list.begin();
    assert(it->i == 80);

    rit = list.rbegin();
    assert(rit->i == 30);

    list.clear();
    assert(list.length() == 0);

    return 0;
}
