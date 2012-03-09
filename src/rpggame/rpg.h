#include "cube.h"

// console message types

enum
{
    CON_CHAT       = 1<<8,
    CON_TEAMCHAT   = 1<<9,
    CON_GAMEINFO   = 1<<10,
    CON_FRAG_SELF  = 1<<11,
    CON_FRAG_OTHER = 1<<12,
    CON_TEAMKILL   = 1<<13
};
enum
{
    N_CONNECT = 0, N_SERVINFO, N_WELCOME, N_INITCLIENT, N_POS, N_TEXT, N_SOUND, N_CDIS,
    N_SHOOT, N_EXPLODE, N_SUICIDE,
    N_DIED, N_DAMAGE, N_HITPUSH, N_SHOTFX, N_EXPLODEFX,
    N_TRYSPAWN, N_SPAWNSTATE, N_SPAWN, N_FORCEDEATH,
    N_GUNSELECT, N_TAUNT,
    N_MAPCHANGE, N_MAPVOTE, N_ITEMSPAWN, N_ITEMPICKUP, N_ITEMACC, N_TELEPORT, N_JUMPPAD,
    N_PING, N_PONG, N_CLIENTPING,
    N_TIMEUP, N_MAPRELOAD, N_FORCEINTERMISSION,
    N_SERVMSG, N_ITEMLIST, N_RESUME,
    N_EDITMODE, N_EDITENT, N_EDITF, N_EDITT, N_EDITM, N_FLIP, N_COPY, N_PASTE, N_ROTATE, N_REPLACE, N_DELCUBE, N_REMIP, N_NEWMAP, N_GETMAP, N_SENDMAP, N_CLIPBOARD, N_EDITVAR,
    N_MASTERMODE, N_KICK, N_CLEARBANS, N_CURRENTMASTER, N_SPECTATOR, N_SETMASTER, N_SETTEAM,
    N_BASES, N_BASEINFO, N_BASESCORE, N_REPAMMO, N_BASEREGEN, N_ANNOUNCE,
    N_LISTDEMOS, N_SENDDEMOLIST, N_GETDEMO, N_SENDDEMO,
    N_DEMOPLAYBACK, N_RECORDDEMO, N_STOPDEMO, N_CLEARDEMOS,
    N_TAKEFLAG, N_RETURNFLAG, N_RESETFLAG, N_INVISFLAG, N_TRYDROPFLAG, N_DROPFLAG, N_SCOREFLAG, N_INITFLAGS,
    N_SAYTEAM,
    N_CLIENT,
    N_AUTHTRY, N_AUTHCHAL, N_AUTHANS, N_REQAUTH,
    N_PAUSEGAME,
    N_ADDBOT, N_DELBOT, N_INITAI, N_FROMAI, N_BOTLIMIT, N_BOTBALANCE,
    N_MAPCRC, N_CHECKMAPS,
    N_SWITCHNAME, N_SWITCHMODEL, N_SWITCHTEAM,
    N_SERVER_MESSAGE,
    NUMSV
};

enum
{
	STATE_ALIVE = 0,
	STATE_DEATH
};
enum
{
	AISTATE_SIGHT,
	AISTATE_ATTACKING,
	AISTATE_RUNNING,
	AISTATE_SLEEPING, //;)
	AISTATE_PATROL,
	AISTATE_RUN,
	AISTATE_FIND_FOOD //for animal ai
};

enum { ETR_SPAWN = ET_GAMESPECIFIC, ETR_SUN};

enum { DEFAULT_ROTSPEED = 200 };

static const int SPAWNNAMELEN = 64;

struct rpgentity : extentity
{
    char name[SPAWNNAMELEN];

    rpgentity() { memset(name, 0, SPAWNNAMELEN); }
};

namespace entities
{
    extern vector<extentity *> ents;
    extern void startmap();
};

namespace game
{
	struct rpgent;
    struct rpgobj;
    struct rpgaction;
    
    struct rpg_player;
    struct rpg_item;
    struct rpg_position;
    extern vector<rpg_player *> players;
    extern vector<rpg_item *> items;
    struct rpg_position
    {
    	int x;
    	int y;
    	int z;
    	float yaw;//TODO:remove
    	
    	rpg_position() : x(0), y(0), z(0), yaw(0) {};
    	rpg_position(int x_, int y_, int z_) : x(0), y(0), z(0), yaw(0)
    	{
    		x = x_;
    		y = y_;
    		z = z_;
    	};
    };
    struct rpg_item : dynent
    {
    	float alpha;
    	
    	//position of the player / ai / weapon
    	rpg_position position;
    	
    	bool picked_up;
    	rpg_player *owner;
    	
    	rpg_item() : alpha(0), picked_up(false), owner(NULL) {};
    	rpg_item(rpg_position position_) : alpha(0), picked_up(false), owner(NULL) { position = position_; };
    };
    struct rpg_weapon : rpg_item
    {
    	bool damageble;
    	int damage;
    	
    	bool unlimited_ammo;
    	int ammo;
    	int max_ammo;
    	
    	bool regain_ammo;
    	int regain_ammo_speed;
    	
    	float range;
    	
    	bool limit_usage;
    	int max_usage;
    	
    	int usage;
    	
    	bool regain_usage;
    	int regain_useage_speed;
    	
    	bool magic;
    	int usage_cost;
    	
    	rpg_weapon() : damageble(false), damage(0), unlimited_ammo(false), ammo(0), max_ammo(0), regain_ammo(false), regain_ammo_speed(0), range(100), limit_usage(false), max_usage(0), usage(0), regain_usage(0), regain_useage_speed(0), magic(false), usage_cost(0) {};

		//fist
    	rpg_weapon(bool is_fist) : damageble(false), damage(0), unlimited_ammo(true), ammo(0), max_ammo(0), regain_ammo(false), regain_ammo_speed(0), range(10), limit_usage(false), max_usage(0), usage(0), regain_usage(0), regain_useage_speed(0), magic(false), usage_cost(0) {};
    	
    	void changeweaponsound() {};
    };
    
    struct rpg_player : rpg_item
    {
      	//the speed of THIS ai / player
    	int move_speed;
    	
    	bool show_in_stats;
    	
    	//TODO:remove
    	float invisible;
    	
    	bool unlimited_health;   	
    	int player_model;
    	int maxhealth;
    	int health;
    	
    	vector<rpg_item> *inventory;
    	vector<rpg_weapon> *weapons; //0 is fist (weapons that show up in the hud and can be changed with the scrollwheel.
    	int current_weapon;
    	
    	int lastaction, lastpain;
    	bool attacking;
    	
 		//old
 		vec mppos, mpdir; // TODO: remove
 		
 		vector<rpg_player> *enemies;
 		
 		float targetyaw, rotation_speed;
 		
 		float yaw;
    
		//enum { ROTSPEED = 200 };
		rpg_player() : move_speed(40), show_in_stats(false), unlimited_health(false), player_model(0), maxhealth(100), health(100), current_weapon(0), lastaction(0), lastpain(0), attacking(false), yaw(0)  {
//			rpg_weapon fist (true);
//			weapons.add(&fist);
		};

		float relyaw(vec &t) { return -(float)atan2(t.x-position.x, t.y-position.y)/RAD; };
		float relyaw(rpg_position &t) { return -(float)atan2(t.x-position.x, t.y-position.y)/RAD; };
		
		void normalize_yaw(float angle)
		{
			while(yaw<angle-180.0f) yaw += 360.0f;
			while(yaw>angle+180.0f) yaw -= 360.0f;
		}
		void updateplayer ()
		{
		
		}

		void tryattackobj(rpgobj &eo, rpgobj &weapon);
		void tryattack(vec &lookatpos, rpgobj &weapon);
		void updateprojectile();
		bool allowmove();

		void transition(int _state, int _moving, int n); 
		void gotoyaw(float yaw, int s, int m, int t);
		void gotopos(vec &pos, int s, int m, int t);
		void goroam();
		void stareorroam();
		
		void update(float playerdist);
    };
    
    struct rpg_ai : rpg_player //ai extends the player
    {
//    	const bool is_ai = true;
    	
    	//field of vieuw of the current ai
    	float fov;

    	//regenerate health rate
    	float regain_rate;
    	
    	
    	//move to on next update
    	rpg_position move_to;
    	
    	bool attacking;
    	
    	//vector<rpg_event_listeners *> listeners; //event listeners for this ai
    	
    	//all weapons currently owning
    	vector<rpg_weapon> weapons; //0 is reserved as slap
    	//the id of the current weapon using.
    	int current_weapon;
    	int state;
    	int aistate;

    	rpg_player &enemy_sighted;
    	
    	int rotospeed; //what does this exacely do?
    	
    	//constructor
//    	rpg_ai() {is_ai = true; enemy_sighted = NULL};
    	
		void init ()
		{
			conoutf("\fs\f0AI\fl assigned to you");
		};
		
    	bool tryattackobj(rpg_player &enemy)
    	{
    		if (state != STATE_ALIVE)
    			return false; //ai is death, will be removed
    		
    		if (enemy.invisible > 100) //completely invisible
    			return false;
    		
    		if (weapons[current_weapon].ammo == 0) //weapon empty (make ammo -1 for unlimited ammo)
    		{
    			float distance = relyaw(enemy.position);
   		
    			int bestweapon = current_weapon;
    			int bestscore = 0;
    			//find good new weapon
    			loopv(weapons)
    			{
    				if(weapons[i].range > distance)
    					continue;

    				int score = 0;
    				
    				//find difference between distance and range
    				score -= distance - weapons[i].range;
    				score += weapons[i].damage;
    				
    				//no teload => better score
    				if( i != current_weapon )
    					score --;
    				else
    					score ++;
    				
    				//more ammo => better score (no need to reload anymore soon)
    				score += weapons[current_weapon].ammo;
    				
    				//not the best weapon
    				if(bestscore > score)
    				{
//    					delete (*score);//needed?
    				}
    				else
    				{
    					bestweapon = i;
    					bestscore = score;
//    					delete (*score); // needed?
    				}
    				
    			}
    			
    			
    			weapons[current_weapon].changeweaponsound();
    			current_weapon = bestweapon;
    			//changeweaponanim();
    			return false;
    		}
			
			float distance = 1e10f;
			float rotation = 0.0; // 2d rotation to the enemy
			bool faced_to_enemy = false;
			
			//find distance and direction
			
			if (enemy.invisible > 0)
			{
				rotation += enemy.invisible; //harder to see
				distance += enemy.invisible; //harder to see
			}
			
			if(rotation >= fov)
				return false; //enemy not sighted
			
			if(!faced_to_enemy)
			{
				//rotate a bit
				
				//enemy_sighted =& enemy; //detected, just not attacking yet // WHY doesn't this work?
				return true; //see up
			}			
			
			if(weapons[current_weapon].range > 0 && weapons[current_weapon].range > distance)//check range
			{
				//get closer to enemy
				// += move_speed //let some monseters/ai move faster than other ones
				
				//enemy_sighted =& enemy;// WHY doesn't this work?
				return true;//out of range, but detected
			}
			//shoot_at(&enemy);//TODO
			return true;
		}
				
		bool allowmove()
		{
			if(rotation_speed && position.yaw!=move_to.yaw)
			{
		        float diff = rotation_speed*curtime/1000.0f, maxdiff = fabs(move_to.yaw-position.yaw);
		        if(diff >= maxdiff)
		        {
		            position.yaw = move_to.yaw;
		            rotation_speed = 0;
		        }
		        else move_to.yaw += (move_to.yaw>position.yaw ? 1 : -1) * min(diff, maxdiff);
//		        normalize_yaw(move_to.yaw);//TODO:create this function, what does this do? :D
        	}
        	
        	return true;
        }

		void transition(int _state, int _moving, int n) //what does this exacely do?
		{
        	aistate = _state;
//        	move = _moving; waht does these things do
//        	trigger = lastmillis+n;
        }

		void gotoyaw(float yaw, int s, int m, int t)
		{
        	move_to.yaw = yaw;
        	rotation_speed = DEFAULT_ROTSPEED;
        	transition(s, m, t);
    	}
    	
    	void gotopos(rpg_position &pos, int s, int m, int t)
    	{
    		gotoyaw(relyaw(pos), s, m, t);
    	}

		void move_to_position (int x, int y, int z)	
		{
			
		}
		void face_to()
		{
		}
		
		void showhealth()
		{
		
		}
		
		void update ()
		{
			if(maxhealth == -1)
				unlimited_health = true;
			
			if(!unlimited_health)
			{
				if (health < maxhealth) //damaged
					showhealth();
			
				health = health + regain_rate; //update health
			
				//max is really max
				if(health > maxhealth)
					health = maxhealth;
			
				if (state == STATE_DEATH)
				{
					//stopmoving(); //stop moving
					remove(true);
					return;
				};
			
				if (state == AISTATE_SLEEPING)
				{}
				else if (state == AISTATE_RUN)
				{
					//run from player and try to survive
				}
			}
			
			if (tryattackobj(enemy_sighted) == true) //object is still close
			{
				
			}
			else
			{
			
				//check distance near players
				loopv(players)
				{
//					if (tryattackobj(players[i]) == true) // try to attack // TODO:FIX
//					{
//						enemy_sighted = &players[i];
//						break; //enemy sighted
//					}
				}
			}
				
			if(!unlimited_health && (health < maxhealth/100*5)) //has bad health
			{
				if(aistate == AISTATE_ATTACKING)
					aistate = AISTATE_RUN;
				else if (false) //no persons verry close
					aistate = AISTATE_SLEEPING; //sleep wil regain health faster
			}
				
			//move_to(position); //TODO:get this working
			face_to();
			position = move_to; // move to new position
		}
		void remove (bool slow = false)
		{
		
		}
    };

    struct rpgquest
    {
        rpgquest *next;
        const char *npc;
        const char *questline;
        bool completed;

        rpgquest(rpgquest *_n, const char *_npc, const char *_ql) : next(_n), npc(_npc), questline(_ql), completed(false) {}
    };
    
	extern void r_sais(const char *name, const char *text);
	extern void r_sais_team(const char *name, const char *text);
	
    extern rpgent *player1;
    extern vector<rpgobj *> objects, stack;
    extern rpgobj *pointingat;
    extern rpgobj *playerobj;
    extern rpgobj *selected;
    extern rpgquest *quest;
    extern rpgquest *currentquest;

    extern void pushobj(rpgobj *o);
    extern void popobj();
    extern void spawn(char *name);
    extern void placeinworld(vec &pos, float yaw);
    extern void spawnfroment(rpgentity &e);
    extern void removefromworld(rpgobj *worldobj);
    extern void removefromsystem(rpgobj *o);
    extern void taken(rpgobj *worldobj, rpgobj *newowner);
    extern void clearworld();
    extern void updateobjects();
    extern void renderobjects();
    extern void listquests(bool completed, g3d_gui &g);
    extern rpgquest *addquest(const char *questline, const char *npc);
    extern void g3d_npcmenus();
     
    extern float intersectdist;
    extern bool intersect(dynent *d, const vec &from, const vec &to, float &dist = intersectdist);

#include "stats.h"
#include "rpgent.h"
#include "rpgobj.h"
}

