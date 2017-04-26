project(tawashi VERSION 0.1.3 LANGUAGES CXX)

find_package(Boost 1.53.0 REQUIRED COMPONENTS program_options filesystem system)
find_package(SourceHighlight REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TAWASHI_CONFIG_PATH "etc" CACHE STRING "Path where config file will be located, absolute or relative to the install prefix")
set(TAWASHI_CONFIG_FILE "${PROJECT_NAME}.ini" CACHE STRING "Filename of the config file in TAWASHI_CONFIG_PATH")

add_executable(${PROJECT_NAME}
	main.cpp
	split_get_vars.cpp
	response.cpp
	submit_paste_response.cpp
	get_env.cpp
	cgi_environment_vars.cpp
	cgi_env.cpp
	num_to_token.cpp
	cgi_post.cpp
	escapist.cpp
	index_response.cpp
	pastie_response.cpp
	ini_file.cpp
	pathname/pathname.cpp
	response_factory.cpp
	list_highlight_langs.cpp
	settings_bag.cpp
	sanitized_utf8.cpp
)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}Config.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}Config.h"
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include
	PRIVATE ${TAWASHI_SOURCE_ROOT}/lib/kakoune
	PRIVATE ${TAWASHI_SOURCE_ROOT}/lib/mstch/include
)
target_include_directories(${PROJECT_NAME} SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
	PRIVATE ${TAWASHI_SOURCE_ROOT}/lib/better-enums
	PRIVATE ${SourceHighlight_INCLUDE_DIR}
	PRIVATE ${TAWASHI_SOURCE_ROOT}/lib/utf8_v2_3_4/source
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE ${Boost_LIBRARIES}
	PRIVATE incredis
	PRIVATE ${SourceHighlight_LIBRARIES}
	PRIVATE mstch
	PRIVATE houdini
)
target_compile_definitions(${PROJECT_NAME}
	PRIVATE BOOST_SPIRIT_USE_PHOENIX_V3=1
)
target_compile_options(${PROJECT_NAME}
	PRIVATE -fdiagnostics-color=always
)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)
