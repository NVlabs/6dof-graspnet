project(libother)

set(libother_SRC
number.cpp
otherderived.cpp
otherobjecttype.cpp
othermultiplederived.cpp
)

add_definitions("-DLIBOTHER_BUILD")
add_library(libother SHARED ${libother_SRC})
set_property(TARGET libother PROPERTY PREFIX "")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${libsample_SOURCE_DIR}
                    ${libsample_SOURCE_DIR}/..)
target_link_libraries(libother libsample)

