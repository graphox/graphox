// main.cpp: initialisation & main loop

#include "engine.h"
#include "cube.h"

VARP(defaultload, 0, 0, 1);

#ifdef _ENABLE_JOYSTIC_
VAR(enable_joystick, 0, 0, 1);

VAR(joystick_invert_x, 0, 1, 1);
VAR(joystick_invert_y, 0, 0, 1);

FVAR(joystick_sens, 0, 1.0, 1);

bool joystick_inited = false;
SDL_Joystick *joystick;
#endif

string graphox_version = "2";
ICOMMAND(getgraphoxversion, "", (), result(graphox_version));

void cleanup()
{
    recorder::stop();
    cleanupserver();
    SDL_ShowCursor(1);
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    SDL_SetGamma(1, 1, 1);
    freeocta(worldroot);
    extern void clear_command(); clear_command();
    extern void clear_console(); clear_console();
    extern void clear_mdls();    clear_mdls();
    extern void clear_sound();   clear_sound();
    SDL_Quit();
}

void quit()                     // normal exit
{
    extern void writeinitcfg();
    writeinitcfg();

    writeservercfg();

    writecfg();

    game::writestats();

    abortconnect();
    disconnect();
    localdisconnect();
    cleanup();
    exit(EXIT_SUCCESS);
}

void fatal(const char *s, ...)    // failure exit
{
    static int errors = 0;
    errors++;

    if(errors <= 2) // print up to one extra recursive error
    {
        defvformatstring(msg,s,s);
        puts(msg);

        if(errors <= 1) // avoid recursion
        {
            if(SDL_WasInit(SDL_INIT_VIDEO))
            {
                SDL_ShowCursor(1);
                SDL_WM_GrabInput(SDL_GRAB_OFF);
                SDL_SetGamma(1, 1, 1);
            }
            #ifdef WIN32
                MessageBox(NULL, msg, "Cube 2: Sauerbraten fatal error", MB_OK|MB_SYSTEMMODAL);
            #endif
            SDL_Quit();
        }
    }

    exit(EXIT_FAILURE);
}

SDL_Surface *screen = NULL;

int curtime = 0, totalmillis = 1, lastmillis = 1;

dynent *player = NULL;

int initing = NOT_INITING;
static bool restoredinits = false;

bool initwarning(const char *desc, int level, int type)
{
    if(initing < level)
    {
        addchange(desc, type);
        return true;
    }
    return false;
}

#define SCR_MINW 320
#define SCR_MINH 200
#define SCR_MAXW 10000
#define SCR_MAXH 10000
#define SCR_DEFAULTW 1024
#define SCR_DEFAULTH 768
VARF(scr_w, SCR_MINW, -1, SCR_MAXW, initwarning("screen resolution"));
VARF(scr_h, SCR_MINH, -1, SCR_MAXH, initwarning("screen resolution"));
VARF(colorbits, 0, 0, 32, initwarning("color depth"));
VARF(depthbits, 0, 0, 32, initwarning("depth-buffer precision"));
VARF(stencilbits, 0, 0, 32, initwarning("stencil-buffer precision"));
VARF(fsaa, -1, -1, 16, initwarning("anti-aliasing"));
VARF(vsync, -1, -1, 1, initwarning("vertical sync"));

void writeinitcfg()
{
    if(!restoredinits) return;
    stream *f = openfile("init.cfg", "w");
    if(!f) return;
    f->printf("// automatically written on exit, DO NOT MODIFY\n// modify settings in game\n");
    extern int theme;
    f->printf("theme %d\n", theme);
    extern int fullscreen;
    f->printf("fullscreen %d\n", fullscreen);
    f->printf("scr_w %d\n", scr_w);
    f->printf("scr_h %d\n", scr_h);
    f->printf("colorbits %d\n", colorbits);
    f->printf("depthbits %d\n", depthbits);
    f->printf("stencilbits %d\n", stencilbits);
    f->printf("fsaa %d\n", fsaa);
    f->printf("vsync %d\n", vsync);
    extern int useshaders, shaderprecision, forceglsl;
    f->printf("shaders %d\n", useshaders);
    f->printf("shaderprecision %d\n", shaderprecision);
    f->printf("forceglsl %d\n", forceglsl);
    extern int soundchans, soundfreq, soundbufferlen;
    f->printf("soundchans %d\n", soundchans);
    f->printf("soundfreq %d\n", soundfreq);
    f->printf("soundbufferlen %d\n", soundbufferlen);
    delete f;
}

COMMAND(quit, "");

void getbackgroundres(int &w, int &h)
{
    float wk = 1, hk = 1;
    if(w < 1024) wk = 1024.0f/w;
    if(h < 768) hk = 768.0f/h;
    wk = hk = max(wk, hk);
    w = int(ceil(w*wk));
    h = int(ceil(h*hk));
}

string backgroundcaption = "";
Texture *backgroundmapshot = NULL;
string backgroundmapname = "";
char *backgroundmapinfo = NULL;

float circle[3];

string backgroundimg;
string backgroundtxt;
string backgroundmap;

void restorebackground()
{
    if(renderedframe) return;
    renderbackground(backgroundcaption[0] ? backgroundcaption : NULL, backgroundmapshot, backgroundmapname[0] ? backgroundmapname : NULL, backgroundmapinfo, true);
}

int font_loaded_theme = -1;
void renderbackground(const char *caption, Texture *mapshot, const char *mapname, const char *mapinfo, bool restore, bool force)
{
	if(font_loaded_theme != theme)
	{
		if(theme == 0)
		    if(!execfile("data/font.cfg", false)) fatal("cannot find font definitions");
		if(theme == 1)
		    if(!execfile("data/themes/se/font.cfg", false)) fatal("cannot find font definitions");
		if(theme == 2)
		    if(!execfile("data/themes/crash/font.cfg", false)) fatal("cannot find font definitions");
		if(theme == 3)
		    if(!execfile("data/themes/red eclipse/font.cfg", false)) fatal("cannot find font definitions");
		    
		font_loaded_theme = theme;
	}

	if(theme == 0)
	{
		if(!inbetweenframes && !force) return;

		stopsounds(); // stop sounds while loading

		int w = screen->w, h = screen->h;
		getbackgroundres(w, h);
		gettextres(w, h);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, w, h, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		defaultshader->set();
		glEnable(GL_TEXTURE_2D);

		static int lastupdate = -1, lastw = -1, lasth = -1;
		static float backgroundu = 0, backgroundv = 0, detailu = 0, detailv = 0;
		static int numdecals = 0;
		static struct decal { float x, y, size; int side; } decals[12];
		if((renderedframe && !mainmenu && lastupdate != lastmillis) || lastw != w || lasth != h)
		{
			lastupdate = lastmillis;
			lastw = w;
			lasth = h;

			backgroundu = rndscale(1);
			backgroundv = rndscale(1);
			detailu = rndscale(1);
			detailv = rndscale(1);
			numdecals = sizeof(decals)/sizeof(decals[0]);
			numdecals = numdecals/3 + rnd((numdecals*2)/3 + 1);
			float maxsize = min(w, h)/16.0f;
			loopi(numdecals)
			{
				decal d = { rndscale(w), rndscale(h), maxsize/2 + rndscale(maxsize/2), rnd(2) };
				decals[i] = d;
			}
		}
		else if(lastupdate != lastmillis) lastupdate = lastmillis;

		loopi(restore ? 1 : 3)
		{
			glColor3f(1, 1, 1);
			settexture("data/themes/default/background.png", 0);
			float bu = w*0.67f/256.0f + backgroundu, bv = h*0.67f/256.0f + backgroundv;
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0,  0);  glVertex2f(0, 0);
			glTexCoord2f(bu, 0);  glVertex2f(w, 0);
			glTexCoord2f(0,  bv); glVertex2f(0, h);
			glTexCoord2f(bu, bv); glVertex2f(w, h);
			glEnd();
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			settexture("data/background_detail.png", 0);
			float du = w*0.8f/512.0f + detailu, dv = h*0.8f/512.0f + detailv;
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0,  0);  glVertex2f(0, 0);
			glTexCoord2f(du, 0);  glVertex2f(w, 0);
			glTexCoord2f(0,  dv); glVertex2f(0, h);
			glTexCoord2f(du, dv); glVertex2f(w, h);
			glEnd();
			settexture("data/background_decal.png", 3);
			glBegin(GL_QUADS);
			loopj(numdecals)
			{
				float hsz = decals[j].size, hx = clamp(decals[j].x, hsz, w-hsz), hy = clamp(decals[j].y, hsz, h-hsz), side = decals[j].side;
				glTexCoord2f(side,   0); glVertex2f(hx-hsz, hy-hsz);
				glTexCoord2f(1-side, 0); glVertex2f(hx+hsz, hy-hsz);
				glTexCoord2f(1-side, 1); glVertex2f(hx+hsz, hy+hsz);
				glTexCoord2f(side,   1); glVertex2f(hx-hsz, hy+hsz);
			}
			glEnd();
			float lh = 0.5f*min(w, h), lw = lh*2,
				  lx = 0.5f*(w - lw), ly = 0.5f*(h*0.5f - lh);
			settexture((maxtexsize ? min(maxtexsize, hwtexsize) : hwtexsize) >= 1024 && (screen->w > 1280 || screen->h > 800) ? "data/logo_1024.png" : "data/logo.png", 3);
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0, 0); glVertex2f(lx,    ly);
			glTexCoord2f(1, 0); glVertex2f(lx+lw, ly);
			glTexCoord2f(0, 1); glVertex2f(lx,    ly+lh);
			glTexCoord2f(1, 1); glVertex2f(lx+lw, ly+lh);
			glEnd();

			if(caption)
			{
				int tw = text_width(caption);
				float tsz = 0.04f*min(w, h)/FONTH,
					  tx = 0.5f*(w - tw*tsz), ty = h - 0.075f*1.5f*min(w, h) - 1.25f*FONTH*tsz;
				glPushMatrix();
				glTranslatef(tx, ty, 0);
				glScalef(tsz, tsz, 1);
				draw_text(caption, 0, 0);
				glPopMatrix();
			}
			if(mapshot || mapname)
			{
				int infowidth = 12*FONTH;
				float sz = 0.35f*min(w, h), msz = (0.75f*min(w, h) - sz)/(infowidth + FONTH), x = 0.5f*(w-sz), y = ly+lh - sz/15;
				if(mapinfo)
				{
					int mw, mh;
					text_bounds(mapinfo, mw, mh, infowidth);
					x -= 0.5f*(mw*msz + FONTH*msz);
				}
				if(mapshot && mapshot!=notexture)
				{
					glBindTexture(GL_TEXTURE_2D, mapshot->id);
					glBegin(GL_TRIANGLE_STRIP);
					glTexCoord2f(0, 0); glVertex2f(x,    y);
					glTexCoord2f(1, 0); glVertex2f(x+sz, y);
					glTexCoord2f(0, 1); glVertex2f(x,    y+sz);
					glTexCoord2f(1, 1); glVertex2f(x+sz, y+sz);
					glEnd();
				}
				else
				{
					int qw, qh;
					text_bounds("?", qw, qh);
					float qsz = sz*0.5f/max(qw, qh);
					glPushMatrix();
					glTranslatef(x + 0.5f*(sz - qw*qsz), y + 0.5f*(sz - qh*qsz), 0);
					glScalef(qsz, qsz, 1);
					draw_text("?", 0, 0);
					glPopMatrix();
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				}
				settexture("data/mapshot_frame.png", 3);
				glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(0, 0); glVertex2f(x,    y);
				glTexCoord2f(1, 0); glVertex2f(x+sz, y);
				glTexCoord2f(0, 1); glVertex2f(x,    y+sz);
				glTexCoord2f(1, 1); glVertex2f(x+sz, y+sz);
				glEnd();
				if(mapname)
				{
					int tw = text_width(mapname);
					float tsz = sz/(8*FONTH),
						  tx = 0.9f*sz - tw*tsz, ty = 0.9f*sz - FONTH*tsz;
					if(tx < 0.1f*sz) { tsz = 0.1f*sz/tw; tx = 0.1f; }
					glPushMatrix();
					glTranslatef(x+tx, y+ty, 0);
					glScalef(tsz, tsz, 1);
					draw_text(mapname, 0, 0);
					glPopMatrix();
				}
				if(mapinfo)
				{
					glPushMatrix();
					glTranslatef(x+sz+FONTH*msz, y, 0);
					glScalef(msz, msz, 1);
					draw_text(mapinfo, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF, -1, infowidth);
					glPopMatrix();
				}
			}
			glDisable(GL_BLEND);
			if(!restore) swapbuffers();
		}
		glDisable(GL_TEXTURE_2D);

		if(!restore)
		{
			renderedframe = false;
			copystring(backgroundcaption, caption ? caption : "");
			backgroundmapshot = mapshot;
			copystring(backgroundmapname, mapname ? mapname : "");
			if(mapinfo != backgroundmapinfo)
			{
				DELETEA(backgroundmapinfo);
				if(mapinfo) backgroundmapinfo = newstring(mapinfo);
			}
		}
	}

	if(theme == 1)
	{
		if(!inbetweenframes && !force) return;

		if(!mainmenu) stopsounds(); // stop sounds while loading

		int w = screen->w, h = screen->h;
		getbackgroundres(w, h);
		gettextres(w, h);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, w, h, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		defaultshader->set();
		glEnable(GL_TEXTURE_2D);

		static int lastupdate = -1, lastw = -1, lasth = -1;
		static float backgroundu = 0, backgroundv = 0;
		if((renderedframe && !mainmenu && lastupdate != lastmillis) || lastw != w || lasth != h)
		{
			lastupdate = lastmillis;
			lastw = w;
			lasth = h;

			backgroundu = rndscale(1);
			backgroundv = rndscale(1);
		}
		else if(lastupdate != lastmillis) lastupdate = lastmillis;

		copystring(backgroundimg, !force ? "data/themes/se/background_blured.png" : "data/themes/se/background.png");

		loopi(restore ? 1 : 3)
		{
			glColor3f(1, 1, 1);
			if(force)
			{
				settexture(backgroundimg, 0);
				glBegin(GL_QUADS);
				glTexCoord2f(0,    0);  glVertex2f(0, 0);
				glTexCoord2f(1.0f, 0);  glVertex2f(w, 0);
				glTexCoord2f(1.0f, 1.0f); glVertex2f(w, h);
				glTexCoord2f(0,    1.0f); glVertex2f(0, h);
				glEnd();
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			glEnable(GL_BLEND);

			if(mapshot || mapname)
			{
				if(mapshot && mapshot!=notexture)
				{
					defformatstring(mshot)("data/themes/se/mshots/%s.jpg", mapname);
					copystring(backgroundimg, mshot);

				}
				else
				{
					settexture("data/themes/se/background_blured.png", 0);
					glBegin(GL_TRIANGLE_STRIP);
					glTexCoord2f(0, 0); glVertex2f(0, 0);
					glTexCoord2f(1, 0); glVertex2f(w, 0);
					glTexCoord2f(0, 1); glVertex2f(0, h);
					glTexCoord2f(1, 1); glVertex2f(w, h);
					glEnd();
				}
				if(mapname) copystring(backgroundmap, mapname);
				else copystring(backgroundmap, "");
			}

			glDisable(GL_BLEND);
			if(!restore) swapbuffers();
		}
		glDisable(GL_TEXTURE_2D);

		if(!restore)
		{
			renderedframe = false;
			copystring(backgroundcaption, caption ? caption : "");
			backgroundmapshot = mapshot;
			copystring(backgroundmapname, mapname ? mapname : "");
			if(mapinfo != backgroundmapinfo)
			{
				DELETEA(backgroundmapinfo);
				if(mapinfo) backgroundmapinfo = newstring(mapinfo);
			}
		}
	}

	if(theme == 2 || theme == 3)
	{
		if(!inbetweenframes && !force) return;

		if(!mainmenu) stopsounds(); // stop sounds while loading

		int w = screen->w, h = screen->h;
		getbackgroundres(w, h);
		gettextres(w, h);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, w, h, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		defaultshader->set();
		glEnable(GL_TEXTURE_2D);

		static int lastupdate = -1, lastw = -1, lasth = -1;
		static float backgroundu = 0, backgroundv = 0;
		if((renderedframe && !mainmenu && lastupdate != lastmillis) || lastw != w || lasth != h)
		{
			lastupdate = lastmillis;
			lastw = w;
			lasth = h;

			backgroundu = rndscale(1);
			backgroundv = rndscale(1);
		}
		else if(lastupdate != lastmillis) lastupdate = lastmillis;

		if(theme == 2)
            copystring(backgroundimg, !force ? "data/themes/crash/background_start.jpg" : "data/themes/crash/background.jpg");
		if(theme == 3)
            copystring(backgroundimg, !force ? "data/themes/red eclipse/background.png" : "data/themes/red eclipse/background.png");

		loopi(restore ? 1 : 3)
		{
			glColor3f(1, 1, 1);
			if(force)
			{
				settexture(backgroundimg, 0);
				glBegin(GL_QUADS);
				glTexCoord2f(0,    0);  glVertex2f(0, 0);
				glTexCoord2f(1.0f, 0);  glVertex2f(w, 0);
				glTexCoord2f(1.0f, 1.0f); glVertex2f(w, h);
				glTexCoord2f(0,    1.0f); glVertex2f(0, h);
				glEnd();
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			glEnable(GL_BLEND);

			if(caption) copystring(backgroundtxt, caption);
			else copystring(backgroundtxt, "");

			if(mapshot || mapname)
			{
				if(mapshot && mapshot!=notexture)
				{
					defformatstring(mshot)("data/themes/se/mshots/%s.jpg", mapname);
					copystring(backgroundimg, mshot);

				}
				else
				{
					settexture("data/themes/crash/background_start.jpg", 0);
					glBegin(GL_TRIANGLE_STRIP);
					glTexCoord2f(0, 0); glVertex2f(0, 0);
					glTexCoord2f(1, 0); glVertex2f(w, 0);
					glTexCoord2f(0, 1); glVertex2f(0, h);
					glTexCoord2f(1, 1); glVertex2f(w, h);
					glEnd();
				}
				if(mapname) copystring(backgroundmap, mapname);
				else copystring(backgroundmap, "");
			}

			glDisable(GL_BLEND);
			if(!restore) swapbuffers();
		}
		glDisable(GL_TEXTURE_2D);

		if(!restore)
		{
			renderedframe = false;
			copystring(backgroundcaption, caption ? caption : "");
			backgroundmapshot = mapshot;
			copystring(backgroundmapname, mapname ? mapname : "");
			if(mapinfo != backgroundmapinfo)
			{
				DELETEA(backgroundmapinfo);
				if(mapinfo) backgroundmapinfo = newstring(mapinfo);
			}
		}
	}
}

float loadprogress = 0;

void renderprogress(float bar, const char *text, GLuint tex, bool background)   // also used during loading
{
	//puts("render");
#if 0
	if(!inbetweenframes || envmapping) return;

	clientkeepalive();      // make sure our connection doesn't time out while loading maps etc.

	#ifdef __APPLE__
		interceptkey(SDLK_UNKNOWN); // keep the event queue awake to avoid 'beachball' cursor
	#endif

	graphox::theme::render_loading_screen(bar, text, tex, background);
	
	return;

#else
#if 0
	if(true)
	{

		extern int sdl_backingstore_bug;
		if(background || sdl_backingstore_bug > 0) restorebackground();

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
		//settexture("data/loading_frame.png", 3);
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
		//puts("renderdone");

	}

	if(theme == 1 || theme == 2 || theme == 3)
	{
#endif
		if(!inbetweenframes || envmapping) return;

		clientkeepalive();      // make sure our connection doesn't time out while loading maps etc.

		#ifdef __APPLE__
		interceptkey(SDLK_UNKNOWN); // keep the event queue awake to avoid 'beachball' cursor
		#endif

		extern int sdl_backingstore_bug;
		if(background || sdl_backingstore_bug > 0) restorebackground();

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
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		defaultshader->set();
		glColor3f(1, 1, 1);

		if(!settexture(backgroundimg, 0))
		{
			if(theme == 1)
				settexture("data/themes/se/background_blured.png", 0);
			if(theme == 2)
				settexture("data/themes/crash/background_start.jpg", 0);
			if(theme == 3)
				settexture("data/themes/red eclipse/background.png", 0);
		}
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(0, 0);
		glTexCoord2f(1, 0); glVertex2f(w, 0);
		glTexCoord2f(1, 1); glVertex2f(w, h);
		glTexCoord2f(0, 1); glVertex2f(0, h);
		glEnd();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if(defaultload == 0)
		{
			settexture("data/themes/se/circle1.png");
			glPushMatrix();
			glTranslatef(w/2, h/2, 0);
			glRotatef(circle[0], 0.0f, 0.0f, 1.0f);
			glTranslatef(w/-2, h/-2, 0);
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex2f(w/2-300, h/2-300);
			glTexCoord2f(1, 0); glVertex2f(w/2+300, h/2-300);
			glTexCoord2f(1, 1); glVertex2f(w/2+300, h/2+300);
			glTexCoord2f(0, 1); glVertex2f(w/2-300, h/2+300);
			glEnd();
			glPopMatrix();
			if(circle[0] >= 360) circle[0] = 0;
			else circle[0]++;

			settexture("data/themes/se/circle2.png");
			glPushMatrix();
			glTranslatef(w/2, h/2, 0);
			glRotatef(circle[1], 0.0f, 0.0f, -1.0f);
			glTranslatef(w/-2, h/-2, 0);
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex2f(w/2-300, h/2-300);
			glTexCoord2f(1, 0); glVertex2f(w/2+300, h/2-300);
			glTexCoord2f(1, 1); glVertex2f(w/2+300, h/2+300);
			glTexCoord2f(0, 1); glVertex2f(w/2-300, h/2+300);
			glEnd();
			glPopMatrix();
			if(circle[1] >= 360) circle[1] = 0;
			else circle[1]++;

			settexture("data/themes/se/circle3.png");
			glPushMatrix();
			glTranslatef(w/2, h/2, 0);
			glRotatef(circle[2], 0.0f, 0.0f, 1.0f);
			glTranslatef(w/-2, h/-2, 0);
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex2f(w/2-300, h/2-300);
			glTexCoord2f(1, 0); glVertex2f(w/2+300, h/2-300);
			glTexCoord2f(1, 1); glVertex2f(w/2+300, h/2+300);
			glTexCoord2f(0, 1); glVertex2f(w/2-300, h/2+300);
			glEnd();
			glPopMatrix();
			if(circle[2] >= 360) circle[2] = 0;
			else circle[2] += 2.0f;
		}
		else
		{
			float fh = 0.075f*min(w, h), fw = fh*10,
				fx = renderedframe ? w - fw - fh/4 : 0.5f*(w - fw),
				fy = renderedframe ? fh/4 : h - fh*1.5f,
				fu1 = 0/512.0f, fu2 = 511/512.0f,
				  fv1 = 0/64.0f, fv2 = 52/64.0f;
			if(theme == 1)
				settexture("data/themes/se/loading_frame.png", 3);
			if(theme == 2 || theme == 3)
				settexture("data/themes/crash/loading_frame.png", 3);
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(fu1, fv1); glVertex2f(fx,    fy);
			glTexCoord2f(fu2, fv1); glVertex2f(fx+fw, fy);
			glTexCoord2f(fu1, fv2); glVertex2f(fx,    fy+fh);
			glTexCoord2f(fu2, fv2); glVertex2f(fx+fw, fy+fh);
			glEnd();

			float bw = fw*(511 - 2*17)/511.0f, bh = fh*20/52.0f,
				  bx = fx + fw*17/511.0f, by = fy + fh*16/52.0f,
				  bv1 = 0/32.0f, bv2 = 20/32.0f,
				  su1 = 0/32.0f, su2 = 7/32.0f, sw = fw*7/511.0f,
				  eu1 = 23/32.0f, eu2 = 30/32.0f, ew = fw*7/511.0f,
				  mw = bw - sw - ew,
				  ex = bx+sw + max(mw*bar, fw*7/511.0f);

			glPopMatrix();
			if(theme == 1)
				settexture("data/themes/se/loading_bar.png", 3);
			if(theme == 2 || theme == 3)
				settexture("data/themes/crash/loading_bar.png", 3);
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

			settexture("data/themes/se/mapshot_frame.png", 3);
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0, 0); glVertex2f(x,    y);
			glTexCoord2f(1, 0); glVertex2f(x+sz, y);
			glTexCoord2f(0, 1); glVertex2f(x,    y+sz);
			glTexCoord2f(1, 1); glVertex2f(x+sz, y+sz);
			glEnd();
		}

        int wb, hb;
		glPushMatrix();
		glScalef(1/1.2f, 1/1.2f, 1);
		text_bounds(backgroundmap, wb, hb);
		int x = ((w/2)*1.2f)-wb/2,
			y = (h/2)*1.2f;

		draw_text(backgroundmap, x, y-128);
		glPopMatrix();

		glPushMatrix();
		glScalef(1/1.6f, 1/1.6f, 1);
		text_bounds(backgroundtxt, wb, hb);
		int xx = ((w/2)*1.6f)-wb/2,
			yy = (h/2)*1.6f;

		draw_text(backgroundtxt, xx, yy-64);
		glPopMatrix();

		if(text)
		{
			int wb, hb;
			glPushMatrix();
			glScalef(1/3.2f, 1/3.2f, 1);
			text_bounds(text, wb, hb);
			int x = ((w/2)*3.2f)-wb/2,
				y = (h/2)*3.2f;

			draw_text(text, x, y);
			glPopMatrix();
		}

		glDisable(GL_BLEND);


		glDisable(GL_TEXTURE_2D);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		swapbuffers();
#if 0
	}
#endif
#endif
}

void keyrepeat(bool on)
{
    SDL_EnableKeyRepeat(on ? SDL_DEFAULT_REPEAT_DELAY : 0,
                             SDL_DEFAULT_REPEAT_INTERVAL);
}

static bool grabinput = false, minimized = false;

void inputgrab(bool on)
{
#ifndef WIN32
    if(!(screen->flags & SDL_FULLSCREEN)) SDL_WM_GrabInput(SDL_GRAB_OFF);
    else
#endif
    SDL_WM_GrabInput(on ? SDL_GRAB_ON : SDL_GRAB_OFF);
    SDL_ShowCursor(on ? SDL_DISABLE : SDL_ENABLE);
}

void setfullscreen(bool enable)
{
    if(!screen) return;
#if defined(WIN32) || defined(__APPLE__)
    initwarning(enable ? "fullscreen" : "windowed");
#else
    if(enable == !(screen->flags&SDL_FULLSCREEN))
    {
        SDL_WM_ToggleFullScreen(screen);
        inputgrab(grabinput);
    }
#endif
}

#ifdef _DEBUG
VARF(fullscreen, 0, 0, 1, setfullscreen(fullscreen!=0));
#else
VARF(fullscreen, 0, 1, 1, setfullscreen(fullscreen!=0));
#endif

void screenres(int *w, int *h)
{
#if !defined(WIN32) && !defined(__APPLE__)
    if(initing >= INIT_RESET)
    {
#endif
        scr_w = clamp(*w, SCR_MINW, SCR_MAXW);
        scr_h = clamp(*h, SCR_MINH, SCR_MAXH);
#if defined(WIN32) || defined(__APPLE__)
        initwarning("screen resolution");
#else
        return;
    }
    SDL_Surface *surf = SDL_SetVideoMode(clamp(*w, SCR_MINW, SCR_MAXW), clamp(*h, SCR_MINH, SCR_MAXH), 0, SDL_OPENGL|(screen->flags&SDL_FULLSCREEN ? SDL_FULLSCREEN : SDL_RESIZABLE));
    if(!surf) return;
    screen = surf;
    scr_w = screen->w;
    scr_h = screen->h;
    glViewport(0, 0, scr_w, scr_h);
#endif
}

COMMAND(screenres, "ii");

VARFP(gamma, 30, 100, 300,
{
	float f = gamma/100.0f;
    if(SDL_SetGamma(f,f,f)==-1)
    {
        conoutf(CON_ERROR, "Could not set gamma (card/driver doesn't support it?)");
        conoutf(CON_ERROR, "sdl: %s", SDL_GetError());
    }
});

void resetgamma()
{
	float f = gamma/100.0f;
	if(f==1) return;
	SDL_SetGamma(1, 1, 1);
	SDL_SetGamma(f, f, f);
}

VAR(dbgmodes, 0, 0, 1);

int desktopw = 0, desktoph = 0;

void setupscreen(int &usedcolorbits, int &useddepthbits, int &usedfsaa)
{
    int flags = SDL_RESIZABLE;
    #if defined(WIN32) || defined(__APPLE__)
    flags = 0;
    #endif
    if(fullscreen) flags = SDL_FULLSCREEN;
    SDL_Rect **modes = SDL_ListModes(NULL, SDL_OPENGL|flags);
    if(modes && modes!=(SDL_Rect **)-1)
    {
        int widest = -1, best = -1;
        for(int i = 0; modes[i]; i++)
        {
            if(dbgmodes) conoutf(CON_DEBUG, "mode[%d]: %d x %d", i, modes[i]->w, modes[i]->h);
            if(widest < 0 || modes[i]->w > modes[widest]->w || (modes[i]->w == modes[widest]->w && modes[i]->h > modes[widest]->h))
                widest = i;
        }
        if(scr_w < 0 || scr_h < 0)
        {
            int w = scr_w, h = scr_h, ratiow = desktopw, ratioh = desktoph;
            if(w < 0 && h < 0) { w = SCR_DEFAULTW; h = SCR_DEFAULTH; }
            if(ratiow <= 0 || ratioh <= 0) { ratiow = modes[widest]->w; ratioh = modes[widest]->h; }
            for(int i = 0; modes[i]; i++) if(modes[i]->w*ratioh == modes[i]->h*ratiow)
            {
                if(w <= modes[i]->w && h <= modes[i]->h && (best < 0 || modes[i]->w < modes[best]->w))
                    best = i;
            }
        }
        if(best < 0)
        {
            int w = scr_w, h = scr_h;
            if(w < 0 && h < 0) { w = SCR_DEFAULTW; h = SCR_DEFAULTH; }
            else if(w < 0) w = (h*SCR_DEFAULTW)/SCR_DEFAULTH;
            else if(h < 0) h = (w*SCR_DEFAULTH)/SCR_DEFAULTW;
            for(int i = 0; modes[i]; i++)
            {
                if(w <= modes[i]->w && h <= modes[i]->h && (best < 0 || modes[i]->w < modes[best]->w || (modes[i]->w == modes[best]->w && modes[i]->h < modes[best]->h)))
                    best = i;
            }
        }
        if(flags&SDL_FULLSCREEN)
        {
            if(best >= 0) { scr_w = modes[best]->w; scr_h = modes[best]->h; }
            else if(desktopw > 0 && desktoph > 0) { scr_w = desktopw; scr_h = desktoph; }
            else if(widest >= 0) { scr_w = modes[widest]->w; scr_h = modes[widest]->h; }
        }
        else if(best < 0)
        {
            scr_w = min(scr_w >= 0 ? scr_w : (scr_h >= 0 ? (scr_h*SCR_DEFAULTW)/SCR_DEFAULTH : SCR_DEFAULTW), (int)modes[widest]->w);
            scr_h = min(scr_h >= 0 ? scr_h : (scr_w >= 0 ? (scr_w*SCR_DEFAULTH)/SCR_DEFAULTW : SCR_DEFAULTH), (int)modes[widest]->h);
        }
        if(dbgmodes) conoutf(CON_DEBUG, "selected %d x %d", scr_w, scr_h);
    }
    if(scr_w < 0 && scr_h < 0) { scr_w = SCR_DEFAULTW; scr_h = SCR_DEFAULTH; }
    else if(scr_w < 0) scr_w = (scr_h*SCR_DEFAULTW)/SCR_DEFAULTH;
    else if(scr_h < 0) scr_h = (scr_w*SCR_DEFAULTH)/SCR_DEFAULTW;

    bool hasbpp = true;
    if(colorbits)
        hasbpp = SDL_VideoModeOK(scr_w, scr_h, colorbits, SDL_OPENGL|flags)==colorbits;

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#if SDL_VERSION_ATLEAST(1, 2, 11)
    if(vsync>=0) SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, vsync);
#endif
    static int configs[] =
    {
        0x7, /* try everything */
        0x6, 0x5, 0x3, /* try disabling one at a time */
        0x4, 0x2, 0x1, /* try disabling two at a time */
        0 /* try disabling everything */
    };
    int config = 0;
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    if(!depthbits) SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    if(!fsaa)
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
    }
    loopi(sizeof(configs)/sizeof(configs[0]))
    {
        config = configs[i];
        if(!depthbits && config&1) continue;
        if(!stencilbits && config&2) continue;
        if(fsaa<=0 && config&4) continue;
        if(depthbits) SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, config&1 ? depthbits : 16);
        if(stencilbits)
        {
            hasstencil = config&2 ? stencilbits : 0;
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, hasstencil);
        }
        else hasstencil = 0;
        if(fsaa>0)
        {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, config&4 ? 1 : 0);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, config&4 ? fsaa : 0);
        }
        screen = SDL_SetVideoMode(scr_w, scr_h, hasbpp ? colorbits : 0, SDL_OPENGL|flags);
        if(screen) break;
    }
    if(!screen) fatal("Unable to create OpenGL screen: %s", SDL_GetError());
    else
    {
        if(!hasbpp) conoutf(CON_WARN, "%d bit color buffer not supported - disabling", colorbits);
        if(depthbits && (config&1)==0) conoutf(CON_WARN, "%d bit z-buffer not supported - disabling", depthbits);
        if(stencilbits && (config&2)==0) conoutf(CON_WARN, "Stencil buffer not supported - disabling");
        if(fsaa>0 && (config&4)==0) conoutf(CON_WARN, "%dx anti-aliasing not supported - disabling", fsaa);
    }

    scr_w = screen->w;
    scr_h = screen->h;

    usedcolorbits = hasbpp ? colorbits : 0;
    useddepthbits = config&1 ? depthbits : 0;
    usedfsaa = config&4 ? fsaa : 0;
}

void resetgl()
{
    clearchanges(CHANGE_GFX);

    renderbackground("resetting OpenGL");

    extern void cleanupva();
    extern void cleanupparticles();
    extern void cleanupsky();
    extern void cleanupmodels();
    extern void cleanuptextures();
    extern void cleanuplightmaps();
    extern void cleanupblendmap();
    extern void cleanshadowmap();
    extern void cleanreflections();
    extern void cleanupglare();
    extern void cleanupdepthfx();
    extern void cleanupshaders();
    extern void cleanupgl();
    recorder::cleanup();
    cleanupva();
    cleanupparticles();
    cleanupsky();
    cleanupmodels();
    cleanuptextures();
    cleanuplightmaps();
    cleanupblendmap();
    cleanshadowmap();
    cleanreflections();
    cleanupglare();
    cleanupdepthfx();
    cleanupshaders();
    cleanupgl();

    SDL_SetVideoMode(0, 0, 0, 0);

    int usedcolorbits = 0, useddepthbits = 0, usedfsaa = 0;
    setupscreen(usedcolorbits, useddepthbits, usedfsaa);
    gl_init(scr_w, scr_h, usedcolorbits, useddepthbits, usedfsaa);

    extern void reloadfonts();
    extern void reloadtextures();
    extern void reloadshaders();
    inbetweenframes = false;
    if(!reloadtexture(*notexture) ||
       !reloadtexture("data/logo.png") ||
       !reloadtexture("data/logo_1024.png") ||
       !reloadtexture("data/background.png") ||
       !reloadtexture("data/background_detail.png") ||
       !reloadtexture("data/background_decal.png") ||
       !reloadtexture("data/mapshot_frame.png") ||
       !reloadtexture("data/loading_frame.png") ||
       !reloadtexture("data/loading_bar.png"))
        fatal("failed to reload core texture");
    reloadfonts();
    inbetweenframes = true;
    renderbackground("initializing...");
	resetgamma();
    reloadshaders();
    reloadtextures();
    initlights();
    allchanged(true);
}

COMMAND(resetgl, "");

static int ignoremouse = 5;

vector<SDL_Event> events;

void pushevent(const SDL_Event &e)
{
    events.add(e);
}

bool interceptkey(int sym)
{
    static int lastintercept = SDLK_UNKNOWN;
    int len = lastintercept == sym ? events.length() : 0;
    SDL_Event event;
    while(SDL_PollEvent(&event)) switch(event.type)
    {
        case SDL_MOUSEMOTION: break;
        default: pushevent(event); break;
    }
    lastintercept = sym;
    if(sym != SDLK_UNKNOWN) for(int i = len; i < events.length(); i++)
    {
        if(events[i].type == SDL_KEYDOWN && events[i].key.keysym.sym == sym) { events.remove(i); return true; }
    }
    return false;
}

static void resetmousemotion()
{
#ifndef WIN32
    if(!(screen->flags&SDL_FULLSCREEN))
    {
        SDL_WarpMouse(screen->w / 2, screen->h / 2);
    }
#endif
}

static inline bool skipmousemotion(SDL_Event &event)
{
    if(event.type != SDL_MOUSEMOTION && event.type != SDL_JOYAXISMOTION) return true;
#ifndef WIN32
    if(!(screen->flags&SDL_FULLSCREEN))
    {
        #ifdef __APPLE__
        if(event.motion.y == 0) return true;  // let mac users drag windows via the title bar
        #endif
        if(event.motion.x == screen->w / 2 && event.motion.y == screen->h / 2) return true;  // ignore any motion events generated SDL_WarpMouse
    }
#endif
    return false;
}

static void checkmousemotion(int &dx, int &dy)
{
    loopv(events)
    {
        SDL_Event &event = events[i];
        if(skipmousemotion(event))
        {
            if(i > 0) events.remove(0, i);
            return;
        }
        dx += event.motion.xrel;
        dy += event.motion.yrel;
    }
    events.setsize(0);
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(skipmousemotion(event))
        {
            events.add(event);
            return;
        }
        dx += event.motion.xrel;
        dy += event.motion.yrel;
    }
}

int current_x;
int current_y;
int lasthat;

void checkinput()
{
    SDL_Event event;
    int lasttype = 0, lastbut = 0;

    #ifndef WIN32
        int current_x = 0, current_y = 0, lasthat = 0;
    #endif

    while(events.length() || SDL_PollEvent(&event))
    {
        if(events.length()) event = events.remove(0);

        switch(event.type)
        {
            case SDL_QUIT:
                quit();
                break;

            #if !defined(WIN32) && !defined(__APPLE__)
            case SDL_VIDEORESIZE:
            	graphox::gui::check_event(event);
                screenres(&event.resize.w, &event.resize.h);
                break;
            #endif

            case SDL_KEYDOWN:
            case SDL_KEYUP:
            	if(graphox::gui::open)
            		graphox::gui::check_event(event);
            	else
                	keypress(event.key.keysym.sym, event.key.state==SDL_PRESSED, event.key.keysym.unicode);
                break;

            case SDL_ACTIVEEVENT:
                if(event.active.state & SDL_APPINPUTFOCUS)
                    inputgrab(grabinput = event.active.gain!=0);
                if(event.active.state & SDL_APPACTIVE)
                    minimized = !event.active.gain;
                break;

            case SDL_MOUSEMOTION:
            	if (graphox::gui::open)
            		graphox::gui::check_event(event);
            	else
            	{
		            if(ignoremouse) { ignoremouse--; break; }
		            if(grabinput && !skipmousemotion(event))
		            {
		                int dx = event.motion.xrel, dy = event.motion.yrel;
		                checkmousemotion(dx, dy);
		                resetmousemotion();
		                if(!g3d_movecursor(dx, dy)) mousemove(dx, dy);
		            }
				}
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            	if(graphox::gui::open)
            		graphox::gui::check_event(event);
            	else
            	{
		            if(lasttype==event.type && lastbut==event.button.button) break; // why?? get event twice without it
		            keypress(-event.button.button, event.button.state!=0, 0);
		            lasttype = event.type;
		            lastbut = event.button.button;
				}
                break;
			#ifdef _ENABLE_JOYSTIC_

			case SDL_JOYAXISMOTION:
				#define J_SENS 150
				if (!( ( event.jaxis.value < -3200 ) || (event.jaxis.value > 3200 ) ))
					return;

				event.jaxis.value /= J_SENS;
				event.jaxis.value /= joystick_sens;

				printf("joystick: %d %d\n",event.jaxis.axis, event.jaxis.value);
                if( event.jaxis.axis == 0)
                {
                    int diff = current_x - event.jaxis.value;
                    if (joystick_invert_x) diff *= -1;
                    current_x = current_x + diff;
                    mousemove(diff, 0);
                }

                if( event.jaxis.axis == 1)
                {
                    int diff = current_y - event.jaxis.value;

                    if (joystick_invert_y) diff *= -1;

                    current_y = current_y + diff;
                    mousemove(0, diff);
                }


			break;

			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
				keypress(530+event.jbutton.button, event.jbutton.state==SDL_PRESSED, 0);
			break;

			case SDL_JOYHATMOTION:  // Handle Hat Motion

            if ( event.jhat.value & SDL_HAT_UP )
            {
                lasthat = 521;
                keypress(521, true, 0);
            }
            else if (event.jhat.value & SDL_HAT_LEFT)
            {
                                        lasthat = 515;
                                        keypress(515, true, 0);
            }
//          else if(event.jhat.value & SDL_HAT_RIGHTDOWN)
//              keypress(520, true, 0);
            else if(event.jhat.value & SDL_HAT_DOWN)
            {
                lasthat = 522;
                keypress(522, true, 0);
            }
            else if(event.jhat.value & SDL_HAT_RIGHT)
            {
                lasthat = 516;
                keypress(516, true, 0);
            }
            else if((event.jhat.value & SDL_HAT_CENTERED) || lasthat != -1)
            {
                if(lasthat == -1) break;
                keypress(lasthat, false, 0);
                lasthat = -1;
            }
            break;

			#endif //end of #ifdef _ENABLE_JOYSTIC_

        }
    }
}

void swapbuffers()
{
    recorder::capture();
    SDL_GL_SwapBuffers();
}

VARF(gamespeed, 10, 100, 1000, if(multiplayer()) gamespeed = 100);

VARF(paused, 0, 0, 1, if(multiplayer()) paused = 0);

VAR(mainmenufps, 0, 60, 1000);
VARP(maxfps, 0, 200, 1000);

void limitfps(int &millis, int curmillis)
{
    int limit = mainmenu && mainmenufps ? (maxfps ? min(maxfps, mainmenufps) : mainmenufps) : maxfps;
    if(!limit) return;
    static int fpserror = 0;
    int delay = 1000/limit - (millis-curmillis);
    if(delay < 0) fpserror = 0;
    else
    {
        fpserror += 1000%limit;
        if(fpserror >= limit)
        {
            ++delay;
            fpserror -= limit;
        }
        if(delay > 0)
        {
            SDL_Delay(delay);
            millis += delay;
        }
    }
}

#if defined(WIN32) && !defined(_DEBUG) && !defined(__GNUC__)
void stackdumper(unsigned int type, EXCEPTION_POINTERS *ep)
{
    if(!ep) fatal("unknown type");
    EXCEPTION_RECORD *er = ep->ExceptionRecord;
    CONTEXT *context = ep->ContextRecord;
    string out, t;
    formatstring(out)("Cube 2: Sauerbraten Win32 Exception: 0x%x [0x%x]\n\n", er->ExceptionCode, er->ExceptionCode==EXCEPTION_ACCESS_VIOLATION ? er->ExceptionInformation[1] : -1);
    STACKFRAME sf = {{context->Eip, 0, AddrModeFlat}, {}, {context->Ebp, 0, AddrModeFlat}, {context->Esp, 0, AddrModeFlat}, 0};
    SymInitialize(GetCurrentProcess(), NULL, TRUE);

    while(::StackWalk(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), GetCurrentThread(), &sf, context, NULL, ::SymFunctionTableAccess, ::SymGetModuleBase, NULL))
    {
        struct { IMAGEHLP_SYMBOL sym; string n; } si = { { sizeof( IMAGEHLP_SYMBOL ), 0, 0, 0, sizeof(string) } };
        IMAGEHLP_LINE li = { sizeof( IMAGEHLP_LINE ) };
        DWORD off;
        if(SymGetSymFromAddr(GetCurrentProcess(), (DWORD)sf.AddrPC.Offset, &off, &si.sym) && SymGetLineFromAddr(GetCurrentProcess(), (DWORD)sf.AddrPC.Offset, &off, &li))
        {
            char *del = strrchr(li.FileName, '\\');
            formatstring(t)("%s - %s [%d]\n", si.sym.Name, del ? del + 1 : li.FileName, li.LineNumber);
            concatstring(out, t);
        }
    }
    fatal(out);
}
#endif

#define MAXFPSHISTORY 60

int fpspos = 0, fpshistory[MAXFPSHISTORY];

void resetfpshistory()
{
    loopi(MAXFPSHISTORY) fpshistory[i] = 1;
    fpspos = 0;
}

void updatefpshistory(int millis)
{
    fpshistory[fpspos++] = max(1, min(1000, millis));
    if(fpspos>=MAXFPSHISTORY) fpspos = 0;
}

void getfps(int &fps, int &bestdiff, int &worstdiff)
{
    int total = fpshistory[MAXFPSHISTORY-1], best = total, worst = total;
    loopi(MAXFPSHISTORY-1)
    {
        int millis = fpshistory[i];
        total += millis;
        if(millis < best) best = millis;
        if(millis > worst) worst = millis;
    }

    fps = (1000*MAXFPSHISTORY)/total;
    bestdiff = 1000/best-fps;
    worstdiff = fps-1000/worst;
}

void getfps_(int *raw)
{
    int fps, bestdiff, worstdiff;
    if(*raw) fps = 1000/fpshistory[(fpspos+MAXFPSHISTORY-1)%MAXFPSHISTORY];
    else getfps(fps, bestdiff, worstdiff);
    intret(fps);
}

COMMANDN(getfps, getfps_, "i");

bool inbetweenframes = false, renderedframe = true;

static bool findarg(int argc, char **argv, const char *str)
{
    for(int i = 1; i<argc; i++) if(strstr(argv[i], str)==argv[i]) return true;
    return false;
}

static int clockrealbase = 0, clockvirtbase = 0;
static void clockreset() { clockrealbase = SDL_GetTicks(); clockvirtbase = totalmillis; }
VARFP(clockerror, 990000, 1000000, 1010000, clockreset());
VARFP(clockfix, 0, 0, 1, clockreset());

#ifdef _ENABLE_JOYSTIC_
void initjoystick()
{
	if(enable_joystick != 1 || joystick_inited == true)
		return;

	printf("initiating joystick");

	SDL_JoystickEventState(SDL_ENABLE);
	joystick = SDL_JoystickOpen(0);
	if(!joystick){ printf("no joystic found!"); return; };
	assert(joystick);

	joystick_inited = true;
	printf("%s: %d axes, %d buttons, %d balls, %d hats\n", SDL_JoystickName(0), SDL_JoystickNumAxes(joystick), SDL_JoystickNumButtons(joystick), SDL_JoystickNumBalls(joystick), SDL_JoystickNumHats(joystick));
}
#endif

void readstats()
{
    char buf[64];   //Stats might be long after longer play and i'm afraid of that 32 won't be engough in that situtation
    int statsdata[2];
    stream *f = openfile("data/themes/stats", "r");
    if(!f)
    {
        conoutf("error opening stats file");
        return;
    }
    while(f->getline(buf, sizeof(buf)))
    {
        sscanf(buf, "stats[%d] = %d", &statsdata[0], &statsdata[1]);
        game::stats[statsdata[0]] = statsdata[1];
    }
    f->close();
}

namespace lua
{
	namespace module 
	{
		void open_luaproxy(lua_State * L);
	}
}

int main(int argc, char **argv)
{
	

	graphox::scripting::init();
	
	//lua_State *L = graphox::scripting::get_state();
	//lua::module::open_luaproxy(L);
	
	graphox::scripting::finalize_bindings();
	
	graphox::scripting::execute("init.lua");
	
	#ifdef WIN32
    //atexit((void (__cdecl *)(void))_CrtDumpMemoryLeaks);
    #ifndef _DEBUG
    #ifndef __GNUC__
    __try {
    #endif
    #endif
    #endif

    int dedicated = 0;
    char *load = NULL, *initscript = NULL;
	//bool graphox_gui = false;
	
    #define log(s) puts("init: " s)

    initing = INIT_RESET;
	
	execfile("init.cfg", false);
    for(int i = 1; i<argc; i++)
    {
        if(argv[i][0]=='-') switch(argv[i][1])
        {
            case 'q': printf("Using home directory: %s\n", &argv[i][2]); sethomedir(&argv[i][2]); break;
            case 'k': printf("Adding package directory: %s\n", &argv[i][2]); addpackagedir(&argv[i][2]); break;
            case 'r': /* removed, look above */ break;
            case 'd': dedicated = atoi(&argv[i][2]); if(dedicated<=0) dedicated = 2; break;
            case 'w': scr_w = clamp(atoi(&argv[i][2]), SCR_MINW, SCR_MAXW); if(!findarg(argc, argv, "-h")) scr_h = -1; break;
            case 'h': scr_h = clamp(atoi(&argv[i][2]), SCR_MINH, SCR_MAXH); if(!findarg(argc, argv, "-w")) scr_w = -1; break;
            case 'z': depthbits = atoi(&argv[i][2]); break;
            case 'b': colorbits = atoi(&argv[i][2]); break;
            case 'a': fsaa = atoi(&argv[i][2]); break;
            case 'v': vsync = atoi(&argv[i][2]); break;
            case 't': fullscreen = atoi(&argv[i][2]); break;
            case 's': stencilbits = atoi(&argv[i][2]); break;
            case 'f':
            {
                extern int useshaders, shaderprecision, forceglsl;
                int n = atoi(&argv[i][2]);
                useshaders = n > 0 ? 1 : 0;
                shaderprecision = clamp(n >= 4 ? n - 4 : n - 1, 0, 2);
                forceglsl = n >= 4 ? 1 : 0;
                break;
            }
            case 'l':
            {
                char pkgdir[] = "packages/";
                load = strstr(path(&argv[i][2]), path(pkgdir));
                if(load) load += sizeof(pkgdir)-1;
                else load = &argv[i][2];
                break;
            }
            case 'x': initscript = &argv[i][2]; break;
            
            //case 'g':
            //	graphox_gui = atoi(&argv[i][2]) != 0;
            //	break;
            
            default: if(!serveroption(argv[i])) gameargs.add(argv[i]); break;
        }
        else gameargs.add(argv[i]);
    }
    initing = NOT_INITING;

//    execfile("mod.cfg");//results in error

    if(dedicated <= 1)
    {
        log("sdl");

        int par = 0;
        #ifdef _DEBUG
        par = SDL_INIT_NOPARACHUTE;
        #ifdef WIN32
        SetEnvironmentVariable("SDL_DEBUG", "1");
        #endif
        #endif
#ifdef _ENABLE_JOYSTIC_
        if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK| par)<0) fatal("Unable to initialize SDL: %s", SDL_GetError());
#else
		if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO|SDL_INIT_AUDIO| par)<0) fatal("Unable to initialize SDL: %s", SDL_GetError());
#endif
    }

    log("net");
    if(enet_initialize()<0) fatal("Unable to initialise network module");
    atexit(enet_deinitialize);
    enet_time_set(0);

    log("game");
    game::parseoptions(gameargs);
    initserver(dedicated>0, dedicated>1);  // never returns if dedicated
    ASSERT(dedicated <= 1);
    game::initclient();
    
    //log("graphox");
    //graphox::init();

	log("stats: read");
	readstats();

    log("video: mode");
    const SDL_VideoInfo *video = SDL_GetVideoInfo();
    if(video)
    {
        desktopw = video->current_w;
        desktoph = video->current_h;
    }
    int usedcolorbits = 0, useddepthbits = 0, usedfsaa = 0;
    setupscreen(usedcolorbits, useddepthbits, usedfsaa);

    log("video: misc");
    SDL_WM_SetCaption("Cube 2: Sauerbraten", NULL);
    keyrepeat(false);
    SDL_ShowCursor(0);

    log("gl");
    gl_checkextensions();
    gl_init(scr_w, scr_h, usedcolorbits, useddepthbits, usedfsaa);
    notexture = textureload("packages/textures/notexture.png");
    if(!notexture) fatal("could not find core textures");
    
    //log("gui");
    //graphox::gui::init("main");

    log("console");
    persistidents = false;
    
    if(!rawexecfile("data/stdlib.cfg", true))
    #ifdef WIN32
    	fatal("You are running from the wrong directory. Please run the bat file in the main folder and make shure that the repo dir isn't missing.");
    #else
    	#if defined(__MAC__)
    		fatal("Cannot find data/stdlib.cfg");
    	#else
    		fatal("You are running from the wrong directory. Please run the ./sauerbraten_unix file in the main folder and make shure that the repo dir isn't missing.");
    	#endif
    #endif
    if(theme == 0)
        if(!execfile("data/font.cfg", false)) fatal("cannot find font definitions");
    if(theme == 1)
        if(!execfile("data/themes/se/font.cfg", false)) fatal("cannot find font definitions");
    if(theme == 2)
        if(!execfile("data/themes/crash/font.cfg", false)) fatal("cannot find font definitions");
    if(theme == 3)
        if(!execfile("data/themes/red eclipse/font.cfg", false)) fatal("cannot find font definitions");
    if(!setfont("default")) fatal("no default font specified");

    inbetweenframes = true;
    renderbackground("initializing...");

    log("gl: effects");
    loadshaders();
    particleinit();
    initdecals();

    log("world");
    camera1 = player = game::iterdynents(0);
    emptymap(0, true, NULL, false);

    log("sound");
    initsound();

    log("cfg");
    execfile("data/keymap.cfg");
    execfile("data/stdedit.cfg");
    execfile("data/menus.cfg");
    execfile("data/sounds.cfg");
    execfile("data/brush.cfg");
    if(!execfile("data/themes/commands.cfg")) fatal("cannot find graphox commands file");
    if(!execfile("data/themes/menus.cfg")) fatal("cannot find graphox menus file");
    execfile("mybrushes.cfg", false);
    if(game::savedservers()) execfile(game::savedservers(), false);

    persistidents = true;

    initing = INIT_LOAD;
    if(!execfile(game::savedconfig(), false))
    {
        execfile(game::defaultconfig());
        writecfg(game::restoreconfig());
    }
    execfile(game::autoexec(), false);
    initing = NOT_INITING;

    persistidents = false;

    string gamecfgname;
    copystring(gamecfgname, "data/game_");
    concatstring(gamecfgname, game::gameident());
    concatstring(gamecfgname, ".cfg");
    execfile(gamecfgname);

    game::loadconfigs();

    persistidents = true;

    if(execfile("once.cfg", false)) remove(findfile("once.cfg", "rb"));

    if(load)
    {
        log("localconnect");
        //localconnect();
        game::changemap(load);
    }

    if(initscript) execute(initscript);

    log("mainloop");

    initmumble();

    #ifdef _ENABLE_JOYSTIC_
	log("joystick");
    initjoystick();
   	#endif

    resetfpshistory();

    inputgrab(grabinput = true);

    for(;;)
    {
    	graphox::scripting::frame();
    	
        static int frames = 0;
        int millis = SDL_GetTicks() - clockrealbase;
        if(clockfix) millis = int(millis*(double(clockerror)/1000000));
        millis += clockvirtbase;
        if(millis<totalmillis) millis = totalmillis;
        limitfps(millis, totalmillis);
        int elapsed = millis-totalmillis;
        if(multiplayer(false)) curtime = game::ispaused() ? 0 : elapsed;
        else
        {
            static int timeerr = 0;
            int scaledtime = elapsed*gamespeed + timeerr;
            curtime = scaledtime/100;
            timeerr = scaledtime%100;
            if(curtime>200) curtime = 200;
            if(paused || game::ispaused()) curtime = 0;
        }
        lastmillis += curtime;
        totalmillis = millis;

        checkinput();
        menuprocess();
        tryedit();

        if(lastmillis) game::updateworld();
        if(!mainmenu) game::statsacc();

        checksleep(lastmillis);

        game::dotime();

        serverslice(false, 0);

        if(frames) updatefpshistory(elapsed);
        frames++;

        // miscellaneous general game effects
        recomputecamera();
        updateparticles();
        updatesounds();

        if(minimized) continue;

        inbetweenframes = false;
        if(mainmenu) gl_drawmainmenu(screen->w, screen->h);
        else gl_drawframe(screen->w, screen->h);
        
        //if(graphox_gui && graphox::gui::open)
        //	graphox::gui::render(screen->w, screen->h);
        
        swapbuffers();
        renderedframe = inbetweenframes = true;
    }

    ASSERT(0);
    return EXIT_FAILURE;

    #if defined(WIN32) && !defined(_DEBUG) && !defined(__GNUC__)
    } __except(stackdumper(0, GetExceptionInformation()), EXCEPTION_CONTINUE_SEARCH) { return 0; }
    #endif
}
