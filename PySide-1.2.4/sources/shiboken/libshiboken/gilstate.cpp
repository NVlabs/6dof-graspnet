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

#include "gilstate.h"

namespace Shiboken
{

GilState::GilState()
    : m_locked(false)
{
    if (Py_IsInitialized()) {
        m_gstate = PyGILState_Ensure();
        m_locked = true;
    }
}

GilState::~GilState()
{
    release();
}

void GilState::release()
{
    if (m_locked && Py_IsInitialized()) {
        PyGILState_Release(m_gstate);
        m_locked = false;
    }
}

} // namespace Shiboken

