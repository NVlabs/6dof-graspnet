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


#include "expression.h"
#include <sstream>

Expression::Expression() : m_value(0), m_operation(None), m_operand1(0), m_operand2(0)
{
}

Expression::Expression(int number) : m_value(number), m_operation(None), m_operand1(0), m_operand2(0)
{
}

Expression::Expression(const Expression& other)
{
    m_operand1 = other.m_operand1 ? new Expression(*other.m_operand1) : 0;
    m_operand2 = other.m_operand2 ? new Expression(*other.m_operand2) : 0;
    m_value = other.m_value;
    m_operation = other.m_operation;
}

Expression& Expression::operator=(const Expression& other)
{
    delete m_operand1;
    delete m_operand2;
    m_operand1 = other.m_operand1 ? new Expression(*other.m_operand1) : 0;
    m_operand2 = other.m_operand2 ? new Expression(*other.m_operand2) : 0;
    m_operation = other.m_operation;
    m_value = other.m_value;
    return *this;
}

Expression::~Expression()
{
    delete m_operand1;
    delete m_operand2;
}

Expression Expression::operator+(const Expression& other)
{
    Expression expr;
    expr.m_operation = Add;
    expr.m_operand1 = new Expression(*this);
    expr.m_operand2 = new Expression(other);
    return expr;
}

Expression Expression::operator-(const Expression& other)
{
    Expression expr;
    expr.m_operation = Add;
    expr.m_operand1 = new Expression(*this);
    expr.m_operand2 = new Expression(other);
    return expr;
}

Expression Expression::operator<(const Expression& other)
{
    Expression expr;
    expr.m_operation = LessThan;
    expr.m_operand1 = new Expression(*this);
    expr.m_operand2 = new Expression(other);
    return expr;
}

Expression Expression::operator>(const Expression& other)
{
    Expression expr;
    expr.m_operation = GreaterThan;
    expr.m_operand1 = new Expression(*this);
    expr.m_operand2 = new Expression(other);
    return expr;
}

std::string Expression::toString() const
{
    if (m_operation == None) {
        std::ostringstream s;
        s << m_value;
        return s.str();
    }

    std::string result;
    result += '(';
    result += m_operand1->toString();
    char op;
    switch (m_operation) {
        case Add:
            op = '+';
            break;
        case Sub:
            op = '-';
            break;
        case LessThan:
            op = '<';
            break;
        case GreaterThan:
            op = '<';
            break;
        case None: // just to avoid the compiler warning
        default:
            op = '?';
            break;
    }
    result += op;
    result += m_operand2->toString();
    result += ')';
    return result;
}

