
#ifndef GRAPHOX_H
#define GRAPHOX_H

#include "cube.h"
#include "graphox/exception.h"

#ifndef G_NO_SCRIPTING
	#include "graphox/scripting.h"
	#include "graphox/js.h"
#endif

namespace graphox
{
	void init();
	void init_done();
	void main(int time);
}

#endif
