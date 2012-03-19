set(CUBE2_ENGINE cube2_engine CACHE INTERNAL "cube2_engine library")

set(ENGINE_SOURCES
	../shared/cube.h
	
	engine.h
	world.h
	octa.h
	lightmap.h
	bih.h
	texture.h
	model.h
	varray.h
	textedit.h
	
	animmodel.h
	depthfx.h
	explosion.h
	iqm.h
	lensflare.h
	lightning.h
	md2.h
	md3.h
	md5.h
	model.h
	mpr.h
	obj.h
	ragdoll.h
	rendertarget.h
	scale.h
	skelmodel.h
	smd.h
	textedit.h
	texture.h
	varray.h
	vertmodel.h
	world.h
	
	../include/SDL.h
	
	rendergl.cpp
	
	3dgui.cpp
	bih.cpp
	blend.cpp
	blob.cpp
	client.cpp
	command.cpp
	console.cpp
	cubeloader.cpp
	decal.cpp
	dynlight.cpp
	glare.cpp
	grass.cpp
	lightmap.cpp
	main.cpp
	#master.cpp
	material.cpp
	menus.cpp
	movie.cpp
	normal.cpp
	octa.cpp
	octaedit.cpp
	octarender.cpp
	pch.cpp
	physics.cpp
	pvs.cpp
	
	rendermodel.cpp
	renderparticles.cpp
	rendersky.cpp
	rendertext.cpp
	renderva.cpp
	#server.cpp
	serverbrowser.cpp
	shader.cpp
	shadowmap.cpp
	sound.cpp
	texture.cpp
	water.cpp
	world.cpp
	worldio.cpp
)


add_library(${CUBE2_ENGINE} SHARED ${ENGINE_SOURCES})
target_link_libraries(${CUBE2_ENGINE} ${SDL_LIBRARIES} -lz)

