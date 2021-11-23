/*
 * This file is part of the Shiboken Python Binding Generator project.
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

#ifndef PHOTON_H
#define PHOTON_H

#include <list>
#include "libsamplemacros.h"

// This namespace and classes simulate
// situations found in Qt's phonon module.

namespace Photon
{

enum ClassType {
    BaseType = 0,
    IdentityType = 1,
    DuplicatorType = 2
};

class LIBSAMPLE_API Base
{
public:
    explicit Base(int value) : m_value(value) {}
    virtual ~Base() {}
    inline void setValue(int value) { m_value = value; }
    inline int value() const { return m_value; }

    template <class T> bool isType() { return type() == T::staticType; }
    bool isType(ClassType t) { return type() == t; }

    virtual ClassType type() const { return BaseType; };
    static const ClassType staticType = BaseType;

protected:
    int m_value;
};

template<ClassType CLASS_TYPE>
class LIBSAMPLE_API TemplateBase : public Base
{
public:
    explicit TemplateBase(int value) : Base(value) {}
    inline int multiplicator() const { return (int)CLASS_TYPE; }
    inline int calculate() const { return m_value * ((int)CLASS_TYPE); }
    static inline ClassType classType() { return CLASS_TYPE; }

    inline int sumValueUsingPointer(TemplateBase<CLASS_TYPE>* other) const { return m_value + other->m_value; }
    inline int sumValueUsingReference(TemplateBase<CLASS_TYPE>& other) const { return m_value + other.m_value; }

    inline std::list<TemplateBase<CLASS_TYPE> > getListOfThisTemplateBase()
    {
        std::list<TemplateBase<CLASS_TYPE> > objs;
        objs.push_back(*this);
        objs.push_back(*this);
        return objs;
    }

    static inline TemplateBase<CLASS_TYPE>* passPointerThrough(TemplateBase<CLASS_TYPE>* obj) { return obj; }

    virtual ClassType type() const { return CLASS_TYPE; }
    static const ClassType staticType = CLASS_TYPE;
};

#if defined _WIN32 || defined __CYGWIN__
template class LIBSAMPLE_API TemplateBase<IdentityType>;
template class LIBSAMPLE_API TemplateBase<DuplicatorType>;
#endif

typedef TemplateBase<IdentityType> ValueIdentity;
typedef TemplateBase<DuplicatorType> ValueDuplicator;

LIBSAMPLE_API int callCalculateForValueDuplicatorPointer(ValueDuplicator* value);
LIBSAMPLE_API int callCalculateForValueDuplicatorReference(ValueDuplicator& value);
LIBSAMPLE_API int countValueIdentities(const std::list<ValueIdentity>& values);
LIBSAMPLE_API int countValueDuplicators(const std::list<TemplateBase<DuplicatorType> >& values);

// This simulates an internal error (SEGV) caused by 'noexcept' in
// boost::intrusive_ptr before support for 'noexcept' was added. The ENTIRE
// code below is needed to trigger the exception; it isn't seen with just a
// 'noexcept' following a declaration.
//
// NOTE: For reasons that should be fairly obvious, this test unfortunately can
//       only be "run" when building in C++11 mode.
#if __cplusplus < 201103L
#define noexcept
#endif
class Pointer
{
public:
    Pointer() noexcept : px(0) {}
    Pointer(int* p) : px(p) {}

    void reset() noexcept { Pointer().swap(*this); }

    int* get() const noexcept { return px; }
    int& operator*() const { return *px; }

    void swap(Pointer& rhs) noexcept
    {
        int* tmp = px;
        px = rhs.px;
        rhs.px = tmp;
    }

private:
    int* px;
};

} // namespace Photon

#endif // PHOTON_H
