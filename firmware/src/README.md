# Style

This project uses a lightweight C++ style to target a wide range of embedded devices. The C++11 standard is used however the style leans toward C++03 and even ANSI C. C++11 is what is known by developers today but there is some delay in safety standards, which will hopefully catch up soon. MISRA C++ was published in 2008 and uses C++03. However AUTOSAR C++ has published guidance up to C++14. The style recommendation is to keep it simple and as C++03 as possible, but handy C++11 features such as nullptr, default and delete are OK to use.

Most of the STL, dynamic memory allocation, lamda functions and smart pointers shouldn't be used. Everything should be allocated at compile time. Virtual interfaces can be used sparingly but prefer to use templates where possible. Macros should not be used. Pointer arithmetic should be avoided unless strictly necessary. Try not to modify default operator behaviour as it can be unexpected and confusing to others. Exceptions are not used although alternative error handling patterns are being investigated. Auto shouldn't be used. Specific types should be used, eg. uint32_t instead of int. Not all microcontrollers support floating point (although it is popular) so a fixed point system is used.

A brief overview on MISRA and AUTOSAR: https://pvs-studio.com/en/blog/posts/cpp/0702/

A simple comparison of C++98: https://www.cplusplus.com/doc/oldtutorial/ vs C++11 https://www.cplusplus.com/doc/tutorial/

