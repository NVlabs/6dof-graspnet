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

#ifndef MDERIVED_H
#define MDERIVED_H

#include "libsamplemacros.h"
#include <string>

class Base1
{
public:
    Base1() : m_value(1) {}
    virtual ~Base1() {}
    virtual int base1Method() { return m_value; }

    virtual void publicMethod() {};
private:
    int m_value;
};

class Base2
{
public:
    Base2() : m_value(2) {}
    virtual ~Base2() {}
    virtual int base2Method() { return m_value; }
private:
    int m_value;
};

class LIBSAMPLE_API MDerived1 : public Base1, public Base2
{
public:
    MDerived1();
    virtual ~MDerived1() {}

    virtual int mderived1Method() { return m_value; }
    virtual int base1Method() { return Base1::base1Method() * 10; }
    virtual int base2Method() { return Base2::base2Method() * 10; }

    inline Base1* castToBase1() { return (Base1*) this; }
    inline Base2* castToBase2() { return (Base2*) this; }

    static MDerived1* transformFromBase1(Base1 *self);
    static MDerived1* transformFromBase2(Base2 *self);

private:
    virtual void publicMethod() {}
    int m_value;
};

class SonOfMDerived1 : public MDerived1
{
public:
    SonOfMDerived1() : m_value(0) {}
    ~SonOfMDerived1() {}

    inline MDerived1* castToMDerived1() { return (MDerived1*) this; }

    int sonOfMDerived1Method() { return m_value; }
private:
    int m_value;
};

class Base3
{
public:
    explicit Base3(int val = 3) : m_value(val) {}
    virtual ~Base3() {}
    int base3Method() { return m_value; }
private:
    int m_value;
};

class Base4
{
public:
    Base4() : m_value(4) {}
    virtual ~Base4() {}
    int base4Method() { return m_value; }
private:
    int m_value;
};

class Base5
{
public:
    Base5() : m_value(5) {}
    virtual ~Base5() {}
    virtual int base5Method() { return m_value; }
private:
    int m_value;
};

class Base6
{
public:
    Base6() : m_value(6) {}
    virtual ~Base6() {}
    virtual int base6Method() { return m_value; }
private:
    int m_value;
};


class LIBSAMPLE_API MDerived2 : public Base3, public Base4, public Base5, public Base6
{
public:
    MDerived2();
    virtual ~MDerived2() {}

    inline int base4Method() { return Base3::base3Method() * 10; }
    inline int mderived2Method() { return m_value; }

    inline Base3* castToBase3() { return (Base3*) this; }
    inline Base4* castToBase4() { return (Base4*) this; }
    inline Base5* castToBase5() { return (Base5*) this; }
    inline Base6* castToBase6() { return (Base6*) this; }

private:
    int m_value;
};

class LIBSAMPLE_API MDerived3 : public MDerived1, public MDerived2
{
public:
    MDerived3();
    virtual ~MDerived3() {}

    inline virtual int mderived3Method() { return m_value; }

    inline MDerived1* castToMDerived1() { return (MDerived1*) this; }
    inline MDerived2* castToMDerived2() { return (MDerived2*) this; }

    inline Base3* castToBase3() { return (Base3*) this; }

private:
    int m_value;
};

class LIBSAMPLE_API MDerived4 : public Base3, public Base4
{
public:
    MDerived4();
    ~MDerived4() {}

    inline int mderived4Method() { return 0; }

    inline Base3* castToBase3() { return (Base3*) this; }
    inline Base4* castToBase4() { return (Base4*) this; }
private:
    int m_value;
};

class LIBSAMPLE_API MDerived5 : public Base3, public Base4
{
public:
    MDerived5();
    virtual ~MDerived5() {}

    virtual int mderived5Method() { return 0; }

    inline Base3* castToBase3() { return (Base3*) this; }
    inline Base4* castToBase4() { return (Base4*) this; }
};

#endif // MDERIVED_H

