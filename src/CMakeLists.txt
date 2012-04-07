
set(G_COMPILE_TYPE SHARED CACHE INTERNAL "shared of static libs?")

cmake_policy(SET CMP0012 NEW)

include_directories(
	${CURRENT_SOURCE_DIR}
	.
	engine
	fpsgame
	shared
	include
	#third_party
	#third_party/javascript/
	
	#TODO: autofind these packages
	/usr/include
	/usr/local/include
	/usr/include/svncpp
	/usr/include/subversion-1
	/usr/include/apr-1.0
)

include(CheckLibraryExists)

include(FindLua51)
include(FindPythonLibs)

check_library_exists(z gzopen "" ZLIB_EXISTS)
find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_image REQUIRED)
find_package(ZLIB REQUIRED)
find_library(M_LIB m)

if(NOT ${ZLIB_EXISTS})
    message(FATAL_ERROR "missing required library: ZLIB")
endif(NOT ${ZLIB_EXISTS})

if(NOT ${LUA51_FOUND})
	message("missing required library: LUA 5.1, trying 5.0")
	
	include(FindLua50)
	
	if(NOT ${LUA50_FOUND})
		message(FATAL_ERROR "missing required library: LUA 5.0 and 5.1")
	endif(NOT ${LUA50_FOUND})
	add_definitions(-DG_LUA_50)
else(NOT ${LUA51_FOUND})

	add_definitions(-DG_LUA_51)
endif(NOT ${LUA51_FOUND})

if(NOT ${PYTHONLIBS_FOUND})
	message(FATAL_ERROR "missing required library: PYTHON")
endif(NOT ${PYTHONLIBS_FOUND})

add_definitions(
	-D_C2_DISABLE_UINTPTR_T_
	-DSDL_VIDEO_DRIVER_X11 #TODO: make this unneeded
	-DGRAPHOX_DISALE_SVN #don't forget to uncomment the cpp files	
) #TODO: scan system

add_subdirectory(third_party)

include_directories(
	${CMAKE_SOURCE_DIR}/src/third_party/v8/include
	${LUA_INCLUDE_DIR}
	${PYTHON_INCLUDE_PATH}
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include

	${JANSSON_INCLUDE_DIRS}
)

set(OFTL
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/new.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/utils.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/traits.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/algorithm.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/functional.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/iterator.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/string.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/stdio.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/pair.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/hashmap.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/hashset.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/map.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/set.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/vector.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/shared_ptr.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/random.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/filesystem.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/list.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/lua.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/tuple.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/lua/base.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/lua/function.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/lua/dynamic.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/lua/table.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/lua/state.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include/OFTL/lua/stack.h
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/src/filesystem/platform_posix.cpp
)

add_library(OFTL ${G_COMPILE_TYPE} ${OFTL})
target_link_libraries(OFTL ${LUA_LIBRARIES})

link_directories(${CMAKE_SOURCE_DIR}/src/third_party/v8)

#due to sauer's bad design this is IMPOSSIBLE:
#set(CUBE2_ENGINE cube2_engine CACHE INTERNAL "cube2_engine library")
#set(CUBE2_GAME cube2_game CACHE INTERNAL "cube2_game target")
#add_subdirectory(engine)
#add_subdirectory(fpsgame)

#add_library(${CUBE2_ENGINE} SHARED ${ENGINE_SOURCES})
#add_library(${CUBE2_GAME} SHARED ${CUBE2_CLIENT_SOURCES})

#target_link_libraries(${CUBE2_ENGINE} ${SDL_LIBRARIES} ${CUBE2_TOOLS})
#target_link_libraries(${CUBE2_GAME} ${CUBE2_TOOLS} ${CUBE2_ENGINE})

add_executable(graphox_client
	shared/cube.h
	
	engine/engine.h
	engine/world.h
	engine/octa.h
	engine/lightmap.h
	engine/bih.h
	engine/texture.h
	engine/model.h
	engine/varray.h
	engine/textedit.h
	engine/
	engine/animmodel.h
	engine/depthfx.h
	engine/explosion.h
	engine/iqm.h
	engine/lensflare.h
	engine/lightning.h
	engine/md2.h
	engine/md3.h
	engine/md5.h
	engine/model.h
	engine/mpr.h
	engine/obj.h
	engine/ragdoll.h
	engine/rendertarget.h
	engine/scale.h
	engine/skelmodel.h
	engine/smd.h
	engine/textedit.h
	engine/texture.h
	engine/varray.h
	engine/vertmodel.h
	engine/world.h

	fpsgame/game.h
	fpsgame/ai.h
	fpsgame/game.h
	fpsgame/capture.h
	fpsgame/ctf.h


	include/SDL.h
	
	engine/rendergl.cpp
	
	engine/3dgui.cpp
	engine/bih.cpp
	engine/blend.cpp
	engine/blob.cpp
	engine/client.cpp
	engine/command.cpp
	engine/console.cpp
	engine/cubeloader.cpp
	engine/decal.cpp
	engine/dynlight.cpp
	engine/glare.cpp
	engine/grass.cpp
	engine/lightmap.cpp
	engine/main.cpp
	#master.cpp
	engine/material.cpp
	engine/menus.cpp
	engine/movie.cpp
	engine/normal.cpp
	engine/octa.cpp
	engine/octaedit.cpp
	engine/octarender.cpp
	engine/pch.cpp
	engine/physics.cpp
	engine/pvs.cpp
	
	engine/rendermodel.cpp
	engine/renderparticles.cpp
	engine/rendersky.cpp
	engine/rendertext.cpp
	engine/renderva.cpp
	engine/server.cpp
	engine/serverbrowser.cpp
	engine/shader.cpp
	engine/shadowmap.cpp
	engine/sound.cpp
	engine/texture.cpp
	engine/water.cpp
	engine/world.cpp
	engine/worldio.cpp

	fpsgame/help.cpp
	fpsgame/ai.cpp
	fpsgame/client.cpp
	fpsgame/entities.cpp
	fpsgame/fps.cpp
	fpsgame/help.cpp
	fpsgame/monster.cpp
	fpsgame/movable.cpp
	fpsgame/pch.cpp
	fpsgame/render.cpp
	fpsgame/scoreboard.cpp
	fpsgame/server.cpp
	fpsgame/stats.cpp
	fpsgame/waypoint.cpp
	fpsgame/weapon.cpp
	
	shared/crypto.cpp
	shared/geom.cpp
	shared/pch.cpp
	shared/stream.cpp
	shared/tools.cpp
	shared/zip.cpp

	#third_party/javascript/script_engine_v8.cpp

	#graphox/init.cpp	
	#graphox/svn.cpp
	#graphox/repo.cpp
	
	#graphox/config.cpp
	#graphox/logging.cpp

)

add_definitions(-std=c++0x -fpermissive)
add_library(graphox ${G_COMPILE_TYPE}
	graphox/graphox.cpp
)

target_link_libraries(graphox
    ${SDL_LIBRARY}
    ${SDLMIXER_LIBRARY}
    ${SDLIMAGE_LIBRARY}
    ${SDLSOUND_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${ZLIB_LIBRARIES}
	${ENET_LIBRARY}
	${LUA_LIBRARIES}
	${PYTHON_LIBRARIES}
	${JANSON_LIBRARY}
	
	OFTL	
	svncpp
	svn_client-1
	v8
	#-lrt
	-lz
)

target_link_libraries(graphox_client
    ${SDL_LIBRARY}
    ${SDLMIXER_LIBRARY}
    ${SDLIMAGE_LIBRARY}
    ${SDLSOUND_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${ZLIB_LIBRARIES}
	${ENET_LIBRARY}
	${LUA_LIBRARIES}
	${PYTHON_LIBRARIES}
	
	graphox
	svncpp
	svn_client-1
	v8
	-lrt -lz
)

INSTALL(
	TARGETS
		graphox_client
		OFTL
		graphox
	RUNTIME DESTINATION
		${INSTALL_RUNTIME_DIR}
	LIBRARY DESTINATION
		${INSTALL_LIBRARY_DIR}
	ARCHIVE DESTINATION
		${INSTALL_RUNTIME_DIR}
)

