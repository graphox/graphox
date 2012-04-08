
#ifndef GRAPHOX_FILESYSTEM_CPP
#define GRAPHOX_FILESYSTEM_CPP

namespace graphox
{
	namespace filesystem
	{
		bool file_exist(const char *path)
		{
			#ifdef WIN32
				if(GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES)
					return false;
			#else
				if(access(path, R_OK | 0) == -1)
					return false;
			#endif
			
			return true;
		}
	
		types::Vector<types::String> searchpaths;
		const char *locate(const char *name)
		{
			searchpaths.push_back(types::String("repo/graphox/%s"));
			searchpaths.push_back(types::String("repo/ref/%s"));
			
			searchpaths.push_back(types::String("scripts/%s.lua"));
			searchpaths.push_back(types::String("repo/%s"));
			
			for (types::Vector<types::String>::it it = searchpaths.begin(); it != searchpaths.end(); ++it)
			{
				const char *lname = it->get_buf();
				//printf("%s\n", lname);
								
				types::String formated = it->format(lname, name);
				const char *path = formated.get_buf();
				
				//printf(" > %s\n", path);
				
				if (file_exist(path))
					return path;
				
			}
			
			throw new graphox::Exception(404, "Could not find file");
		}
		
		#ifdef OFTL
		types::String *locate()
		{
		
		}
		#endif
	}
}

#endif
