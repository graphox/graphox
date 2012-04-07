/* OFTL unit test runner
 * 
 * Written in C++ and OFTL :)
 * Licensed under MIT, same as the rest of the library.
 *
 * Version 0.0.1
 */

#include <stdio.h>

#include "OFTL/filesystem.h"

/* don't do this elsewhere :P */
#if !(defined(_WIN32) || defined(WIN32))
#include "filesystem/platform_posix.cpp"
#else
#include "filesystem/platform_win32.cpp"
#endif

using namespace types;
using namespace filesystem;

#if !(defined(_WIN32) || defined(WIN32))
#define DEFAULT_CXX "g++"
#else
#define DEFAULT_CXX "mingw32-g++"
#endif
#define MANDATORY_CXXFLAGS \
    "-Wall -fno-rtti -fno-exceptions " \
    "-std=c++0x -pedantic -pedantic-errors " \
    "-I../include"
#define OUTFILE "curr.out"

int main(int argc, char **argv)
{
    printf("Running OFTL unit tests ...\n");

    /* acquire needed information */
    String cxx = getenv("CXX");
    if (cxx.is_empty())
        cxx = DEFAULT_CXX;

    char *optional_cxxflags = getenv("CXXFLAGS");

    String cxxflags = MANDATORY_CXXFLAGS;
    if (optional_cxxflags)
    {
        cxxflags += ' ';
        cxxflags += optional_cxxflags;
    }

    bool verbose = getenv("VERBOSE") ? true : false;

    /* get list of test modules */
    File_Vector modules;

    if (argc <= 1)
        modules = list(".");
    else
    {
        ++argv;
        while (*argv)
        {
            modules.push_back(*argv);
            ++argv;
        }
    }

    /* for each module, run a set of tests */
    for (File_Vector::cit it = modules.begin(); it != modules.end(); ++it)
    {
        if (it->type() != OFTL_FILE_DIR) continue;
        String mn = it->filename();

        for (File_Iterator tit = it->begin(); tit != it->end(); ++tit)
        {
            if (tit->extension() != ".cpp") continue;

            String params = "";

            String name = tit->path().substr(
                0, tit->path().length() - tit->extension().length()
            );
            name += ".cfg";
            FILE *f = fopen(name.get_buf(), "r");
            if   (f)
            {
                fseek(f, 0, SEEK_END);
                size_t s = ftell(f) - 1;
                fseek(f, 0, SEEK_SET);

                params.resize(s);
                fread(&params[0], 1, s, f);
            }

            String tn = tit->filename();
            printf(
                "\033[1;36m[%s]\033[m\033[1;31m %s ... \033[m",
                mn.get_buf(), tn.get_buf()
            );

            String cmd = String().format(
                "%s %s %s -o %s %s/%s",
                cxx.get_buf(), cxxflags.get_buf(), params.get_buf(),
                OUTFILE, mn.get_buf(), tn.get_buf()
            );

            if (verbose)
                printf("%s\n", cmd.get_buf());

            FILE *log = fopen(String().format(
                "%s/%s.txt", mn.get_buf(),
                tn.substr(0, tn.length() - 4).get_buf()
            ).get_buf(), "w");

            int  ret = system(cmd.get_buf());
            if (!ret)
            {
                printf("\033[1;34mRunning ... \033[m");
                ret = system("./"OUTFILE);

                fprintf(log, "Retval: %i", ret);

                if (!ret)
                {
                    fprintf(log, " (success)\n");
                    printf("\033[1;32mSuccess!\033[m\n");
                }
                else
                {
                    fprintf(log, " (failure)\n");
                    printf("\033[1;31mFailed!\033[m\n");
                }
            }
            else
            {
                fprintf(log, "Retval: %i (compilation failure)\n", ret);
                printf("\033[1;35mCompilation failed.\033[m\n");
            }

            fclose(log);
        }
    }
    remove(OUTFILE);

    return 0;
}
