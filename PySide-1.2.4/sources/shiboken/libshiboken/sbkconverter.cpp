/*
 * This file is part of the Shiboken Python Bindings Generator project.
 *
 * Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * Contact: PySide team <contact@pyside.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "sbkconverter.h"
#include "sbkconverter_p.h"
#include "basewrapper_p.h"
#include "google/dense_hash_map"
#include "autodecref.h"
#include "sbkdbg.h"

static SbkConverter** PrimitiveTypeConverters;

typedef google::dense_hash_map<std::string, SbkConverter*> ConvertersMap;
static ConvertersMap converters;

namespace Shiboken {
namespace Conversions {

void init()
{
    static SbkConverter* primitiveTypeConverters[] = {
        Primitive<PY_LONG_LONG>::createConverter(),
        Primitive<bool>::createConverter(),
        Primitive<char>::createConverter(),
        Primitive<const char*>::createConverter(),
        Primitive<double>::createConverter(),
        Primitive<float>::createConverter(),
        Primitive<int>::createConverter(),
        Primitive<long>::createConverter(),
        Primitive<short>::createConverter(),
        Primitive<signed char>::createConverter(),
        Primitive<std::string>::createConverter(),
        Primitive<unsigned PY_LONG_LONG>::createConverter(),
        Primitive<unsigned char>::createConverter(),
        Primitive<unsigned int>::createConverter(),
        Primitive<unsigned long>::createConverter(),
        Primitive<unsigned short>::createConverter(),
        Primitive<void*>::createConverter()
    };
    PrimitiveTypeConverters = primitiveTypeConverters;

    assert(converters.empty());
    converters.set_empty_key("");
    converters.set_deleted_key("?");
    converters["PY_LONG_LONG"] = primitiveTypeConverters[SBK_PY_LONG_LONG_IDX];
    converters["bool"] = primitiveTypeConverters[SBK_BOOL_IDX];
    converters["char"] = primitiveTypeConverters[SBK_CHAR_IDX];
    converters["const char *"] = primitiveTypeConverters[SBK_CONSTCHARPTR_IDX];
    converters["double"] = primitiveTypeConverters[SBK_DOUBLE_IDX];
    converters["float"] = primitiveTypeConverters[SBK_FLOAT_IDX];
    converters["int"] = primitiveTypeConverters[SBK_INT_IDX];
    converters["long"] = primitiveTypeConverters[SBK_LONG_IDX];
    converters["short"] = primitiveTypeConverters[SBK_SHORT_IDX];
    converters["signed char"] = primitiveTypeConverters[SBK_SIGNEDCHAR_IDX];
    converters["std::string"] = primitiveTypeConverters[SBK_STD_STRING_IDX];
    converters["unsigned PY_LONG_LONG"] = primitiveTypeConverters[SBK_UNSIGNEDPY_LONG_LONG_IDX];
    converters["unsigned char"] = primitiveTypeConverters[SBK_UNSIGNEDCHAR_IDX];
    converters["unsigned int"] = primitiveTypeConverters[SBK_UNSIGNEDINT_IDX];
    converters["unsigned long"] = primitiveTypeConverters[SBK_UNSIGNEDLONG_IDX];
    converters["unsigned short"] = primitiveTypeConverters[SBK_UNSIGNEDSHORT_IDX];
    converters["void*"] = primitiveTypeConverters[SBK_VOIDPTR_IDX];
}

static SbkConverter* createConverterObject(PyTypeObject* type,
                                           PythonToCppFunc toCppPointerConvFunc,
                                           IsConvertibleToCppFunc toCppPointerCheckFunc,
                                           CppToPythonFunc pointerToPythonFunc,
                                           CppToPythonFunc copyToPythonFunc)
{
    SbkConverter* converter = new SbkConverter;
    converter->pythonType = type;

    converter->pointerToPython = pointerToPythonFunc;
    converter->copyToPython = copyToPythonFunc;

    if (toCppPointerCheckFunc && toCppPointerConvFunc)
        converter->toCppPointerConversion = std::make_pair(toCppPointerCheckFunc, toCppPointerConvFunc);
    converter->toCppConversions.clear();

    return converter;
}

SbkConverter* createConverter(SbkObjectType* type,
                              PythonToCppFunc toCppPointerConvFunc,
                              IsConvertibleToCppFunc toCppPointerCheckFunc,
                              CppToPythonFunc pointerToPythonFunc,
                              CppToPythonFunc copyToPythonFunc)
{
    SbkConverter* converter = createConverterObject((PyTypeObject*)type,
                                                    toCppPointerConvFunc, toCppPointerCheckFunc,
                                                    pointerToPythonFunc, copyToPythonFunc);
    type->d->converter = converter;
    return converter;
}

SbkConverter* createConverter(PyTypeObject* type, CppToPythonFunc toPythonFunc)
{
    return createConverterObject(type, 0, 0, 0, toPythonFunc);
}

void deleteConverter(SbkConverter* converter)
{
    if (converter) {
        converter->toCppConversions.clear();
        delete converter;
    }
}

void setCppPointerToPythonFunction(SbkConverter* converter, CppToPythonFunc pointerToPythonFunc)
{
    converter->pointerToPython = pointerToPythonFunc;
}

void setPythonToCppPointerFunctions(SbkConverter* converter,
                                    PythonToCppFunc toCppPointerConvFunc,
                                    IsConvertibleToCppFunc toCppPointerCheckFunc)
{
    converter->toCppPointerConversion = std::make_pair(toCppPointerCheckFunc, toCppPointerConvFunc);
}

void addPythonToCppValueConversion(SbkConverter* converter,
                                   PythonToCppFunc pythonToCppFunc,
                                   IsConvertibleToCppFunc isConvertibleToCppFunc)
{
    converter->toCppConversions.push_back(std::make_pair(isConvertibleToCppFunc, pythonToCppFunc));
}
void addPythonToCppValueConversion(SbkObjectType* type,
                                   PythonToCppFunc pythonToCppFunc,
                                   IsConvertibleToCppFunc isConvertibleToCppFunc)
{
    addPythonToCppValueConversion(type->d->converter, pythonToCppFunc, isConvertibleToCppFunc);
}

PyObject* pointerToPython(SbkObjectType* type, const void* cppIn)
{
    return pointerToPython(type->d->converter, cppIn);
}

PyObject* pointerToPython(SbkConverter* converter, const void* cppIn)
{
    assert(converter);
    if (!cppIn)
        Py_RETURN_NONE;
    return converter->pointerToPython(cppIn);
}

PyObject* referenceToPython(SbkObjectType* type, const void* cppIn)
{
    return referenceToPython(type->d->converter, cppIn);
}

PyObject* referenceToPython(SbkConverter* converter, const void* cppIn)
{
    assert(cppIn);

    PyObject* pyOut = (PyObject*)BindingManager::instance().retrieveWrapper(cppIn);
    if (pyOut) {
        Py_INCREF(pyOut);
        return pyOut;
    }
    return converter->pointerToPython(cppIn);
}

static inline PyObject* CopyCppToPython(SbkConverter* converter, const void* cppIn)
{
    if (!cppIn)
        Py_RETURN_NONE;
    return converter->copyToPython(cppIn);
}
PyObject* copyToPython(SbkObjectType* type, const void* cppIn)
{
    return CopyCppToPython(type->d->converter, cppIn);
}
PyObject* copyToPython(SbkConverter* converter, const void* cppIn)
{
    return CopyCppToPython(converter, cppIn);
}

PythonToCppFunc isPythonToCppPointerConvertible(SbkObjectType* type, PyObject* pyIn)
{
    assert(pyIn);
    return type->d->converter->toCppPointerConversion.first(pyIn);
}

static inline PythonToCppFunc IsPythonToCppConvertible(SbkConverter* converter, PyObject* pyIn)
{
    assert(pyIn);
    ToCppConversionList& convs = converter->toCppConversions;
    for (ToCppConversionList::iterator conv = convs.begin(); conv != convs.end(); ++conv) {
        PythonToCppFunc toCppFunc = 0;
        if ((toCppFunc = (*conv).first(pyIn)))
            return toCppFunc;
    }
    return 0;
}
PythonToCppFunc isPythonToCppValueConvertible(SbkObjectType* type, PyObject* pyIn)
{
    return IsPythonToCppConvertible(type->d->converter, pyIn);
}
PythonToCppFunc isPythonToCppConvertible(SbkConverter* converter, PyObject* pyIn)
{
    return IsPythonToCppConvertible(converter, pyIn);
}

PythonToCppFunc isPythonToCppReferenceConvertible(SbkObjectType* type, PyObject* pyIn)
{
    if (pyIn != Py_None) {
        PythonToCppFunc toCpp = isPythonToCppPointerConvertible(type, pyIn);
        if (toCpp)
            return toCpp;
    }
    return isPythonToCppValueConvertible(type, pyIn);
}

void nonePythonToCppNullPtr(PyObject*, void* cppOut)
{
    assert(cppOut);
    *((void**)cppOut) = 0;
}

void* cppPointer(PyTypeObject* desiredType, SbkObject* pyIn)
{
    assert(pyIn);
    if (!ObjectType::checkType(desiredType))
        return pyIn;
    SbkObjectType* inType = (SbkObjectType*)Py_TYPE(pyIn);
    if (ObjectType::hasCast(inType))
        return ObjectType::cast(inType, pyIn, desiredType);
    return Object::cppPointer(pyIn, desiredType);
}

void pythonToCppPointer(SbkObjectType* type, PyObject* pyIn, void* cppOut)
{
    assert(type);
    assert(pyIn);
    assert(cppOut);
    *((void**)cppOut) = (pyIn == Py_None) ? 0 : cppPointer((PyTypeObject*)type, (SbkObject*)pyIn);
}

void pythonToCppPointer(SbkConverter* converter, PyObject* pyIn, void* cppOut)
{
    assert(converter);
    assert(pyIn);
    assert(cppOut);
    *((void**)cppOut) = (pyIn == Py_None) ? 0 : cppPointer((PyTypeObject*)converter->pythonType, (SbkObject*)pyIn);
}

static void _pythonToCppCopy(SbkConverter* converter, PyObject* pyIn, void* cppOut)
{
    assert(converter);
    assert(pyIn);
    assert(cppOut);
    PythonToCppFunc toCpp = IsPythonToCppConvertible(converter, pyIn);
    if (toCpp)
        toCpp(pyIn, cppOut);
}

void pythonToCppCopy(SbkObjectType* type, PyObject* pyIn, void* cppOut)
{
    assert(type);
    _pythonToCppCopy(type->d->converter, pyIn, cppOut);
}

void pythonToCppCopy(SbkConverter* converter, PyObject* pyIn, void* cppOut)
{
    _pythonToCppCopy(converter, pyIn, cppOut);
}

bool isImplicitConversion(SbkObjectType* type, PythonToCppFunc toCppFunc)
{
    // This is the Object Type or Value Type conversion that only
    // retrieves the C++ pointer held in the Python wrapper.
    if (toCppFunc == type->d->converter->toCppPointerConversion.second)
        return false;

    // Object Types doesn't have any kind of value conversion,
    // only C++ pointer retrieval.
    if (type->d->converter->toCppConversions.empty())
        return false;

    // The first conversion of the non-pointer conversion list is
    // a Value Type's copy to C++ function, which is not an implicit
    // conversion.
    // Otherwise it must be one of the implicit conversions.
    // Note that we don't check if the Python to C++ conversion is in
    // the list of the type's conversions, for it is expected that the
    // caller knows what he's doing.
    ToCppConversionList::iterator conv = type->d->converter->toCppConversions.begin();
    return toCppFunc != (*conv).second;
}

void registerConverterName(SbkConverter* converter , const char* typeName)
{
    ConvertersMap::iterator iter = converters.find(typeName);
    if (iter == converters.end())
        converters.insert(std::make_pair(typeName, converter));
}

SbkConverter* getConverter(const char* typeName)
{
    ConvertersMap::const_iterator it = converters.find(typeName);
    if (it != converters.end())
        return it->second;
    if (Py_VerboseFlag > 0)
        SbkDbg() << "Can't find type resolver for type '" << typeName << "'.";
    return 0;
}

SbkConverter* primitiveTypeConverter(int index)
{
    return PrimitiveTypeConverters[index];
}

bool checkSequenceTypes(PyTypeObject* type, PyObject* pyIn)
{
    assert(type);
    assert(pyIn);
    if (!PySequence_Check(pyIn))
        return false;
    int size = PySequence_Size(pyIn);
    for (int i = 0; i < size; ++i) {
        if (!PyObject_TypeCheck(AutoDecRef(PySequence_GetItem(pyIn, i)), type))
            return false;
    }
    return true;
}
bool convertibleSequenceTypes(SbkConverter* converter, PyObject* pyIn)
{
    assert(converter);
    assert(pyIn);
    if (!PySequence_Check(pyIn))
        return false;
    int size = PySequence_Size(pyIn);
    for (int i = 0; i < size; ++i) {
        if (!isPythonToCppConvertible(converter, AutoDecRef(PySequence_GetItem(pyIn, i))))
            return false;
    }
    return true;
}
bool convertibleSequenceTypes(SbkObjectType* type, PyObject* pyIn)
{
    assert(type);
    return convertibleSequenceTypes(type->d->converter, pyIn);
}

bool checkPairTypes(PyTypeObject* firstType, PyTypeObject* secondType, PyObject* pyIn)
{
    assert(firstType);
    assert(secondType);
    assert(pyIn);
    if (!PySequence_Check(pyIn))
        return false;
    if (PySequence_Size(pyIn) != 2)
        return false;
    if (!PyObject_TypeCheck(AutoDecRef(PySequence_GetItem(pyIn, 0)), firstType))
        return false;
    if (!PyObject_TypeCheck(AutoDecRef(PySequence_GetItem(pyIn, 1)), secondType))
        return false;
    return true;
}
bool convertiblePairTypes(SbkConverter* firstConverter, bool firstCheckExact, SbkConverter* secondConverter, bool secondCheckExact, PyObject* pyIn)
{
    assert(firstConverter);
    assert(secondConverter);
    assert(pyIn);
    if (!PySequence_Check(pyIn))
        return false;
    if (PySequence_Size(pyIn) != 2)
        return false;
    AutoDecRef firstItem(PySequence_GetItem(pyIn, 0));
    if (firstCheckExact) {
        if (!PyObject_TypeCheck(firstItem, firstConverter->pythonType))
            return false;
    } else if (!isPythonToCppConvertible(firstConverter, firstItem)) {
        return false;
    }
    AutoDecRef secondItem(PySequence_GetItem(pyIn, 1));
    if (secondCheckExact) {
        if (!PyObject_TypeCheck(secondItem, secondConverter->pythonType))
            return false;
    } else if (!isPythonToCppConvertible(secondConverter, secondItem)) {
        return false;
    }

    return true;
}

bool checkDictTypes(PyTypeObject* keyType, PyTypeObject* valueType, PyObject* pyIn)
{
    assert(keyType);
    assert(valueType);
    assert(pyIn);
    if (!PyDict_Check(pyIn))
        return false;

    PyObject* key;
    PyObject* value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(pyIn, &pos, &key, &value)) {
        if (!PyObject_TypeCheck(key, keyType))
            return false;
        if (!PyObject_TypeCheck(value, valueType))
            return false;
    }
    return true;
}

bool convertibleDictTypes(SbkConverter* keyConverter, bool keyCheckExact, SbkConverter* valueConverter, bool valueCheckExact, PyObject* pyIn)
{
    assert(keyConverter);
    assert(valueConverter);
    assert(pyIn);
    if (!PyDict_Check(pyIn))
        return false;
    PyObject* key;
    PyObject* value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(pyIn, &pos, &key, &value)) {
        if (keyCheckExact) {
            if (!PyObject_TypeCheck(key, keyConverter->pythonType))
                return false;
        } else if (!isPythonToCppConvertible(keyConverter, key)) {
            return false;
        }
        if (valueCheckExact) {
            if (!PyObject_TypeCheck(value, valueConverter->pythonType))
                return false;
        } else if (!isPythonToCppConvertible(valueConverter, value)) {
            return false;
        }
    }
    return true;
}

PyTypeObject* getPythonTypeObject(SbkConverter* converter)
{
    if (converter)
        return converter->pythonType;
    return 0;
}

PyTypeObject* getPythonTypeObject(const char* typeName)
{
    return getPythonTypeObject(getConverter(typeName));
}

bool pythonTypeIsValueType(SbkConverter* converter)
{
    assert(converter);
    return converter->pointerToPython && converter->copyToPython;
}

bool pythonTypeIsObjectType(SbkConverter* converter)
{
    return converter->pointerToPython && !converter->copyToPython;
}

bool pythonTypeIsWrapperType(SbkConverter* converter)
{
    return converter->pointerToPython;
}

SpecificConverter::SpecificConverter(const char* typeName)
    : m_type(InvalidConversion)
{
    m_converter = getConverter(typeName);
    if (!m_converter)
        return;
    int len = strlen(typeName);
    char lastChar = typeName[len -1];
    if (lastChar == '&') {
        m_type = ReferenceConversion;
    } else if (lastChar == '*' || pythonTypeIsObjectType(m_converter)) {
        m_type = PointerConversion;
    } else {
        m_type = CopyConversion;
    }
}

PyObject* SpecificConverter::toPython(const void* cppIn)
{
    switch (m_type) {
    case CopyConversion:
        return copyToPython(m_converter, cppIn);
    case PointerConversion:
        return pointerToPython(m_converter, *((const void**)cppIn));
    case ReferenceConversion:
        return referenceToPython(m_converter, cppIn);
    default:
        PyErr_SetString(PyExc_RuntimeError, "tried to use invalid converter in 'C++ to Python' conversion");
    }
    return 0;
}

void SpecificConverter::toCpp(PyObject* pyIn, void* cppOut)
{
    switch (m_type) {
    case CopyConversion:
        pythonToCppCopy(m_converter, pyIn, cppOut);
        break;
    case PointerConversion:
        pythonToCppPointer(m_converter, pyIn, cppOut);
        break;
    case ReferenceConversion:
        pythonToCppPointer(m_converter, pyIn, &cppOut);
        break;
    default:
        PyErr_SetString(PyExc_RuntimeError, "tried to use invalid converter in 'Python to C++' conversion");
    }
}

} } // namespace Shiboken::Conversions
