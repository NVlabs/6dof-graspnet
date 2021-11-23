.. _modifying-functions:

Modifying Functions
-------------------

.. _modify-argument:

modify-argument
^^^^^^^^^^^^^^^

    The modify-argument node specifies which of the given function's arguments the
    modification affects, and is a child of the modify-function node. Use the
    remove-argument, replace-default-expression, remove-default-expression,
    replace-type, reference-count and define-ownership nodes to specify the details
    of the modification.

    .. code-block:: xml

         <modify-function>
             <modify-argument index="return | this | 1 ..." >
                 // modifications
             </modify-argument>
         </modify-function>

    Set the ``index`` attribute to "1" for the first argument, "2" for the second
    one and so on. Alternatively, set it to "return" or "this" if you want to
    modify the function's return value or the object the function is called upon,
    respectively.

.. _remove:

remove
^^^^^^

    The remove node removes the given method from the generated target language
    API, and it is a child of the modify-function node.

    .. code-block:: xml

         <modify-function>
             <remove class="all" />
         </modify-function>

    .. warning:: This tag is deprecated, use the ``remove`` attribute from :ref:`modify-function` tag instead.

.. _access:

access
^^^^^^

    The access node changes the access privileges of the given function in the
    generated target language API, and it is a child of the modify-function node.

    .. code-block:: xml

        <modify-function>
            <access modifier="public | protected | private"/>
        </modify-function>

    .. warning:: This tag is deprecated, use the ``access`` attribute from :ref:`modify-function` tag instead.

.. _rename:

rename
^^^^^^

    The rename node changes the name of the given function in the generated target
    language API, and it is a child of the modify-function node.

    .. code-block:: xml

        <modify-function>
            <rename to="..." />
        </modify-function>

    The ``to`` attribute is the new name of the function.

    .. warning:: This tag is deprecated, use the ``rename`` attribute from :ref:`modify-function` tag instead.
