project(shibokenmodule)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shibokenmodule.txt.in"
               "${CMAKE_CURRENT_BINARY_DIR}/shibokenmodule.txt" @ONLY)


set(sample_SRC
${CMAKE_CURRENT_BINARY_DIR}/shiboken/shiboken_module_wrapper.cpp
)

add_custom_command(OUTPUT ${sample_SRC}
COMMAND ${shibokengenerator_BINARY_DIR}/shiboken --project-file=${CMAKE_CURRENT_BINARY_DIR}/shibokenmodule.txt ${GENERATOR_EXTRA_FLAGS}
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
COMMENT "Running generator for 'shiboken'..."
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}
                    ${SBK_PYTHON_INCLUDE_DIR}
                    ${libshiboken_SOURCE_DIR}
                    ${libshiboken_BINARY_DIR})
add_library(shibokenmodule MODULE ${sample_SRC})
set_property(TARGET shibokenmodule PROPERTY PREFIX "")
set_property(TARGET shibokenmodule PROPERTY OUTPUT_NAME "shiboken")
if(WIN32)
    set_property(TARGET shibokenmodule PROPERTY SUFFIX ".pyd")
endif()
target_link_libraries(shibokenmodule
                      ${SBK_PYTHON_LIBRARIES}
                      libshiboken)

add_dependencies(shibokenmodule shiboken)

install(TARGETS shibokenmodule DESTINATION ${PYTHON_SITE_PACKAGES})