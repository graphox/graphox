
#include "graphox_internal.h"
#include "graphox.h"
#include "svn.h"


namespace graphox
{
	namespace svn_manager
	{
		char *lastmessage;
		bool busy = false;	
		
		void message(const char *message)
		{
			puts(message);
			//copystring(lastmessage, message);
			delete message;
		}
	
		int checkout_package_thread(void *info_)
		{
			checkout_info *info = (checkout_info *)info_;
			
			busy = true;
			//set meta storage directory
			message("setting meta folder..");
			svn::Context *context = new svn::Context(std::string("data/svn"));
	
			//create client
			message("creating client..");
			svn::Client	*client = new svn::Client(context);
			
			//create message callback
			//callbacklistener* listener = new callbacklistener(&message);
			//context->setListener(listener);
			
			//output folder
			message("setting output folder..");
			svn::Path destFolder  (info->directory);
			
			//let's checkout
			try
			{
				message("starting checkout ...");
				client->checkout(info->url, destFolder, svn::Revision::HEAD, true);
				message("checkout done!");			
			}
			
			//something went wrong :/
			catch(svn::ClientException &error)
			{
				char *temp_msg;
				sprintf(temp_msg, "checkout \"%s\" to %s failed: %s", info->url, info->directory, error.message());
				message(temp_msg);
				delete temp_msg;
			}
			catch(svn::Exception &error)
			{
				char *temp_msg;
				sprintf(temp_msg, "checkout \"%s\" to %s failed: %s", info->url, info->directory, error.message());
				message(temp_msg);
				delete temp_msg;
			}
			catch(std::exception &error)
			{
				char *temp_msg;
				sprintf(temp_msg, "checkout \"%s\" to %s failed: %s", info->url, info->directory, error.what());
				message(temp_msg);
				delete temp_msg;
			}
			catch(...)
			{
				char *temp_msg;
				sprintf(temp_msg, "checkout \"%s\" to %s failed: unkown", info->url, info->directory);
				message(temp_msg);
				delete temp_msg;
			}
			
			delete context;
			delete client;
			
			busy = false;
			return 1;
		};
		
		void checkout_package (char *directory, char *url)
		{
			if (!busy)
			{
				checkout_info *info = new checkout_info;
				info->directory = directory;
				info->url = url;
				
				SDL_CreateThread(checkout_package_thread, (void*)info);
				
				//make shure our data arrives correctly
				SDL_Delay(500);
			}
			else
				message("\f3Cannot checkout multipue packages at the same time");
		};
		
		ICOMMAND(checkout_package, "ss", (char *directory, char *url), checkout_package(directory, url));
		
		int update_package_thread(void *info_)
		{
			checkout_info *info = (checkout_info *)info_;
			
			busy = true;
			//set meta storage directory
			message("setting meta folder..");
			svn::Context *context = new svn::Context(std::string("data/svn"));
	
			//create client
			message("creating client..");
			svn::Client	*client = new svn::Client(context);
			
			//create message callback
			//callbacklistener* listener = new callbacklistener(&message);
			//context->setListener(listener);
			
			//output folder
			message("setting output folder..");
			svn::Path destFolder  (info->directory);
			
			//let's checkout
			try
			{
				message("starting update ...");
				client->update(destFolder, svn::Revision::HEAD, true, true);
				message("update done!");			
			}
			
			//something went wrong :/
			catch(svn::ClientException &error)
			{
				char *temp_msg;
				sprintf(temp_msg, "checkout \"%s\" to %s failed: %s", info->url, info->directory, error.message());
				message(temp_msg);
				delete temp_msg;
			}
			catch(svn::Exception &error)
			{
				char *temp_msg;
				sprintf(temp_msg, "checkout \"%s\" to %s failed: %s", info->url, info->directory, error.message());
				message(temp_msg);
				delete temp_msg;
			}
			catch(std::exception &error)
			{
				char *temp_msg;
				sprintf(temp_msg, "checkout \"%s\" to %s failed: %s", info->url, info->directory, error.what());
				message(temp_msg);
				delete temp_msg;
			}
			catch(...)
			{
				char *temp_msg;
				sprintf(temp_msg, "checkout \"%s\" to %s failed: unkown", info->url, info->directory);
				message(temp_msg);
				delete temp_msg;
			}
			
			busy = false;
			
			delete context;
			delete client;
			
			return 1;
		};
		
		void update_package (char *directory)
		{
			if (!busy)
			{
				checkout_info *info = new checkout_info;
				info->directory = directory;
				
				SDL_CreateThread(update_package_thread, (void*)info);
				
				//make shure our data arrives correctly
				SDL_Delay(500);
			}
			else
				message("\f3Cannot update multipue packages at the same time");
		};
		
		ICOMMAND(update_package, "s", (char *directory), update_package(directory));
	};
};
