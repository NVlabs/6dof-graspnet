#ifndef APIEXTRACTORMACROS_H
#define APIEXTRACTORMACROS_H


// APIEXTRACTOR_API is used for the public API symbols.
#if defined _WIN32
    #define APIEXTRACTOR_DEPRECATED(func) __declspec(deprecated) func
#elif __GNUC__ >= 4
    #define APIEXTRACTOR_DEPRECATED(func) func __attribute__ ((deprecated))
#else
    #define APIEXTRACTOR_DEPRECATED(func) func
#endif
#endif
