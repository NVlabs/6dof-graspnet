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

#include "sbkmodule.h"
#include "basewrapper.h"
#include "bindingmanager.h"

// TODO: for performance reasons this should be a sparse_hash_map,
// because there'll be very few modules as keys. The sparse_hash_map
// is missing from the code added in ../ext/sparsehash/google directory.
#include "google/dense_hash_map"

/// This hash maps module objects to arrays of Python types.
typedef google::dense_hash_map<PyObject*, PyTypeObject**> ModuleTypesMap;

/// This hash maps module objects to arrays of converters.
typedef google::dense_hash_map<PyObject*, SbkConverter**> ModuleConvertersMap;

/// All types produced in imported modules are mapped here.
static ModuleTypesMap moduleTypes;
static ModuleConvertersMap moduleConverters;

namespace Shiboken
{
namespace Module
{

void init()
{
    // Initializes type registry for modules.
    moduleTypes.set_empty_key((ModuleTypesMap::key_type)0);
    moduleTypes.set_deleted_key((ModuleTypesMap::key_type)1);
    moduleConverters.set_empty_key((ModuleConvertersMap::key_type)0);
    moduleConverters.set_deleted_key((ModuleConvertersMap::key_type)1);
}

PyObject* import(const char* moduleName)
{
    PyObject* sysModules = PyImport_GetModuleDict();
    PyObject* module = PyDict_GetItemString(sysModules, moduleName);
    if (module)
        Py_INCREF(module);
    else
        module = PyImport_ImportModule(moduleName);

    if (!module)
        PyErr_Format(PyExc_ImportError,"could not import module '%s'", moduleName);

    return module;
}

PyObject* create(const char* moduleName, void* moduleData)
{
    Shiboken::init();
#ifndef IS_PY3K
    return Py_InitModule(moduleName, (PyMethodDef*)moduleData);
#else
    return PyModule_Create(reinterpret_cast<PyModuleDef*>(moduleData));
#endif
}

void registerTypes(PyObject* module, PyTypeObject** types)
{
    ModuleTypesMap::iterator iter = moduleTypes.find(module);
    if (iter == moduleTypes.end())
        moduleTypes.insert(std::make_pair(module, types));
}

PyTypeObject** getTypes(PyObject* module)
{
    ModuleTypesMap::iterator iter = moduleTypes.find(module);
    return (iter == moduleTypes.end()) ? 0 : iter->second;
}

void registerTypeConverters(PyObject* module, SbkConverter** converters)
{
    ModuleConvertersMap::iterator iter = moduleConverters.find(module);
    if (iter == moduleConverters.end())
        moduleConverters.insert(std::make_pair(module, converters));
}

SbkConverter** getTypeConverters(PyObject* module)
{
    ModuleConvertersMap::iterator iter = moduleConverters.find(module);
    return (iter == moduleConverters.end()) ? 0 : iter->second;
}

} } // namespace Shiboken::Module
