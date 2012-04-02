#include "game.h"

namespace game
{
	VARP(hudtheme, 0, 1, 1);
	VARP(radartheme, 0, 1, 1);

	VARP(_Graphox_autosorry, 0, 0, 1);
	VARP(_Graphox_autonp, 0, 0, 1);

	string sr_string = "Sorry for the teamkill %s";
	string np_string = "No problem %s";

	void switch_sr(const char *team)
    {
        if(team[0])
        {
            copystring(sr_string, team);
        }
        else conoutf("sr string = %s", sr_string);
    }
    ICOMMAND(_Graphox_sr, "s", (char *s), switch_sr(s));
    ICOMMAND(_Graphox_get_sr, "", (), result(sr_string));

  	void switch_np(const char *team)
    {
        if(team[0])
        {
            copystring(np_string, team);
        }
        else conoutf("np string = %s", np_string);
    }
    ICOMMAND(_Graphox_np, "s", (char *s), switch_np(s));
    ICOMMAND(_Graphox_get_np, "", (), result(np_string));

	enum { SETTING_TYPE_UNKNOWN, SETTING_TYPE_TEXT, SETTING_TYPE_INT, SETTING_TYPE_FLOAT };
	struct player_setting
	{
		const char *name;
		const char *s_value;
		int i_value;
		float f_value;

		int type;

		player_setting(const char *name_) : name(name_), type(SETTING_TYPE_UNKNOWN) {};
		player_setting(const char *name_, const char *value_) : name(name_), s_value(value_), type(SETTING_TYPE_TEXT) {};
		player_setting(const char *name_, int value_) : name(name_), i_value(value_), type(SETTING_TYPE_INT) {};
		player_setting(const char *name_, float value_) : name(name_), f_value(value_), type(SETTING_TYPE_FLOAT) {};
	};
	struct local_player : fpsent
	{
		int following, followdir;
		int lasthit;
		bool thirdperson;
		vector<player_setting> settings;

		void init() //use init instead of constructor. will might overwrite the fpsent constructor otherwise
		{
			following = -1;
			following = 0;
			lasthit = 0;
			thirdperson = false;
		}

		void taunt()
		{
			if(this->state!=CS_ALIVE || this->physstate<PHYS_SLOPE) return;
			if(lastmillis-this->lasttaunt<1000) return;

			this->lasttaunt = lastmillis;
			addmsg(N_TAUNT, "rc", player1);
		};

		void follow(char *arg)
		{
			if(arg[0] ? this->state==CS_SPECTATOR : this->following>=0)
			{
				following = arg[0] ? parseplayer(arg) : -1;
            	if(following==this->clientnum) following = -1;
            	followdir = 0;
				conoutf("follow %s", following>=0 ? "on" : "off");
			};
		};

		void nextfollow(int dir)
		{
			if(this->state!=CS_SPECTATOR || clients.empty())
			{
				this->stopfollowing();
				return;
			};

			int cur = following >= 0 ? following : (dir < 0 ? clients.length() - 1 : 0);

			loopv(clients)
			{
				cur = (cur + dir + clients.length()) % clients.length();

				if(clients[cur] && clients[cur]->state!=CS_SPECTATOR)
				{
					if(following<0) conoutf("follow on");

					following = cur;
					followdir = dir;

					return;
				};
			};

			stopfollowing();
    	};

    	void stopfollowing()
    	{
    		if(following<0) return;

    		following = -1;
    		followdir = 0;
			conoutf("follow off");
		}

		fpsent *followingplayer()
		{
			if(this->state!=CS_SPECTATOR || following<0) return NULL;

			fpsent *target = getclient(following);

			if(target && target->state!=CS_SPECTATOR) return target;

			return NULL;
		}

		fpsent *hudplayer()
		{
			if(thirdperson) return this;

			fpsent *target = followingplayer();

			return target ? target : this;
		};

		void setupcamera()
		{
			fpsent *target = followingplayer();

			if(target)
			{
				this->yaw = target->yaw;
				this->pitch = target->state==CS_DEAD ? 0 : target->pitch;
				this->o = target->o;
				this->resetinterp();
			}
		};

		bool detachcamera()
		{
			fpsent *d = hudplayer();
			return d->state==CS_DEAD;
		}

		bool collidecamera()
		{
			switch(this->state)
			{
				case CS_EDITING: return false;
				case CS_SPECTATOR: return followingplayer()!=NULL;
			}

			return true;
		};

		void respawnself()
		{
			if(paused || ispaused()) return;

			if(m_mp(gamemode))
			{
				if(this->respawned!=this->lifesequence)
				{
					addmsg(N_TRYSPAWN, "rc", this);
					this->respawned = this->lifesequence;
				}
			}
			else
			{
				spawnplayer(this);
				showscores(false);

				lasthit = 0;

				if(cmode) cmode->respawned(this);
			}
		};

		void update()
		{
			if(this->state==CS_DEAD)
			{
				if(this->ragdoll) moveragdoll(this);
				else if(lastmillis-this->lastpain<2000)
				{
					this->move = this->strafe = 0;
					moveplayer(this, 10, true);
				}
			}
			else if(!intermission)
			{
				if(this->ragdoll) cleanragdoll(this);

				moveplayer(this, 10, true);
				swayhudgun(curtime);

				entities::checkitems(this);

				if(m_sp)
				{
//					if(slowmosp) checkslowmo();
					if(m_classicsp) entities::checktriggers();
				}
				else if(cmode) cmode->checkitems(this);
			};
		};
	};

	vector<local_player *> local_players; //our clients

    bool intermission = false;
    int maptime = 0, maprealtime = 0, maplimit = -1;
    int respawnent = -1;
    int lasthit = 0, lastspawnattempt = 0;

    int following = -1, followdir = 0;

    fpsent *player1 = NULL;         // our client
    vector<fpsent *> players;       // other clients
    int savedammo[NUMGUNS];

    bool clientoption(const char *arg) { return false; }

    void taunt()
    {
//    	if (local_players.size() > 1)
//    	{
  //  		conoutf("Taunt not supported in splitscreen");
//    		return;
//    	}
        if(player1->state!=CS_ALIVE || player1->physstate<PHYS_SLOPE) return;
        if(lastmillis-player1->lasttaunt<1000) return;
        player1->lasttaunt = lastmillis;
        addmsg(N_TAUNT, "rc", player1);
    }
    COMMAND(taunt, "");

    ICOMMAND(getfollow, "", (),
    {
        fpsent *f = followingplayer();
        intret(f ? f->clientnum : -1);
    });

	void follow(char *arg)
    {
        if(arg[0] ? player1->state==CS_SPECTATOR : following>=0)
        {
            following = arg[0] ? parseplayer(arg) : -1;
            if(following==player1->clientnum) following = -1;
            followdir = 0;
            conoutf("follow %s", following>=0 ? "on" : "off");
        }
	}
    COMMAND(follow, "s");

    void nextfollow(int dir)
    {
        if(player1->state!=CS_SPECTATOR || clients.empty())
        {
            stopfollowing();
            return;
        }
        int cur = following >= 0 ? following : (dir < 0 ? clients.length() - 1 : 0);
        loopv(clients)
        {
            cur = (cur + dir + clients.length()) % clients.length();
            if(clients[cur] && clients[cur]->state!=CS_SPECTATOR)
            {
                if(following<0) conoutf("follow on");
                following = cur;
                followdir = dir;
                return;
            }
        }
        stopfollowing();
    }
    ICOMMAND(nextfollow, "i", (int *dir), nextfollow(*dir < 0 ? -1 : 1));


    const char *getclientmap() { return clientmap; }

    void resetgamestate()
    {
        if(m_classicsp)
        {
            clearmovables();
            clearmonsters();                 // all monsters back at their spawns for editing
            entities::resettriggers();
        }
        clearprojectiles();
        clearbouncers();
    }

    fpsent *spawnstate(fpsent *d)              // reset player state not persistent accross spawns
    {
        d->respawn();
        d->spawnstate(gamemode);
        return d;
    }

    void respawnself()
    {
        if(paused || ispaused()) return;
        if(m_mp(gamemode))
        {
            if(player1->respawned!=player1->lifesequence)
            {
                addmsg(N_TRYSPAWN, "rc", player1);
                player1->respawned = player1->lifesequence;
            }
        }
        else
        {
            spawnplayer(player1);
            showscores(false);
            lasthit = 0;
            if(cmode) cmode->respawned(player1);
        }
    }

    fpsent *pointatplayer()
    {
        loopv(players) if(players[i] != player1 && intersect(players[i], player1->o, worldpos)) return players[i];
        return NULL;
    }

    void stopfollowing()
    {
        if(following<0) return;
        following = -1;
        followdir = 0;
        conoutf("follow off");
    }

    fpsent *followingplayer()
    {
        if(player1->state!=CS_SPECTATOR || following<0) return NULL;
        fpsent *target = getclient(following);
        if(target && target->state!=CS_SPECTATOR) return target;
        return NULL;
    }

    fpsent *hudplayer()
    {
        if(thirdperson) return player1;
        fpsent *target = followingplayer();
        return target ? target : player1;
    }

    void setupcamera()
    {
        fpsent *target = followingplayer();
        if(target)
        {
            player1->yaw = target->yaw;
            player1->pitch = target->state==CS_DEAD ? 0 : target->pitch;
            player1->o = target->o;
            player1->resetinterp();
        }
    }

    bool detachcamera()
    {
        fpsent *d = hudplayer();
        return d->state==CS_DEAD;
    }

    bool collidecamera()
    {
        switch(player1->state)
        {
            case CS_EDITING: return false;
            case CS_SPECTATOR: return followingplayer()!=NULL;
        }
        return true;
    }

    VARP(smoothmove, 0, 75, 100);
    VARP(smoothdist, 0, 32, 64);

    void predictplayer(fpsent *d, bool move)
    {
        d->o = d->newpos;
        d->yaw = d->newyaw;
        d->pitch = d->newpitch;
        if(move)
        {
            moveplayer(d, 1, false);
            d->newpos = d->o;
        }
        float k = 1.0f - float(lastmillis - d->smoothmillis)/smoothmove;
        if(k>0)
        {
            d->o.add(vec(d->deltapos).mul(k));
            d->yaw += d->deltayaw*k;
            if(d->yaw<0) d->yaw += 360;
            else if(d->yaw>=360) d->yaw -= 360;
            d->pitch += d->deltapitch*k;
        }
    }

    void otherplayers(int curtime)
    {
        loopv(players)
        {
            fpsent *d = players[i];
            if(d == player1 || d->ai) continue;

            if(d->state==CS_ALIVE)
            {
                if(lastmillis - d->lastaction >= d->gunwait) d->gunwait = 0;
                if(d->quadmillis) entities::checkquad(curtime, d);
            }
            else if(d->state==CS_DEAD && d->ragdoll) moveragdoll(d);

            const int lagtime = lastmillis-d->lastupdate;
            if(!lagtime || intermission) continue;
            else if(lagtime>1000 && d->state==CS_ALIVE)
            {
                d->state = CS_LAGGED;
                continue;
            }
            if(d->state==CS_ALIVE || d->state==CS_EDITING)
            {
                if(smoothmove && d->smoothmillis>0) predictplayer(d, true);
                else moveplayer(d, 1, false);
            }
            else if(d->state==CS_DEAD && !d->ragdoll && lastmillis-d->lastpain<2000) moveplayer(d, 1, true);
        }
    }

    VARFP(slowmosp, 0, 0, 1,
    {
        if(m_sp && !slowmosp) setvar("gamespeed", 100);
    });

    void checkslowmo()
    {
        static int lastslowmohealth = 0;
        setvar("gamespeed", intermission ? 100 : clamp(player1->health, 25, 200), true, false);
        if(player1->health<player1->maxhealth && lastmillis-max(maptime, lastslowmohealth)>player1->health*player1->health/2)
        {
            lastslowmohealth = lastmillis;
            player1->health++;
        }
    }

    void updateworld()        // main game update loop
    {
        if(!maptime) { maptime = lastmillis; maprealtime = totalmillis; return; }
        if(!curtime) { gets2c(); if(player1->clientnum>=0) c2sinfo(); return; }

        physicsframe();
        ai::navigate();
        entities::checkquad(curtime, player1);
        updateweapons(curtime);
        otherplayers(curtime);
        ai::update();
        moveragdolls();
        gets2c();
        updatemovables(curtime);
        updatemonsters(curtime);
        if(player1->state==CS_DEAD)
        {
            if(player1->ragdoll) moveragdoll(player1);
            else if(lastmillis-player1->lastpain<2000)
            {
                player1->move = player1->strafe = 0;
                moveplayer(player1, 10, true);
            }
        }
        else if(!intermission)
        {
            if(player1->ragdoll) cleanragdoll(player1);
            moveplayer(player1, 10, true);
            swayhudgun(curtime);
            entities::checkitems(player1);
            if(m_sp)
            {
                if(slowmosp) checkslowmo();
                if(m_classicsp) entities::checktriggers();
            }
            else if(cmode) cmode->checkitems(player1);
        }
        if(player1->clientnum>=0) c2sinfo();   // do this last, to reduce the effective frame lag
    }

    void spawnplayer(fpsent *d)   // place at random spawn
    {
        if(cmode) cmode->pickspawn(d);
        else findplayerspawn(d, d==player1 && respawnent>=0 ? respawnent : -1);
        spawnstate(d);
        if(d==player1)
        {
            if(editmode) d->state = CS_EDITING;
            else if(d->state != CS_SPECTATOR) d->state = CS_ALIVE;
        }
        else d->state = CS_ALIVE;
    }

    VARP(spawnwait, 0, 0, 1000);

    void respawn()
    {
        if(player1->state==CS_DEAD)
        {
            player1->attacking = false;
            int wait = cmode ? cmode->respawnwait(player1) : 0;
            if(wait>0)
            {
                lastspawnattempt = lastmillis;
                //conoutf(CON_GAMEINFO, "\f2you must wait %d second%s before respawn!", wait, wait!=1 ? "s" : "");
                return;
            }
            if(lastmillis < player1->lastpain + spawnwait) return;
            if(m_dmsp) { changemap(clientmap, gamemode); return; }    // if we die in SP we try the same map again
            respawnself();
            if(m_classicsp)
            {
                conoutf(CON_GAMEINFO, "\f2You wasted another life! The monsters stole your armour and some ammo...");
                loopi(NUMGUNS) if(i!=GUN_PISTOL && (player1->ammo[i] = savedammo[i]) > 5) player1->ammo[i] = max(player1->ammo[i]/3, 5);
            }
        }
    }

    // inputs

    void doattack(bool on)
    {
        if(intermission) return;
        if((player1->attacking = on)) respawn();
    }

    bool canjump()
    {
        if(!intermission) respawn();
        return player1->state!=CS_DEAD && !intermission;
    }

    bool allowmove(physent *d)
    {
        if(d->type!=ENT_PLAYER) return true;
        return !((fpsent *)d)->lasttaunt || lastmillis-((fpsent *)d)->lasttaunt>=1000;
    }

    VARP(hitsound, 0, 0, 1);

    void damaged(int damage, fpsent *d, fpsent *actor, bool local)
    {
        if(d->state!=CS_ALIVE || intermission) return;

        if(local) damage = d->dodamage(damage);
        else if(actor==player1) return;

        fpsent *h = hudplayer();
        if(h!=player1 && actor==h && d!=actor)
        {
            if(hitsound && lasthit != lastmillis) playsound(S_HIT);
            lasthit = lastmillis;
        }
        if(d==h)
        {
            damageblend(damage);
            damagecompass(damage, actor->o);
        }
        damageeffect(damage, d, d!=h);

		ai::damaged(d, actor);

        if(m_sp && slowmosp && d==player1 && d->health < 1) d->health = 1;

        if(d->health<=0) { if(local) killed(d, actor); }
        else if(d==h) playsound(S_PAIN6);
        else playsound(S_PAIN1+rnd(5), &d->o);
    }

    VARP(deathscore, 0, 1, 1);

    void deathstate(fpsent *d, bool restore)
    {
        d->state = CS_DEAD;
        d->lastpain = lastmillis;
        if(!restore) gibeffect(max(-d->health, 0), d->vel, d);
        if(d==player1)
        {
            if(deathscore) showscores(true);
            disablezoom();
            if(!restore) loopi(NUMGUNS) savedammo[i] = player1->ammo[i];
            d->attacking = false;
            if(!restore) d->deaths++;
            //d->pitch = 0;
            d->roll = 0;
            playsound(S_DIE1+rnd(2));
        }
        else
        {
            d->move = d->strafe = 0;
            d->resetinterp();
            d->smoothmillis = 0;
            playsound(S_DIE1+rnd(2), &d->o);
        }
    }

    int lastacc = 0;
    bool firstcheck = true;
    int oldacc = 0;

    void statsacc() //accuracy stats updating
    {
        int accuracy = player1->totaldamage*100/max(player1->totalshots, 1);    //current accuracy
        if((totalmillis >= lastacc+30000 && firstcheck) || (totalmillis >= lastacc+5000 && !firstcheck))  //First check - 30s to prevent getting 100% accuracy into avg
        {
            if(!game::stats[3] && accuracy)
            {
                game::stats[3] = accuracy;
                game::stats[11]++;
                oldacc = accuracy;
                firstcheck = false;
            }
            else if(game::stats[3] && accuracy && accuracy != oldacc)
            {
                game::stats[3] = ((game::stats[3]*game::stats[11])+accuracy) / (game::stats[11]+1);
                game::stats[11]++;
                oldacc = accuracy;
            }
            lastacc = totalmillis;
        }
    }

    int ffrag = 0;
    int when = 0;
    string who;
    string whospec;
    int ffrag2 = 0;
    int when2 = 0;
    string who3;
    string who5;
    string who6;
    int ismate = 0;
    int ismate2 = 0;

    void killed(fpsent *d, fpsent *actor)
    {
        fpsent *f = followingplayer();

    	d->deaths++;
        if(d->state==CS_EDITING)
        {
            d->editstate = CS_DEAD;
            if(d!=player1) d->resetinterp();
            return;
        }
        else if(d->state!=CS_ALIVE || intermission) return;

        fpsent *h = followingplayer();

        if(!h) h = player1;

        int contype = d==h || actor==h ? CON_FRAG_SELF : CON_FRAG_OTHER;

        string dname, aname;

        copystring(dname, d==player1 ? "you" : colorname(d));
        copystring(aname, actor==player1 ? "you" : colorname(actor));

        if(d==player1) game::stats[4]++;

        if(actor->type==ENT_AI)
        {
            conoutf(contype, "\f2%s got killed by %s", dname, aname);
            ffrag2 = 1;
            when2 = totalmillis;
            copystring(who3, aname);
            ismate = 0;
        }
        else if(d==actor || actor->type==ENT_INANIMATE)
        {
            if(d==player1)
            {
                conoutf(contype, "\f6you died");
                game::stats[18]++;
            }
            else conoutf(contype, "\f2%s died", dname);
        }
        else if(isteam(d->team, actor->team))
        {
            contype |= CON_TEAMKILL;

            if(actor==player1)
            {
            	conoutf(contype, "\f3you fragged a teammate (%s)", dname);

            	game::stats[26]++;

                if(_Graphox_autosorry == 1)
            	{
            		defformatstring(message)(sr_string, dname);

            		conoutf(CON_CHAT, "\f4| %s", message);
                    addmsg(N_TEXT, "rcs", player1, message);
                }

                ffrag = 1;
                when = totalmillis;
                copystring(who, dname);
                ismate2 = 1;
            }
            else if(d==player1)
            {
            	conoutf(contype, "\f3you got fragged by a teammate (%s)", aname);

            	game::stats[27]++;

                if(_Graphox_autonp == 1)
            	{
            		defformatstring(message)(np_string, aname);

            		conoutf(CON_CHAT, "\f4| %s", message);
                    addmsg(N_TEXT, "rcs", player1, message);
                }

                ffrag2 = 1;
                when2 = totalmillis;
                copystring(who3, aname);
                ismate = 1;
            }
            else conoutf(contype, "\f2%s fragged a teammate (%s)", aname, dname);
        }
        else
        {
            if(d==player1)
            {
                conoutf(contype, "\f6you got fragged by %s", aname);
                ffrag2 = 1;
                when2 = totalmillis;
                copystring(who3, aname);
                ismate = 0;
            }
            else
            {
                conoutf(contype, actor == player1 ? "\f0%s fragged %s" : "\f2%s fragged %s", aname, dname);

                if(actor==player1)
                {
                    crosshairbump();

                    ffrag = 1;
                    when = totalmillis;
                    copystring(who, dname);
                    ismate2 = 0;

                    if(actor==player1)
                    {
                        if(d->ai) game::stats[6]++;
                        if(!d->ai) game::stats[8]++;
                        if(player1->gunselect == 0) game::stats[17]++;
                        if(!strcmp(d->name, "unnamed")) game::stats[7]++;
                    }
                }
            }
        }
        defformatstring(who4)("\f2you were fragged by %s%s", who3, ismate?", your \f3teammate":"");
        copystring(who5, who4);
        defformatstring(who2)("you fragged %s%s", who, ismate2?", your \f3teammate":"");
        copystring(who6, who2);
        deathstate(d);
		ai::killed(d, actor);
    }

    void timeupdate(int secs)
    {
        if(secs > 0)
        {
            maplimit = lastmillis + secs*1000;
        }
        else
        {
            intermission = true;
            player1->attacking = false;
            if(cmode) cmode->gameover();
            conoutf(CON_GAMEINFO, "\f2intermission:");
            conoutf(CON_GAMEINFO, "\f2game has ended!");
            if(m_ctf) conoutf(CON_GAMEINFO, "\f2player frags: %d, flags: %d, deaths: %d", player1->frags, player1->flags, player1->deaths);
            else conoutf(CON_GAMEINFO, "\f2player frags: %d, deaths: %d", player1->frags, player1->deaths);
            int accuracy = (player1->totaldamage*100)/max(player1->totalshots, 1);
            conoutf(CON_GAMEINFO, "\f2player total damage dealt: %d, damage wasted: %d, accuracy(%%): %d", player1->totaldamage, player1->totalshots-player1->totaldamage, accuracy);
            if(m_sp) spsummary(accuracy);

            showscores(true);
            disablezoom();

            if(identexists("intermission")) execute("intermission");
        }
    }

    int getclientfrags(int cn)
    {
        fpsent *d = getclient(cn);
        return d ? d->frags : -1;
    }
    ICOMMAND(getclientfrags, "i", (int *cn), intret(getclientfrags(*cn)));
    ICOMMAND(getfrags, "", (), intret(player1->frags));

    int getclientflags(int cn)
    {
        fpsent *d = getclient(cn);
        return d ? d->flags : -1;
    }
    ICOMMAND(getclientflags, "i", (int *cn), intret(getclientflags(*cn)));
    ICOMMAND(getflags, "", (), intret(player1->flags));

    int getclientdeaths(int cn)
    {
        fpsent *d = getclient(cn);
        return d ? d->deaths : -1;
    }
    ICOMMAND(getclientdeaths, "i", (int *cn), intret(getclientdeaths(*cn)));
    ICOMMAND(getdeaths, "", (), intret(player1->deaths));

    ICOMMAND(getaccuracy, "", (), intret((player1->totaldamage*100)/max(player1->totalshots, 1)));
    ICOMMAND(gettotaldamage, "", (), intret(player1->totaldamage));
    ICOMMAND(gettotalshots, "", (), intret(player1->totalshots));

    vector<fpsent *> clients;

    fpsent *newclient(int cn)   // ensure valid entity
    {
        if(cn < 0 || cn > max(0xFF, MAXCLIENTS + MAXBOTS))
        {
            neterr("clientnum", false);
            return NULL;
        }

        if(cn == player1->clientnum) return player1;

        while(cn >= clients.length()) clients.add(NULL);
        if(!clients[cn])
        {
            fpsent *d = new fpsent;
            d->clientnum = cn;
            clients[cn] = d;
            players.add(d);
        }
        return clients[cn];
    }

    fpsent *getclient(int cn)   // ensure valid entity
    {
        if(cn == player1->clientnum) return player1;
        return clients.inrange(cn) ? clients[cn] : NULL;
    }

    void clientdisconnected(int cn, bool notify)
    {
        if(!clients.inrange(cn)) return;
        if(following==cn)
        {
            if(followdir) nextfollow(followdir);
            else stopfollowing();
        }
        fpsent *d = clients[cn];
        if(!d) return;
        if(notify && d->name[0]) conoutf("player %s disconnected", colorname(d));
        removeweapons(d);
        removetrackedparticles(d);
        removetrackeddynlights(d);
        if(cmode) cmode->removeplayer(d);
        players.removeobj(d);
        DELETEP(clients[cn]);
        cleardynentcache();
    }

    void clearclients(bool notify)
    {
        loopv(clients) if(clients[i]) clientdisconnected(i, notify);
    }

    void initclient()
    {
        player1 = spawnstate(new fpsent);
        players.add(player1);
    }

    VARP(showmodeinfo, 0, 1, 1);

    void startgame()
    {
        clearmovables();
        clearmonsters();

        clearprojectiles();
        clearbouncers();
        clearragdolls();

        // reset perma-state
        loopv(players)
        {
            fpsent *d = players[i];
            d->frags = d->flags = 0;
            d->deaths = 0;
            d->totaldamage = 0;
            d->totalshots = 0;
            d->maxhealth = 100;
            d->lifesequence = -1;
            d->respawned = d->suicided = -2;
        }

        setclientmode();

        intermission = false;
        maptime = maprealtime = 0;
        maplimit = -1;

        if(cmode)
        {
            cmode->preload();
            cmode->setup();
        }

        conoutf(CON_GAMEINFO, "\f2game mode is %s", server::modename(gamemode));

        if(m_sp)
        {
            defformatstring(scorename)("bestscore_%s", getclientmap());
            const char *best = getalias(scorename);
            if(*best) conoutf(CON_GAMEINFO, "\f2try to beat your best score so far: %s", best);
        }
        else
        {
            const char *info = m_valid(gamemode) ? gamemodes[gamemode - STARTGAMEMODE].info : NULL;
            if(showmodeinfo && info) conoutf(CON_GAMEINFO, "\f0%s", info);
        }

        if(player1->playermodel != playermodel) switchplayermodel(playermodel);

        showscores(false);
        disablezoom();
        lasthit = 0;

        if(identexists("mapstart")) execute("mapstart");
    }

    void startmap(const char *name)   // called just after a map load
    {
        ai::savewaypoints();
        ai::clearwaypoints(true);

        respawnent = -1; // so we don't respawn at an old spot
        if(!m_mp(gamemode)) spawnplayer(player1);
        else findplayerspawn(player1, -1);
        entities::resetspawns();
        copystring(clientmap, name ? name : "");

        sendmapinfo();
    }

    const char *getmapinfo()
    {
        return showmodeinfo && m_valid(gamemode) ? gamemodes[gamemode - STARTGAMEMODE].info : NULL;
    }

    void physicstrigger(physent *d, bool local, int floorlevel, int waterlevel, int material)
    {
        if(d->type==ENT_INANIMATE) return;
        if     (waterlevel>0) { if(material!=MAT_LAVA) playsound(S_SPLASH1, d==player1 ? NULL : &d->o); }
        else if(waterlevel<0) playsound(material==MAT_LAVA ? S_BURN : S_SPLASH2, d==player1 ? NULL : &d->o);
        if     (floorlevel>0) { if(d==player1 || d->type!=ENT_PLAYER || ((fpsent *)d)->ai) msgsound(S_JUMP, d); if(d==player1) game::stats[2]++; }
        else if(floorlevel<0) { if(d==player1 || d->type!=ENT_PLAYER || ((fpsent *)d)->ai) msgsound(S_LAND, d); }
    }

    void dynentcollide(physent *d, physent *o, const vec &dir)
    {
        switch(d->type)
        {
            case ENT_AI: if(dir.z > 0) stackmonster((monster *)d, o); break;
            case ENT_INANIMATE: if(dir.z > 0) stackmovable((movable *)d, o); break;
        }
    }

    void msgsound(int n, physent *d)
    {
        if(!d || d==player1)
        {
            addmsg(N_SOUND, "ci", d, n);
            playsound(n);
        }
        else
        {
            if(d->type==ENT_PLAYER && ((fpsent *)d)->ai)
                addmsg(N_SOUND, "ci", d, n);
            playsound(n, &d->o);
        }
    }

    int numdynents() { return players.length()+monsters.length()+movables.length(); }

    dynent *iterdynents(int i)
    {
        if(i<players.length()) return players[i];
        i -= players.length();
        if(i<monsters.length()) return (dynent *)monsters[i];
        i -= monsters.length();
        if(i<movables.length()) return (dynent *)movables[i];
        return NULL;
    }

    bool duplicatename(fpsent *d, const char *name = NULL)
    {
        if(!name) name = d->name;
        loopv(players) if(d!=players[i] && !strcmp(name, players[i]->name)) return true;
        return false;
    }

    const char *colorname(fpsent *d, const char *name, const char *prefix)
    {
        if(!name) name = d->name;
        if(name[0] && !duplicatename(d, name) && d->aitype == AI_NONE) return name;
        static string cname[3];
        static int cidx = 0;
        cidx = (cidx+1)%3;
        formatstring(cname[cidx])(d->aitype == AI_NONE ? "%s%s \fs\f5(%d)\fr" : "%s%s \fs\f5[%d]\fr", prefix, name, d->clientnum);
        return cname[cidx];
	}

    void suicide(physent *d)
    {
        if(d==player1 || (d->type==ENT_PLAYER && ((fpsent *)d)->ai))
        {
            if(d->state!=CS_ALIVE) return;
            fpsent *pl = (fpsent *)d;
            if(!m_mp(gamemode)) killed(pl, pl);
            else if(pl->suicided!=pl->lifesequence)
            {
                addmsg(N_SUICIDE, "rc", pl);
                pl->suicided = pl->lifesequence;
            }
        }
        else if(d->type==ENT_AI) suicidemonster((monster *)d);
        else if(d->type==ENT_INANIMATE) suicidemovable((movable *)d);
    }
    ICOMMAND(kill, "", (), suicide(player1));

    bool needminimap() { return m_ctf || m_protect || m_hold || m_capture; }

    void drawicon(int icon, float x, float y, float sz)
    {
   		extern int hudtheme;
        if(hudtheme == 1)
            settexture("data/themes/hud/flags.png");
        else
            settexture("packages/hud/items.png");

        glBegin(GL_TRIANGLE_STRIP);
        float tsz = 0.25f, tx = tsz*(icon%4), ty = tsz*(icon/4);
        glTexCoord2f(tx,     ty);     glVertex2f(x,    y);
        glTexCoord2f(tx+tsz, ty);     glVertex2f(x+sz, y);
        glTexCoord2f(tx,     ty+tsz); glVertex2f(x,    y+sz);
        glTexCoord2f(tx+tsz, ty+tsz); glVertex2f(x+sz, y+sz);
        glEnd();
    }

    float abovegameplayhud(int w, int h)
    {
        switch(hudplayer()->state)
        {
            case CS_EDITING:
            case CS_SPECTATOR:
                return 1;
            default:
                return 1650.0f/1800.0f;
        }
    }

    int ammohudup[3] = { GUN_CG, GUN_RL, GUN_GL },
        ammohuddown[3] = { GUN_RIFLE, GUN_SG, GUN_PISTOL },
        ammohudcycle[7] = { -1, -1, -1, -1, -1, -1, -1 };

    ICOMMAND(ammohudup, "sss", (char *w1, char *w2, char *w3),
    {
        int i = 0;
        if(w1[0]) ammohudup[i++] = parseint(w1);
        if(w2[0]) ammohudup[i++] = parseint(w2);
        if(w3[0]) ammohudup[i++] = parseint(w3);
        while(i < 3) ammohudup[i++] = -1;
    });

    ICOMMAND(ammohuddown, "sss", (char *w1, char *w2, char *w3),
    {
        int i = 0;
        if(w1[0]) ammohuddown[i++] = parseint(w1);
        if(w2[0]) ammohuddown[i++] = parseint(w2);
        if(w3[0]) ammohuddown[i++] = parseint(w3);
        while(i < 3) ammohuddown[i++] = -1;
    });

    ICOMMAND(ammohudcycle, "sssssss", (char *w1, char *w2, char *w3, char *w4, char *w5, char *w6, char *w7),
    {
        int i = 0;
        if(w1[0]) ammohudcycle[i++] = parseint(w1);
        if(w2[0]) ammohudcycle[i++] = parseint(w2);
        if(w3[0]) ammohudcycle[i++] = parseint(w3);
        if(w4[0]) ammohudcycle[i++] = parseint(w4);
        if(w5[0]) ammohudcycle[i++] = parseint(w5);
        if(w6[0]) ammohudcycle[i++] = parseint(w6);
        if(w7[0]) ammohudcycle[i++] = parseint(w7);
        while(i < 7) ammohudcycle[i++] = -1;
    });

    VARP(ammohud, 0, 1, 1);

    void drawammohud(fpsent *d)
    {
        float x = HICON_X + 2*HICON_STEP, y = HICON_Y, sz = HICON_SIZE;
        glPushMatrix();
        glScalef(1/3.2f, 1/3.2f, 1);
        float xup = (x+sz)*3.2f, yup = y*3.2f + 0.1f*sz;
        loopi(3)
        {
            int gun = ammohudup[i];
            if(gun < GUN_FIST || gun > GUN_PISTOL || gun == d->gunselect || !d->ammo[gun]) continue;
            drawicon(HICON_FIST+gun, xup, yup, sz);
            yup += sz;
        }
        float xdown = x*3.2f - sz, ydown = (y+sz)*3.2f - 0.1f*sz;
        loopi(3)
        {
            int gun = ammohuddown[3-i-1];
            if(gun < GUN_FIST || gun > GUN_PISTOL || gun == d->gunselect || !d->ammo[gun]) continue;
            ydown -= sz;
            drawicon(HICON_FIST+gun, xdown, ydown, sz);
        }
        int offset = 0, num = 0;
        loopi(7)
        {
            int gun = ammohudcycle[i];
            if(gun < GUN_FIST || gun > GUN_PISTOL) continue;
            if(gun == d->gunselect) offset = i + 1;
            else if(d->ammo[gun]) num++;
        }
        float xcycle = (x+sz/2)*3.2f + 0.5f*num*sz, ycycle = y*3.2f-sz;
        loopi(7)
        {
            int gun = ammohudcycle[(i + offset)%7];
            if(gun < GUN_FIST || gun > GUN_PISTOL || gun == d->gunselect || !d->ammo[gun]) continue;
            xcycle -= sz;
            drawicon(HICON_FIST+gun, xcycle, ycycle, sz);
        }
        glPopMatrix();
    }

    void drawhudicons(fpsent *d)
    {
    	extern int hudtheme;
        if(hudtheme == 1) return;

        glPushMatrix();
        glScalef(2, 2, 1);

        draw_textf("%d", (HICON_X + HICON_SIZE + HICON_SPACE)/2, HICON_TEXTY/2, d->state==CS_DEAD ? 0 : d->health);
        if(d->state!=CS_DEAD)
        {
            if(d->armour) draw_textf("%d", (HICON_X + HICON_STEP + HICON_SIZE + HICON_SPACE)/2, HICON_TEXTY/2, d->armour);
            draw_textf("%d", (HICON_X + 2*HICON_STEP + HICON_SIZE + HICON_SPACE)/2, HICON_TEXTY/2, d->ammo[d->gunselect]);
        }

        glPopMatrix();

        drawicon(HICON_HEALTH, HICON_X, HICON_Y);
        if(d->state!=CS_DEAD)
        {
            if(d->armour) drawicon(HICON_BLUE_ARMOUR+d->armourtype, HICON_X + HICON_STEP, HICON_Y);
            drawicon(HICON_FIST+d->gunselect, HICON_X + 2*HICON_STEP, HICON_Y);
            if(d->quadmillis) drawicon(HICON_QUAD, HICON_X + 3*HICON_STEP, HICON_Y);
            if(ammohud) drawammohud(d);
        }
    }

    int chh, cah, camh;
    float chs, cas, cams;

    void newhud(int w, int h)
    {
        fpsent *f = followingplayer();

        if(player1->state==CS_DEAD || (player1->state==CS_SPECTATOR && !followingplayer())) return;
        glPushMatrix();
        glScalef(1/1.2f, 1/1.2f, 1);
        if(!m_insta) draw_textf("%d", 80, h*1.2f-128, player1->state==CS_DEAD ? 0 : player1->state==CS_SPECTATOR ? f->health : player1->health);
        defformatstring(ammo)("%d", player1->state==CS_SPECTATOR ? f->ammo[f->gunselect] : player1->ammo[player1->gunselect]);
        int wb, hb;
        text_bounds(ammo, wb, hb);
        draw_textf("%d", w*1.2f-wb-80, h*1.2f-128, player1->state==CS_SPECTATOR ? f->ammo[f->gunselect] : player1->ammo[player1->gunselect]);

        if(player1->quadmillis)
        {
            settexture("data/themes/hud/hud_quaddamage_left.png");
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);   glVertex2f(0,   h*1.2f-207);
            glTexCoord2f(1.0f, 0.0f);   glVertex2f(539, h*1.2f-207);
            glTexCoord2f(1.0f, 1.0f);   glVertex2f(539, h*1.2f);
            glTexCoord2f(0.0f, 1.0f);   glVertex2f(0,   h*1.2f);
            glEnd();

            settexture("data/themes/hud/hud_quaddamage_right.png");
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);   glVertex2f(w*1.2f-135, h*1.2f-207);
            glTexCoord2f(1.0f, 0.0f);   glVertex2f(w*1.2f,     h*1.2f-207);
            glTexCoord2f(1.0f, 1.0f);   glVertex2f(w*1.2f,     h*1.2f);
            glTexCoord2f(0.0f, 1.0f);   glVertex2f(w*1.2f-135, h*1.2f);
            glEnd();
        }

        if(player1->maxhealth > 100)
        {
            settexture("data/themes/hud/hud_megahealth.png");
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);   glVertex2f(0,   h*1.2f-207);
            glTexCoord2f(1.0f, 0.0f);   glVertex2f(539, h*1.2f-207);
            glTexCoord2f(1.0f, 1.0f);   glVertex2f(539, h*1.2f);
            glTexCoord2f(0.0f, 1.0f);   glVertex2f(0,   h*1.2f);
            glEnd();
        }

        int health = (player1->health*100)/player1->maxhealth,
            armour = (player1->armour*100)/200,
            hh = (health*101)/100,
            ah = (armour*167)/100;

        float hs = (health*1.0f)/100,
              as = (armour*1.0f)/100;

        if     (chh>hh) chh -= max(1, ((chh-hh)/4));
        else if(chh<hh) chh += max(1, ((hh-chh)/4));
        if     (chs>hs) chs -= max(0.01f, ((chs-hs)/4));
        else if(chs<hs) chs += max(0.01f, ((hs-chs)/4));

        if     (cah>ah) cah -= max(1, ((cah-ah)/4));
        else if(cah<ah) cah += max(1, ((ah-cah)/4));
        if     (cas>as) cas -= max(0.01f, ((cas-as)/4));
        else if(cas<as) cas += max(0.01f, ((as-cas)/4));

        if(player1->health > 0 && !m_insta)
        {
            settexture("data/themes/hud/hud_health.png");
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f-chs);   glVertex2f(47, h*1.2f-chh-56);
            glTexCoord2f(1.0f, 1.0f-chs);   glVertex2f(97, h*1.2f-chh-56);
            glTexCoord2f(1.0f, 1.0f);      glVertex2f(97, h*1.2f-57);
            glTexCoord2f(0.0f, 1.0f);      glVertex2f(47, h*1.2f-57);
            glEnd();
        }

        if(player1->armour > 0)
        {
            settexture("data/themes/hud/hud_armour.png");
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f,    0.0f);   glVertex2f(130,    h*1.2f-62);
            glTexCoord2f(cas,      0.0f);   glVertex2f(130+cah, h*1.2f-62);
            glTexCoord2f(cas,      1.0f);   glVertex2f(130+cah, h*1.2f-44);
            glTexCoord2f(0.0f,    1.0f);   glVertex2f(130,    h*1.2f-44);
            glEnd();
        }

        if(!m_insta)
        {
            settexture("data/themes/hud/hud_left.png");
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);   glVertex2f(0,   h*1.2f-207);
            glTexCoord2f(1.0f, 0.0f);   glVertex2f(539, h*1.2f-207);
            glTexCoord2f(1.0f, 1.0f);   glVertex2f(539, h*1.2f);
            glTexCoord2f(0.0f, 1.0f);   glVertex2f(0,   h*1.2f);
            glEnd();
        }

        settexture("data/themes/hud/hud_right.png");
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);   glVertex2f(w*1.2f-135, h*1.2f-207);
        glTexCoord2f(1.0f, 0.0f);   glVertex2f(w*1.2f,     h*1.2f-207);
        glTexCoord2f(1.0f, 1.0f);   glVertex2f(w*1.2f,     h*1.2f);
        glTexCoord2f(0.0f, 1.0f);   glVertex2f(w*1.2f-135, h*1.2f);
        glEnd();

        int maxammo;

        switch(player1->gunselect)
        {
            case GUN_FIST:
                maxammo = 1;
                break;

            case GUN_RL:
            case GUN_RIFLE:
                maxammo = m_insta ? 100 : 15;
                break;

            case GUN_SG:
            case GUN_GL:
                maxammo = 30;
                break;

            case GUN_CG:
                maxammo = 60;
                break;

            case GUN_PISTOL:
                maxammo = 120;
                break;
        }

        int curammo = (player1->ammo[player1->gunselect]*100)/maxammo,
            amh = (curammo*101)/100;

        float ams = (curammo*1.0f)/100;

        if     (camh>amh) camh -= max(1, ((camh-amh)/4));
        else if(camh<amh) camh += max(1, ((amh-camh)/4));
        if     (cams>ams) cams -= max(0.01f, ((cams-ams)/4));
        else if(cams<ams) cams += max(0.01f, ((ams-cams)/4));

        if(player1->ammo[player1->gunselect] > 0)
        {
            settexture("data/themes/hud/hud_health.png");
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f-cams);   glVertex2f(w*1.2f-47, h*1.2f-camh-56);
            glTexCoord2f(1.0f, 1.0f-cams);   glVertex2f(w*1.2f-97, h*1.2f-camh-56);
            glTexCoord2f(1.0f, 1.0f);       glVertex2f(w*1.2f-97, h*1.2f-57);
            glTexCoord2f(0.0f, 1.0f);       glVertex2f(w*1.2f-47, h*1.2f-57);
            glEnd();
        }

        glPopMatrix();
        glPushMatrix();

        glScalef(1/4.0f, 1/4.0f, 1);

        defformatstring(icon)("data/themes/hud/guns/%d.png", player1->gunselect);
        settexture(icon);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);   glVertex2f(w*4.0f-1162,    h*4.0f-350);
        glTexCoord2f(1.0f, 0.0f);   glVertex2f(w*4.0f-650,     h*4.0f-350);
        glTexCoord2f(1.0f, 1.0f);   glVertex2f(w*4.0f-650,     h*4.0f-50);
        glTexCoord2f(0.0f, 1.0f);   glVertex2f(w*4.0f-1162,    h*4.0f-50);
        glEnd();



        glPopMatrix();
    }

    void gameplayhud(int w, int h)
    {
    	extern int hudtheme;
		if(hudtheme == 1) newhud(w, h);
        glPushMatrix();
        glScalef(h/1800.0f, h/1800.0f, 1);

        if(player1->state==CS_SPECTATOR)
		{
            int pw, ph, tw, th, fw, fh;
            text_bounds("  ", pw, ph);
            text_bounds("SPECTATOR", tw, th);
            th = max(th, ph);
            fpsent *f = followingplayer();
            text_bounds(f ? colorname(f) : " ", fw, fh);
            fh = max(fh, ph);
            draw_text("SPECTATOR", w*1000/h - tw - pw, 1800 - th - fh);
            if(f) draw_text(colorname(f), w*1000/h - fw - pw, 1800 - fh);
        }

        fpsent *d = hudplayer();
        if(d->state!=CS_EDITING)
        {
            if(d->state!=CS_SPECTATOR) drawhudicons(d);
            if(cmode) cmode->drawhud(d, w, h);
        }

        glPopMatrix();
    }

    int clipconsole(int w, int h)
    {
        if(cmode) return cmode->clipconsole(w, h);
        return 0;
    }

    VARP(teamcrosshair, 0, 1, 1);
    VARP(hitcrosshair, 0, 425, 1000);

    const char *defaultcrosshair(int index)
    {
        switch(index)
        {
            case 1: return "data/teammate.png";
            default: return "data/crosshair.png";
        }
    }

    int selectcrosshair(float &r, float &g, float &b, int &w, int &h)
    {
        fpsent *d = hudplayer();
        if(d->state==CS_SPECTATOR) return -1;

        //if(d->state!=CS_ALIVE) return 0;

        int crosshair = 0;
        if(lasthit && lastmillis - lasthit < hitcrosshair) crosshair = 2;
        else if(teamcrosshair)
        {
            dynent *o = intersectclosest(d->o, worldpos, d);
            if(o && o->type==ENT_PLAYER && isteam(((fpsent *)o)->team, d->team))
            {
                crosshair = 1;
                r = g = 0;
            }
        }

        if(crosshair!=1 && !editmode && !m_insta)
        {
            if(d->health<=25) { r = 1.0f; g = b = 0; }
            else if(d->health<=50) { r = 1.0f; g = 0.5f; b = 0; }
        }
        if(d->gunwait) { r *= 0.5f; g *= 0.5f; b *= 0.5f; }

        if(ffrag && totalmillis-when<=4000)
        {
            int tw, th;
            text_bounds(who6, tw, th);
            glPushMatrix();
            glScalef(1/2.5f, 1/2.5f, 1);
            draw_textf(who6, (w*2.5f - tw)/2, h*2.5f-500, who);
            glPopMatrix();
        }
        if(ffrag2 && totalmillis-when2<=4000)
        {
            int tw, th;
            text_bounds(who5, tw, th);
            glPushMatrix();
            glScalef(1/2.5f, 1/2.5f, 1);
            draw_textf(who5, (w*2.5f - tw)/2, h*2.5f-400, who);
            glPopMatrix();
        }

        return crosshair;
    }

    void lighteffects(dynent *e, vec &color, vec &dir)
    {
#if 0
        fpsent *d = (fpsent *)e;
        if(d->state!=CS_DEAD && d->quadmillis)
        {
            float t = 0.5f + 0.5f*sinf(2*M_PI*lastmillis/1000.0f);
            color.y = color.y*(1-t) + t;
        }
#endif
    }

    bool serverinfostartcolumn(g3d_gui *g, int i)
	{
		static const char *names[] = { "ping ", "players ", "map ", "mode ", "master ", "host ", "port ", "description " };
		//"sortbyping"= 1
		//"sortbyplayers" = 2
		//"sortbymap" = 3
		//"sortbymode" = 4
		//"sortbymaster" = 5
		//"sortbyhost" = 6
		//"sortbyport" = 7
		//"sortbydescription" = 8

		static int actions[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
		static const int struts[] =  { 0,       0,          12,     12,      8,         13,      6,       24 };
		if(size_t(i) >= sizeof(names)/sizeof(names[0])) return false;
		g->pushlist();
		       if(g->button(names[i], 0xFFFF80)&G3D_UP)
			   {

				   sortbythat = i+1;

			   }
        g->separator();
		//g->button(names[i], 0xFFFF80, "sortlikeme"/*!i ? " " : NULL*/); //a_teammate 26.02.2011
		if(struts[i]) g->strut(struts[i]);
		g->mergehits(true);
		return true;
	}

    void serverinfoendcolumn(g3d_gui *g, int i)
    {
        g->mergehits(false);
        g->poplist();
    }

    const char *mastermodecolor(int n, const char *unknown)
    {
        return (n>=MM_START && size_t(n-MM_START)<sizeof(mastermodecolors)/sizeof(mastermodecolors[0])) ? mastermodecolors[n-MM_START] : unknown;
    }

    const char *mastermodeicon(int n, const char *unknown)
    {
        return (n>=MM_START && size_t(n-MM_START)<sizeof(mastermodeicons)/sizeof(mastermodeicons[0])) ? mastermodeicons[n-MM_START] : unknown;
    }

	bool serverinfoentry(g3d_gui *g, int i, const char *name, int port, const char *sdesc, const char *map, int ping, const vector<int> &attr, int np)
	{
		if(ping < 0 || attr.empty() || attr[0]!=PROTOCOL_VERSION)
		{
			switch(i)
			{
			case 0:
				if(g->button(" ", 0xFFFFDD, "serverunk")&G3D_UP) return true;
				break;

			case 1:
			case 2:
			case 3:
			case 4:
				if(g->button(" ", 0xFFFFDD)&G3D_UP) return true;
				break;

			case 5:
				if(g->buttonf("%s ", 0xFFFFDD, NULL, name)&G3D_UP) return true;
				break;

			case 6:
				if(g->buttonf("%d ", 0xFFFFDD, NULL, port)&G3D_UP) return true;
				break;

			case 7:
				if(ping < 0)
				{
					if(g->button(sdesc, 0xFFFFDD)&G3D_UP) return true;
				}
				else if(g->buttonf("[%s protocol] ", 0xFFFFDD, NULL, attr.empty() ? "unknown" : (attr[0] < PROTOCOL_VERSION ? "older" : "newer"))&G3D_UP) return true;
				break;
			}
			return false;
		}

		switch(i)
		{
		case 0:
			{
				const char *icon = attr.inrange(3) && np >= attr[3] ? "serverfull" : (attr.inrange(4) ? mastermodeicon(attr[4], "serverunk") : "serverunk");
				if(g->buttonf("%d ", 0xFFFFDD, icon, ping)&G3D_UP) return true;
				break;
			}

		case 1:
			if(attr.length()>=4)
			{
				if(g->buttonf(np >= attr[3] ? "\f3%d/%d " : "%d/%d ", 0xFFFFDD, NULL, np, attr[3])&G3D_UP) return true;
			}
			else if(g->buttonf("%d ", 0xFFFFDD, NULL, np)&G3D_UP) return true;
			break;

		case 2:
			if(g->buttonf("%.25s ", 0xFFFFDD, NULL, map)&G3D_UP) return true;
			break;

		case 3:
			if(g->buttonf("%s ", 0xFFFFDD, NULL, attr.length()>=2 ? server::modename(attr[1], "") : "")&G3D_UP) return true;
			break;

		case 4:
			if(g->buttonf("%s%s ", 0xFFFFDD, NULL, attr.length()>=5 ? mastermodecolor(attr[4], "") : "", attr.length()>=5 ? server::mastermodename(attr[4], "") : "")&G3D_UP) return true;
			break;

		case 5:
			if(g->buttonf("%s ", 0xFFFFDD, NULL, name)&G3D_UP) return true;
			break;

		case 6:
			if(g->buttonf("%d ", 0xFFFFDD, NULL, port)&G3D_UP) return true;
			break;

		case 7:
			if(g->buttonf("%.25s", 0xFFFFDD, NULL, sdesc)&G3D_UP) return true;
			break;
		}
		return false;
	}

    // any data written into this vector will get saved with the map data. Must take care to do own versioning, and endianess if applicable. Will not get called when loading maps from other games, so provide defaults.
    void writegamedata(vector<char> &extras) {}
    void readgamedata(vector<char> &extras) {}

    const char *gameident() { return "fps"; }
    const char *savedconfig() { return "config.cfg"; }
    const char *restoreconfig() { return "restore.cfg"; }
    const char *defaultconfig() { return "data/defaults.cfg"; }
    const char *autoexec() { return "autoexec.cfg"; }
    const char *savedservers() { return "servers.cfg"; }

    void loadconfigs()
    {
        execfile("auth.cfg", false);
    }
}

