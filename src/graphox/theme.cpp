#if 0
//#ifndef GRAPHOX_THEME_CPP
#define GRAPHOX_THEME_CPP

#include "theme.h"
//#include "engine.h"

namespace graphox
{
	types::Map<const char *, void *> engineinfo;
	
	template<typename T>
	void engineinfo_set(const char *name, T &info)
	{
		engineinfo[name] = (void *)info;
	}
	
	template<typename T>
	T get(const char *name)
	{
		return (T) engineinfo["name"];
	}
	
	namespace theme
	{
		void loading_theme::render(float bar, const char *text, GLuint tex, bool background)
		{
			#if 0
			if(background || graphox::get<int>("sdl_backingstore_bug") > 0)
				restorebackground();
			#endif

			int w = screen->w, h = screen->h;
			
			getbackgroundres(w, h);
			gettextres(w, h);

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glOrtho(0, w, h, 0, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glEnable(GL_TEXTURE_2D);
			defaultshader->set();
			glColor3f(1, 1, 1);

			float fh = 0.075f*min(w, h), fw = fh*10,
				  fx = renderedframe ? w - fw - fh/4 : 0.5f*(w - fw),
				  fy = renderedframe ? fh/4 : h - fh*1.5f,
				  fu1 = 0/512.0f, fu2 = 511/512.0f,
				  fv1 = 0/64.0f, fv2 = 52/64.0f;

			settexture("data/loading_frame.png", 3);
			
			
			glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(fu1, fv1); glVertex2f(fx,    fy);
				glTexCoord2f(fu2, fv1); glVertex2f(fx+fw, fy);
				glTexCoord2f(fu1, fv2); glVertex2f(fx,    fy+fh);
				glTexCoord2f(fu2, fv2); glVertex2f(fx+fw, fy+fh);
			glEnd();

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			float bw = fw*(511 - 2*17)/511.0f, bh = fh*20/52.0f,
				  bx = fx + fw*17/511.0f, by = fy + fh*16/52.0f,
				  bv1 = 0/32.0f, bv2 = 20/32.0f,
				  su1 = 0/32.0f, su2 = 7/32.0f, sw = fw*7/511.0f,
				  eu1 = 23/32.0f, eu2 = 30/32.0f, ew = fw*7/511.0f,
				  mw = bw - sw - ew,
				  ex = bx+sw + max(mw*bar, fw*7/511.0f);
				  
			if(bar > 0)
			{
				settexture("data/loading_bar.png", 3);
				glBegin(GL_QUADS);
				glTexCoord2f(su1, bv1); glVertex2f(bx,    by);
				glTexCoord2f(su2, bv1); glVertex2f(bx+sw, by);
				glTexCoord2f(su2, bv2); glVertex2f(bx+sw, by+bh);
				glTexCoord2f(su1, bv2); glVertex2f(bx,    by+bh);

				glTexCoord2f(su2, bv1); glVertex2f(bx+sw, by);
				glTexCoord2f(eu1, bv1); glVertex2f(ex,    by);
				glTexCoord2f(eu1, bv2); glVertex2f(ex,    by+bh);
				glTexCoord2f(su2, bv2); glVertex2f(bx+sw, by+bh);

				glTexCoord2f(eu1, bv1); glVertex2f(ex,    by);
				glTexCoord2f(eu2, bv1); glVertex2f(ex+ew, by);
				glTexCoord2f(eu2, bv2); glVertex2f(ex+ew, by+bh);
				glTexCoord2f(eu1, bv2); glVertex2f(ex,    by+bh);
				glEnd();
			}

			if(text)
			{
				int tw = text_width(text);
				float tsz = bh*0.8f/FONTH;
				if(tw*tsz > mw) tsz = mw/tw;
				glPushMatrix();
				glTranslatef(bx+sw, by + (bh - FONTH*tsz)/2, 0);
				glScalef(tsz, tsz, 1);
				draw_text(text, 0, 0);
				glPopMatrix();
			}

			glDisable(GL_BLEND);

			if(tex)
			{
				glBindTexture(GL_TEXTURE_2D, tex);
				float sz = 0.35f*min(w, h), x = 0.5f*(w-sz), y = 0.5f*min(w, h) - sz/15;
				glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(0, 0); glVertex2f(x,    y);
				glTexCoord2f(1, 0); glVertex2f(x+sz, y);
				glTexCoord2f(0, 1); glVertex2f(x,    y+sz);
				glTexCoord2f(1, 1); glVertex2f(x+sz, y+sz);
				glEnd();

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				settexture("data/mapshot_frame.png", 3);
				glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(0, 0); glVertex2f(x,    y);
				glTexCoord2f(1, 0); glVertex2f(x+sz, y);
				glTexCoord2f(0, 1); glVertex2f(x,    y+sz);
				glTexCoord2f(1, 1); glVertex2f(x+sz, y+sz);
				glEnd();
				glDisable(GL_BLEND);
			}

			glDisable(GL_TEXTURE_2D);

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			swapbuffers();	
		}
		
		loading_theme *l_theme = new loading_theme;
		
		void render_loading_screen(float bar, const char *text, GLuint tex, bool background)
		{
			l_theme->render(bar, text, tex, background);
		};
	}
}

#endif
