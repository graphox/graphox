set(CUBE2_GAME cube2_game CACHE INTERNAL "cube2_game target")

set(CUBE2_CLIENT_SOURCES
	game.h
	ai.h
	game.h
	capture.h
	ctf.h
	help.cpp
	
	ai.cpp
	client.cpp
	entities.cpp
	fps.cpp
	help.cpp
	monster.cpp
	movable.cpp
	pch.cpp
	render.cpp
	scoreboard.cpp
	server.cpp
	stats.cpp
	waypoint.cpp
	weapon.cpp
)

add_executable(${CUBE2_GAME} ${CUBE2_CLIENT_SOURCES})

target_link_libraries(${CUBE2_GAME}
	-lz
	${CUBE2_TOOLS}
	${CUBE2_ENGINE}
	${ENET_LIBRARY}
)



install(TARGETS 
        ${CUBE2_GAME}      
    RUNTIME DESTINATION ${INSTALL_RUNTIME_DIR} 
    LIBRARY DESTINATION ${INSTALL_LIBRARY_DIR} 
    ARCHIVE DESTINATION ${INSTALL_ARCHIVE_DIR})
