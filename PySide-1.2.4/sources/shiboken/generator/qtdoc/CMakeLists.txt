project(qtdoc_generator)

set(qtdoc_generator_SRC
qtdocgenerator.cpp
)

include_directories(${generators_SOURCE_DIR}
                    ${QT_QTCORE_INCLUDE_DIR}
                    ${APIEXTRACTOR_INCLUDE_DIR})
add_executable(docgenerator main.cpp)
set_target_properties(docgenerator PROPERTIES OUTPUT_NAME docgenerator${generator_SUFFIX})

target_link_libraries(docgenerator ${QT_QTCORE_LIBRARY})

add_library(qtdoc_generator SHARED ${qtdoc_generator_SRC})
target_link_libraries(qtdoc_generator ${APIEXTRACTOR_LIBRARY} ${QT_QTCORE_LIBRARY} genrunner)
set_property(TARGET qtdoc_generator PROPERTY PREFIX "")

install(TARGETS qtdoc_generator DESTINATION ${generator_plugin_DIR})
install(TARGETS docgenerator DESTINATION bin)

