/*
 * This file is part of the PySide Tools project.
 *
 * Copyright (C) 1992-2005 Trolltech AS. All rights reserved.
 * Copyright (C) 2002-2007 Detlev Offenbach <detlev@die-offenbachs.de>
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: PySide team <pyside@openbossa.org>
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

#ifndef METATRANSLATOR_H
#define METATRANSLATOR_H

#include <qmap.h>
#include <qstring.h>
#include <translator.h>
#include <qlist.h>
#include <qlocale.h>
#include <qdir.h>

class QTextCodec;

class MetaTranslatorMessage : public TranslatorMessage
{
public:
    enum Type { Unfinished, Finished, Obsolete };

    MetaTranslatorMessage();
    MetaTranslatorMessage( const char *context, const char *sourceText,
                           const char *comment, 
                           const QString &fileName, int lineNumber,
                           const QStringList& translations = QStringList(),
                           bool utf8 = false, Type type = Unfinished,
                           bool plural = false );
    MetaTranslatorMessage( const MetaTranslatorMessage& m );

    MetaTranslatorMessage& operator=( const MetaTranslatorMessage& m );

    void setType( Type nt ) { ty = nt; }
    Type type() const { return ty; }
    bool utf8() const { return utfeight; }
    bool isPlural() const { return m_plural; }
    void setPlural(bool isplural) { m_plural = isplural; }

    bool operator==( const MetaTranslatorMessage& m ) const;
    bool operator!=( const MetaTranslatorMessage& m ) const
    { return !operator==( m ); }
    bool operator<( const MetaTranslatorMessage& m ) const;
    bool operator<=( const MetaTranslatorMessage& m )
    { return !operator>( m ); }
    bool operator>( const MetaTranslatorMessage& m ) const
    { return this->operator<( m ); }
    bool operator>=( const MetaTranslatorMessage& m ) const
    { return !operator<( m ); }

private:
    bool utfeight;
    Type ty;
    bool m_plural;
};

class MetaTranslator
{
public:
    MetaTranslator();
    MetaTranslator( const MetaTranslator& tor );

    MetaTranslator& operator=( const MetaTranslator& tor );

    void clear();
    bool load( const QString& filename );
    bool save( const QString& filename ) const;
    bool release( const QString& filename, bool verbose = false,
                  bool ignoreUnfinished = false,
                  Translator::SaveMode mode = Translator::Stripped ) const;

    bool contains( const char *context, const char *sourceText,
                   const char *comment ) const;

    MetaTranslatorMessage find( const char *context, const char *sourceText,
                   const char *comment ) const;
    MetaTranslatorMessage find(const char *context, const char *comment, 
                    const QString &fileName, int lineNumber) const;

    void insert( const MetaTranslatorMessage& m );

    void stripObsoleteMessages();
    void stripEmptyContexts();

    void setCodec( const char *name ); // kill me
    void setCodecForTr( const char *name ) { setCodec(name); }
    QTextCodec *codecForTr() const { return codec; }
    QString toUnicode( const char *str, bool utf8 ) const;

    QString languageCode() const;
    static void languageAndCountry(const QString &languageCode, QLocale::Language *lang, QLocale::Country *country);
    void setLanguageCode(const QString &languageCode);
    QList<MetaTranslatorMessage> messages() const;
    QList<MetaTranslatorMessage> translatedMessages() const;
    static int grammaticalNumerus(QLocale::Language language, QLocale::Country country);
    static QStringList normalizedTranslations(const MetaTranslatorMessage& m,
                    QLocale::Language lang, QLocale::Country country);

private:
    void makeFileNamesAbsolute(const QDir &oldPath);

    typedef QMap<MetaTranslatorMessage, int> TMM;
    typedef QMap<int, MetaTranslatorMessage> TMMInv;

    TMM mm;
    QByteArray codecName;
    QTextCodec *codec;
    QString m_language;     // A string beginning with a 2 or 3 letter language code (ISO 639-1 or ISO-639-2),
                            // followed by the optional country variant to distinguist between country-specific variations
                            // of the language. The language code and country code are always separated by '_'
                            // Note that the language part can also be a 3-letter ISO 639-2 code.
                            // Legal examples:
                            // 'pt'         portuguese, assumes portuguese from portugal
                            // 'pt_BR'      Brazilian portuguese (ISO 639-1 language code)
                            // 'por_BR'     Brazilian portuguese (ISO 639-2 language code)
};

/*
  This is a quick hack. The proper way to handle this would be
  to extend MetaTranslator's interface.
*/
#define ContextComment "QT_LINGUIST_INTERNAL_CONTEXT_COMMENT"

#endif
