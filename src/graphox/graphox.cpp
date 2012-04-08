#ifndef GRAPHOX_CPP
#define GRAPHOX_CPP

	#include "OFTL/stdio.h"
	#include "OFTL/string.h"
	#include "OFTL/map.h"
	#include "OFTL/hashmap.h"
	#include "OFTL/filesystem.h"
	
	#include <unistd.h>

	#include "graphox/graphox.h"
	
	namespace graphox
	{
		using namespace types;
		using namespace filesystem;
	}
	
	#include "graphox/exception.cpp"

	#ifndef G_NO_SCRIPTING
		#include "graphox/scripting.cpp"
		//#include "graphox/js.cpp"
		#include "graphox/lua.cpp"
		//#include "graphox/python.cpp"
	#endif

	#include "graphox/theme.cpp"
	#include "graphox/filesystem.cpp"
#endif
