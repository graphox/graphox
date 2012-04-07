/* Recursive ls: shows a directory tree
 *
 * Author: Daniel 'q66' Kolesa <quaker66@gmail.com>
 */

#include <stdio.h>

#include "OFTL/filesystem.h"

/* don't do this elsewhere :P */
#if !(defined(_WIN32) || defined(WIN32))
#include "filesystem/platform_posix.cpp"
#else
#include "filesystem/platform_win32.cpp"
#endif

using namespace filesystem;

void list_dir(const File_Info& path, uint level = 0)
{
    for (File_Info::it it = path.begin(); it != path.end(); ++it)
    {
        for (uint i = 0; i < level; ++i)
            printf("    ");

        if (it->type() == OFTL_FILE_DIR)
        {
            printf("+ %s\n", it->filename().get_buf());
            list_dir(*it, level + 1);
        }
        else
            printf("- %s\n", it->filename().get_buf());
    }
}

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        printf("No argument provided, listing cwd.\n");
        list_dir(".");
        return 0;
    }
    list_dir(argv[1]);
    return 0;
}
