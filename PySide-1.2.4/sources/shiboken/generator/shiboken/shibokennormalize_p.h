/*
 * This file is part of the PySide project.
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

#ifndef SHIBOKENNORMALIZE_P_H
#define SHIBOKENNORMALIZE_P_H

#include <QMetaObject>
#include <QByteArray>


#if (QT_VERSION < QT_VERSION_CHECK(4, 7, 0))
    QByteArray QMetaObject_normalizedTypeQt47(const char *type);
    QByteArray QMetaObject_normalizedSignatureQt47(const char *type);

    #define SBK_NORMALIZED_TYPE(x) QMetaObject_normalizedTypeQt47(x)
    #define SBK_NORMALIZED_SIGNATURE(x) QMetaObject_normalizedSignatureQt47(x)
#else
    #define SBK_NORMALIZED_TYPE(x) QMetaObject::normalizedType(x)
    #define SBK_NORMALIZED_SIGNATURE(x) QMetaObject::normalizedSignature(x)
#endif

#endif //SHIBOKENNORMALIZE_P_H
