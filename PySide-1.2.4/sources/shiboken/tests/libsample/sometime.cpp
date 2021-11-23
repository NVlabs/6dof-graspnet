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

#include "sometime.h"
#include <stdio.h>

void
Time::setTime()
{
    m_hour = 0;
    m_minute = 0;
    m_second = 0;
    m_msec = 0;
    m_is_null = true;
}

void
Time::setTime(int h, int m, int s, int ms)
{
    m_hour = h;
    m_minute = m;
    m_second = s;
    m_msec = ms;
    m_is_null = false;
}


Time::NumArgs
Time::somethingCompletelyDifferent()
{
    return ZeroArgs;
}

Time::NumArgs
Time::somethingCompletelyDifferent(int h, int m, ImplicitConv ic, ObjectType* type)
{
    if (type)
        return FourArgs;
    if (ic.ctorEnum() == ImplicitConv::CtorThree && ic.objId() == -1)
        return TwoArgs;
    return ThreeArgs;
}

Str
Time::toString() const
{
    if (m_is_null)
        return Str();
    char buffer[13];
    sprintf(buffer, "%02d:%02d:%02d.%03d", m_hour, m_minute, m_second, m_msec);
    return Str(buffer);
}

bool
Time::operator==(const Time& other) const
{
    return m_hour == other.m_hour
            && m_minute == other.m_minute
            && m_second == other.m_second
            && m_msec == other.m_msec
            && m_is_null == m_is_null;
}

bool
Time::operator!=(const Time& other) const
{
    return !operator==(other);
}

Time::operator Str() const
{
    return Time::toString();
}

