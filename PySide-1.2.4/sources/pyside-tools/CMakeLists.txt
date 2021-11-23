cmake_minimum_required(VERSION 2.6)
project(pyside-tools)

find_package(Shiboken 1.1.1 REQUIRED)
find_package(Qt4 4.5.0 REQUIRED)
find_package(PySide 1.0.6 REQUIRED)

set(pyside_tools_MAJOR_VERSION "0")
set(pyside_tools_MINOR_VERSION "2")
set(pyside_tools_MICRO_VERSION "15")
set(pyside_tools_VERSION "${pyside_tools_MAJOR_VERSION}.${pyside_tools_MINOR_VERSION}.${pyside_tools_MICRO_VERSION}")

configure_file("pysideuic/__init__.py.in" "__init__.py" @ONLY)

option(BUILD_TESTS "Build tests." TRUE)

# UIC stuff
if (NOT SITE_PACKAGE)
    execute_process(
        COMMAND ${SHIBOKEN_PYTHON_INTERPRETER} -c "from __future__ import print_function; \\
            from distutils import sysconfig; \\
            print(sysconfig.get_python_lib(1,0,prefix='${CMAKE_INSTALL_PREFIX}'))"
        OUTPUT_VARIABLE SITE_PACKAGE
    OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (NOT SITE_PACKAGE)
        message(FATAL_ERROR "Could not detect Python module installation directory.")
    endif()
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/pyside-uic
        DESTINATION bin
        PERMISSIONS
        OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_READ
        WORLD_EXECUTE WORLD_READ)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/__init__.py"
        DESTINATION "${SITE_PACKAGE}/pysideuic")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pysideuic
        DESTINATION ${SITE_PACKAGE}
        PATTERN "*.in" EXCLUDE
        PATTERN "*pysideuic/__init__.py" EXCLUDE
        PATTERN "*pysideuic\\__init__.py" EXCLUDE)

# Man pages for pyside-uic
if (NOT win32)
    file(GLOB manpages "${CMAKE_CURRENT_SOURCE_DIR}/pysideuic/*.1")
    install(FILES ${manpages} DESTINATION share/man/man1)
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)

add_custom_target(uninstall "${CMAKE_COMMAND}"
                  -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

set(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${pyside_tools_VERSION})
add_custom_target(dist
    COMMAND mkdir -p "${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}" &&
            git log > "${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}/ChangeLog" &&
            git archive --prefix=${ARCHIVE_NAME}/ HEAD --format=tar --output="${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar" &&
            tar -C "${CMAKE_BINARY_DIR}" --owner=root --group=root -r "${ARCHIVE_NAME}/ChangeLog" -f "${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar" &&
            bzip2 -f9 "${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar" &&
            echo "Source package created at ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.bz2.\n"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

set(PYSIDERCC_EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/pyrcc/pyside-rcc)
add_subdirectory(pyrcc)
add_subdirectory(pylupdate)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

