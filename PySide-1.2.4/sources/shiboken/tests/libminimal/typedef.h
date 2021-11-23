#ifndef TYPEDEF_H
#define TYPEDEF_H

#include "libminimalmacros.h"

#include <vector>

// Test wrapping of a typedef
typedef std::vector<int> MyArrayInt;

LIBMINIMAL_API bool arrayFuncInt(std::vector<int> a);
LIBMINIMAL_API bool arrayFuncIntTypedef(MyArrayInt a);

LIBMINIMAL_API std::vector<int> arrayFuncIntReturn(int size);
LIBMINIMAL_API MyArrayInt arrayFuncIntReturnTypedef(int size);

// Test wrapping of a typedef of a typedef
typedef MyArrayInt MyArray;

LIBMINIMAL_API bool arrayFunc(std::vector<int> a);
LIBMINIMAL_API bool arrayFuncTypedef(MyArray a);

LIBMINIMAL_API std::vector<int> arrayFuncReturn(int size);
LIBMINIMAL_API MyArray arrayFuncReturnTypedef(int size);

#endif
