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

#ifndef COMPLEX_H
#define COMPLEX_H

#include "libsamplemacros.h"

class LIBSAMPLE_API Complex
{
public:
    Complex(double real = 0.0, double imag = 0.0);
    ~Complex() {}

    inline double real() const { return m_real; }
    inline void setReal(double real) { m_real = real; }
    inline double imag() const { return m_imag; }
    inline void setImaginary(double imag) { m_imag = imag; }

    Complex operator+(Complex& other);

    void show();

private:
    double m_real;
    double m_imag;
};

#endif // COMPLEX_H

