cmake_minimum_required(VERSION 3.0)

project(lliurex-plymouth-theme)

add_subdirectory(src)

#render logo
set(SVG "${CMAKE_CURRENT_SOURCE_DIR}/logo.svg")
set(PNG "${CMAKE_CURRENT_BINARY_DIR}/logo.png")

add_custom_command(
        OUTPUT ${PNG}
        COMMAND rsvg-convert -o ${PNG} ${SVG}
        DEPENDS ${SVG}
)

add_custom_target(logo DEPENDS ${PNG})

add_dependencies(lliurex logo)

#install stuff
install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/lliurex.plymouth
    DESTINATION share/plymouth/themes/lliurex/
)

install(
    FILES ${PNG}
    DESTINATION share/plymouth/themes/lliurex/
)

install( FILES "font.ttf" DESTINATION share/plymouth/themes/lliurex/)

