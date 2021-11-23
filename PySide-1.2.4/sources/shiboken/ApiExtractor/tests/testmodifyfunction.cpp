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

#include "testmodifyfunction.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestModifyFunction::testRenameArgument()
{
    const char* cppCode ="\
    struct A {\
        void method(int=0);\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <primitive-type name='int'/>\
        <object-type name='A'> \
        <modify-function signature='method(int)'>\
            <modify-argument index='1'>\
                <rename to='otherArg' />\
            </modify-argument>\
        </modify-function>\
        </object-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    const AbstractMetaFunction* func = classA->findFunction("method");
    Q_ASSERT(func);

    QCOMPARE(func->argumentName(1), QString("otherArg"));
}

void TestModifyFunction::testOwnershipTransfer()
{
    const char* cppCode ="\
    struct A {};\
    struct B {\
        virtual A* method();\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <object-type name='A' /> \
        <object-type name='B'> \
        <modify-function signature='method()'>\
            <modify-argument index='return'>\
                <define-ownership owner='c++' /> \
            </modify-argument>\
        </modify-function>\
        </object-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classB = classes.findClass("B");
    const AbstractMetaFunction* func = classB->findFunction("method");

    QCOMPARE(func->ownership(func->ownerClass(), TypeSystem::TargetLangCode, 0), TypeSystem::CppOwnership);
}


void TestModifyFunction::invalidateAfterUse()
{
    const char* cppCode ="\
    struct A {\
        virtual void call(int *a);\
    };\
    struct B : A {\
    };\
    struct C : B {\
        virtual void call2(int *a);\
    };\
    struct D : C {\
        virtual void call2(int *a);\
    };\
    struct E : D {\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <primitive-type name='int'/>\
        <object-type name='A'> \
        <modify-function signature='call(int*)'>\
          <modify-argument index='1' invalidate-after-use='true'/>\
        </modify-function>\
        </object-type>\
        <object-type name='B' /> \
        <object-type name='C'> \
        <modify-function signature='call2(int*)'>\
          <modify-argument index='1' invalidate-after-use='true'/>\
        </modify-function>\
        </object-type>\
        <object-type name='D'> \
        <modify-function signature='call2(int*)'>\
          <modify-argument index='1' invalidate-after-use='true'/>\
        </modify-function>\
        </object-type>\
        <object-type name='E' /> \
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false, "0.1");
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classB = classes.findClass("B");
    const AbstractMetaFunction* func = classB->findFunction("call");
    QCOMPARE(func->modifications().size(), 1);
    QCOMPARE(func->modifications().at(0).argument_mods.size(), 1);
    QVERIFY(func->modifications().at(0).argument_mods.at(0).resetAfterUse);

    AbstractMetaClass* classC = classes.findClass("C");
    QVERIFY(classC);
    func = classC->findFunction("call");
    QCOMPARE(func->modifications().size(), 1);
    QCOMPARE(func->modifications().at(0).argument_mods.size(), 1);
    QVERIFY(func->modifications().at(0).argument_mods.at(0).resetAfterUse);

    func = classC->findFunction("call2");
    QCOMPARE(func->modifications().size(), 1);
    QCOMPARE(func->modifications().at(0).argument_mods.size(), 1);
    QVERIFY(func->modifications().at(0).argument_mods.at(0).resetAfterUse);

    AbstractMetaClass* classD =  classes.findClass("D");
    QVERIFY(classD);
    func = classD->findFunction("call");
    QCOMPARE(func->modifications().size(), 1);
    QCOMPARE(func->modifications().at(0).argument_mods.size(), 1);
    QVERIFY(func->modifications().at(0).argument_mods.at(0).resetAfterUse);

    func = classD->findFunction("call2");
    QCOMPARE(func->modifications().size(), 1);
    QCOMPARE(func->modifications().at(0).argument_mods.size(), 1);
    QVERIFY(func->modifications().at(0).argument_mods.at(0).resetAfterUse);

    AbstractMetaClass* classE = classes.findClass("E");
    QVERIFY(classE);
    func = classE->findFunction("call");
    QVERIFY(func);
    QCOMPARE(func->modifications().size(), 1);
    QCOMPARE(func->modifications().at(0).argument_mods.size(), 1);
    QVERIFY(func->modifications().at(0).argument_mods.at(0).resetAfterUse);

    func = classE->findFunction("call2");
    QVERIFY(func);
    QCOMPARE(func->modifications().size(), 1);
    QCOMPARE(func->modifications().at(0).argument_mods.size(), 1);
    QVERIFY(func->modifications().at(0).argument_mods.at(0).resetAfterUse);
}

void TestModifyFunction::testWithApiVersion()
{
    const char* cppCode ="\
    struct A {};\
    struct B {\
        virtual A* method();\
        virtual B* methodB();\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <object-type name='A' /> \
        <object-type name='B'> \
        <modify-function signature='method()' since='0.1'>\
            <modify-argument index='return'>\
                <define-ownership owner='c++' /> \
            </modify-argument>\
        </modify-function>\
        <modify-function signature='methodB()' since='0.2'>\
            <modify-argument index='return'>\
                <define-ownership owner='c++' /> \
            </modify-argument>\
        </modify-function>\
        </object-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false, "0.1");
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classB = classes.findClass("B");
    const AbstractMetaFunction* func = classB->findFunction("method");

    QCOMPARE(func->ownership(func->ownerClass(), TypeSystem::TargetLangCode, 0), TypeSystem::CppOwnership);

    func = classB->findFunction("methodB");
    QVERIFY(func->ownership(func->ownerClass(), TypeSystem::TargetLangCode, 0) != TypeSystem::CppOwnership);
}

void TestModifyFunction::testGlobalFunctionModification()
{
    const char* cppCode ="\
    struct A {};\
    void function(A* a = 0);\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <primitive-type name='A'/>\
        <function signature='function(A*)'>\
            <modify-function signature='function(A*)'>\
                <modify-argument index='1'>\
                    <replace-type modified-type='A'/>\
                    <replace-default-expression with='A()'/>\
                </modify-argument>\
            </modify-function>\
        </function>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);
    QCOMPARE(t.builder()->globalFunctions().size(), 1);

    FunctionModificationList mods = TypeDatabase::instance()->functionModifications("function(A*)");
    QCOMPARE(mods.count(), 1);
    QList<ArgumentModification> argMods = mods.first().argument_mods;
    QCOMPARE(argMods.count(), 1);
    ArgumentModification argMod = argMods.first();
    QCOMPARE(argMod.replacedDefaultExpression, QString("A()"));

    const AbstractMetaFunction* func = t.builder()->globalFunctions().first();
    QVERIFY(func);
    QCOMPARE(func->arguments().count(), 1);
    const AbstractMetaArgument* arg = func->arguments().first();
    QCOMPARE(arg->type()->cppSignature(), QString("A *"));
    QCOMPARE(arg->originalDefaultValueExpression(), QString("0"));
    QCOMPARE(arg->defaultValueExpression(), QString("A()"));
}

QTEST_APPLESS_MAIN(TestModifyFunction)

#include "testmodifyfunction.moc"
