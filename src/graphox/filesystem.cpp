
#ifndef GRAPHOX_FILESYSTEM_CPP
#define GRAPHOX_FILESYSTEM_CPP

namespace graphox
{
	namespace filesystem
	{
		types::Vector<types::String> searchpaths;

		bool inited = false;
		void init()
		{
			if(inited)
				return;
			else
				inited = true;

			searchpaths.push_back(types::String("repo/graphox/%s"));
			searchpaths.push_back(types::String("repo/ref/%s"));
			
			searchpaths.push_back(types::String("scripts/%s.lua"));
			searchpaths.push_back(types::String("repo/%s"));
		}
		
		bool file_exist(const char *path)
		{
			#ifdef WIN32
				if(GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES)
					return false;
			#else
				if(access(path, R_OK | 0) == -1)
					return false;
			#endif
			
			//printf("found \"%s\"\n", path);
			
			return true;
		}
	
		char *locate(const char *name)
		{
			init();
			//printf("indeed, %s\n", name);
			
			for (types::Vector<types::String>::it it = searchpaths.begin(); it != searchpaths.end(); ++it)
			{
				const char *lname = it->get_buf();
				//printf("%s\n", lname);
				
				types::String_Base<char> *newstring = new types::String_Base<char>(lname);
								
				types::String formated = newstring->format(lname, name);
				const char *path = formated.get_buf();
				
				//printf(" > \"%s\"\n", path);
				
				delete newstring;
				
				if (file_exist(path))
				{
					char *newpath = new char[formated.length() + 1];
					
					int i = 0;
					for(types::String::it char_it = formated.begin(); char_it != formated.end(); ++char_it)
					{
						newpath[i] = char_it[0];
						i ++;
						
						//printf("%i : \"%s\" -> \"%s\"\n", i, char_it, newpath);
					}
					
					newpath[i] = "\0"[0];
					
					return newpath;
				}
				
			}
			
			throw new graphox::Exception(404, "Could not find file\n");
		}
		
		#ifdef OFTL
		types::String *locate()
		{
		
		}
		#endif
	}
}

#endif
