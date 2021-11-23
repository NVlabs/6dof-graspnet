project(shibokengenerator)

set(shiboken_SRC
../generator.cpp
cppgenerator.cpp
headergenerator.cpp
overloaddata.cpp
shibokengenerator.cpp
shibokennormalize.cpp
main.cpp
)

include_directories(${generators_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${APIEXTRACTOR_INCLUDE_DIR}
                    ${QT_INCLUDE_DIR}
                    ${QT_QTCORE_INCLUDE_DIR}
                    ${QT_QTXML_INCLUDE_DIR})

add_executable(shiboken ${shiboken_SRC})
set_target_properties(shiboken PROPERTIES OUTPUT_NAME shiboken${shiboken_SUFFIX})
target_link_libraries(shiboken
                      ${APIEXTRACTOR_LIBRARY}
                      ${QT_QTCORE_LIBRARY}
                      ${QT_QTXML_LIBRARY})

configure_file(shibokenconfig.h.in "${CMAKE_CURRENT_BINARY_DIR}/shibokenconfig.h" @ONLY)

install(TARGETS shiboken DESTINATION bin)
