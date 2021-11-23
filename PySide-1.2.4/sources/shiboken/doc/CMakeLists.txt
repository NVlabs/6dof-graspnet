
find_program(SPHINX sphinx-build DOC "Path to sphinx-build binary.")

if (SPHINX)
    message("-- sphinx-build - found")
    configure_file(conf.py.in conf.py @ONLY)
    add_custom_target(doc ${SPHINX} -b html -c . ${CMAKE_CURRENT_SOURCE_DIR} html )
else()
    message("-- sphinx-build - not found! doc target disabled")
endif()

if (NOT win32)
    file(GLOB manpages "${CMAKE_CURRENT_SOURCE_DIR}/*.1")
    install(FILES ${manpages} DESTINATION share/man/man1)
endif()
