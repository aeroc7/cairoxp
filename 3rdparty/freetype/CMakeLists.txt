# freetype version 2.10.4
set(DEP_FREETYPE_VERSION "2.10.4")
set(DEP_FREETYPE_LOC "https://gitlab.freedesktop.org/freetype/freetype/-/archive/VER-2-10-4/freetype-VER-2-10-4.zip")

include(ExternalProject)

ExternalProject_Add(freetype-build
    URL                 ${DEP_FREETYPE_LOC}
    URL_HASH            SHA256=0412aba9795726e72a1bc3496c5511236ea5ff20d55f55cb16e48cc60cb178b2
    CMAKE_ARGS          -G "Unix Makefiles"
                        -DCMAKE_BUILD_TYPE=Release
                        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                        -DCMAKE_C_FLAGS=-fPIC
                        -DCMAKE_DISABLE_FIND_PACKAGE_ZLIB=TRUE
                        -DCMAKE_DISABLE_FIND_PACKAGE_BZip2=TRUE
                        -DCMAKE_DISABLE_FIND_PACKAGE_PNG=TRUE
                        -DCMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=TRUE
                        -DCMAKE_DISABLE_FIND_PACKAGE_BrotliDec=TRUE
                        -DBUILD_SHARED_LIBS=OFF
    BUILD_BYPRODUCTS    <INSTALL_DIR>/lib/libfreetype.a
)

ExternalProject_Get_Property(freetype-build INSTALL_DIR)

file(MAKE_DIRECTORY ${INSTALL_DIR}/include/freetype2)
add_library(freetype STATIC IMPORTED GLOBAL)
set_target_properties(freetype
    PROPERTIES
        IMPORTED_LOCATION ${INSTALL_DIR}/lib/libfreetype.a
        INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include/freetype2
        VERSION ${DEP_FREETYPE_VERSION}
)

add_dependencies(freetype freetype-build)