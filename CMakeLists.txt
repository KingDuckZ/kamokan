cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
project(tawashi_top)

set(TAWASHI_SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(lib/incredis)
add_subdirectory(src)
