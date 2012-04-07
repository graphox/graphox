OFTL - OctaForge Template Library

Version 0.8.3 (20111211)

is a set of various generic features for the C++ programming language,
simillar in function to the C++ standard library, but with simpler,
more transparent implementation, designed to work well with the
"C with classes" style of writing with usage of the C standard
library.

The API is designed from ground up and NOT compatible with the C++
standard library, albeit freely inspired.

It's released under MIT license and created for needs of the OctaForge
project (http://octaforge.org), however is not limited to it and is
meant to be independent.

The library requires C++11 support in compiler (variadic templates and
rvalue references, implemented in gcc 4.3).

No external dependencies are required (except the optional Lua module,
which requires either LuaC or LuaJIT).

Author: Daniel "q66" Kolesa <quaker66@gmail.com>

USAGE:
    Include the headers you need in your project.

    To run unit tests, see README.txt in tests/.
    There are also examples, in examples/, which
    show real usage cases.

    The filesystem module has implementation files
    in src/, one for POSIX systems, one for Windows.
    Compile your project appropriately using those
    files.
