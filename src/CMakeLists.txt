

include_directories(
	${CURRENT_SOURCE_DIR}
	.
	engine
	fpsgame
	shared
	include
)

include(CheckLibraryExists)

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

set (SDL_LIBRARIES

	SDL
	SDL_image
	SDLmain
	SDL_mixer
#	SDL_ttf
#	SDL_joystick
#	SDL_video
   ${SDL_LIBRARY}
   ${SDLIMAGE_LIBRARY}
   ${SDLMIXER_LIBRARY}
   ${ZLIB_LIBRARY}
   ${OPENGL_LIBRARY}
)

add_definitions(-D_C2_DISABLE_UINTPTR_T_ -DSDL_VIDEO_DRIVER_X11)
set(CUBE2_TOOLS cube2_tools CACHE INTERNAL "cube2_tools library")

set(TOOLS_SOURCES
	shared/igame.h
	shared/iengine.h

	shared/cube.h
	shared/tools.h
	shared/geom.h
	shared/ents.h
	shared/command.h
)

add_library(${CUBE2_TOOLS} SHARED ${TOOLS_SOURCES})

set_property(TARGET ${CUBE2_TOOLS} PROPERTY LINKER_LANGUAGE CXX)

target_link_libraries(${CUBE2_TOOLS} -lz)

add_subdirectory(enet)

add_subdirectory(engine)
add_subdirectory(fpsgame)


