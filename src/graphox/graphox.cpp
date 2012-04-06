#define G_NO_SCRIPTING

#ifndef GRAPHOX_CPP
#define GRAPHOX_CPP

	#include "graphox/graphox.h"
	
	#include "graphox/exception.cpp"

	#ifndef G_NO_SCRIPTING
		#include "graphox/scripting.cpp"
		#include "graphox/js.cpp"
	#endif

	#include "graphox/theme.cpp"
#endif
