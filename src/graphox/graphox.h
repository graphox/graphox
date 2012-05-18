#ifndef GRAPHOX_H
#define GRAPHOX_H

//#include "GL/gl.h"

#include "jansson.h"

#include "graphox/exception.h"

#include "graphox/scripting.h"
#include "graphox/lua.h"

//#include "graphox/js.h"
//#include "graphox/python.h"

#include "graphox/theme.h"
#include "graphox/filesystem.h"

#include "graphox/gui.h"

namespace graphox
{
	//const char *version = "1.5.0";
	void init();
	void init_done();
	void main(int time);
}

#endif
