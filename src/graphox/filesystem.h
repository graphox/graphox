
#ifndef GRAPHOX_FILESYSTEM_H
#define GRAPHOX_FILESYSTEM_H

namespace graphox
{
	namespace filesystem
	{
		char *locate(const char *fname);
		
		#ifdef OFTL
		types::String *locate();
		#endif
	}
}

#endif
