#include "rpg.h"

namespace game
{
	VARP(radartheme, 0, 1, 1);
	VARP(hudtheme, 0, 1, 1);
	VARP(hudside, 0, 1, 1);
	
	VAR(aidebug, 0, 0, 6);

    #define N(n) int stats::pointscale_##n, stats::percentscale_##n; //needed?
    RPGSTATNAMES
    #undef N

	//all current players and monsters and such
	vector<rpg_player *> players;
	
	//the main player
	rpgent *player1 = NULL;
	rpg_player *player1_ = NULL;
	
	//connected to any server?
	bool connected = false;

	//time
	int maptime = 0;
	
	//name of the map
	string mapname = "";
	
	//is the game paused
	bool paused = false;
	
    void updateworld()
    {
        if(!maptime) { maptime = lastmillis; return; }
        if(!curtime) return;
        physicsframe();
        updateobjects();
        player1->updateplayer();
        player1_->updateplayer();
        
        
//        loopv(players)
//        	players[i]->update(); //update all players
    }
	
	void changemap(const char *name) 
    {
		if(!connected) localconnect();
        if(!load_world(name)) emptymap(0, true, name);
    }
    
    ICOMMAND(map, "s", (char *s), changemap(s));
    
    //connect to game
    void gameconnect(bool _remote)
    {
    	connected = true;
    }
	
	//disconnect from game
	void gamedisconnect(bool cleanup) 
    { 
        connected = false; 
    }
	void forceedit(const char *a){};



    
    struct rpgmenu : g3d_callback
    {
        int time, tab, which;
        vec pos;
        
        rpgmenu() : time(0), tab(1), which(0), pos(0, 0, 0) {}

        void show(int n)
        {
            if((time && n==which) || !n)
                time = 0;
            else
            {
                time = starttime();
                pos  = menuinfrontofplayer();        
                which = n;
            }
        }

        void gui(g3d_gui &g, bool firstpass)
        {
            g.start(time, 0.03f, &tab);
            switch(which)
            {
                default:
                case 1:
                    g.tab("inventory", 0xFFFFF);
                    playerobj->invgui(g);
                    break;

                case 2:
                    g.tab("stats", 0xFFFFF);
                    playerobj->st_show(g);
                    break;

                case 3:
                    g.tab("active quests", 0xFFFFF);
                    listquests(false, g);
                    g.tab("completed quests", 0xFFFFF);
                    listquests(true, g);
                    break;
            }
            g.end();
        }

        void render()
        {
            if(time) g3d_addgui(this, pos, GUI_2D);
        }
    } menu;
    ICOMMAND(showplayergui, "i", (int *which), menu.show(*which));

    void initclient()
    {
        player1 = new rpgent(playerobj, vec(0, 0, 0), 0, 100, ENT_PLAYER);
        clearworld();
        //TODO:create players
//        players.add(player1);
    }

    void physicstrigger(physent *d, bool local, int floorlevel, int waterlevel, int material)
    {
        if     (waterlevel>0) playsoundname("free/splash1", d==player1 ? NULL : &d->o);
        else if(waterlevel<0) playsoundname("free/splash2", d==player1 ? NULL : &d->o);
        if     (floorlevel>0) { if(local) playsoundname("aard/jump"); else if(d->type==ENT_AI) playsoundname("aard/jump", &d->o); }
        else if(floorlevel<0) { if(local) playsoundname("aard/land"); else if(d->type==ENT_AI) playsoundname("aard/land", &d->o); }    
    }

    void dynentcollide(physent *d, physent *o, const vec &dir) {}
   
    void bounced(physent *d, const vec &surface) {}
 
    void edittrigger(const selinfo &sel, int op, int arg1, int arg2, int arg3) {}
    void vartrigger(ident *id) {}

    const char *getclientmap() { return mapname; }
    const char *getmapinfo() { return NULL; }
    void resetgamestate() {}
    void suicide(physent *d)
    {
		if(lastmillis%10 == 1)
		{
			playsoundname("aard/die1", d==player1 ? NULL : &d->o);
			playerobj->takedamage(10);
		}
    }
    void newmap(int size) {}

    void startmap(const char *name)
    {
        clearworld();
        copystring(mapname, name ? name : "");
        maptime = 0;
        findplayerspawn(player1);
        if(name) playerobj->st_init();
        entities::startmap();

    }
    void r_sais(const char *name, const char *text)
    {
    	if(!name[0])
			conoutf(CON_CHAT, "\f0 %s", text);
    	else
    		conoutf(CON_CHAT, "%s:\f0 %s", name, text);
    }
    COMMAND(r_sais, "ss");
    void r_sais_team(const char *name, const char *text)
    {
        if(!name[0])
			conoutf(CON_TEAMCHAT, "\f0 %s", text);
    	else
    		conoutf(CON_TEAMCHAT, "%s:\f0 %s", name, text);
    }
    COMMAND(r_sais_team, "ss");


    void quad(int x, int y, int xs, int ys)
    {
        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0, 0); glVertex2i(x,    y);
        glTexCoord2f(1, 0); glVertex2i(x+xs, y);
        glTexCoord2f(0, 1); glVertex2i(x,    y+ys);
        glTexCoord2f(1, 1); glVertex2i(x+xs, y+ys);
        glEnd();
    }
  
    bool needminimap() { return true; }
 
    float abovegameplayhud(int w, int h)
    {
        switch(player1->state)
        {
            case CS_EDITING:
                return 1;
            default:
                return (h-min(128, h))/float(h);
        }
    }
 
	int chh, cah, camh;
    float chs, cas, cams;
 
    void drawradar(float x, float y, float s)
    {
        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(x,   y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(x+s, y);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(x,   y+s);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(x+s, y+s);
        glEnd();
    } 
 	void drawblip(dynent *d, float x, float y, float s, const vec &pos, bool flagblip, float scale)
    {
        vec dir = d->o;
        dir.sub(pos).div(scale);
        float size = flagblip ? 0.1f : 0.05f,
              xoffset = flagblip ? -2*(3/32.0f)*size : -size,
              yoffset = flagblip ? -2*(1 - 3/32.0f)*size : -size,
              dist = dir.magnitude2(), maxdist = 1 - 0.05f - 0.05f;
        if(dist >= maxdist) dir.mul(maxdist/dist);
        dir.rotate_around_z(-camera1->yaw*RAD);
        drawradar(x + s*0.5f*(1.0f + dir.x + xoffset), y + s*0.5f*(1.0f + dir.y + yoffset), size*s);
    }

    void drawblip(dynent *d, float x, float y, float s, int i, bool flagblip, float scale)
    {
        
		if(radartheme != 1)
        	settexture("packages/hud/blip_neutral_flag.png", 3);
       	else
        	settexture("data/themes/hud/blip_red_flag.png", 3);
		
		if(game::radartheme != 1)
			drawblip(d, x, y, s, d->o, flagblip, scale);
		else
			drawblip(d, x, y, s, d->o, flagblip, scale);
    }
 
    void gameplayhud(int w, int h)
    {
    
    	if(hudtheme == 1)
    	{
			if(player1->state==CS_DEAD || player1->state==CS_SPECTATOR) return;
			glPushMatrix();
			glScalef(1/1.2f, 1/1.2f, 1);
			
			
        	draw_textf("%d", 80, h*1.2f-128, playerobj->s_hp); //draw health as a number
        	
        	int wb, hb;
        	
        	text_bounds(selected ? selected->name : "(none)", wb, hb); //needed and what does this do?
        	        	
        	draw_textf("%s", w*1.2f-wb-80, h*1.2f-128, selected ? selected->name : "(none)");
        	
		    if(playerobj->eff_maxhp() > 100)
		    {
				if(hudside == 0)
				{
					settexture("data/themes/hud/hud_megahealth.png");
					glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 0.0f);   glVertex2f(0,   h*1.2f-207);
					glTexCoord2f(1.0f, 0.0f);   glVertex2f(539, h*1.2f-207);
					glTexCoord2f(1.0f, 1.0f);   glVertex2f(539, h*1.2f);
					glTexCoord2f(0.0f, 1.0f);   glVertex2f(0,   h*1.2f);
					glEnd();
				}
				else if(hudside == 0)
				{
					settexture("data/themes/hud/hud_megahealth_2.png");
					glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 0.0f);   glVertex2f(w*1.2f-539, h*1.2f-207);
					glTexCoord2f(1.0f, 0.0f);   glVertex2f(w*1.2f,     h*1.2f-207);
					glTexCoord2f(1.0f, 1.0f);   glVertex2f(w*1.2f,     h*1.2f);
					glTexCoord2f(0.0f, 1.0f);   glVertex2f(w*1.2f-539, h*1.2f);
					glEnd();
				}
		    }

		    int health = (playerobj->s_hp*100)/playerobj->eff_maxhp(),
		        armour = (playerobj->s_mana*100)/playerobj->eff_maxmana(),
		        hh = (health*101)/100,
		        ah = (armour*167)/100;

		    float hs = (health*1.0f)/100,
		          as = (armour*1.0f)/100;

			//what does this do?
		    if     (chh>hh) chh -= max(1, ((chh-hh)/4));
		    else if(chh<hh) chh += max(1, ((hh-chh)/4));
		    if     (chs>hs) chs -= max(0.01f, ((chs-hs)/4));
		    else if(chs<hs) chs += max(0.01f, ((hs-chs)/4));

		    if     (cah>ah) cah -= max(1, ((cah-ah)/4));
		    else if(cah<ah) cah += max(1, ((ah-cah)/4));
		    if     (cas>as) cas -= max(0.01f, ((cas-as)/4));
		    else if(cas<as) cas += max(0.01f, ((as-cas)/4));


		    if(playerobj->s_hp > 0)
		    {
				if(hudside == 0)
				{
					settexture("data/themes/hud/hud_health.png");
					glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 1.0f-chs);   glVertex2f(47, h*1.2f-chh-56);
					glTexCoord2f(1.0f, 1.0f-chs);   glVertex2f(97, h*1.2f-chh-56);
					glTexCoord2f(1.0f, 1.0f);      glVertex2f(97, h*1.2f-57);
					glTexCoord2f(0.0f, 1.0f);      glVertex2f(47, h*1.2f-57);
					glEnd();
				}
				else if(hudside == 0)
				{
					settexture("data/themes/hud/hud_health_2.png");
					glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 1.0f-chs);   glVertex2f(w*1.2f-97, h*1.2f-chh-56);
					glTexCoord2f(1.0f, 1.0f-chs);   glVertex2f(w*1.2f-47, h*1.2f-chh-56);
					glTexCoord2f(1.0f, 1.0f);       glVertex2f(w*1.2f-47, h*1.2f-57);
					glTexCoord2f(0.0f, 1.0f);       glVertex2f(w*1.2f-97, h*1.2f-57);
					glEnd();
				}
		    }

		    if(playerobj->s_mana > 0)
		    {
				if(hudside == 0)
				{
					settexture("data/themes/hud/hud_armour.png");
					glBegin(GL_QUADS);
					glTexCoord2f(0.0f,    0.0f);   glVertex2f(130,    h*1.2f-62);
					glTexCoord2f(cas,      0.0f);   glVertex2f(130+cah, h*1.2f-62);
					glTexCoord2f(cas,      1.0f);   glVertex2f(130+cah, h*1.2f-44);
					glTexCoord2f(0.0f,    1.0f);   glVertex2f(130,    h*1.2f-44);
					glEnd();
				}
				else if(hudside == 1)
				{
					settexture("data/themes/hud/hud_armour_2.png");
					glBegin(GL_QUADS);
					glTexCoord2f(0.0f,    0.0f);    glVertex2f(w*1.2f-130+cah,  h*1.2f-62);
					glTexCoord2f(cas,      0.0f);   glVertex2f(w*1.2f-130,      h*1.2f-62);
					glTexCoord2f(cas,      1.0f);   glVertex2f(w*1.2f-130,      h*1.2f-44);
					glTexCoord2f(0.0f,    1.0f);    glVertex2f(w*1.2f-130+cah,  h*1.2f-44);
					glEnd();
				}
		    }

			if(hudside == 0)
			{
				settexture("data/themes/hud/hud_left.png");
				glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);   glVertex2f(0,   h*1.2f-207);
				glTexCoord2f(1.0f, 0.0f);   glVertex2f(539, h*1.2f-207);
				glTexCoord2f(1.0f, 1.0f);   glVertex2f(539, h*1.2f);
				glTexCoord2f(0.0f, 1.0f);   glVertex2f(0,   h*1.2f);
				glEnd();
			}
			else if(hudside == 1)
			{
				settexture("data/themes/hud/hud_left_2.png");
				glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);   glVertex2f(w*1.2f-539, h*1.2f-207);
				glTexCoord2f(1.0f, 0.0f);   glVertex2f(w*1.2f,     h*1.2f-207);
				glTexCoord2f(1.0f, 1.0f);   glVertex2f(w*1.2f,     h*1.2f);
				glTexCoord2f(0.0f, 1.0f);   glVertex2f(w*1.2f-539, h*1.2f);
				glEnd();
			}


			if(hudside == 0)
			{
				settexture("data/themes/hud/hud_right.png");
				glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);   glVertex2f(w*1.2f-135, h*1.2f-207);
				glTexCoord2f(1.0f, 0.0f);   glVertex2f(w*1.2f,     h*1.2f-207);
				glTexCoord2f(1.0f, 1.0f);   glVertex2f(w*1.2f,     h*1.2f);
				glTexCoord2f(0.0f, 1.0f);   glVertex2f(w*1.2f-135, h*1.2f);
				glEnd();
			}
			else if(hudside == 1)
			{
				settexture("data/themes/hud/hud_right_2.png");
				glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);   glVertex2f(0,     h*1.2f-207);
				glTexCoord2f(1.0f, 0.0f);   glVertex2f(135,   h*1.2f-207);
				glTexCoord2f(1.0f, 1.0f);   glVertex2f(135,   h*1.2f);
				glTexCoord2f(0.0f, 1.0f);   glVertex2f(0,     h*1.2f);
				glEnd();
			}

	        glPopMatrix();

        glPushMatrix();

        glScalef(1/4.0f, 1/4.0f, 1);

		glPopMatrix();

		glPushMatrix();
        glScalef(h/1800.0f, h/1800.0f, 1);
        glPopMatrix();

		}
		else	//end of hudtheme == 1
		{    
		    glPushMatrix();
		    glScalef(0.5f, 0.5f, 1);
		    draw_textf("using: %s", 636*2, h*2-256+149, selected ? selected->name : "(none)");       // temp     
		    glPopMatrix();

		    settexture("packages/hud/hud_rpg.png", 3);
		    
		    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		    quad(0, h-128, 768, 128);        
		    settexture("packages/hud/hbar.png", 3);
		    glColor4f(1, 0, 0, 0.5f);
		    quad(130, h-128+57, 193*playerobj->s_hp/playerobj->eff_maxhp(), 17);        
		    glColor4f(0, 0, 1, 0.5f);
		    quad(130, h-128+87, 193*playerobj->s_mana/playerobj->eff_maxmana(), 17);        
        }
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        int s = 1800/4, x = 1800*w/h - s - s/10, y = s/10, minimapalpha = 1;
        float minradarscale = 1, maxradarscale = 2;
        glColor4f(1, 1, 1, minimapalpha);
        if(minimapalpha >= 1) glDisable(GL_BLEND);
        bindminimap();
        

        
		vec pos = vec(player1->o).sub(minimapcenter).mul(minimapscale).add(0.5f), dir;
        vecfromyawpitch(camera1->yaw, 0, 1, 0, dir);
        float scale = clamp(max(minimapradius.x, minimapradius.y)/3, float(minradarscale), float(maxradarscale));
        glBegin(GL_TRIANGLE_FAN);
        loopi(16)
        {
            vec tc = vec(dir).rotate_around_z(i/16.0f*2*M_PI);
            glTexCoord2f(pos.x + tc.x*scale*minimapscale.x, pos.y + tc.y*scale*minimapscale.y);
            vec v = vec(0, -1, 0).rotate_around_z(i/16.0f*2*M_PI);
            glVertex2f(x + 0.5f*s*(1.0f + v.x), y + 0.5f*s*(1.0f + v.y));
        }
        glEnd();
        
        if(minimapalpha >= 1) glEnable(GL_BLEND);
        glColor3f(1, 1, 1);
        /*
        float margin = 0.04f, roffset = s*margin, rsize = s + 2*roffset;
        if(game::radartheme != 1)
        	settexture("packages/hud/radar.png", 3);
        else
        	settexture("data/themes/hud/radar.png", 3);
        drawradar(x - roffset, y - roffset, rsize);

		//drawblip(player1, x, y, s, 1, true, scale);*/
		
        
    }
   
    int clipconsole(int w, int h)
    {
        return 0;
    }
 
    void preload() {}

    void renderavatar()
    {
    }

    bool canjump() { return true; }
    bool allowmove(physent *d) { return d->type!=ENT_AI || ((rpgent *)d)->allowmove(); }
    void doattack(bool on) { player1->attacking = on; }
    dynent *iterdynents(int i) { return i ? objects[i-1]->ent : player1; }
    int numdynents() { return objects.length()+1; }

    void rendergame(bool mainpass)
    {
        if(isthirdperson()) renderclient(player1, "ogro", NULL, 0, ANIM_ATTACK1, 300, player1->lastaction, player1->lastpain);
        renderobjects();
    }
    
    void g3d_gamemenus() { g3d_npcmenus(); menu.render(); }

    const char *defaultcrosshair(int index)
    {
        return "data/crosshair.png";
    }

    int selectcrosshair(float &r, float &g, float &b)
    {
        if(player1->state==CS_DEAD) return -1;
        return 0;
    }

    bool serverinfostartcolumn(g3d_gui *g, int i) { return false; }
    void serverinfoendcolumn(g3d_gui *g, int i) {}
    bool serverinfoentry(g3d_gui *g, int i, const char *name, int port, const char *sdesc, const char *map, int ping, const vector<int> &attr, int np) { return false; }

    void setupcamera()
    {
    }

    bool detachcamera()
    {
        return player1->state==CS_DEAD;
    }

    bool collidecamera()
    {
        return player1->state!=CS_EDITING;
    }

    void lighteffects(dynent *e, vec &color, vec &dir)
    {
    }

    void adddynlights()
    {
    }

    void dynlighttrack(physent *owner, vec &o)
    {
    }

    void particletrack(physent *owner, vec &o, vec &d)
    {
    }

    void writegamedata(vector<char> &extras) {}
    void readgamedata (vector<char> &extras) {}

    const char *gameident()     { return "rpg"; }
    const char *savedconfig()   { return "rpg_config.cfg"; }
    const char *restoreconfig() { return "rpg_restore.cfg"; }
    const char *defaultconfig() { return "data/defaults.cfg"; }
    const char *autoexec()      { return "rpg_autoexec.cfg"; }
    const char *savedservers()  { return NULL; }

    void loadconfigs() {}

    void parseoptions(vector<const char *> &args) {}
    void connectattempt(const char *name, const char *password, const ENetAddress &address) {}
    void connectfail() {}
    void parsepacketclient(int chan, packetbuf &p) {}
    bool allowedittoggle() {
    	return true;
    }
    void edittoggled(bool on) {}
    void writeclientinfo(stream *f) {}
    void toserver(char *text) {}
    bool ispaused() { return false; }
    
    void process_data(int cn, rpgent *d, ucharbuf &p)
    {
		static char text[MAXTRANS];
        int type;
        bool mapchanged = false, initmap = false;

        while(p.remaining()) switch(type = getint(p))
        {
/*        	case N_SERVER_MESSAGE:
        	{
				getstring(text, p);
				conoutf(CON_GAMEINFO, "%s", text);
        	}
        	case N_TEXT:
        	{
        		if(getint(p) == 1) //fake
        		{
		    		//fake message
		    		getstring(name, p);
		    		getstring(text, p);
		    		if(getint(p) == 0) //teamchat 0=no 1 = yes (however we havn't got teams
		    			r_sais(name, text);
		    		else
		    			r_sais_team(name, text);
        		}
        		else
        		{
        			getstring(text, p);
        			filtertext(text, text);
        			
        			if(d->state!=CS_DEAD && d->state!=CS_SPECTATOR)
        				particle_textcopy(d->abovehead(), text, PART_TEXT, 2000, 0x32FF64, 4.0f, -8);
        			
//        			conoutf(CON_CHAT, "%s:\f0 %s", colorname(d), text);
				}
        		break;
        	}
         
            case N_PONG:
                addmsg(N_CLIENTPING, "i", player1->ping = (player1->ping*5+lastmillis-getint(p))/6);
                break;

			//ping of player
            case N_CLIENTPING:
                if(!d) return;
                d->ping = getint(p);
                break;

        */
        }
	}
}

namespace server
{
    void *newclientinfo() { return NULL; }
    void deleteclientinfo(void *ci) {}
    void serverinit() {}
    int reserveclients() { return 0; }
    int numchannels() { return 0; }
    void clientdisconnect(int n) {}
    int clientconnect(int n, uint ip) { return DISC_NONE; }
    void localdisconnect(int n) {}
    void localconnect(int n) {}
    bool allowbroadcast(int n) { return true; }
    void recordpacket(int chan, void *data, int len) {}
    void parsepacket(int sender, int chan, packetbuf &p) {}
    void sendservmsg(const char *s) {}
    bool sendpackets(bool force) { return false; }
    void serverinforeply(ucharbuf &req, ucharbuf &p) {}
    void serverupdate() {}
    bool servercompatible(char *name, char *sdec, char *map, int ping, const vector<int> &attr, int np) { return true; }

    int serverinfoport(int servport) { return 0; }
    int serverport(int infoport) { return 0; }
    const char *defaultmaster() { return ""; }
    int masterport() { return 0; }
    int laninfoport() { return 0; }
    void processmasterinput(const char *cmd, int cmdlen, const char *args) {}
    bool ispaused() { return false; }
}
