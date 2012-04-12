
#ifndef GRAPHOX_GUI_H
#define GRAPHOX_GUI_H

#include "SDL.h"

namespace graphox
{
	namespace gui
	{
		class guielement;
		class gui;
		
		extern gui *current;		
		extern bool open;
		void init(const char *name = "main");
		void check_event(SDL_Event &e);
		void render(int w, int h);

	}
}

#endif
