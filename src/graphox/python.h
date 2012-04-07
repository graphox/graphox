#ifndef GRAPHOX_PYTHON_H
#define GRAPHOX_PYTHON_H

#include "graphox/graphox.h"

namespace graphox
{
	namespace scripting
	{
		namespace python
		{
			void init();
			void unload();
			void execute(const char *filename);
  		}
	}
}

#endif


