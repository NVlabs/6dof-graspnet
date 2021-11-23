
add_executable(pyside-rcc main.cpp rcc.cpp)
include_directories(pyside-rcc
                    ${QT_INCLUDE_DIR}
                    ${QT_QT_INCLUDE_DIR}
                    ${QT_QTXML_INCLUDE_DIR}
                    ${QT_QTCORE_INCLUDE_DIR})

target_link_libraries(pyside-rcc
                      ${QT_QTCORE_LIBRARY}
                      ${QT_QTXML_LIBRARY})

install(TARGETS pyside-rcc RUNTIME DESTINATION bin)

# Man pages
if (NOT win32)
    file(GLOB manpages "${CMAKE_CURRENT_SOURCE_DIR}/*.1")
    install(FILES ${manpages} DESTINATION share/man/man1)
endif()

