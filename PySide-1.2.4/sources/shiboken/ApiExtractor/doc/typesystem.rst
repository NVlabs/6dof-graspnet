The API Extractor Type System
*****************************

The typesystem is used by a binding generator or any other software using the APIExtractor library
to map a C++ library API onto a higher level language.

The typesystem specification is a handwritten XML document listing the types
that will be available in the generated target language API; types that are not
declared in the specification will be ignored along with everything depending on
them. In addition, it is possible to manipulate and modify types and functions.
It is even possible to use the typesystem specification to inject arbitrary
code into the source files, such as an extra member function.

Below there is a complete reference guide to the various nodes (XML tags) of the typesystem.
For usage examples, take a look at the typesystem files used to generate PySide. These files
can be found in the PySide/<QT_MODULE_NAME> directory of the PySide package.

.. toctree::

    typesystem_specifying_types
    typesystem_manipulating_objects
    typesystem_modify_function
    typesystem_arguments
    typesystem_solving_compilation
    typesystem_templates
    typesystem_conversionrule
    typesystem_documentation


