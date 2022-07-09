# Testing
- Unit testing in PlatformIO is done using Unity by default.
- More info about Unity and additional lightweight C tools can be found at http://www.throwtheswitch.org
- Unity is targeted toward C and embedded C++ paradigm rather than say
gtest which is aimed more at a Linux OOP C++.
- Another tool to evaluate is https://cpputest.github.io/

## Folder structure
- Each directory under test/ is compiled as its own program which can be run natively or on a target.
- This offers advantages to test on a target device.
- Disadvantages are that it creates a repetitive folder structure that is disconnected from the lib and src folders.
- One option is to create a test runner but that builds in assumptions.
- Files in src/ are not included in tests by default.

For example:
```
|--lib
|  |
|  |--core
|  |  |--a.cpp
|  |  |--b.cpp
|  |--foo
|  |  |--foo.cpp

|--test
|  |
|  |--test_core_a
|  |  |--test_core_a.cpp
|  |--test_core_b
|  |  |--test_core_b.cpp
|  |--test_foo
|  |  |--test_foo.cpp
```
You can filter on tests, eg.
`pio test -e native -f "test_core*"`