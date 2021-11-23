Solving compilation problems
----------------------------

suppress-warning
^^^^^^^^^^^^^^^^

    The generator will generate several warnings which may be irrelevant to the
    user. The suppress-warning node suppresses the specified warning, and it is
    a child of the typesystem node.

    .. code-block:: xml

        <typesystem>
            <suppress-warning text="..." />
        </typesystem>

    The **text** attribute is the warning text to suppress, and may contain the *
    wildcard (use "" to escape regular expression matching if the warning contain
    a regular "*").

extra-includes
^^^^^^^^^^^^^^

    The extra-includes node contains declarations of additional include files,
    and it can be a child of the interface-type, namespace-type, value-type and
    object-type nodes.

    The generator automatically tries to read the global header for each type but
    sometimes it is required to include extra files in the generated C++ code to
    make sure that the code compiles. These files must be listed using include
    nodes witin the extra-include node:

    .. code-block:: xml

         <value-type>
             <extra-includes>
                 <include file-name="..." location="global | local"/>
             </extra-includes>
         </value-type>

    The **file-name** attribute is the file to include, such as "QStringList".
    The **location** attribute is where the file is located: *global* means that
    the file is located in $INCLUDEPATH and will be included using #include <...>,
    *local* means that the file is in a local directory and will be included
    using #include "...".


include
^^^^^^^

    The include node specifies the name and location of a file that must be
    included, and it is a child of the interface-type, namespace-type, value-type,
    object-type or extra-includes nodes

    The generator automatically tries to read the global header for each type. Use
    the include node to override this behavior, providing an alternative file. The
    include node can also be used to specify extra include files.

    .. code-block:: xml

         <value-type>
             <include file-name="..."
                 location="global | local"/>
         </value-type>

    The **file-name** attribute is the file to include, such as "QStringList".
    The **location** attribute is where the file is located: *global* means that
    the file is located in $INCLUDEPATH and will be included using #include <...>,
    *local* means that the file is in a local directory and will be included
    using #include "...".
