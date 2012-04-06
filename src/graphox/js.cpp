
#ifndef GRAPHOX_JS_CPP
#define GRAPHOX_JS_CPP

#include "graphox/js.h"

namespace graphox
{
	namespace scripting
	{
		namespace js
		{
			void js_error_info::print(const char *message)
			{
				printf("\t\ton %s:%i : %s : %s", file, line, message, stacktrace);
				printf("line:\n\t%s\n\t", script_line);

				for (int i = 0; i < startcolumn; i++)
					printf(" ");
					
				for (int i = startcolumn; i < endcolumn; i++)
					printf("^");
					
				printf("\n");			
			}
				
			void js_error_info::print()
			{
				printf("\t\ton %s:%i : %s", file, line, stacktrace);
				printf("line:\n\t%s\n\t", script_line);

				for (int i = 0; i < startcolumn; i++)
					printf(" ");
					
				for (int i = startcolumn; i < endcolumn; i++)
					printf("^");
					
				printf("\n");			
			}
		
			v8::Handle<v8::Value> print(const v8::Arguments& args)
			{
				bool first = true;
				for(int i = 0; i < args.Length(); i++)
				{
					v8::HandleScope handle_scope;
					
					if (first)
						first = false;
					
					else
						printf(" ");
					
					v8::String::Utf8Value str(args[i]);
					
					const char* cstr = *str ? *str : "<string conversion failed>";
					
					graphox::scripting::print(cstr);
				}
				
				graphox::scripting::print("\n");
				
				return v8::Undefined();
			}

			void bind_functions(v8::Handle<v8::ObjectTemplate> global)
			{
				global->Set(v8::String::New("print"), v8::FunctionTemplate::New(graphox::scripting::js::print));
			}
			
			void init()
			{
				v8::HandleScope handle_scope;
				
				//Global object
				global = v8::ObjectTemplate::New();
				
				bind_functions(global);
  
   				context = v8::Context::New(NULL, global);
  			}
  			
 			void handle_exceptions(v8::TryCatch* try_catch)
  			{
  				v8::HandleScope handle_scope;
  				v8::String::Utf8Value exception(try_catch->Exception());
  				
  				const char* exception_string = *exception;
  				
  				v8::Handle<v8::Message> message = try_catch->Message();
  				
  				if (message.IsEmpty())
  				{
  					// V8 didn't provide any extra information about this error; just
  					// print the exception.
  					throw new graphox::Exception(500, exception_string);
  				}
  				else
  				{
  					graphox::scripting::js::js_error_info *error_info;
						error_info = new graphox::scripting::js::js_error_info(
  							message->GetLineNumber(),
  							(const char *)*message->GetScriptResourceName(),
  							(const char *)*message->GetSourceLine(),
  							(int) message->GetStartColumn(),
  							(int) message->GetEndColumn(),
  							(const char *)*try_catch->StackTrace()
  						);

					throw new graphox::scripting::Exception(
  						500,
  						exception_string,
  						error_info
  					);
  				}
  				
  			}
  			
			void execute(const char *filename)
  			{
  			  	graphox::scripting::js::js_error_info *error_info;
				
					error_info = new graphox::scripting::js::js_error_info(
  						-1,
  						filename,
  						"> v8 bug -> compiling means segmentation fault\0",
  						12,
  						21,
  						"\0"
  						);

					throw new graphox::scripting::Exception(
  						500,
  						"Could not compile script",
  						error_info
  					);
  						
  			
  			
				char *contents = loadfile(filename, NULL);
				
				v8::TryCatch try_catch;
				
				v8::HandleScope handle_scope;
				
				v8::Handle<v8::String> file_name = v8::String::New(filename);
				v8::Handle<v8::String> file_contents = v8::String::New(contents);
				delete[] contents;
				
				if(file_contents.IsEmpty())
				{
					printf("Error reading '%s'\n", filename);
					throw new graphox::Exception(501, "error reading");
				}
								
				puts("calling");
				
				v8::Handle<v8::Script> script = v8::Script::Compile(file_contents, file_name);

				/*
				
				//compilation errors
				if(script.IsEmpty())
					handle_exceptions(&try_catch);
					
				//run script
				else
				{
					v8::Handle<v8::Value> result = script->Run();
					
					if(result.IsEmpty())
					{
						assert(try_catch.HasCaught());
						handle_exceptions(&try_catch);
					}
					else
					{
						assert(!try_catch.HasCaught());
						
						if(!result->IsUndefined())
						{
							printf("%s\n", (const char *) *v8::String::Utf8Value(result));
						}
					}
				}
				*/
  			}
  		}
	}
}

#endif
