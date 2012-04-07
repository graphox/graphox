

#jansson

project(jansson C)

include(CheckSymbolExists)
include(CheckCSourceCompiles)

check_symbol_exists(localeconv locale.h json_have_localeconv)
if(NOT json_have_localeconv)
    set(json_have_localeconv 0)
endif()


check_c_source_compiles("int main() { long long test = 0; return test; }" json_have_long_long)
if(NOT json_have_long_long)
    set(json_have_long_long 0)
endif()

check_c_source_compiles("inline int test() { return 0; }\nint main() { return test(); }" json_inline)
if(json_inline)
    set(json_inline inline)
endif()

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/jansson_config.h.in
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/jansson_config.h
)

set(JANSSON_INCLUDE_DIRS
	${CMAKE_CURRENT_SOURCE_DIR}/jansson
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/src
	
	CACHE INTERNAL "jansson include dirs"
)

include_directories(${JANSSON_INCLUDE_DIRS})
set(JANSSON_LIBRARY "libjson" CACHE INTERNAL "jansson json library")

set(JANSSON_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/dump.c 
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/error.c 
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/hashtable.c 
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/hashtable.h 
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/jansson_private.h 
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/load.c 
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/memory.c 
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/pack_unpack.c 
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/strbuffer.c 
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/strbuffer.h 
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/utf.c 
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/utf.h 
	${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/value.c
)

add_library(${JANSSON_LIBRARY} ${G_COMPILE_TYPE} ${JANSSON_SOURCES})

install(
	FILES
		${CMAKE_CURRENT_SOURCE_DIR}/jansson/src/jansson.h
		${CMAKE_CURRENT_SOURCE_DIR}/jansson/jansson_config.h
	DESTINATION
		${CMAKE_CURRENT_SOURCE_DIR}/jansson/include
)

install(TARGETS 
            ${JANSSON_LIBRARY} 
       LIBRARY DESTINATION
            ${INSTALL_LIBRARY_DIR} 
       ARCHIVE DESTINATION
            ${INSTALL_LIBRARY_DIR})


