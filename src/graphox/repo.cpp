#include "graphox_internal.h"
#include "repo.h"

namespace graphox
{
	namespace reposity
	{
		vector<const char *> paths;
		
		int update_thread(void *)
		{
			loopv(paths)
			{
				puts("new info");
				graphox::svn_manager::checkout_info *info = new graphox::svn_manager::checkout_info;
				
				puts("format");
				sprintf(info->directory, "repo/%s", newstring(paths[i]));
				
				puts(info->directory);
				
				puts("call");
				graphox::svn_manager::update_package_thread((void *)info);
			}
			return 0;
		};
		
		void update()
		{
			puts("creating update thread..");
			SDL_CreateThread(update_thread, (void*)0);
				
			//make shure our data arrives correctly
			SDL_Delay(500);
		};
		
		void update_list(const char *filename)
		{
			//load the cubescript file
			if(!execfile(filename, true))
				conoutf("\t ^ reposities not loaded");
		};
		
		void update_list()
		{
			update_list(findfile("repo/list.cfg", "r"));
		};
		
		void add_reposity(const char *path)
		{
			puts(path);
			paths.add(newstring(path));
		};
		

		void write_list()
		{
			stream *f = openfile(findfile("repo/list.cfg", "r"), "w");

			if(!f)
			{
				conoutf("Could not write reposity list!");
				return;
			}
			
			f->printf("// Genetated, do NOT change\n\n");
			
			loopv(paths)
			{
				puts(paths[i]);
				f->printf("repo_add \"%s\" \n", paths[i]);
			}
			
			delete f;
		}		
		
		ICOMMAND(repo_init, "ss", (char *directory, char *url), graphox::svn_manager::checkout_package(directory, url));
		ICOMMAND(repo_add, "s", (const char *directory), add_reposity(directory));

		ICOMMAND(repo_update, "", (), update());

		ICOMMAND(repo_save, "", (), write_list());
		ICOMMAND(repo_load, "", (), update_list());
	}
}
