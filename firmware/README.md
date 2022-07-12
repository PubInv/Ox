# Ox Firmware

Ox firmware uses [PlatformIO](https://platformio.org/) for development.

## Testing

PlatformIO advocates for a component/library level organization with remote repositories. One disadvantage of PlatformIO is that unit tests are on a project level, with each test suite requiring it's own main(). This is a limitation of Scons on which PlatformIO is built. One solution is to run a Python script to search for all unit tests in the lib/ directory and copy them into the test/test_all/ directory and autogenerate a test runner.

This is implemented with test/test_all/create_tests.py which needs to be run manually. This will search for any files named test_*.cpp and test_*.h in the lib/ directory. It will create the runner with a main(), populate the Unity RUN_TEST calls and also copy the files because this is where PlatformIO is looking for the tests.

It's a little clunky right now but to use the test runner:

From test/test_all/:

`python3 test/test_all/create_tests.py`

From root directory (where platform.ini is located):

`pio test -e native -f test_all`
