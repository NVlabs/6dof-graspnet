
macro(ADD_RCC_TEST name pyfile qrcfile)
    add_test(${name} ${CMAKE_CURRENT_SOURCE_DIR}/run_test.sh
                     ${PYSIDERCC_EXECUTABLE}
                     ${CMAKE_CURRENT_SOURCE_DIR}/${pyfile}
                     ${CMAKE_CURRENT_SOURCE_DIR}/${qrcfile}
                     ${CMAKE_CURRENT_SOURCE_DIR})
endmacro()

add_rcc_test(RccTest rcc_test.py example.qrc)
add_rcc_test(RccImageTest rcc_image_test.py image.qrc)

