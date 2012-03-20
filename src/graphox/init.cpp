#include "graphox/extern.h"
extern void conoutf(const char *, ...);
namespace graphox
{
	void init()
	{
		conoutf("adding repo/graphox");
		addpackagedir("repo/graphox");
	}
}
