cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(houdini C)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -Wno-missing-field-initializers")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -Wextra -Wno-missing-field-initializers")

add_library(${PROJECT_NAME} STATIC
	houdini/buffer.c
	houdini/houdini_href_e.c
	houdini/houdini_html_e.c
	houdini/houdini_html_u.c
	houdini/houdini_js_e.c
	houdini/houdini_js_u.c
	houdini/houdini_uri_e.c
	houdini/houdini_uri_u.c
	houdini/houdini_xml_e.c
)

target_include_directories(${PROJECT_NAME}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/houdini
)
