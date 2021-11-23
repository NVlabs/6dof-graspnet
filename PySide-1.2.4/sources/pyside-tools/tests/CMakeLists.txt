find_package(Shiboken)
add_subdirectory(rcc)
add_test(QWizard ${SHIBOKEN_PYTHON_INTERPRETER} ${CMAKE_SOURCE_DIR}/pyside-uic "${CMAKE_CURRENT_SOURCE_DIR}/qwizard_test.ui")
set_tests_properties(QWizard PROPERTIES
                     ENVIRONMENT "PYTHONPATH=$ENV{PYTHONPATH}:${CMAKE_SOURCE_DIR}")
