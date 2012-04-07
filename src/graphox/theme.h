#if 0 //ndef GRAPHOX_THEME_H
#define GRAPHOX_THEME_H

#include "graphox.h"

namespace graphox
{
	namespace theme
	{
		class loading_theme
		{
			public:
				void render(float bar, const char *text, GLuint tex, bool background);
		};

		void render_loading_screen(float bar, const char *text, GLuint tex, bool background);
	}
}

#endif
