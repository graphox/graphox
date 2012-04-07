/* Shows how to expose functions into Lua with OFTL::lua
 *
 * Author: Daniel 'q66' Kolesa <quaker66@gmail.com>
 */

#include "OFTL/lua.h"

using namespace lua;
using namespace types;

typedef Vector<Object> objvec_t;

objvec_t fun(objvec_t args)
{
    return args;
}

int luafun(lua_State *L)
{
    printf("Function exposed with the Lua C API..\n");
    printf("First argument: %s\n", lua_tostring(L, -1));
    printf("This will return one value (5).\n");
    lua_pushinteger(L, 5);
    return 1;
}

Tuple<int, float> argfun(
    int a, bool b, const char *c, float d, String e, Function f
)
{
    printf(
        "arg a: %i\n"
        "arg b: %i\n"
        "arg c: %s\n"
        "arg d: %f\n"
        "arg e: %s\n"
        "arg f: calling ..\n",
        a, b, c, d, e.get_buf()
    );
    f(" hello world!");
    return make_tuple(5, 134.3f);
}

void error_handler(const State& s, const char *msg)
{
    fprintf(stderr, "LOG: %s\n", msg);
}

int main()
{
    lua::State lua;
    lua.open_libs();

    lua["fun"] = &fun;

    Function f = lua["fun"];
    Table    t = f.call<Table>(5, "foo", true, 3.14f, "hey", lua["print"], 5);

    for (Table::pit it = t.pbegin(); it != t.pend(); ++it)
    {
        Object k = (*it).first;
        Object o = (*it).second;

        switch (o.type())
        {
            case TYPE_NUMBER:
                printf(
                    "%i: %i / %f\n",
                    k.to<int>(), o.to<int>(), o.to<float>()
                );
                break;
            case TYPE_STRING:
                printf("%i: %s\n", k.to<int>(), o.to<const char*>());
                break;
            case TYPE_BOOLEAN:
                printf("%i: %i\n", k.to<int>(), o.to<bool>());
                break;
            case TYPE_FUNCTION:
                printf("%i: function, calling ...\n", k.to<int>());
                o.to<Function>()("    hello world!");
                break;
            default:
                printf("%i: unknown\n", k.to<int>());
                break;
        }
    }

    printf("Setting up and calling native Lua C API function...\n");
    lua["cfun"] = &luafun;

    int r = lua["cfun"].to<Function>().call<int>("some string");
    printf("retval: %i\n", r);

    printf("Function with native arguments!\n");
    lua["argfun"] = &argfun;

    Function af = lua["argfun"];
    auto y = af.call<int, float>(
        134, true, "hello!", 6.28f, "a string", lua["print"]
    );
    printf("retvals: %i %f\n", types::get<0>(y), types::get<1>(y));

    Tuple<int, const char*> rets;
    auto err = lua.do_string("return 5, 'foo'", rets, ERROR_TRACEBACK);

    printf("RETS:  %i %s\n", get<0>(rets), get<1>(rets));
    printf("ERROR: %i %s\n", get<0>(err ), get<1>(err ));

    lua.set_panic_handler(&error_handler);
    lua.error("Something bad happened that was supposed to happen.");

    return 0;
}
