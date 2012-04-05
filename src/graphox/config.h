#include <map>

#ifndef GRAPHOX_CONFIG_H
#define GRAPHOX_CONFIG_H

namespace graphox
{
	namespace config
	{
		enum
		{
			TYPE_STRING,
			TYPE_INT,
			TYPE_FLOAT			
		};
		
		class setting
		{
			const char *name;
			int type;
			
			union
			{
				const char *value_c;
				int value_i;
				float value_f;
			};
		};
		
		class Moduleconfig
		{
			public:
				void write();
				void write(const char *fname);
				
				void open();
				void open(const char *fname);
				
				void set(const char *var, int value);
				void set(const char *var, const char *value);
				void set(const char *var, float *value);
				
				const char *getc(const char *var);
				int geti(const char *var);
				float getf(const char *var);
				
				Moduleconfig(const char *name_) : name(name_), saved(false);
			
			private:
				const char *name;
				bool saved;
				std::map<const char *, setting *> settings;
		};
	}
}

#endif
