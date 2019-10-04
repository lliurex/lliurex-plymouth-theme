
find_package(PkgConfig REQUIRED)
pkg_check_modules(PLY_CORE REQUIRED ply-splash-core)
pkg_check_modules(PLY_GRAPHICS REQUIRED ply-splash-graphics)
pkg_check_modules(SYSTEMD REQUIRED libsystemd)
pkg_check_modules(FREETYPE REQUIRED freetype2)

add_library(lliurex SHARED plugin.c log.c text.c cmdline.c i18.c)
target_include_directories(lliurex PUBLIC ${PLY_CORE_INCLUDE_DIRS} ${PLY_GRAPHICS_INCLUDE_DIRS} ${SYSTEMD_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(lliurex ${PLY_CORE_LIBRARIES} ${PLY_GRAPHICS_LIBRARIES} ${SYSTEMD_LIBRARIES} ${FREETYPE_LIBRARIES})

install(
    TARGETS lliurex
    DESTINATION lib/x86_64-linux-gnu/plymouth/
)
