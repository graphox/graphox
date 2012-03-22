#include "config.h"
namespace graphox
{
	namespace config
	{
		void Moduleconfig::write()
		{
			const char *fname;
			formatstring(fname)("data/config/%s.cs", name);
			write(name);
		}
		
		void Modlueconfig::write(const char *fname)
		{
			if(saved == true)
				#if !defined(GRAPHOX_CONFIG_POLITE) && defined(GRAPHOX_CONFIG_RUDE)
					throw new exception("already saved");
				#else
				{
					conoutf("Already saved");
					return;				
				}
				#endif
			stream *f = openfile(path(fname, true), "w");
			
			f->printf("// Module config of %s", name);
			
			loopv(settings)
			{
				switch (type)
				{
					case TYPE_STRING:
						f->printf("cfg_char %s %s %i %s", name, settings[i]->name, settings[i]->type, settings[i]->value_c);
						break;
						
					case TYPE_INT:
						f->printf("cfg_int %s %s %i %s", name, settings[i]->name, settings[i]->type, settings[i]->value_i);
						break;
						
					case TYPE_FLOAT:
						f->printf("cfg_float %s %s %i %s", name, settings[i]->name, settings[i]->type, settings[i]->value_f);
						break;
					
					default:
						#ifndef GRAPHOX_CONFIG_POLITE
							throw new exception("Invalid type, could not write");
						#else
							conoutf("Could not write config line! invalid vartype.");
						#endif
						break
				}
				
			}
			
			delete f;
			saved = true;
		}
	}
}
