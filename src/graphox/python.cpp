
#ifndef GRAPHOX_PYTHON_CPP
#define GRAPHOX_PYTHON_CPP

#include "Python.h"
#include "graphox/python.h"

namespace graphox
{
	namespace scripting
	{
		namespace python
		{
/*			int print(lua_State *L)
			{
				/ * get number of arguments * /
				int n = lua_gettop(L);
				bool first = true;
				
				for(int i = 1; i <= n; i++)
				{
					if(first)
						first = false;
					else
						printf(" ");
					
					printf("%s", lua_tostring(L, i));
				}
				
				printf("\n");
				
				return 0;
			}*/
			
			void init()
			{
				puts("Python: init");
				Py_Initialize();
			}
   			
   			void unload()
   			{
   				puts("Python: unload");
				Py_Finalize();
   			}
  			
			void execute(const char *filename)
  			{
  				puts("Python: execute");
  				PyObject *file_name = PyString_FromString(filename);
  				PyObject *module = PyImport_Import(file_name);
  				
  				puts("Python: clean");
				Py_DECREF(file_name);
				//Py_DECREF(module);

				puts("Python: error");
				PyObject *error = PyErr_Occurred();
	
				if (error != NULL)
				{
					puts("Python: print_error");
					fprintf(stderr, "Failed to load \"%s\"\n", filename);
					PyErr_Print();
				}
  			}
  		}
	}
}

#endif
