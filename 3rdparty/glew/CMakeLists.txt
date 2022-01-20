# glew version 2.2.0
set(DEP_GLEW_VERSION "2.2.0")
set(DEP_GLEW_LOC "https://sourceforge.net/projects/glew/files/glew/2.2.0/glew-2.2.0.zip/download")

include(ExternalProject)

find_program(MAKE_EXE NAMES gmake nmake make)

# GLEW will use "libGLEW" as an output on Unix platforms
if(WIN32)
    set(LIBRARY_OUTPUT "libglew32.a")
elseif(UNIX)
    set(LIBRARY_OUTPUT "libGLEW.a")
endif()

ExternalProject_Add(glew-build
    URL                 ${DEP_GLEW_LOC}
    URL_HASH            SHA256=a9046a913774395a095edcc0b0ac2d81c3aacca61787b39839b941e9be14e0d4
    SOURCE_SUBDIR       build/cmake
    CMAKE_ARGS          -G "Unix Makefiles"
                        -DCMAKE_BUILD_TYPE=Release
                        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                        -DCMAKE_C_FLAGS=-fPIC
                        -DGLEW_X11=OFF
                        -DBUILD_UTILS=OFF
                        -DBUILD_SHARED_LIBS=OFF
    BUILD_BYPRODUCTS    <INSTALL_DIR>/lib/${LIBRARY_OUTPUT}
)

ExternalProject_Get_Property(glew-build INSTALL_DIR)

file(MAKE_DIRECTORY ${INSTALL_DIR}/include)
add_library(glew STATIC IMPORTED GLOBAL)
set_target_properties(glew
    PROPERTIES
        IMPORTED_LOCATION ${INSTALL_DIR}/lib/${LIBRARY_OUTPUT}
        INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include
        VERSION ${DEP_GLEW_VERSION}
)

add_dependencies(glew glew-build)