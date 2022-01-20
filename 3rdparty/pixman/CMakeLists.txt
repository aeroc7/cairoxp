# pixman version 0.40.0
set(DEP_PIXMAN_VERSION "0.40.0")
set(DEP_PIXMAN_LOC "https://gitlab.freedesktop.org/pixman/pixman/-/archive/pixman-0.40.0/pixman-pixman-0.40.0.zip")

include(ExternalProject)

find_program(MAKE_EXE NAMES gmake nmake make)

ExternalProject_Add(pixman-build
    URL                 ${DEP_PIXMAN_LOC}
    URL_HASH            SHA256=e44eba32adab5204f16e7f86cb9004d01ca570c38985b7a9c21fd8815ad3e844
    CONFIGURE_COMMAND   meson build --prefix=<INSTALL_DIR> --libdir "lib" --buildtype release
                        --default-library=static -D openmp=disabled -D gtk=disabled -D libpng=disabled
                        -D tests=disabled
    PATCH_COMMAND       patch
                        -u <SOURCE_DIR>/test/meson.build
                        -i ${CMAKE_CURRENT_LIST_DIR}/pixman_prng_test_failure.patch
    BUILD_COMMAND       cd build && meson compile
    INSTALL_COMMAND     cd build && meson install
    BUILD_IN_SOURCE     TRUE
    BUILD_BYPRODUCTS    <INSTALL_DIR>/lib/libpixman-1.a
)

ExternalProject_Get_Property(pixman-build INSTALL_DIR)

file(MAKE_DIRECTORY ${INSTALL_DIR}/include/pixman-1)
add_library(pixman STATIC IMPORTED GLOBAL)
set_target_properties(pixman
    PROPERTIES
        IMPORTED_LOCATION ${INSTALL_DIR}/lib/libpixman-1.a
        INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include/pixman-1
        VERSION ${DEP_PIXMAN_VERSION}
)

add_dependencies(pixman pixman-build)