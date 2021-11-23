find_program(PYTHON_EXECUTABLE NAMES python2.7 python2.6 python2.5)

if (NOT PYTHON_EXECUTABLE)
    find_package(PythonInterp REQUIRED)
else()
    set(PYTHONINTERP_FOUND 1)
endif()

if (PYTHONINTERP_FOUND AND UNIX AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    # This is for Debian
    set(PYTHON_EXECUTABLE_TMP "${PYTHON_EXECUTABLE}-dbg")

    if (NOT EXISTS "${PYTHON_EXECUTABLE_TMP}")
        # On Fedora we usually have the suffix as debug. As we didn't
        # find python interpreter with the suffix dbg we'll fall back
        # to the suffix as debug.
        set(PYTHON_EXECUTABLE_TMP "${PYTHON_EXECUTABLE}-debug")
    endif()
    # Falling back to the standard interpreter.
    if (NOT EXISTS "${PYTHON_EXECUTABLE_TMP}")
        set(PYTHON_EXECUTABLE_TMP "${PYTHON_EXECUTABLE}")
    endif()

    set(PYTHON_EXECUTABLE "${PYTHON_EXECUTABLE_TMP}")
endif()

# Detect if the python libs were compiled in debug mode
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; \\
        print bool(sysconfig.get_config_var('Py_DEBUG'))"
    OUTPUT_VARIABLE PYTHON_WITH_DEBUG
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "import sys; \\
        from distutils import sysconfig; \\
        vr = sys.version_info; \\
        suffix = '-dbg' if bool(sysconfig.get_config_var('Py_DEBUG')) else ''; \\
        print '-python%d.%d%s' % (vr[0], vr[1], suffix)"
    OUTPUT_VARIABLE PYTHON_SUFFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE)

# Fix missing variable on UNIX env
if (NOT PYTHON_DEBUG_LIBRARIES AND UNIX)
    string(REPLACE "-dbg" "" PYTHON_NAME_TMP ${PYTHON_SUFFIX})
    string(REPLACE "-python" "python" PYTHON_NAME ${PYTHON_NAME_TMP})
    find_library(LIBRARY_FOUND ${PYTHON_NAME}_d)
    if (LIBRARY_FOUND)
        set(PYTHON_DEBUG_LIBRARIES "${LIBRARY_FOUND}")
    else()
        set(PYTHON_DEBUG_LIBRARIES "${PYTHON_LIBRARIES}")
    endif()
endif()
