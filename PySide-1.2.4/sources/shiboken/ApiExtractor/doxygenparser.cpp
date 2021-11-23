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

#include "doxygenparser.h"
#include "abstractmetalang.h"
#include "reporthandler.h"

#include <QtXmlPatterns/QXmlQuery>

namespace
{

QString getSectionKindAttr(const AbstractMetaFunction* func)
{
    if (func->isSignal()) {
        return QLatin1String("signal");
    } else {
        QString kind = func->isPublic() ? QLatin1String("public") : QLatin1String("protected");
        if (func->isStatic())
            kind += QLatin1String("-static");
        else if (func->isSlot())
            kind += QLatin1String("-slot");
        return kind;
    }
}

}

Documentation DoxygenParser::retrieveModuleDocumentation()
{
        return retrieveModuleDocumentation(packageName());
}

void DoxygenParser::fillDocumentation(AbstractMetaClass* metaClass)
{
    if (!metaClass)
        return;

    QString doxyFileSuffix;
    if (metaClass->enclosingClass()) {
        doxyFileSuffix += metaClass->enclosingClass()->name();
        doxyFileSuffix += "_1_1"; // FIXME: Check why _1_1!!
    }
    doxyFileSuffix += metaClass->name();
    doxyFileSuffix += ".xml";

    const char* prefixes[] = { "class", "struct", "namespace" };
    const int numPrefixes = sizeof(prefixes) / sizeof(const char*);
    bool isProperty = false;

    QString doxyFilePath;
    for (int i = 0; i < numPrefixes; ++i) {
        doxyFilePath = documentationDataDirectory() + "/" + prefixes[i] + doxyFileSuffix;
        if (QFile::exists(doxyFilePath))
            break;
        doxyFilePath.clear();
    }

    if (doxyFilePath.isEmpty()) {
        ReportHandler::warning("Can't find doxygen file for class "
                               + metaClass->name() + ", tried: "
                               + documentationDataDirectory() + "/{struct|class|namespace}"
                               + doxyFileSuffix);
        return;
    }
    QXmlQuery xquery;
    xquery.setFocus(QUrl(doxyFilePath));

    // Get class documentation
    QString classDoc = getDocumentation(xquery, "/doxygen/compounddef/detaileddescription",
                                        metaClass->typeEntry()->docModifications());
    if (classDoc.isEmpty()) {
        ReportHandler::warning("Can't find documentation for class \""
                               + metaClass->name() + "\".");
    }
    metaClass->setDocumentation(classDoc);

    //Functions Documentation
    AbstractMetaFunctionList funcs = metaClass->functionsInTargetLang();
    foreach (AbstractMetaFunction *func, funcs) {
        if (!func || func->isPrivate())
            continue;

        QString query = "/doxygen/compounddef/sectiondef";
        // properties
        if (func->isPropertyReader() || func->isPropertyWriter()
            || func->isPropertyResetter()) {
            query += "[@kind=\"property\"]/memberdef/name[text()=\""
                     + func->propertySpec()->name() + "\"]";
            isProperty = true;
        } else { // normal methods
            QString kind = getSectionKindAttr(func);
            query += "[@kind=\"" + kind + "-func\"]/memberdef/name[text()=\""
                     + func->originalName() + "\"]";

            if (func->arguments().isEmpty()) {
                QString args = func->isConstant() ? "() const " : "()";
                query += "/../argsstring[text()=\"" + args + "\"]";
            } else {
                int i = 1;
                foreach (AbstractMetaArgument* arg, func->arguments()) {
                    QString type;
                    if (!arg->type()->isPrimitive()) {
                        query += "/../param[" + QString::number(i) + "]/type/ref[text()=\""
                                 + arg->type()->name() + "\"]/../..";
                    } else {
                        query += "/../param[" + QString::number(i) + "]/type[text()=\""
                                 + arg->type()->name() + "\"]/..";
                    }
                    ++i;
                }
            }
        }
        if (!isProperty) {
            query += "/../detaileddescription";
        } else {
            query = "(" + query;
            query += "/../detaileddescription)[1]";
        }
        QString doc = getDocumentation(xquery, query, DocModificationList());
        func->setDocumentation(doc);
        isProperty = false;
    }

    //Fields
    AbstractMetaFieldList fields = metaClass->fields();
    foreach (AbstractMetaField *field, fields) {
        if (field->isPrivate())
            return;

        QString query = "/doxygen/compounddef/sectiondef/memberdef/name[text()=\""
                        + field->name() + "\"]/../detaileddescription";
        QString doc = getDocumentation(xquery, query, DocModificationList());
        field->setDocumentation(doc);
    }

    //Enums
    AbstractMetaEnumList enums = metaClass->enums();
    foreach (AbstractMetaEnum *meta_enum, enums) {
        QString query = "/doxygen/compounddef/sectiondef/memberdef[@kind=\"enum\"]/name[text()=\"" + meta_enum->name() + "\"]/..";
        QString doc = getDocumentation(xquery, query, DocModificationList());
        meta_enum->setDocumentation(doc);
    }

}

Documentation DoxygenParser::retrieveModuleDocumentation(const QString& name){

    QString sourceFile = documentationDataDirectory() + '/' + "indexpage.xml";

    if (!QFile::exists(sourceFile)) {
        ReportHandler::warning("Can't find doxygen XML file for module "
                               + name + ", tried: "
                                                 + sourceFile);
        return Documentation();
    }

    QXmlQuery xquery;
    xquery.setFocus(QUrl(sourceFile));

    // Module documentation
    QString query = "/doxygen/compounddef/detaileddescription";
    return Documentation(getDocumentation(xquery, query, DocModificationList()));
}

