Ownership Management
********************

Among the various types of instances interactions, sometimes an object
may be *owned* by another object, so that the owner is responsible for
destroying the owned object, like in Qt's object system [#]_.
This kind of relationship has a big role on interfacing with the target language, like
with Python's reference counting.


Ownership transfers
-------------------

From C++ to target
^^^^^^^^^^^^^^^^^^

    When an object that is currently owned by C++ has its ownership transfered
    back to the target language, the binding can know for sure when the object will be deleted and
    tie the C++ instance existence to the wrapper, calling the C++ destructor normally when the
    wrapper is deleted.

    .. code-block:: xml

        <modify-argument index="1">
            <define-ownership class="target" owner="target" />
        </modify-argument>


From target to C++
^^^^^^^^^^^^^^^^^^

    In the opposite direction,when an object ownership is transfered from the target language
    to C++, the native code takes full control of the object life and you don't
    know when that object will be deleted, rendering the wrapper object invalid,
    unless you're wrapping an object with a virtual destructor,
    so you can override it and be notified of its destruction.

    By default it's safer to just render the wrapper
    object invalid and raise some error if the user tries to access
    one of this objects members or pass it as argument to some function, to avoid unpleasant segfaults.
    Also you should avoid calling the C++ destructor when deleting the wrapper.

    .. code-block:: xml

        <modify-argument index="1">
            <define-ownership class="target" owner="c++" />
        </modify-argument>


Parent-child relationship
-------------------------

One special type of relationship is the parent-child. When an object is called
the parent of another object (the child), the former is in charge of deleting its
child when deleted and the target language can trust that the child will be alive
as long as the parent is, unless some other method can take the C++ ownership away from the parent.

One of the main uses of this scheme is Qt's object system, with ownership among QObject-derived
classes, creating "trees" of instances.

    .. code-block:: xml

        <modify-argument index="this">
            <parent index="1" action="add">
        </modify-argument>

In this example, the instance with the method that is being invoked (indicated by 'index="this"' on
modify-argument) will be marked as a child
of the first argument using the `parent` tag. To remove ownership, just use "remove" in the action attribute. **Removing
parentship also transfers the ownership back to python.**

Invalidation after use
----------------------

Sometimes an object is created as a virtual method call argument and destroyed after the
call returned. In this case, you should use the ``invalidate-after-use`` attribute in the
``modify-argument`` tag to mark the wrapper as invalid right after the virtual method returns.

    .. code-block:: xml

        <modify-argument index="2" invalidate-after-use="yes"/>

In this example the second argument will be invalidated after this method call.

.. [#] See *Object Trees and Object Ownership* http://doc.trolltech.com/4.5/objecttrees.html
