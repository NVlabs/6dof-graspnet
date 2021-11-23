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

#include "testaddfunction.h"
#include <QtTest/QTest>
#include "testutil.h"


void TestAddFunction::testParsingFuncNameAndConstness()
{
    // generic test...
    const char sig1[] = "func(type1, const type2, const type3* const)";
    AddedFunction f1(sig1, "void", 0);
    QCOMPARE(f1.name(), QString("func"));
    QCOMPARE(f1.arguments().count(), 3);
    AddedFunction::TypeInfo retval = f1.returnType();
    QCOMPARE(retval.name, QString("void"));
    QCOMPARE(retval.indirections, 0);
    QCOMPARE(retval.isConstant, false);
    QCOMPARE(retval.isReference, false);

    // test with a ugly template as argument and other ugly stuff
    const char sig2[] = "    _fu__nc_       (  type1, const type2, const Abc<int& , C<char*> *   >  * *, const type3* const    )   const ";
    AddedFunction f2(sig2, "const Abc<int& , C<char*> *   >  * *", 0);
    QCOMPARE(f2.name(), QString("_fu__nc_"));
    QList< AddedFunction::TypeInfo > args = f2.arguments();
    QCOMPARE(args.count(), 4);
    retval = f2.returnType();
    QCOMPARE(retval.name, QString("Abc<int& , C<char*> *   >"));
    QCOMPARE(retval.indirections, 2);
    QCOMPARE(retval.isConstant, true);
    QCOMPARE(retval.isReference, false);
    retval = args[2];
    QCOMPARE(retval.name, QString("Abc<int& , C<char*> *   >"));
    QCOMPARE(retval.indirections, 2);
    QCOMPARE(retval.isConstant, true);
    QCOMPARE(retval.isReference, false);

    // function with no args.
    const char sig3[] = "func()";
    AddedFunction f3(sig3, "void", 0);
    QCOMPARE(f3.name(), QString("func"));
    QCOMPARE(f3.arguments().count(), 0);
}

void TestAddFunction::testAddFunction()
{
    const char cppCode[] = "struct B {}; struct A { void a(int); };";
    const char xmlCode[] = "\
    <typesystem package='Foo'>\
        <primitive-type name='int' />\
        <primitive-type name='float' />\
        <value-type name='B' />\
        <value-type name='A'>\
            <add-function signature='b(int, float = 4.6, const B&amp;)' return-type='int' access='protected'>\
            </add-function>\
        </value-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    TypeDatabase* typeDb = TypeDatabase::instance();
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    QCOMPARE(classA->functions().count(), 4); // default ctor, default copy ctor, func a() and the added function

    AbstractMetaFunction* addedFunc = classA->functions().last();
    QCOMPARE(addedFunc->visibility(), uint(AbstractMetaFunction::Protected));
    QCOMPARE(addedFunc->functionType(), AbstractMetaFunction::NormalFunction);
    QVERIFY(addedFunc->isUserAdded());
    QCOMPARE(addedFunc->ownerClass(), classA);
    QCOMPARE(addedFunc->implementingClass(), classA);
    QCOMPARE(addedFunc->declaringClass(), classA);
    QVERIFY(!addedFunc->isVirtual());
    QVERIFY(!addedFunc->isSignal());
    QVERIFY(!addedFunc->isSlot());
    QVERIFY(!addedFunc->isStatic());

    AbstractMetaType* returnType = addedFunc->type();
    QCOMPARE(returnType->typeEntry(), typeDb->findPrimitiveType("int"));
    AbstractMetaArgumentList args = addedFunc->arguments();
    QCOMPARE(args.count(), 3);
    QCOMPARE(args[0]->type()->typeEntry(), returnType->typeEntry());
    QCOMPARE(args[1]->defaultValueExpression(), QString("4.6"));
    QCOMPARE(args[2]->type()->typeEntry(), typeDb->findType("B"));
}

void TestAddFunction::testAddFunctionConstructor()
{
    const char cppCode[] = "struct A { A() {} };";
    const char xmlCode[] = "\
    <typesystem package='Foo'>\
        <primitive-type name='int' />\
        <value-type name='A'>\
            <add-function signature='A(int)' />\
        </value-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    QCOMPARE(classA->functions().count(), 3); // default and added ctors
    AbstractMetaFunction* addedFunc = classA->functions().last();
    QCOMPARE(addedFunc->visibility(), uint(AbstractMetaFunction::Public));
    QCOMPARE(addedFunc->functionType(), AbstractMetaFunction::ConstructorFunction);
    QCOMPARE(addedFunc->arguments().size(), 1);
    QVERIFY(addedFunc->isUserAdded());
    QVERIFY(!addedFunc->type());
}

void TestAddFunction::testAddFunctionTagDefaultValues()
{
    const char cppCode[] = "struct A {};";
    const char xmlCode[] = "\
    <typesystem package='Foo'>\
        <value-type name='A'>\
            <add-function signature='func()' />\
        </value-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    QCOMPARE(classA->functions().count(), 3); // default ctor, default copy ctor and the added function
    AbstractMetaFunction* addedFunc = classA->functions().last();
    QCOMPARE(addedFunc->visibility(), uint(AbstractMetaFunction::Public));
    QCOMPARE(addedFunc->functionType(), AbstractMetaFunction::NormalFunction);
    QVERIFY(addedFunc->isUserAdded());
    QVERIFY(!addedFunc->type());
}

void TestAddFunction::testAddFunctionCodeSnippets()
{
    const char cppCode[] = "struct A {};";
    const char xmlCode[] = "\
    <typesystem package='Foo'>\
        <value-type name='A'>\
            <add-function signature='func()'>\
                <inject-code class='target' position='end'>Hi!, I am the code.</inject-code>\
            </add-function>\
        </value-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    AbstractMetaFunction* addedFunc = classA->functions().last();
    QVERIFY(addedFunc->hasInjectedCode());
}

void TestAddFunction::testAddFunctionWithoutParenteses()
{
    const char sig1[] = "func";
    AddedFunction f1(sig1, "void", 0);

    QCOMPARE(f1.name(), QString("func"));
    QCOMPARE(f1.arguments().count(), 0);
    QCOMPARE(f1.isConstant(), false);

    const char cppCode[] = "struct A {};";
    const char xmlCode[] = "\
    <typesystem package='Foo'>\
        <value-type name='A'>\
            <add-function signature='func'>\
                <inject-code class='target' position='end'>Hi!, I am the code.</inject-code>\
            </add-function>\
        </value-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    const AbstractMetaFunction* addedFunc = classA->findFunction("func");
    QVERIFY(addedFunc);
    QVERIFY(addedFunc->hasInjectedCode());
    QCOMPARE(addedFunc->injectedCodeSnips(CodeSnip::Any, TypeSystem::TargetLangCode).count(), 1);
}

void TestAddFunction::testAddFunctionWithDefaultArgs()
{
    const char sig1[] = "func";
    AddedFunction f1(sig1, "void", 0);

    QCOMPARE(f1.name(), QString("func"));
    QCOMPARE(f1.arguments().count(), 0);
    QCOMPARE(f1.isConstant(), false);

    const char cppCode[] = "struct A { };";
    const char xmlCode[] = "\
    <typesystem package='Foo'>\
        <primitive-type name='int'/> \
        <value-type name='A'>\
            <add-function signature='func(int, int)'>\
              <modify-argument index='2'>\
                <replace-default-expression with='2'/> \
              </modify-argument> \
            </add-function>\
        </value-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    const AbstractMetaFunction* addedFunc = classA->findFunction("func");
    QVERIFY(addedFunc);
    AbstractMetaArgument *arg = addedFunc->arguments()[1];
    QCOMPARE(arg->defaultValueExpression(), QString("2"));
}

void TestAddFunction::testAddFunctionAtModuleLevel()
{
    const char cppCode[] = "struct A { };";
    const char xmlCode[] = "\
    <typesystem package='Foo'>\
        <primitive-type name='int'/> \
        <value-type name='A'/>\
        <add-function signature='func(int, int)'>\
            <inject-code class='target' position='beginning'>custom_code();</inject-code>\
        </add-function>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);

    TypeDatabase* typeDb = TypeDatabase::instance();

    AddedFunctionList addedFuncs = typeDb->findGlobalUserFunctions("func");

    QCOMPARE(addedFuncs.size(), 1);

    FunctionModificationList mods = typeDb->functionModifications("func(int,int)");

    QCOMPARE(mods.size(), 1);
    QVERIFY(mods.first().isCodeInjection());
    CodeSnip snip = mods.first().snips.first();
    QCOMPARE(snip.code(), QString("custom_code();"));
}

void TestAddFunction::testAddFunctionWithVarargs()
{
    const char sig1[] = "func(int,char,...)";
    AddedFunction f1(sig1, "void", 0);

    QCOMPARE(f1.name(), QString("func"));
    QCOMPARE(f1.arguments().count(), 3);
    QVERIFY(!f1.isConstant());

    const char cppCode[] = "struct A {};";
    const char xmlCode[] = "\
    <typesystem package='Foo'>\
        <primitive-type name='int'/> \
        <primitive-type name='char'/> \
        <value-type name='A'>\
            <add-function signature='func(int,char,...)'/>\
        </value-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    const AbstractMetaFunction* addedFunc = classA->findFunction("func");
    QVERIFY(addedFunc);
    const AbstractMetaArgument* arg = addedFunc->arguments().last();
    QVERIFY(arg->type()->isVarargs());
    QVERIFY(arg->type()->typeEntry()->isVarargs());
}

void TestAddFunction::testAddStaticFunction()
{
    const char cppCode[] = "struct A { };";
    const char xmlCode[] = "\
    <typesystem package='Foo'>\
        <primitive-type name='int'/> \
        <value-type name='A'>\
            <add-function signature='func(int, int)' static='yes'>\
                <inject-code class='target' position='beginning'>custom_code();</inject-code>\
            </add-function>\
        </value-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    const AbstractMetaFunction* addedFunc = classA->findFunction("func");
    QVERIFY(addedFunc);
    QVERIFY(addedFunc->isStatic());
}

void TestAddFunction::testAddGlobalFunction()
{
    const char cppCode[] = "struct A { };struct B {};";
    const char xmlCode[] = "\
    <typesystem package='Foo'>\
        <primitive-type name='int'/> \
        <value-type name='A' />\
        <add-function signature='globalFunc(int, int)' static='yes'>\
            <inject-code class='target' position='beginning'>custom_code();</inject-code>\
        </add-function>\
        <add-function signature='globalFunc2(int, int)' static='yes'>\
            <inject-code class='target' position='beginning'>custom_code();</inject-code>\
        </add-function>\
        <value-type name='B' />\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaFunctionList globalFuncs = t.builder()->globalFunctions();
    QCOMPARE(globalFuncs.count(), 2);
    QVERIFY(!t.builder()->classes().findClass("B")->findFunction("globalFunc"));
    QVERIFY(!t.builder()->classes().findClass("B")->findFunction("globalFunc2"));
    QVERIFY(!globalFuncs[0]->injectedCodeSnips().isEmpty());
    QVERIFY(!globalFuncs[1]->injectedCodeSnips().isEmpty());
}

void TestAddFunction::testAddFunctionWithApiVersion()
{
    const char cppCode[] = "";
    const char xmlCode[] = "\
    <typesystem package='Foo'>\
        <primitive-type name='int'/> \
        <add-function signature='globalFunc(int, int)' static='yes' since='1.3'>\
            <inject-code class='target' position='beginning'>custom_code();</inject-code>\
        </add-function>\
        <add-function signature='globalFunc2(int, int)' static='yes' since='0.1'>\
            <inject-code class='target' position='beginning'>custom_code();</inject-code>\
        </add-function>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, true, "0.1");
    AbstractMetaFunctionList globalFuncs = t.builder()->globalFunctions();
    QCOMPARE(globalFuncs.count(), 1);
}

void TestAddFunction::testModifyAddedFunction()
{
    const char cppCode[] = "class Foo { };";
    const char xmlCode[] = "\
    <typesystem package='Package'>\
        <primitive-type name='float'/>\
        <primitive-type name='int'/>\
        <value-type name='Foo'>\
            <add-function signature='method(float, int)'>\
                <inject-code class='target' position='beginning'>custom_code();</inject-code>\
                <modify-argument index='2'>\
                    <replace-default-expression with='0' />\
                    <rename to='varName' />\
                </modify-argument>\
            </add-function>\
        </value-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* foo = classes.findClass("Foo");
    const AbstractMetaFunction* method = foo->findFunction("method");
    QCOMPARE(method->arguments().size(), 2);
    AbstractMetaArgument* arg = method->arguments().at(1);
    QCOMPARE(arg->defaultValueExpression(), QString("0"));
    QCOMPARE(arg->name(), QString("varName"));
    QCOMPARE(method->argumentName(2), QString("varName"));
}

void TestAddFunction::testAddFunctionOnTypedef()
{
    const char cppCode[] = "template<class T> class Foo { }; typedef Foo<int> FooInt;";
    const char xmlCode[] = "\
    <typesystem package='Package'>\
        <custom-type name='PySequence'/>\
        <primitive-type name='int'/>\
        <value-type name='FooInt'>\
            <add-function signature='FooInt(PySequence)'>\
                <inject-code class='target' position='beginning'>custom_code();</inject-code>\
            </add-function>\
            <add-function signature='method()'>\
                <inject-code class='target' position='beginning'>custom_code();</inject-code>\
            </add-function>\
        </value-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* foo = classes.findClass("FooInt");
    QVERIFY(foo->hasNonPrivateConstructor());
    AbstractMetaFunctionList lst = foo->queryFunctions(AbstractMetaClass::Constructors);
    foreach(AbstractMetaFunction* f, lst)
        QVERIFY(f->signature().startsWith(f->name()));
    QCOMPARE(lst.size(), 2);
    const AbstractMetaFunction* method = foo->findFunction("method");
    QVERIFY(method);
}

void TestAddFunction::testAddFunctionWithTemplateArg()
{
    const char cppCode[] = "template<class T> class Foo { };";
    const char xmlCode[] = "\
    <typesystem package='Package'>\
        <primitive-type name='int'/>\
        <container-type name='Foo'  type='list'/>\
        <add-function signature='func(Foo&lt;int>)' />\
    </typesystem>";

    TestUtil t(cppCode, xmlCode);
    QCOMPARE(t.builder()->globalFunctions().size(), 1);
    AbstractMetaFunction* func = t.builder()->globalFunctions().first();
    AbstractMetaArgument* arg = func->arguments().first();
    QCOMPARE(arg->type()->instantiations().count(), 1);
}

QTEST_APPLESS_MAIN(TestAddFunction)

#include "testaddfunction.moc"

