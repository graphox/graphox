# config
set (LDBUS_MAJOR_VERSION "0")
set (LDBUS_MINOR_VERSION "1")
set (LDBUS_PATCH_VERSION "1")

set(DBUS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lua_dbus)

set (CMAKE_MODULE_PATH ${DBUS_PATH}/cmake)

find_package (Eina REQUIRED)
find_package (Ecore REQUIRED)
find_package (E_DBus REQUIRED)
find_package (DBus REQUIRED)


include_directories(
	${ECORE_INCLUDE_DIR}
	${E_DBUS_INCLUDE_DIR}
	${DBUS_INCLUDE_DIR}
	${EINA_INCLUDE_DIR}
)

set(LDBUS_LIBRARY ldbus CACHE INERNAL "lua dbus library")
add_library(${LDBUS_LIBRARY} MODULE 
	${DBUS_PATH}/src/ldbus.c
	${DBUS_PATH}/src/ldbus_conn.c
	${DBUS_PATH}/src/ldbus_arg.c
	${DBUS_PATH}/src/ldbus_hand.c
)
set_target_properties(${LDBUS_LIBRARY} PROPERTIES PREFIX "")

target_link_libraries(${LDBUS_LIBRARY}
	${EINA_LIBRARIES}
	${ECORE_LIBRARIES}
	${E_DBUS_LIBRARIES}
	${DBUS_LIBRARIES}
	${LUA_LIBRARIES}
)

install(TARGETS 
			${LDBUS_LIBRARY}
       LIBRARY DESTINATION
            ${INSTALL_LIBRARY_DIR} 
       ARCHIVE DESTINATION
            ${INSTALL_LIBRARY_DIR})
