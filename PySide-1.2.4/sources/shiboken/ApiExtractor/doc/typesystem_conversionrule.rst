.. _conversion-rule-tag:

Conversion Rule Tag
-------------------

.. _conversion-rule:

conversion-rule
^^^^^^^^^^^^^^^

    The **conversion-rule** tag specifies how a **primitive-type**, a **container-type**,
    or a **value-type** may be converted to and from the native C++ language types to the
    target language types.

    .. code-block:: xml

        <value-type>
            <conversion-rule>
                <native-to-target>
                // Code to convert a native value to a target language object.
                </native-to-target>
                <target-to-native>
                    <add-conversion type='TARGETTYPEA' check='TARGETTYPEA_CHECK(%in)'>
                    // Code to convert target language type object of type TARGETTYPEA
                    // to the C++ native type represented by the value/primitive/container-type.
                    </add-conversion>
                    <add-conversion type='TARGETTYPEB' check='TARGETTYPEB_CHECK(%in)'>
                    // Code to convert target language type object of type TARGETTYPEB
                    // to the C++ native type represented by the value/primitive/container-type.
                    </add-conversion>
                </target-to-native>
            </conversion-rule>
        </value-type>

    The example above show the structure of a complete conversion rule. Each of the
    child tags comprising the conversion rule are described in their own sections
    below.


.. _native-to-target:

native-to-target
^^^^^^^^^^^^^^^^

    The **native-to-target** tag tells how to convert a native C++ value to its
    target language equivalent. The text inside the tag is a C++ code the takes
    an input value an does what's needed to convert it to the output value.
    ``insert-template`` tags may be used to insert commonly repeating code.

    .. code-block:: xml

        <conversion-rule>
            <native-to-target>
            // Code to convert a native value to a target language object.
            </native-to-target>
        </conversion-rule>

    Use the replace node to modify the template code.
    Notice that the generator must provide type system variables for the input
    and output values and types, namely **%in**, **%out**, **%INTYPE** and
    **%OUTTYPE**. In the case of container types, **%INTYPE** refers to the
    full container type (e.g. **"list<int>"**) and **%INTYPE_0**, **%INTYPE_1**,
    **%INTYPE_#**, should be replaced by the types used in the container template
    (e.g. **%INTYPE_0** correspondes to **"int"** for **"list<int>"**).


.. _target-to-native:

target-to-native
^^^^^^^^^^^^^^^^

    The **target-to-native** tag encloses at least one, but usually many, conversions
    from target language values to C++ native values. The *optional* attribute
    ``replace`` tells if the target language to C++ conversions will be added to, or if
    they will replace the implicit conversions collected by *ApiExtractor*. The default
    value for it is *yes*.


    .. code-block:: xml

        <conversion-rule>
            <target-to-native replace='yes|no'>\
            // List of target to native conversions meant to replace or expand
            // the already existing implicit conversions.
            </target-to-native>
        </conversion-rule>


.. _add-conversion:

add-conversion
^^^^^^^^^^^^^^

    Each **add-conversion** tag adds a rule for conversion of a target language type,
    indicated by the ``type`` attribute, to the C++ native type represented by the
    **primitive-type**, a **container-type**, or **value-type**, to which the parent
    **conversion-rule** belongs.

    .. code-block:: xml

        <target-to-native>
            <add-conversion type='TARGETTYPE' check='TARGETTYPECHECK(%in)'>
            // Code to convert target language type object of type TARGETTYPE_A
            // to the C++ native type represented by the value/primitive/container-type.
            </add-conversion>
        <target-to-native>

   The ``check`` attribute tells how a target value should be checked to see if it belongs to
   the type expected. This attribute is *optional*, for it can be derived from the ``type``
   attribute, but it isn't unusual that some special check is needed. The variables
   **%in**, **%out**, **%INTYPE**, **%INTYPE_#**, and **%OUTTYPE**, must be provided by
   the generator as in the ``native-to-target`` tag.

