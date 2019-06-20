
find_package(PkgConfig REQUIRED)
pkg_check_modules(PLY_CORE REQUIRED ply-splash-core)
pkg_check_modules(PLY_GRAPHICS REQUIRED ply-splash-graphics)
pkg_check_modules(SYSTEMD REQUIRED libsystemd)

add_library(lliurex SHARED plugin.c log.c)
target_include_directories(lliurex PUBLIC ${PLY_CORE_INCLUDE_DIRS} ${PLY_GRAPHICS_INCLUDE_DIRS} ${SYSTEMD_INCLUDE_DIRS})
target_link_libraries(lliurex ${PLY_CORE_LIBRARIES} ${PLY_GRAPHICS_LIBRARIES} ${SYSTEMD_LIBRARIES})

