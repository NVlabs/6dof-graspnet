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

#include "debugfreehook.h"
#include "bindingmanager.h"
#include "gilstate.h"

#if defined(_WIN32) && defined(_DEBUG)
#include <crtdbg.h>
#include <windows.h>
#endif

#ifdef __GLIBC__
#include <malloc.h>
#endif

#ifdef __APPLE__
#include <malloc/malloc.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>
#endif

#ifdef SHIBOKEN_INSTALL_FREE_DEBUG_HOOK
extern "C" {

static int testPointerBeingFreed(void *ptr)
{
    // It is an error for a deleted pointer address to still be registered
    // in the BindingManager
    if (Shiboken::BindingManager::instance().hasWrapper(ptr)) {
        Shiboken::GilState state;

        SbkObject *wrapper = Shiboken::BindingManager::instance().retrieveWrapper(ptr);

        fprintf(stderr, "SbkObject still in binding map when deleted: ");
        PyObject_Print((PyObject*)wrapper, stderr, 0);
        fprintf(stderr, "\n");

#ifdef _WIN32
        DebugBreak();
#else
        assert(0);
#endif
        return FALSE;
    }

    return TRUE;
}

#if defined(_WIN32) && defined(_DEBUG)
static _CRT_ALLOC_HOOK lastCrtAllocHook;
static int DebugAllocHook(int nAllocType, void *pvData,
                          size_t nSize, int nBlockUse, long lRequest,
                          const unsigned char * szFileName, int nLine)
{
    // It is an error for a deleted pointer address to still be registered
    // in the BindingManager
    if ( nAllocType == _HOOK_FREE) {
        if ( !testPointerBeingFreed(pvData) ) {
            return 0;
        }
    }

    if ( lastCrtAllocHook != NULL ) {
        return lastCrtAllocHook(nAllocType, pvData, nSize, nBlockUse, lRequest,
                                szFileName, nLine);
    }

    return 1;
}
#endif // _WIN32 && _DEBUG

#ifdef __GLIBC__
static void (*lastFreeHook)(void* ptr, const void* caller);
static void DebugFreeHook(void* ptr, const void* caller)
{
    testPointerBeingFreed(ptr);

    if ( lastFreeHook != NULL )
        lastFreeHook(ptr, caller);
}
#endif // __GLIBC__

#ifdef __APPLE__
static malloc_zone_t lastMallocZone;
static void DebugFreeHook(malloc_zone_t *zone, void *ptr)
{
    testPointerBeingFreed(ptr);

    if ( lastMallocZone.free != NULL )
        lastMallocZone.free(zone, ptr);
}
static void DebugFreeDefiniteSizeHook(malloc_zone_t *zone, void *ptr, size_t size)
{
    testPointerBeingFreed(ptr);

    if ( lastMallocZone.free_definite_size != NULL )
        lastMallocZone.free_definite_size(zone, ptr, size);
}
#endif __APPLE__

void debugInstallFreeHook(void)
{
#if defined(_WIN32) && defined(_DEBUG)
    lastCrtAllocHook = _CrtSetAllocHook(DebugAllocHook);
#endif

#ifdef __GLIBC__
    // __free_hook is not thread safe so it marked as deprecated.  Use here
    // is hopefully safe and should catch errors in a single threaded program
    // and only miss some in a multithreaded program
    lastFreeHook = __free_hook;
    __free_hook = DebugFreeHook;
#endif

#ifdef __APPLE__
    malloc_zone_t* zone = malloc_default_zone();
    assert(zone != NULL);
    //remove the write protection from the zone struct
    if (zone->version >= 8) {
      vm_protect(mach_task_self(), (uintptr_t)zone, sizeof(*zone), 0, VM_PROT_READ | VM_PROT_WRITE);
    }
    lastMallocZone = *zone;
    zone->free = DebugFreeHook;
    zone->free_definite_size = DebugFreeDefiniteSizeHook;
    if (zone->version >= 8) {
      vm_protect(mach_task_self(), (uintptr_t)zone, sizeof(*zone), 0, VM_PROT_READ);
    }
#endif
}

void debugRemoveFreeHook(void)
{
#if defined(_WIN32) && defined(_DEBUG)
    _CrtSetAllocHook(lastCrtAllocHook);
#endif

#ifdef __GLIBC__
    __free_hook = lastFreeHook;
#endif

#ifdef __APPLE__
    malloc_zone_t* zone = malloc_default_zone();
    assert(zone != NULL);
    //remove the write protection from the zone struct
    if (zone->version >= 8) {
      vm_protect(mach_task_self(), (uintptr_t)zone, sizeof(*zone), 0, VM_PROT_READ | VM_PROT_WRITE);
    }
    zone->free = lastMallocZone.free;
    zone->free_definite_size = lastMallocZone.free_definite_size;
    if (zone->version >= 8) {
      vm_protect(mach_task_self(), (uintptr_t)zone, sizeof(*zone), 0, VM_PROT_READ);
    }
#endif
}

} // extern "C"
#endif // SHIBOKEN_INSTALL_DEBUG_FREE_HOOK
