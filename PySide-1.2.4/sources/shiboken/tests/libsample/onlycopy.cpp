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

#include "onlycopy.h"

OnlyCopy::OnlyCopy(const OnlyCopy& other)
{
    m_value = other.m_value;
}

OnlyCopy&
OnlyCopy::operator=(const OnlyCopy& other)
{
    m_value = other.m_value;
    return *this;
}

OnlyCopy
FriendOfOnlyCopy::createOnlyCopy(int value)
{

    return OnlyCopy(value);
}

std::list<OnlyCopy>
FriendOfOnlyCopy::createListOfOnlyCopy(int quantity)
{
    std::list<OnlyCopy> list;
    for (int i = 0; i < quantity; ++i)
        list.push_back(createOnlyCopy(i));
    return list;
}
