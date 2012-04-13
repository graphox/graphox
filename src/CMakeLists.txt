
set(G_COMPILE_TYPE SHARED CACHE INTERNAL "shared of static libs?")

cmake_policy(SET CMP0012 NEW)

include_directories(
	${CURRENT_SOURCE_DIR}
	.
	engine
	fpsgame
	shared
)

include(CheckLibraryExists)

include(FindLua51)
#include(FindPythonLibs)
include(FindDevIL)
include(FindSDL)

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

#if(NOT ${PYTHONLIBS_FOUND})
#	message(FATAL_ERROR "missing required library: PYTHON")
#endif(NOT ${PYTHONLIBS_FOUND})

if(NOT ${IL_FOUND})
	message(FATAL_ERROR "missing required library: DEVIL")
endif(NOT ${IL_FOUND})

add_subdirectory(third_party)

add_definitions(
	-DGRAPHOX_DISABLE_SVN
	
	#OFTL
	-std=c++0x
	
	#cube2
	-fpermissive
)

include_directories(
	#${CMAKE_SOURCE_DIR}/src/third_party/v8/include
	${LUA_INCLUDE_DIR}
	#${PYTHON_INCLUDE_PATH}
	${CMAKE_SOURCE_DIR}/src/third_party/OFTL/include
	${IL_INCLUDE_DIR}
	${SDL_INCLUDE_DIR}
	${JANSSON_INCLUDE_DIRS}
)

set(OFTL_SOURCES)


IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(OFTL_SOURCES ${CMAKE_SOURCE_DIR}/src/third_party/OFTL/src/filesystem/platform_win32.cpp)
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(OFTL_SOURCES ${CMAKE_SOURCE_DIR}/src/third_party/OFTL/src/filesystem/platform_posix.cpp)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

add_library(OFTL ${G_COMPILE_TYPE} ${OFTL_SOURCES})
target_link_libraries(OFTL ${LUA_LIBRARIES})

#link_directories(${CMAKE_SOURCE_DIR}/src/third_party/v8)

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
	#${PYTHON_LIBRARIES}
	${JANSON_LIBRARY}
	${IL_LIBRARIES}
	${ILU_LIBRARIES}
	${ILUT_LIBRARIES}

	OFTL	
	#svncpp
	#svn_client-1
	#v8
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
#	${LUA_LIBRARIES}
#	${PYTHON_LIBRARIES}

	graphox
#	svncpp
#	svn_client-1
#	v8
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

