#ifndef GRAPHOX_JS_H
#define GRAPHOX_JS_H

#include <v8.h>
#include "graphox/graphox.h"

namespace graphox
{
	namespace scripting
	{
		namespace js
		{
			struct js_error_info : graphox::scripting::error_info
			{
				const char *script_line;
				int startcolumn;
				int endcolumn;
				const char *stacktrace;
				
				//why needed?
				int line;
				const char *file;
				
				void print();
				void print(const char *);
				
				js_error_info(
					int line_,
					const char *file_,
					const char *rline_,
					int startcolumn_,
					int endcolumn_,
					const char *stacktrace_
				) :
					line(line_),
					file(file_),
					script_line(rline_),
					startcolumn(startcolumn_),
					endcolumn(endcolumn_),
					stacktrace(stacktrace_) {} ;
			};
		
			v8::Handle<v8::Value> print(const v8::Arguments& args);

			v8::Handle<v8::ObjectTemplate> global;
			v8::Persistent<v8::Context> context;
			
			void bind_functions(v8::Handle<v8::ObjectTemplate> global);
			void init();
 			void handle_exceptions(v8::TryCatch* try_catch);
			void execute(const char *filename);
  		}
	}
}

#if 0
void jsrun()
{
/*
  // Create a stack-allocated handle scope.
  HandleScope handle_scope;

  // Create a new context.
  Persistent<Context> context = Context::New();
  
  // Enter the created context for compiling and
  // running the hello world script. 
  Context::Scope context_scope(context);

  // Create a string containing the JavaScript source code.
  Handle<String> source = String::New("'Hello' + ', World!'");

  // Compile the source code.
  Handle<Script> script = Script::Compile(source);
  
  // Run the script to get the result.
  Handle<Value> result = script->Run();
  
  // Dispose the persistent context.
  context.Dispose();

  // Convert the result to an ASCII string and print it.
  String::AsciiValue ascii(result);
  printf("%s\n", *ascii);/*/
  
  try
  {
  	graphox::scripting::js::init();
  	graphox::scripting::js::execute("init.js");
  }
  
  //TODO: 1 catch
  catch(graphox::Exception *e)
  {
  	e->print();
  }
  
  catch(graphox::scripting::Exception *e)
  {
  	e->print();
  }
};

COMMAND(jsrun, "");

#endif

#endif


