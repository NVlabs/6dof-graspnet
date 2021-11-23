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

#ifndef OBJECTTYPE_H
#define OBJECTTYPE_H

#include <list>
#include "str.h"
#include "null.h"

#include "libsamplemacros.h"

struct Event
{
    enum EventType {
        NO_EVENT,
        BASIC_EVENT,
        SOME_EVENT,
        ANY_EVENT
    };
    Event(EventType eventType) : m_eventType(eventType) {}
    EventType eventType() { return m_eventType; }
private:
    EventType m_eventType;
};

class ObjectTypeLayout;
class ObjectType;
typedef std::list<ObjectType*> ObjectTypeList;

class LIBSAMPLE_API ObjectType
{
public:

    explicit ObjectType(ObjectType* parent = 0);
    virtual ~ObjectType();

    // factory method
    inline static ObjectType* create() { return new ObjectType(); }
    static ObjectType* createWithChild();

    void setParent(ObjectType* parent);
    inline ObjectType* parent() const { return m_parent; }
    inline const ObjectTypeList& children() const { return m_children; }
    void killChild(const Str& name);
    void removeChild(ObjectType* child);
    ObjectType* takeChild(ObjectType* child);
    virtual ObjectType* takeChild(const Str& name);
    ObjectType* findChild(const Str& name);

    Str objectName() const;
    void setObjectName(const Str& name);

    inline unsigned long identifier() const { return reinterpret_cast<unsigned long>(this); }

    bool causeEvent(Event::EventType eventType);

    // Returns true if the event is processed.
    virtual bool event(Event* event);
    static int processEvent(ObjectTypeList objects, Event *event);

    void callInvalidateEvent(Event* event);
    virtual void invalidateEvent(Event* event) {}

    // This nonsense method emulate QWidget.setLayout method
    // All layout objects will became children of this object.
    void setLayout(ObjectTypeLayout* layout);
    inline ObjectTypeLayout* layout() const { return m_layout; }

    // This method should be reimplemented by ObjectTypeLayout.
    virtual bool isLayoutType() { return false; }

    unsigned char callWithEnum(const Str& prefix, Event::EventType type, unsigned char value=80);
    unsigned char callWithEnum(const Str& prefix, unsigned char value=0);

    //Functions used in test with named arguments
    void setObjectSplittedName(const char*, const Str& prefix = Str("<unk"), const Str& suffix = Str("nown>"));
    void setObjectNameWithSize(const char*, int size=9, const Str& name = Str("<unknown>"));
    void setObjectNameWithSize(const Str& name = Str("<unknown>"), int size=9);

    //Function used to confuse the generator when two values accept Null as arg
    void setObject(ObjectType *);
    void setObject(const Null&);
    int callId() const;

    //Function used to create a parent from C++
    virtual bool isPython() { return false; }
    void callVirtualCreateChild();
    virtual ObjectType* createChild(ObjectType* parent);
    static std::size_t createObjectType();

    //return a parent from C++
    ObjectType* getCppParent() {
        if (!m_parent) {
            ObjectType* parent = new ObjectType();
            setParent(parent);
        }
        return m_parent;
    }

    void destroyCppParent() {
        delete m_parent;
        m_parent = 0;
    }

    //Deprecated test
    bool deprecatedFunction() { return true; }

    // nextInFocusChain simply returns the parent to test object cycles; the parent
    // may be returned by the QWidget's implementation but isn't always returned
    ObjectType* nextInFocusChain() { return m_parent; }

private:
    ObjectType(const ObjectType&);
    ObjectType& operator=(const ObjectType&);

    ObjectTypeLayout* takeLayout();

    Str m_objectName;
    ObjectType* m_parent;
    ObjectTypeList m_children;

    ObjectTypeLayout* m_layout;


    //used on overload null test
    int m_call_id;
};

LIBSAMPLE_API unsigned int objectTypeHash(const ObjectType* objectType);

class LIBSAMPLE_API OtherBase {
public:
    OtherBase() {};
    virtual ~OtherBase();
};

class LIBSAMPLE_API ObjectTypeDerived: public ObjectType, public OtherBase {
public:
    ObjectTypeDerived(): ObjectType(), OtherBase() {};

    virtual bool event(Event* event);
    virtual ~ObjectTypeDerived();
};

#endif // OBJECTTYPE_H

