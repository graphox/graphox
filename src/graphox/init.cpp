#include "graphox/extern.h"

#ifndef GRAPHOX_DISALE_SVN	
	#include "graphox/repo.h"
#endif

//#include "script_engine_v8.h"

extern void conoutf(const char *, ...);
namespace graphox
{
	void init()
	{
		conoutf("adding repo/graphox");
		addpackagedir("repo/graphox");
	}
	
	void init_done()
	{
	
	}
	
	void main(int time)
	{
	
	}
}
