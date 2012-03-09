#include "rpg.h"

namespace entities
{
    using namespace game;

    vector<extentity *> ents;

    vector<extentity *> &getents() { return ents; }

    bool mayattach(extentity &e) { return false; }
    bool attachent(extentity &e, extentity &a) { return false; }

    const char *entnameinfo(entity &e) { return ""; }
    const char *entname(int i)
    {
        static const char *entnames[] =
        {
            "none?", "light", "mapmodel", "playerstart", "envmap", "particles", "sound", "spotlight",
            "spawn", "sun" /*sun is light only then it depends on the current wether/ time of the day :) (and doesn't need calclight) => NOT implemented yet*/
        };
        return i>=0 && size_t(i)<sizeof(entnames)/sizeof(entnames[0]) ? entnames[i] : "";
    }

    extentity *newentity() { return new rpgentity; }
    void deleteentity(extentity *e) { delete (rpgentity *)e; }

    void clearents()
    {
        while(ents.length()) deleteentity((rpgentity *)ents.pop());
    }

    float dropheight(entity &e) { return e.type==ET_MAPMODEL ? 0 : 4; }

    int extraentinfosize() { return SPAWNNAMELEN; }
    void writeent(entity &e, char *buf) { memcpy(buf, ((rpgentity &)e).name, SPAWNNAMELEN); }
    void readent (entity &e, char *buf) 
    { 
        memcpy(((rpgentity &)e).name, buf, SPAWNNAMELEN); 
        int ver = getmapversion();
        if(ver <= 30) switch(e.type)
        {
            case ETR_SPAWN:
                e.attr1 = (int(e.attr1)+180)%360;
                break;
        }
    }

    void editent(int i, bool local)
    {
    }

    rpgentity *lastcreated = NULL;

    void fixentity(extentity &e)
    {
        lastcreated = (rpgentity *)&e;
        switch(e.type)
        {
            case ETR_SPAWN:
                e.attr1 = ((dynent *)player1)->yaw;
                break;
			case ETR_SUN:
				break;
        }
    }

    void entradius(extentity &e, bool color)
    {
        switch(e.type)
        {
            case ETR_SPAWN:
            {
                vec dir;
                vecfromyawpitch(e.attr1, 0, 1, 0, dir);
                renderentarrow(e, dir, 4);
                break;
            }
        }
    }

    bool printent(extentity &e, char *buf)
    {
        return false;
    }

    void startmap()
    {
        lastcreated = NULL;
        loopv(ents) switch(ents[i]->type)
        {
        	case ETR_SPAWN:
        		spawnfroment(*(rpgentity *)ents[i]);
        		break;
        	
			case ETR_SUN:
//				adddynlight(ents[i]->o, ents[i]->attr1, vec(255, 255, 255) );
				adddynlight(ents[i]->o, 50, vec(255, 255, 255) );
				break;
        }
    }

    ICOMMAND(spawnname, "s", (char *s), { if(lastcreated) { copystring(lastcreated->name, s, SPAWNNAMELEN); spawnfroment(*lastcreated); } }); 

    void animatemapmodel(const extentity &e, int &anim, int &basetime)
    {
    }

    const char *entmodel(const entity &e)
    {
        return NULL;
    }
}


