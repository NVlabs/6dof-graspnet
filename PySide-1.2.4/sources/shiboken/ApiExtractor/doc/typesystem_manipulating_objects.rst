.. _manipulating-object-and-value-types:

Manipulating Object and Value Types
-----------------------------------

.. _inject-code:

inject-code
^^^^^^^^^^^
    The inject-code node inserts the given code into the generated code for the
    given type or function, and it is a child of the :ref:`object-type`, :ref:`value-type`,
    :ref:`modify-function` and :ref:`add-function` nodes.

    .. code-block:: xml

         <value-type>
             <inject-code class="native | target | target-declaration"
                 position="beginning | end" since="...">
                 // the code
             </inject-code>
         </value-type>

    The ``class`` attribute specifies which module of the generated code that
    will be affected by the code injection. The ``class`` attribute accepts the
    following values:

        * native: The c++ code
        * target: The binding code
        * target-declaration: The code will be injected into the generated header
          file containing the c++ wrapper class definition.

    If the ``position`` attribute is set to *beginning* (the default), the code
    is inserted at the beginning of the function. If it is set to *end*, the code
    is inserted at the end of the function.

    The ``since`` attribute specify the API version where this code was injected.

modify-field
^^^^^^^^^^^^

    The modify-field node allows you to alter the access privileges for a given
    C++ field when mapping it onto the target language, and it is a child of an
    :ref:`object-type` or a :ref:`value-type` node.

    .. code-block:: xml

         <object-type>
             <modify-field name="..."
                 write="true | false"
                 read="true | false" />
         </object-type>

    The ``name`` attribute is the name of the field, the *optional* ``write``
    and ``read`` attributes specify the field's access privileges in the target
    language API (both are set to true by default).
    The ``remove`` attribute is an *optional* attribute, which can mark the field
    to be discarded on generation; it has the same purpose of the deprecated tag
    :ref:`remove`.

.. _modify-function:

modify-function
^^^^^^^^^^^^^^^

    The modify-function node allows you to modify a given C++ function when mapping
    it onto the target language, and it is a child of an :ref:`object-type` or a :ref:`value-type`
    node. Use the :ref:`modify-argument` node to specify which argument the modification
    affects.

    .. code-block:: xml

         <object-type>
             <modify-function signature="..."
                              since="..."
                              remove="all | c++"
                              access="public | private | protected"
                              rename="..." />
         </object-type>

    The ``signature`` attribute is a normalized C++ signature, excluding return
    values but including potential const declarations.

    The ``since`` attribute specify the API version when this function was modified.

    The ``remove``, ``access`` and ``rename`` attributes are *optional* attributes
    for added convenience; they serve the same purpose as the deprecated tags :ref:`remove`, :ref:`access` and :ref:`rename`.

.. _add-function:

add-function
^^^^^^^^^^^^

    The add-function node allows you to add a given function onto the target language,
    and it is a child of an :ref:`object-type` or :ref:`value-type` nodes if the
    function is suposed to be a method, or :ref:`namespace` and :ref:`typesystem` if
    the function is suposed to be a function inside a namespace or a global function.

    Typically when adding a function some code must be injected to provide the function
    logic. This can be done using the :ref:`inject-code` node.

    .. code-block:: xml

         <object-type>
             <add-function signature="..." return-type="..." access="public | protected" static="yes | no" since="..."/>
         </object-type>

    The ``return-type`` attribute defaults to *void*, the ``access`` to *public* and the ``static`` one to *no*.

    The ``since`` attribute specify the API version when this function was added.

.. _conversion-rule-on-types:

conversion-rule
^^^^^^^^^^^^^^^

    The conversion-rule node allows you to write customized code to convert the given argument between the target
    language and C++, and is a child of the :ref:`value-type`, :ref:`object-type`, :ref:`primitive-type` and
    :ref:`container-type` nodes.

    The code pointed by the file attribute is very tied to the generator using APIExtractor, so it don't follow any
    rules, but the generator rules..

    .. code-block:: xml

        <value-type name="Foo">
            <convertion-rule file="my_converter_implementation.h" since="..."/>
        </value-type>

    The ``since`` attribute specify the API version when this conversion rule became valid.

    .. note:: You can also use the conversion-rule node to specify :ref:`how the conversion of a single function argument should be done in a function <conversion-rule>`.

