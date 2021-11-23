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
#include "docparser.h"
#include <QtCore/QDebug>
#include <QtXmlPatterns/QXmlQuery>
#include <QBuffer>

#include <cstdlib>
#include <libxslt/xsltutils.h>
#include <libxslt/transform.h>

DocParser::DocParser()
{
    xmlSubstituteEntitiesDefault(1);
}

DocParser::~DocParser()
{
}

QString DocParser::getDocumentation(QXmlQuery& xquery, const QString& query,
                                    const DocModificationList& mods) const
{
    QString doc = execXQuery(xquery, query);
    return applyDocModifications(mods, doc);
}

QString DocParser::execXQuery(QXmlQuery& xquery, const QString& query) const
{
    QString escapedQuery(query);
    // XQuery can't have invalid XML characters
    escapedQuery.replace("&", "&amp;").replace("<", "&lt;");
    xquery.setQuery(escapedQuery);
    if (!xquery.isValid()) {
        qWarning() << "Bad XQuery: " << escapedQuery;
        return QString();
    }

    QString result;
    xquery.evaluateTo(&result);
    return result;
}

namespace
{

struct XslResources
{
    xmlDocPtr xmlDoc;
    xsltStylesheetPtr xslt;
    xmlDocPtr xslResult;

    XslResources() : xmlDoc(0), xslt(0), xslResult(0) {}

    ~XslResources()
    {
        if (xslt)
            xsltFreeStylesheet(xslt);

        if (xslResult)
            xmlFreeDoc(xslResult);

        if (xmlDoc)
            xmlFreeDoc(xmlDoc);

        xsltCleanupGlobals();
        xmlCleanupParser();
    }
};

} // namespace

QString DocParser::applyDocModifications(const DocModificationList& mods, const QString& xml) const
{
    if (mods.isEmpty())
        return xml;

    bool hasXPathBasedModification = false;
    foreach (DocModification mod, mods) {
        if (mod.mode() == DocModification::XPathReplace) {
            hasXPathBasedModification = true;
            break;
        }
    }

    if (!hasXPathBasedModification)
        return xml;

    QString xsl = QLatin1String("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<xsl:transform version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">\n"
                                "<xsl:template match=\"/\">\n"
                                "    <xsl:apply-templates />\n"
                                "</xsl:template>\n"
                                "<xsl:template match=\"*\">\n"
                                "<xsl:copy>\n"
                                "    <xsl:copy-of select=\"@*\"/>\n"
                                "    <xsl:apply-templates/>\n"
                                "</xsl:copy>\n"
                                "</xsl:template>\n"
                               );
    foreach (DocModification mod, mods) {
        if (mod.mode() == DocModification::XPathReplace) {
            xsl += QLatin1String("<xsl:template match=\"")
                   + mod.xpath().replace("\"", "&quot;") + QLatin1String("\">")
                   + mod.code() + QLatin1String("</xsl:template>\n");
        }
    }
    xsl += QLatin1String("</xsl:transform>");

    XslResources res;
    // Read XML data
    QByteArray xmlData = xml.toUtf8();
    res.xmlDoc = xmlParseMemory(xmlData.constData(), xmlData.size());
    if (!res.xmlDoc)
        return xml;

    // Read XSL data as a XML file
    QByteArray xslData = xsl.toUtf8();
    // xsltFreeStylesheet will delete this pointer
    xmlDocPtr xslDoc = xmlParseMemory(xslData.constData(), xslData.size());
    if (!xslDoc)
        return xml;

    // Parse XSL data
    res.xslt = xsltParseStylesheetDoc(xslDoc);
    if (!res.xslt)
        return xml;

    // Apply XSL
    res.xslResult = xsltApplyStylesheet(res.xslt, res.xmlDoc, 0);
    xmlChar* buffer = 0;
    int bufferSize;
    QString result;
    if (!xsltSaveResultToString(&buffer, &bufferSize, res.xslResult, res.xslt)) {
        result = QString::fromUtf8(reinterpret_cast<char*>(buffer), bufferSize);
        std::free(buffer);
    } else {
        result = xml;
    }

    Q_ASSERT(result != xml);
    return result;
}

