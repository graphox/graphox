/* Hello world for OFTL::lua
 *
 * Author: Daniel 'q66' Kolesa <quaker66@gmail.com>
 */

#include "OFTL/lua.h"

int main()
{
    lua::State lua;
    lua.open_libs();

    lua::Function print = lua["print"];
    print("hello world!");

    return 0;
}
