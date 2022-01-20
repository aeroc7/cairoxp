# cairo version 1.17.4
set(DEP_CAIRO_VERSION "1.17.4")
set(DEP_CAIRO_LOC "https://gitlab.freedesktop.org/cairo/cairo/-/archive/1.17.4/cairo-1.17.4.zip")

include(ExternalProject)

if(NOT TARGET pixman)
    message(FATAL_ERROR "Dependency pixman is needed first")
endif()

get_target_property(pixman_INCLUDE_DIR pixman INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(pixman_LIBRARY pixman IMPORTED_LOCATION)

if(NOT TARGET freetype)
    message(FATAL_ERROR "Dependency freetype is needed first")
endif()

get_target_property(freetype_INCLUDE_DIR freetype INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(freetype_LIBRARY freetype IMPORTED_LOCATION)

ExternalProject_Add(cairo-build
    URL                 ${DEP_CAIRO_LOC}
    CONFIGURE_COMMAND   meson build --prefix=<INSTALL_DIR> --libdir "lib" --buildtype release --default-library=static
                        -D fontconfig=disabled -D freetype=enabled -D cogl=disabled
                        -D directfb=disabled -D gl-backend=disabled -D glesv2=disabled
                        -D glesv3=disabled -D drm=disabled -D openvg=disabled -D png=disabled
                        -D quartz=disabled -D quartz-image=disabled -D qt=disabled -D tee=disabled
                        -D xcb=disabled -D xlib=disabled -D xlib-xcb=disabled -D zlib=disabled
                        -D tests=disabled -D gtk2-utils=disabled -D glib=disabled -D spectre=disabled
                        -D symbol-lookup=disabled --pkg-config-path
                        "${freetype_INCLUDE_DIR}/../../lib/pkgconfig/:${pixman_INCLUDE_DIR}/../../lib/pkgconfig"
    PATCH_COMMAND       patch
                        -u <SOURCE_DIR>/src/cairo-misc.c
                        -i ${CMAKE_CURRENT_LIST_DIR}/cairo_fopen_mingw.patch
    BUILD_COMMAND       cd build && meson compile
    INSTALL_COMMAND     cd build && meson install
    BUILD_IN_SOURCE     TRUE
    BUILD_BYPRODUCTS    <INSTALL_DIR>/lib/libcairo.a
)

add_dependencies(cairo-build pixman freetype)

ExternalProject_Get_Property(cairo-build INSTALL_DIR)

file(MAKE_DIRECTORY ${INSTALL_DIR}/include)
add_library(cairo STATIC IMPORTED GLOBAL)
set_target_properties(cairo
    PROPERTIES
        IMPORTED_LOCATION ${INSTALL_DIR}/lib/libcairo.a
        INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include
        VERSION ${DEP_CAIRO_VERSION}
)

add_dependencies(cairo cairo-build)