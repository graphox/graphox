To run the tests, you need to compile the test runner. Test runner itself
uses OFTL so you'll have to include it. With g++:

    g++ runner.cpp -o runner -I../include -I../src

The src include path is there because the test runner uses the filesystem
module and as it's a simple program, there is no need to compile the
implementation separately - so it just includes it.

Then, just run it. It'll run all tests. If you want to run just specific
module or modules, set them in argv, like this:

    ./runner vector string

That'll run just vector and string tests.
