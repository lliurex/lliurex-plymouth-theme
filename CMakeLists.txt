cmake_minimum_required(VERSION 3.0)

project(lliurex-plymouth-theme)

add_subdirectory(src)

#render SVG's
set(LOGO_SVG "${CMAKE_CURRENT_SOURCE_DIR}/logo.svg")
set(LOGO_PNG "${CMAKE_CURRENT_BINARY_DIR}/logo.png")

set(FFY_SVG "${CMAKE_CURRENT_SOURCE_DIR}/firefly.svg")
set(FFY_PNG "${CMAKE_CURRENT_BINARY_DIR}/firefly.png")

add_custom_command(
        OUTPUT ${LOGO_PNG}
        COMMAND rsvg-convert -o ${LOGO_PNG} ${LOGO_SVG}
        DEPENDS ${LOGO_SVG}
)

add_custom_target(logo DEPENDS ${LOGO_PNG})

add_custom_command(
        OUTPUT ${FFY_PNG}
        COMMAND rsvg-convert -o ${FFY_PNG} ${FFY_SVG}
        DEPENDS ${FFY_SVG}
)

add_custom_target(firefly DEPENDS ${FFY_PNG})

add_dependencies(lliurex logo firefly)

#install stuff
install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/lliurex.plymouth
    DESTINATION share/plymouth/themes/lliurex/
)

install(
    FILES ${LOGO_PNG} ${FFY_PNG}
    DESTINATION share/plymouth/themes/lliurex/
)

install(
    PROGRAMS lliurex-plymouth
    DESTINATION share/initramfs-tools/hooks
)
