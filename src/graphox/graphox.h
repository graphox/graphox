#ifndef GRAPHOX_H
#define GRAPHOX_H

//#include "GL/gl.h"

#include "jansson.h"

#include "graphox/exception.h"

#ifndef G_NO_SCRIPTING
	#include "graphox/scripting.h"
	//#include "graphox/js.h"
	#include "graphox/lua.h"
	//#include "graphox/python.h"
#endif

#include "graphox/theme.h"
#include "graphox/filesystem.h"

namespace graphox
{
	void init();
	void init_done();
	void main(int time);
}

#endif
