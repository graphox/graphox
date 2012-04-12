
#ifndef GRAPHOX_GUI_CPP
#define GRAPHOX_GUI_CPP

#include "gui.h"
#include "GL/gl.h"

#define ILUT_USE_OPENGL
#define ILUT_USE_SDL
#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

namespace graphox
{
	namespace gui
	{
		namespace theme
		{
			GLuint background_img;
			GLuint gui_img;
			const char *get_theme_file(const char *themename, const char *file)
			{
				types::String *filename = new types::String();
				filename->format("themes/%s/%s", themename, file);
				
				return graphox::filesystem::locate(filename->get_buf());
			}
			
			GLuint load_image(const char *themename, const char *name)
			{
				ILuint img_id = 0;
				
				ilEnable (IL_CONV_PAL);
				ilutEnable (ILUT_OPENGL_CONV);				
				ilGenImages (1, &img_id);
				ilBindImage (img_id);
				
				if(!ilLoadImage((ILstring) get_theme_file(themename, name)))
					throw new graphox::Exception(0, "Could not open image file, malformated?");

				
				ilutRenderer (ILUT_OPENGL);
				printf("%s: Data format: %d, data type: %d\n", name, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE));
				
				return ilutGLBindTexImage();			
			}
			
			void load_theme(const char *name = "default") 
			{
				background_img = load_image(name, "background.png");
				gui_img = load_image(name, "gui.png");
				
//				GLuint menu = ilLoadImage((ILstring)get_theme_file(name, "menu.png"));
			}
			
			bool background_rendered = false;
			void reset()
			{
				background_rendered = false;
			}
			
			void render_background(int w, int h)
			{
				glPushMatrix();
				glBindTexture(GL_TEXTURE_2D, GLuint(background_img)); // Select the texture to use and bind to it
				
				// Draw our textured geometry (just a rectangle in this instance)
				glBegin(GL_QUADS);
					glTexCoord2f (0.0, 0.0);
					glVertex2f (0.0, 0.0);
					
					glTexCoord2f (1.0, 0.0);
					glVertex2f (w, 0.0);
					
					glTexCoord2f (1.0, 1.0);
					glVertex2f (w, h);
					
					glTexCoord2f (0.0, 1.0);
					glVertex2f (0.0, h);
				glEnd();
				glPopMatrix();
				
				background_rendered = true;
			}
			
			void render(int x, int y, int w, int h, int t)
			{
				if(!background_rendered)
					render_background(w, h);
			
			}
		}
		
		class guielement
		{
			public:
			guielement *parent;
			types::Vector<guielement>children;
			
			guielement(guielement *parent_) : parent(parent_) {};
			guielement() {};
			
			void init(guielement *parent);
			void render(int w, int h, int time);
			void event(SDL_Event &e);
			void resize(int x, int y);
			void offset(int x, int y);
			
			int size_x, size_y;
			int left_offset, top_offset;
		};
		
		class gui : public guielement
		{
			public:
			gui()
			{
				parent = NULL;
			}
		};
		void guielement::resize(int x, int y)
		{
			size_x = x/100;
			size_y = y/100;		
		}
		
		void guielement::offset(int x, int y)
		{
			left_offset = x/100;
			top_offset = y/100;			
		}
		
		void guielement::init(guielement *parent_)
		{
			parent = parent_;
		}
		
		void guielement::render(int w, int h, int time)
		{
			int width = size_x * w;
			int height = size_y * h;

			int left = left_offset * w;
			int top = top_offset * h;
			
			int right_x = left + width;
			
			int top_y = h - top;
			int bottom_y = top_y - height;
			
			//printf("width = %i, height = %i, left = %i, top = %i, right_x = %i, top_y = %i, bottom_y = %i\n", width, height, left, top, right_x, top_y, bottom_y);
			
			glPushMatrix();
				
			glBindTexture(GL_TEXTURE_2D, GLuint(graphox::gui::theme::gui_img)); // Select the texture to use and bind to it
				

				glBegin(GL_QUADS);
					glTexCoord2f (0.0, 0.0);
					glVertex2f (left, bottom_y);
					
					glTexCoord2f (1.0, 0.0);
					glVertex2f (right_x, bottom_y);
					
					glTexCoord2f (1.0, 1.0);
					glVertex2f (right_x, top_y);
					
					glTexCoord2f (0.0, 1.0);
					glVertex2f (left, top_y);
				glEnd();
			glPopMatrix();
			
			
			for (types::Vector<guielement>::it it = children.begin(); it != children.end(); ++it)
			{
				it->render(w, h, time);
			}		
		}
		
		void guielement::event(SDL_Event &e)
		{
			for (types::Vector<guielement>::it it = children.begin(); it != children.end(); ++it)
			{
				it->event(e);
			}
		}

		class main_gui : public gui {};
		
/*		void main_gui::render(int w, int h, int t)
		{
		}*/


		gui *current;
	
		bool open = false;
		
		void init(const char *name)
		{
			open = true;
			
			/*if ( (iluGetInteger(IL_VERSION_NUM) < IL_VERSION) || (iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION) || (ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION) )
			{
				printf("WARNING: being pollite, devil version mismatch!");
			}*/			
			
			//load gui
			current = new main_gui;
			current->resize(50, 50);
			current->offset(25, 25);			
			
			ilInit();
	        iluInit();

			ilEnable (IL_CONV_PAL);
			ilutEnable (ILUT_OPENGL_CONV);

			
			graphox::gui::theme::load_theme();
		}
		
		void check_event(SDL_Event &e)
		{
			current->event(e);
		}
		

		void render(int w, int h)
		{
			try
			{
				//render background
				graphox::gui::theme::reset();
				
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(0, w, h, 0, -1, 1);
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				
				glEnable(GL_TEXTURE_2D);
			
				glEnable(GL_BLEND);
				// get current "run-time" in seconds
				double current_time = 0.001*SDL_GetTicks();
			
				graphox::gui::theme::render(100, 100, w, h, current_time);
			
				current->render(w, h, current_time);
			}
			catch(graphox::Exception *e)
			{
				printf("Could not render graphox's gui :/");
				e->print();
			}
		}

	}
}

#endif
