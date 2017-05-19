cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
project(tawashi_top VERSION 0.1.9 LANGUAGES NONE)

include(CTest)

set(INCREDIS_FORCE_DISABLE_TESTS ON)
set(TAWASHI_SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(TAWASHI_GEN_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")
set(TAWASHI_CONFIG_PATH "etc" CACHE STRING "Path where config file will be located, absolute or relative to the install prefix")
set(TAWASHI_CONFIG_FILE "tawashi.ini" CACHE STRING "Filename of the config file in TAWASHI_CONFIG_PATH")

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/src/tawashiConfig.h.in"
	"${TAWASHI_GEN_INCLUDE_DIR}/tawashiConfig.h"
)
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/config/debug_tawashi.ini.in"
	"${CMAKE_CURRENT_BINARY_DIR}/tawashi.ini"
)

add_subdirectory(lib/incredis)
add_subdirectory(lib/mstch)
add_subdirectory(lib/houdini)
add_subdirectory(src/tawashi_implem)
add_subdirectory(src/tawashi)

install(DIRECTORY html/website/ DESTINATION html)

if (BUILD_TESTING)
	add_subdirectory(test/unit)
endif()
