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

#ifndef DEBUGFREEHOOK_H
#define DEBUGFREEHOOK_H

// These functions enable C library runtime hooks to try to catch cases where
// C++ object addresses remain in hash table of valid wrappers when the address
// is passed to free.  The hooks are probably not thread safe and thus
// should only be enabled in single threaded environments

// To enable the hook, uncomment the following define.
//#define SHIBOKEN_INSTALL_FREE_DEBUG_HOOK

#ifdef SHIBOKEN_INSTALL_FREE_DEBUG_HOOK
extern "C" {

void debugInstallFreeHook(void);
void debugRemoveFreeHook(void);

} // extern "C"

#endif // SHIBOKEN_INSTALL_FREE_DEBUG_HOOK

#endif // DEBUGFREEHOOK_H
