add_subdirectory(enet)
add_subdirectory(luaproxy)
#add_subdirectory(berikelium)
include(jansson.CMake)

set(USE_DBUS yes CACHE INTERNAL "compile with dbus support")

message(dbus: ${USE_DBUS})

if(${USE_DBUS})
	include(lua_dbus.CMake)
	add_definitions(-DSUE_DBUS)
else(${USE_DBUS})
	add_definitions(-DNO_DBUS)
endif(${USE_DBUS})

