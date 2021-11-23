INCLUDE(FindPython3Interp)
INCLUDE(FindPython3Libs)

find_package(Python3Interp REQUIRED)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # This is for Debian
    set(PYTHON3_EXECUTABLE_TMP "${PYTHON3_DBG_EXECUTABLE}")

    # Fall back to the standard interpreter.
    if(NOT EXISTS "${PYTHON3_EXECUTABLE_TMP}")
        set(PYTHON3_EXECUTABLE_TMP "${PYTHON3_EXECUTABLE}")
    endif()

    set(PYTHON3_EXECUTABLE "${PYTHON3_EXECUTABLE_TMP}")
endif()

# Detect if the python libs were compiled in debug mode
execute_process(
    COMMAND ${PYTHON3_EXECUTABLE} -c "from distutils import sysconfig; \\
        print(bool(sysconfig.get_config_var('Py_DEBUG')))"
    OUTPUT_VARIABLE PYTHON_WITH_DEBUG
    OUTPUT_STRIP_TRAILING_WHITESPACE)

message("PYTHON WITH DEBUG: ${PYTHON3_EXECUTABLE}")

execute_process(
    COMMAND ${PYTHON3_EXECUTABLE} -c "import sys; \\
        from distutils import sysconfig; \\
        vr = sys.version_info; \\
        prefix = '-python%d.%d' % (vr[0], vr[1]); \\
        suffix = prefix + '-dbg' if bool(sysconfig.get_config_var('Py_DEBUG')) else prefix; \\
        suffix = '.' + sysconfig.get_config_var('SOABI') if (vr.major == 3 and vr.minor >= 2 and 'SOABI' in sysconfig.get_config_vars()) else suffix; \\
        print(suffix)"
        OUTPUT_VARIABLE PYTHON_SUFFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE)

#Fix missing variable on UNIX env
if(NOT PYTHON3_DEBUG_LIBRARIES AND UNIX)
    string(REPLACE "-dbg" "" PYTHON_NAME_AUX ${PYTHON_SUFFIX})
    string(REPLACE "-python" "python" PYTHON_NAME ${PYTHON_NAME_AUX})
    find_library(LIBRARY_FOUND ${PYTHON_NAME}_d)
    if (LIBRARY_FOUND)
        set(PYTHON3_DEBUG_LIBRARIES "${LIBRARY_FOUND}")
    else()
        set(PYTHON3_DEBUG_LIBRARIES "${PYTHON3_LIBRARIES}")
    endif()
endif()


