project(libminimal)

set(libminimal_SRC
obj.cpp
listuser.cpp
typedef.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_definitions("-DLIBMINIMAL_BUILD")
add_library(libminimal SHARED ${libminimal_SRC})
set_property(TARGET libminimal PROPERTY PREFIX "")

