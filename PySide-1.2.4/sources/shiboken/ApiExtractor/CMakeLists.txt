project(apiextractor)

find_package(LibXml2 2.6.32)
find_package(LibXslt 1.1.19)

option(DISABLE_DOCSTRINGS "Disable documentation extraction." FALSE)

if (NOT DISABLE_DOCSTRINGS)
    if (NOT LIBXSLT_FOUND OR NOT LIBXML2_FOUND)
        set(DISABLE_DOCSTRINGS TRUE CACHE BOOL "Disable doc strings" PARENT_SCOPE)
        set(DISABLE_DOCSTRINGS TRUE)
        message(WARNING "libxslt and/or libxml not found, disabling support for doc strings!")
    endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(BUILD_TESTS)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/tests)
endif ()

set(QT_USE_QTCORE 1)
set(QT_USE_QTXML 1)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_SHARED)
add_definitions(-DRXX_ALLOCATOR_INIT_0)

set(apiextractor_SRC
apiextractor.cpp
abstractmetabuilder.cpp
abstractmetalang.cpp
asttoxml.cpp
fileout.cpp
graph.cpp
reporthandler.cpp
typeparser.cpp
typesystem.cpp
include.cpp
typedatabase.cpp
parser/ast.cpp
parser/binder.cpp
parser/class_compiler.cpp
parser/codemodel.cpp
parser/codemodel_finder.cpp
parser/compiler_utils.cpp
parser/control.cpp
parser/declarator_compiler.cpp
parser/default_visitor.cpp
parser/dumptree.cpp
parser/lexer.cpp
parser/list.cpp
parser/name_compiler.cpp
parser/parser.cpp
parser/smallobject.cpp
parser/tokens.cpp
parser/type_compiler.cpp
parser/visitor.cpp
parser/rpp/builtin-macros.cpp
parser/rpp/preprocessor.cpp
)

if (NOT DISABLE_DOCSTRINGS)
    set(apiextractor_SRC
        ${apiextractor_SRC}
        docparser.cpp
        doxygenparser.cpp
        qtdocparser.cpp
    )
    set(APIEXTRACTOR_EXTRA_INCLUDES ${LIBXSLT_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR})
    set(APIEXTRACTOR_EXTRA_LIBRARIES ${LIBXSLT_LIBRARIES} ${LIBXML2_LIBRARIES})
else()
    set(APIEXTRACTOR_EXTRA_INCLUDES "")
    set(APIEXTRACTOR_EXTRA_LIBRARIES "")
endif()

set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE PATH "The subdirectory relative to the install prefix where libraries will be installed (default is /lib${LIB_SUFFIX})" FORCE)

qt4_add_resources(apiextractor_RCCS_SRC generator.qrc)
qt4_automoc(apiextractor_SRC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/parser
                    ${CMAKE_CURRENT_SOURCE_DIR}/parser/rpp
                    ${QT_INCLUDE_DIR}
                    ${APIEXTRACTOR_EXTRA_INCLUDES}
                    )

add_library(apiextractor STATIC ${apiextractor_SRC} ${apiextractor_RCCS_SRC})
target_link_libraries(apiextractor ${APIEXTRACTOR_EXTRA_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTXMLPATTERNS_LIBRARY} ${QT_QTXML_LIBRARY})

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
