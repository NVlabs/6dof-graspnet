#include "typedef.h"

//
// Test wrapping of a typedef
//
bool arrayFuncInt(std::vector<int> a)
{
    return a.empty();
}

bool arrayFuncIntTypedef(MyArray a)
{
    return arrayFuncInt(a);
}

std::vector<int> arrayFuncIntReturn(int size)
{
    return std::vector<int>(size);
}

MyArray arrayFuncIntReturnTypedef(int size)
{
    return arrayFuncIntReturn(size);
}

//
// Test wrapping of a typedef of a typedef
//
bool arrayFunc(std::vector<int> a)
{
    return a.empty();
}

bool arrayFuncTypedef(MyArray a)
{
    return arrayFunc(a);
}

std::vector<int> arrayFuncReturn(int size)
{
    return std::vector<int>(size);
}

MyArray arrayFuncReturnTypedef(int size)
{
    return arrayFuncReturn(size);
}
