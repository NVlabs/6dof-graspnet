project(other)

set(other_TYPESYSTEM
${CMAKE_CURRENT_SOURCE_DIR}/typesystem_other.xml
)

set(other_SRC
${CMAKE_CURRENT_BINARY_DIR}/other/extendsnoimplicitconversion_wrapper.cpp
${CMAKE_CURRENT_BINARY_DIR}/other/number_wrapper.cpp
${CMAKE_CURRENT_BINARY_DIR}/other/otherderived_wrapper.cpp
${CMAKE_CURRENT_BINARY_DIR}/other/othermultiplederived_wrapper.cpp
${CMAKE_CURRENT_BINARY_DIR}/other/otherobjecttype_wrapper.cpp
${CMAKE_CURRENT_BINARY_DIR}/other/other_module_wrapper.cpp
)


configure_file("${CMAKE_CURRENT_SOURCE_DIR}/other-binding.txt.in"
               "${CMAKE_CURRENT_BINARY_DIR}/other-binding.txt" @ONLY)

add_custom_command(OUTPUT ${other_SRC}
COMMAND shiboken --project-file=${CMAKE_CURRENT_BINARY_DIR}/other-binding.txt ${GENERATOR_EXTRA_FLAGS}
DEPENDS ${other_TYPESYSTEM} ${CMAKE_CURRENT_SOURCE_DIR}/global.h shiboken
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
COMMENT "Running generator for 'other' test binding..."
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}
                    ${SBK_PYTHON_INCLUDE_DIR}
                    ${libother_SOURCE_DIR}
                    ${libsample_SOURCE_DIR}
                    ${libsample_SOURCE_DIR}/..
                    ${sample_BINARY_DIR}
                    ${sample_BINARY_DIR}/sample
                    ${libshiboken_SOURCE_DIR})
add_library(other MODULE ${other_SRC})
set_property(TARGET other PROPERTY PREFIX "")
if(WIN32)
    set_property(TARGET other PROPERTY SUFFIX ".pyd")
endif()
target_link_libraries(other
                      libother
                      libsample
                      ${SBK_PYTHON_LIBRARIES}
                      libshiboken)

add_dependencies(other sample)

