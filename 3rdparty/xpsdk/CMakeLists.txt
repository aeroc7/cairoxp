# X-Plane SDK version 303
set(DEP_XPLANE_SDK_VERSION "303")
set(DEP_XPLANE_SDK_LOC "http://developer.x-plane.com/wp-content/plugins/code-sample-generation/sample_templates/XPSDK303.zip")

include(ExternalProject)

ExternalProject_Add(xpsdk-build
    URL                 ${DEP_XPLANE_SDK_LOC}
    URL_HASH            SHA256=11a550dc553876d8c0b7187ae2e1c7e9fee5909dce6d8469720cad51e5bcb7ff
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       ""
    INSTALL_COMMAND     ""
)

ExternalProject_Get_Property(xpsdk-build SOURCE_DIR)

add_library(xpsdk INTERFACE)
target_include_directories(xpsdk INTERFACE ${SOURCE_DIR}/CHeaders/XPLM)

if(ENABLE_XPLANE_BUILD)
    if(WIN32)
        target_link_libraries(xpsdk INTERFACE ${SOURCE_DIR}/Libraries/Win/XPLM_64.lib)
    elseif(APPLE)
        target_link_libraries(xpsdk INTERFACE "-F${SOURCE_DIR}/Libraries/Mac/ -framework XPLM")
    endif()
endif()

# Supported SDK versions
set(SDK_DEFS -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM303=1)

if(WIN32)
    list(APPEND SDK_DEFS APL=0 IBM=1 LIN=0)
elseif(APPLE)
    list(APPEND SDK_DEFS APL=1 IBM=0 LIN=0)
else()
    list(APPEND SDK_DEFS APL=0 IBM=0 LIN=1)
endif()

target_compile_definitions(xpsdk INTERFACE
    ${SDK_DEFS}
)

# CMake bug? Can't add properties to INTERFACE libraries, so we create
# an entire separate library to communicate our SDK version
add_library(xpsdk_properties STATIC IMPORTED GLOBAL)
set_target_properties(xpsdk_properties
    PROPERTIES
        VERSION ${DEP_XPLANE_SDK_VERSION}
)

add_dependencies(xpsdk xpsdk_properties xpsdk-build)