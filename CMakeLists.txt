cmake_minimum_required(VERSION 3.17)

project(cairoxp CXX)
include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
set(CMAKE_POSITION_INDEPENDENT_CODE 1)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

# Compiler warnings
add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# Sanitizers
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# Static analysis
include(cmake/StaticAnalyzers.cmake)

add_subdirectory(3rdparty)
add_subdirectory(src)