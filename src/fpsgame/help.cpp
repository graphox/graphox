#include "game.h"

namespace game
{
    void help(const char *helpcom, int type, const char *com)
    {
	const char *comtype = "help ";
	const char *coms = "graphox | get | server | auth | mod";

	if(type == 0) {
	    if(!helpcom[0]) conoutf("no command specified. please use:\n\f0/help \f1<command>");
	    if(!strcmp(helpcom, "setmaster")) conoutf("\f0/setmaster \f6[<password>]\n\f2claims master (or, if password is correct, admin)");
	    if(!strcmp(helpcom, "name")) conoutf("/name [name]\nsets or shows name");
	} else if(type == 1) {
	    if(!strcmp(com, "graphox")) {
		comtype = "graphox ";
		coms = "_Graphox_allow_help | _Graphox_allow_highlight | _Graphox_autonp | _Graphox_autosorry | _Graphox_showaccuracy | _Graphox_showdeaths | _Graphox_showfrags | _Graphox_showkpd | _Graphox_sr | _Graphox_get_sr | _Graphox_np | _Graphox_get_np";
	    } else if(!strcmp(com, "get")) {
		comtype = "get ";
		coms = "getseltex | gettexname | getalias | getmillis | getclientname | getclientping | getfrags | getflags | getdeaths | getaccuracy | gettotaldamage | gettotalshots";
	    } else if(!strcmp(com, "server")) {
		comtype = "server ";
		coms = "kick | pausegame | setmaster | spectator";
	    } else if(!strcmp(com, "auth")) {
		comtype = "auth ";
		coms = "auth | authkey | autoauth | hasauthkey | genauthkey | saveauthkeys";
	    } else if(!strcmp(com, "mod")) {
		comtype = "mod ";
		coms = "theme | hudtheme | radartheme | defaultload | showping | showpj";
	    }

	    conoutf("list of %s commands: %s", comtype, coms);
	}
    }
    ICOMMAND(help, "s", (char *com), help(com, 0, ""));
    ICOMMAND(commands, "s", (char *com), help("", 1, com));

}

