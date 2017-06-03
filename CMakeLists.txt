cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
project(kamokan_top LANGUAGES NONE)

include(CTest)

set(INCREDIS_FORCE_DISABLE_TESTS ON)
set(TAWASHI_SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/config/debug_kamokan.ini.in"
	"${CMAKE_CURRENT_BINARY_DIR}/kamokan.ini"
)

add_subdirectory(lib/incredis)
add_subdirectory(lib/mstch)
add_subdirectory(lib/houdini)
add_subdirectory(src/tawashi)
add_subdirectory(src/kamokan)

install(DIRECTORY html/website/ DESTINATION html)

if (BUILD_TESTING)
	add_subdirectory(test/unit)
	add_subdirectory(test/simulation)
endif()
