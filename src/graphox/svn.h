#ifndef GRAPHOX_SVN_H
#define GRAPHOX_SVN_H

#include "cube.h"
#include <svncpp/client.hpp>
//#include "config.h"

namespace graphox
{
	namespace svn_manager
	{
		extern char *lastmessage;
		extern bool busy;
	
		struct checkout_info
		{
			char *directory;
			char *url;
		};
		
		void message(const char *message);		
		int checkout_package_thread(void *info_);
		void checkout_package (char *directory, char *url);
		int update_package_thread(void *info_);
		void update_package (char *directory);
	};
};

#endif
