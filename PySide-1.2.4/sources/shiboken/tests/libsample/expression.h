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


#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "libsamplemacros.h"
#include <string>

class LIBSAMPLE_API Expression
{
public:
    enum Operation {
        None, Add, Sub, LessThan, GreaterThan
    };

    Expression(int number);
    Expression(const Expression& other);
    Expression& operator=(const Expression& other);

    ~Expression();

    Expression operator>(const Expression& other);
    Expression operator<(const Expression& other);
    Expression operator+(const Expression& other);
    Expression operator-(const Expression& other);

    std::string toString() const;
private:
    int m_value;
    Operation m_operation;
    Expression* m_operand1;
    Expression* m_operand2;

    Expression();
};

#endif // EXPRESSION_H
