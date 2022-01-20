set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# Source file storage
add_library(project_source INTERFACE)

# Add subdirectories here that append source files to the 'library' above.

# Ex:
# add_subdirectory(util)
#
# -> Inside util directory "CMakeLists.txt" file:
#
#    target_sources(project_source INTERFACE
#      ${CMAKE_CURRENT_LIST_DIR}/log.cpp
#    )

# Various library definitions
target_compile_definitions(project_options INTERFACE
    -DGLEW_STATIC
    -DM_PI=3.1415926535897932
)

# X-Plane plugin
add_library(${CMAKE_PROJECT_NAME}_plugin SHARED xplane.cpp cairo_mt.cpp draw.cpp gl_pbo.cpp)

# Link our project libraries
add_library(project_libraries INTERFACE)

if(WIN32)
    target_link_libraries(project_libraries
        INTERFACE
            -static
            -static-libgcc
            -static-libstdc++
            -lopengl32
    )
elseif(UNIX AND NOT APPLE)
   target_link_libraries(project_libraries
        INTERFACE
            -static-libgcc
            -static-libstdc++
            -lm
            GL
   )
elseif(APPLE)
endif()

target_link_libraries(${CMAKE_PROJECT_NAME}_plugin
    PRIVATE
        project_source
        project_options
        project_warnings
        project_libraries
        Threads::Threads
        cairo
        freetype
        pixman
        glew
        xpsdk
)

set_target_properties(${CMAKE_PROJECT_NAME}_plugin PROPERTIES PREFIX "")
set_target_properties(${CMAKE_PROJECT_NAME}_plugin PROPERTIES SUFFIX "")
set_target_properties(${CMAKE_PROJECT_NAME}_plugin PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}.xpl")

# Installation
function(install_target target)
    if(WIN32)
        set(PLATFORM_DIR_NAME "win_x64")
    elseif(UNIX AND NOT APPLE)
        set(PLATFORM_DIR_NAME "lin_x64")
    elseif(APPLE)
        set(PLATFORM_DIR_NAME "mac_x64")
    endif()

    install(
        TARGETS ${target}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}/${PLATFORM_DIR_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}/${PLATFORM_DIR_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}/${PLATFORM_DIR_NAME}
    )
endfunction()

install_target(${CMAKE_PROJECT_NAME}_plugin)
