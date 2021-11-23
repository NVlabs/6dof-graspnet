add_subdirectory(libminimal)
if(NOT DEFINED MINIMAL_TESTS)
    add_subdirectory(libsample)
    add_subdirectory(libother)
endif()

if(WIN32 OR DEFINED AVOID_PROTECTED_HACK)
    message(STATUS "Tests will be generated avoiding the protected hack!")
    set(GENERATOR_EXTRA_FLAGS --avoid-protected-hack)
else()
    message(STATUS "Tests will be generated using the protected hack!")
    set(GENERATOR_EXTRA_FLAGS )
endif()

add_subdirectory(minimalbinding)
if(NOT DEFINED MINIMAL_TESTS)
    add_subdirectory(samplebinding)
    add_subdirectory(otherbinding)
endif()

if(DEFINED MINIMAL_TESTS)
    file(GLOB TEST_FILES minimalbinding/*_test.py)
else()
    file(GLOB TEST_FILES minimalbinding/*_test.py
                         samplebinding/*_test.py
                         otherbinding/*_test.py
                         shibokenmodule/*_test.py)
endif()
list(SORT TEST_FILES)

set(test_blacklist "")

find_package(PythonInterp REQUIRED)
if(NOT CTEST_TESTING_TIMEOUT)
    set(CTEST_TESTING_TIMEOUT 60)
endif()

if(CMAKE_VERSION VERSION_LESS 2.8)
    message("CMake version greater than 2.8 necessary to run tests")
else()
    if(WIN32)
        set(TEST_PYTHONPATH     "${minimal_BINARY_DIR};${sample_BINARY_DIR};${other_BINARY_DIR};${shibokenmodule_BINARY_DIR};${CMAKE_CURRENT_SOURCE_DIR}")
        set(TEST_LIBRARY_PATH   "$ENV{PATH};${libminimal_BINARY_DIR};${libsample_BINARY_DIR};${libother_BINARY_DIR};${libshiboken_BINARY_DIR}")
        set(LIBRARY_PATH_VAR    "PATH")
        string(REPLACE "\\" "/" TEST_PYTHONPATH "${TEST_PYTHONPATH}")
        string(REPLACE "\\" "/" TEST_LIBRARY_PATH "${TEST_LIBRARY_PATH}")
        string(REPLACE ";" "\\;" TEST_PYTHONPATH "${TEST_PYTHONPATH}")
        string(REPLACE ";" "\\;" TEST_LIBRARY_PATH "${TEST_LIBRARY_PATH}")
    else()
        set(TEST_PYTHONPATH     "${minimal_BINARY_DIR}:${sample_BINARY_DIR}:${other_BINARY_DIR}:${shibokenmodule_BINARY_DIR}:${CMAKE_CURRENT_SOURCE_DIR}")
        set(TEST_LIBRARY_PATH   "$ENV{LD_LIBRARY_PATH}:${libminimal_BINARY_DIR}:${libsample_BINARY_DIR}:${libother_BINARY_DIR}:${libshiboken_BINARY_DIR}")
        set(LIBRARY_PATH_VAR    "LD_LIBRARY_PATH")
    endif()
    foreach(test_file ${TEST_FILES})
        string(REGEX MATCH "/([^/]+)(binding|module)/([^/]+)_test.py" tmp ${test_file})
        set(test_name "${CMAKE_MATCH_1}_${CMAKE_MATCH_3}")
        list(FIND test_blacklist ${test_name} expect_fail)
        add_test(${test_name} ${PYTHON_EXECUTABLE} ${test_file})
        set_tests_properties(${test_name} PROPERTIES ENVIRONMENT "PYTHONPATH=${TEST_PYTHONPATH};${LIBRARY_PATH_VAR}=${TEST_LIBRARY_PATH}")
        set_tests_properties(${test_name} PROPERTIES TIMEOUT ${CTEST_TESTING_TIMEOUT})
        if (${expect_fail} GREATER -1)
            set_tests_properties(${test_name} PROPERTIES WILL_FAIL TRUE)
        endif()
    endforeach()
endif()

# FIXME Skipped until add an option to choose the generator
# add_subdirectory(test_generator)

if (NOT APIEXTRACTOR_DOCSTRINGS_DISABLED)
#     project(sphinxtabletest)
# 
#     # TODO
#     set(sphinxtabletest_SRC sphinxtabletest.cpp)
#     qt4_automoc(${sphinxtabletest_SRC})
# 
#     include_directories(${QT_INCLUDE_DIR}
#                         ${QT_QTCORE_INCLUDE_DIR}
#                         ${CMAKE_CURRENT_BINARY_DIR}
#                         ${qtdoc_generator_SOURCE_DIR})
# 
#     add_executable(sphinxtabletest ${sphinxtabletest_SRC})
# 
#     target_link_libraries(sphinxtabletest
#                         ${QT_QTTEST_LIBRARY}
#                         ${APIEXTRACTOR_LIBRARY}
#                         qtdoc_generator
#                         genrunner)
# 
#     add_test("sphinxtable" sphinxtabletest)
#     if (INSTALL_TESTS)
#         install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/sphinxtabletest DESTINATION ${TEST_INSTALL_DIR})
#     endif()
endif()
