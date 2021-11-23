/*
 * This file is part of the API Extractor project.
 *
 * Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * Contact: PySide team <contact@pyside.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "typeparser.h"

#include <QtCore/QDebug>
#include <QtCore/QStack>

class Scanner
{
public:
    enum Token {
        StarToken,
        AmpersandToken,
        LessThanToken,
        ColonToken,
        CommaToken,
        OpenParenToken,
        CloseParenToken,
        SquareBegin,
        SquareEnd,
        GreaterThanToken,

        ConstToken,
        Identifier,
        NoToken
    };

    Scanner(const QString &s)
            : m_pos(0), m_length(s.length()), m_chars(s.constData())
    {
    }

    Token nextToken();
    QString identifier() const;

private:
    int m_pos;
    int m_length;
    int m_tokenStart;
    const QChar *m_chars;
};

QString Scanner::identifier() const
{
    return QString(m_chars + m_tokenStart, m_pos - m_tokenStart);
}

Scanner::Token Scanner::nextToken()
{
    Token tok = NoToken;

    // remove whitespace
    while (m_pos < m_length && m_chars[m_pos] == ' ')
        ++m_pos;

    m_tokenStart = m_pos;

    while (m_pos < m_length) {

        const QChar &c = m_chars[m_pos];

        if (tok == NoToken) {
            switch (c.toLatin1()) {
            case '*': tok = StarToken; break;
            case '&': tok = AmpersandToken; break;
            case '<': tok = LessThanToken; break;
            case '>': tok = GreaterThanToken; break;
            case ',': tok = CommaToken; break;
            case '(': tok = OpenParenToken; break;
            case ')': tok = CloseParenToken; break;
            case '[': tok = SquareBegin; break;
            case ']' : tok = SquareEnd; break;
            case ':':
                tok = ColonToken;
                Q_ASSERT(m_pos + 1 < m_length);
                ++m_pos;
                break;
            default:
                if (c.isLetterOrNumber() || c == '_')
                    tok = Identifier;
                else
                    qFatal("Unrecognized character in lexer: %c", c.toLatin1());
                break;
            }
        }

        if (tok <= GreaterThanToken) {
            ++m_pos;
            break;
        }

        if (tok == Identifier) {
            if (c.isLetterOrNumber() || c == '_')
                ++m_pos;
            else
                break;
        }
    }

    if (tok == Identifier && m_pos - m_tokenStart == 5) {
        if (m_chars[m_tokenStart] == 'c'
            && m_chars[m_tokenStart + 1] == 'o'
            && m_chars[m_tokenStart + 2] == 'n'
            && m_chars[m_tokenStart + 3] == 's'
            && m_chars[m_tokenStart + 4] == 't')
            tok = ConstToken;
    }

    return tok;

}

TypeParser::Info TypeParser::parse(const QString &str)
{
    Scanner scanner(str);

    Info info;
    QStack<Info *> stack;
    stack.push(&info);

    bool colon_prefix = false;
    bool in_array = false;
    QString array;

    Scanner::Token tok = scanner.nextToken();
    while (tok != Scanner::NoToken) {

//         switch (tok) {
//         case Scanner::StarToken: printf(" - *\n"); break;
//         case Scanner::AmpersandToken: printf(" - &\n"); break;
//         case Scanner::LessThanToken: printf(" - <\n"); break;
//         case Scanner::GreaterThanToken: printf(" - >\n"); break;
//         case Scanner::ColonToken: printf(" - ::\n"); break;
//         case Scanner::CommaToken: printf(" - ,\n"); break;
//         case Scanner::ConstToken: printf(" - const\n"); break;
//         case Scanner::SquareBegin: printf(" - [\n"); break;
//         case Scanner::SquareEnd: printf(" - ]\n"); break;
//         case Scanner::Identifier: printf(" - '%s'\n", qPrintable(scanner.identifier())); break;
//         default:
//             break;
//         }

        switch (tok) {

        case Scanner::StarToken:
            ++stack.top()->indirections;
            break;

        case Scanner::AmpersandToken:
            stack.top()->is_reference = true;
            break;

        case Scanner::LessThanToken:
            stack.top()->template_instantiations << Info();
            stack.push(&stack.top()->template_instantiations.last());
            break;

        case Scanner::CommaToken:
            stack.pop();
            stack.top()->template_instantiations << Info();
            stack.push(&stack.top()->template_instantiations.last());
            break;

        case Scanner::GreaterThanToken:
            stack.pop();
            break;

        case Scanner::ColonToken:
            colon_prefix = true;
            break;

        case Scanner::ConstToken:
            stack.top()->is_constant = true;
            break;

        case Scanner::OpenParenToken: // function pointers not supported
        case Scanner::CloseParenToken: {
            Info i;
            i.is_busted = true;
            return i;
        }


        case Scanner::Identifier:
            if (in_array) {
                array = scanner.identifier();
            } else if (colon_prefix || stack.top()->qualified_name.isEmpty()) {
                stack.top()->qualified_name << scanner.identifier();
                colon_prefix = false;
            } else {
                stack.top()->qualified_name.last().append(" " + scanner.identifier());
            }
            break;

        case Scanner::SquareBegin:
            in_array = true;
            break;

        case Scanner::SquareEnd:
            in_array = false;
            stack.top()->arrays += array;
            break;


        default:
            break;
        }

        tok = scanner.nextToken();
    }

    return info;
}

QString TypeParser::Info::instantiationName() const
{
    QString s(qualified_name.join("::"));
    if (!template_instantiations.isEmpty()) {
        QStringList insts;
        foreach (Info info, template_instantiations)
            insts << info.toString();
        s += QString("< %1 >").arg(insts.join(", "));
    }

    return s;
}

QString TypeParser::Info::toString() const
{
    QString s;

    if (is_constant) s += "const ";
    s += instantiationName();
    for (int i = 0; i < arrays.size(); ++i)
        s += "[" + arrays.at(i) + "]";
    s += QString(indirections, '*');
    if (is_reference)  s += '&';

    return s;
}
