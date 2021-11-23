Specifying Types
----------------

.. _typesystem:

typesystem
^^^^^^^^^^

    This is the root node containing all the type system information. It can
    have a number of attributes, described below.

    .. code-block:: xml

        <typesystem package="..." default-superclass="...">
        </typesystem>

    The **package** attribute is a string describing the package to be used,
    e.g. "QtCore".
    The *optional* **default-superclass** attribute is the canonical C++ base class
    name of all objects, e.g., "object".

load-typesystem
^^^^^^^^^^^^^^^

    The load-typesystem node specifies which type systems to load when mapping
    multiple libraries to another language or basing one library on another, and
    it is a child of the typesystem node.

    .. code-block:: xml

        <typesystem>
            <load-typesystem name="..." generate="yes | no" />
        </typesystem>

    The **name** attribute is the filename of the typesystem to load, the
    **generate** attribute specifies whether code should be generated or not. The
    later must be specified when basing one library on another, making the generator
    able to understand inheritance hierarchies, primitive mapping, parameter types
    in functions, etc.

    Most libraries will be based on both the QtCore and QtGui modules, in which
    case code generation for these libraries will be disabled.


rejection
^^^^^^^^^

    The rejection node rejects the given class, or the specified function or
    field, and it is a child of the typesystem node.

    .. code-block:: xml

        <typesystem>
            <rejection class="..."
                function-name="..."
                field-name="..." />
        </typesystem>

    The **class** attribute is the C++ class name of the class to reject. Use the
    *optional* **function-name** or **field-name** attributes to reject a particular
    function or field. Note that the **field-name** and **function-name** cannot
    be specified at the same time. To remove all occurrences of a given field or
    function, set the class attribute to \*.

.. _primitive-type:

primitive-type
^^^^^^^^^^^^^^

    The primitive-type node describes how a primitive type is mapped from C++ to
    the target language, and is a child of the typesystem node. Note that most
    primitives are already specified in the QtCore typesystem.

    .. code-block:: xml

        <typesystem>
            <primitive-type name="..."
                since="..."
                target-name="..."
                default-constructor="..."
                preferred-conversion="yes | no" />
        </typesystem>

    The **name** attribute is the name of the primitive in C++, the optional,
    **target-name** attribute is the name of the primitive type in the target
    language. If the later two attributes are not specified their default value
    will be the same as the **name** attribute.

    The *optional*  **since** value is used to specify the API version of this type.

    If the *optional* **preferred-conversion** attribute is set to *no*, it
    indicates that this version of the primitive type is not the preferred C++
    equivalent of the target language type. For example, in Python both "qint64"
    and "long long" become "long" but we should prefer the "qint64" version. For
    this reason we mark "long long" with preferred-conversion="no".

    The *optional* **default-constructor** specifies the minimal constructor
    call to build one value of the primitive-type. This is not needed when the
    primitive-type may be built with a default constructor (the one without
    arguments).

    The *optional* **preferred-conversion** attribute tells how to build a default
    instance of the primitive type. It should be a constructor call capable of
    creating a instance of the primitive type. Example: a class "Foo" could have
    a **preferred-conversion** value set to "Foo()". Usually this attribute is
    used only for classes declared as primitive types and not for primitive C++
    types, but that depends on the application using *ApiExtractor*.


.. _namespace:

namespace-type
^^^^^^^^^^^^^^

    The namespace-type node maps the given C++ namespace to the target language,
    and it is a child of the typesystem node. Note that within namespaces, the
    generator only supports enums (i.e., no functions or classes).

    .. code-block:: xml

        <typesystem>
            <namespace-type name="..."
                generate="yes | no"
                package="..."
                since="..."
                revision="..." />
        </typesystem>

    The **name** attribute is the name of the namespace, e.g., "Qt".

    The *optional* **generate** attribute is used to inform if you need to prepend
    the given namespace into each generated class. Its default value is **yes**.

    The **package** attribute can be used to override the package of the type system.

    The *optional*  **since** value is used to specify the API version of this type.

    The **revision** attribute can be used to specify a revision for each type, easing the
    production of ABI compatible bindings.

enum-type
^^^^^^^^^

    The enum-type node maps the given enum from C++ to the target language,
    and it is a child of the typesystem node. Use the reject-enum-value to
    reject values.

    .. code-block:: xml

        <typesystem>
            <enum-type name="..."
                identified-by-value="..."
                since="..."
                flags="yes | no"
                flags-revision="..."
                lower-bound="..."
                upper-bound="..."
                force-integer="yes | no"
                extensible="yes | no"
                revision="..." />
        </typesystem>

    The **name** attribute is the fully qualified C++ name of the enum
    (e.g.,"Qt::FillRule"). If the *optional* **flags** attribute is set to *yes*
    (the default is *no*), the generator will expect an existing QFlags<T> for the
    given enum type. The **lower-bound** and **upper-bound** attributes are used
    to specify runtime bounds checking for the enum value. The value must be a
    compilable target language statement, such as "QGradient.Spread.PadSpread"
    (taking again Python as an example). If the **force-integer** attribute is
    set to *yes* (the default is *no*), the generated target language code will
    use the target language integers instead of enums. And finally, the
    **extensible** attribute specifies whether the given enum can be extended
    with user values (the default is *no*).

    The *optional*  **since** value is used to specify the API version of this type.

    The attribute **identified-by-value** helps to specify anonymous enums using the
    name of one of their values, which is unique for the anonymous enum scope.
    Notice that the **enum-type** tag can either have **name** or **identified-by-value**
    but not both.

    The **revision** attribute can be used to specify a revision for each type, easing the
    production of ABI compatible bindings.

    The **flags-revision** attribute has the same purposes of **revision** attribute but
    is used for the QFlag related to this enum.


reject-enum-value
^^^^^^^^^^^^^^^^^

    The reject-enum-value node rejects the enum value specified by the **name**
    attribute, and it is a child of the enum-type node.

    .. code-block:: xml

         <enum-type>
             <reject-enum-value name="..."/>
         </enum-type>

    This node is used when a C++ enum implementation has several identical numeric
    values, some of which are typically obsolete.

.. _value-type:

value-type
^^^^^^^^^^

    The value-type node indicates that the given C++ type is mapped onto the target
    language as a value type. This means that it is an object passed by value on C++,
    i.e. it is stored in the function call stack. It is a child of the :ref:`typesystem` node.

    .. code-block:: xml

        <typesystem>
            <value-type  name="..." since="..."
             copyable="yes | no"
             hash-function="..."
             stream="yes | no"
             default-constructor="..."
             revision="..." />
        </typesystem>

    The **name** attribute is the fully qualified C++ class name, such as
    "QMatrix" or "QPainterPath::Element". The **copyable** attribute is used to
    force or not specify if this type is copyable. The *optional* **hash-function**
    attribute informs the function name of a hash function for the type.

    The *optional* attribute **stream** specifies whether this type will be able to
    use externally defined operators, like QDataStream << and >>. If equals to **yes**,
    these operators will be called as normal methods within the current class.

    The *optional*  **since** value is used to specify the API version of this type.

    The *optional* **default-constructor** specifies the minimal constructor
    call to build one instance of the value-type. This is not needed when the
    value-type may be built with a default constructor (the one without arguments).
    Usually a code generator may guess a minimal constructor for a value-type based
    on its constructor signatures, thus **default-constructor** is used only in
    very odd cases.

    The **revision** attribute can be used to specify a revision for each type, easing the
    production of ABI compatible bindings.

.. _object-type:

object-type
^^^^^^^^^^^

    The object-type node indicates that the given C++ type is mapped onto the target
    language as an object type. This means that it is an object passed by pointer on
    C++ and it is stored on the heap. It is a child of the :ref:`typesystem` node.

    .. code-block:: xml

        <typesystem>
            <object-type name="..."
             since="..."
             copyable="yes | no"
             hash-function="..."
             stream="yes | no"
             revision="..." />
        </typesystem>

    The **name** attribute is the fully qualified C++ class name. If there is no
    C++ base class, the default-superclass attribute can be used to specify a
    superclass for the given type, in the generated target language API. The
    **copyable** and **hash-function** attributes are the same as described for
    :ref:`value-type`.

    The *optional* attribute **stream** specifies whether this type will be able to
    use externally defined operators, like QDataStream << and >>. If equals to **yes**,
    these operators will be called as normal methods within the current class.

    The *optional*  **since** value is used to specify the API version of this type.

    The **revision** attribute can be used to specify a revision for each type, easing the
    production of ABI compatible bindings.

interface-type
^^^^^^^^^^^^^^

    The interface-type node indicates that the given class is replaced by an
    interface pattern when mapping from C++ to the target language. Using the
    interface-type node implicitly makes the given type an :ref:`object-type`.

    .. code-block:: xml

        <typesystem>
            <interface-type name="..."
                since="..."
                package ="..."
                default-superclass ="..."
                revision="..." />
        </typesystem>

    The **name** attribute is the fully qualified C++ class name. The *optional*
    **package** attribute can be used to override the package of the type system.
    If there is no C++ base class, the *optional* **default-superclass** attribute
    can be used to specify a superclass in the generated target language API, for
    the given class.

    The *optional*  **since** value is used to specify the API version of this interface.

    The **revision** attribute can be used to specify a revision for each type, easing the
    production of ABI compatible bindings.

.. _container-type:

container-type
^^^^^^^^^^^^^^

    The container-type node indicates that the given class is a container and
    must be handled using one of the conversion helpers provided by attribute **type**.

    .. code-block:: xml

        <typesystem>
            <container-type name="..."
                since="..."
                type ="..." />
        </typesystem>

    The **name** attribute is the fully qualified C++ class name. The **type**
    attribute is used to indicate what conversion rule will be applied to the
    container. It can be: *list*, *string-list*, *linked-list*, *vector*, *stack*,
    *queue*, *set*, *map*, *multi-map*, *hash*, *multi-hash* or *pair*.

    The *optional*  **since** value is used to specify the API version of this container.


.. _custom-type:

custom-type
^^^^^^^^^^^

    The custom-type node simply makes the parser aware of the existence of a target
    language type, thus avoiding errors when trying to find a type used in function
    signatures and other places. The proper handling of the custom type is meant to
    be done by a generator using the APIExractor.

    .. code-block:: xml

        <typesystem>
            <custom-type name="..." />
        </typesystem>

    The **name** attribute is the name of the custom type, e.g., "PyObject".


.. _function:

function
^^^^^^^^

    The function node indicates that the given C++ global function is mapped onto
    the target language.

    .. code-block:: xml

        <typesystem>
            <function signature="..." rename="..." since="..."/>
        </typesystem>

    This tag has some limitations, it doesn't support function modifications, besides you
    can't add a function overload using :ref:`add-function` tag to an existent function.
    These limitation will be addressed in future versions of ApiExtractor.

    The function tag has two *optional* attributes: **since**, whose value is used to specify
    the API version of this function, and **rename**, to modify the function name.

