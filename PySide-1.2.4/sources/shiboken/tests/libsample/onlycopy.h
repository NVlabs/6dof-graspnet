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

#ifndef ONLYCOPYCLASS_H
#define ONLYCOPYCLASS_H

#include "libsamplemacros.h"
#include <list>

// These classes simulate a situation found in
// QtWebKit's QWebDatabase and QWebSecurityOrigin.

class LIBSAMPLE_API OnlyCopy
{
public:
    OnlyCopy(const OnlyCopy& other);
    OnlyCopy& operator=(const OnlyCopy& other);
    int value() const { return m_value; }
    static int getValue(OnlyCopy onlyCopy) { return onlyCopy.m_value; }
    static int getValueFromReference(const OnlyCopy& onlyCopy) { return onlyCopy.m_value; }
private:
    int m_value;
    OnlyCopy(int value) : m_value(value) {};
    friend class FriendOfOnlyCopy;
};

class LIBSAMPLE_API FriendOfOnlyCopy
{
public:
    static OnlyCopy createOnlyCopy(int value);
    static std::list<OnlyCopy> createListOfOnlyCopy(int quantity);
};

#endif
