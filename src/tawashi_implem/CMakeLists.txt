project(tawashi_implem LANGUAGES CXX C)

find_package(Boost 1.53.0 REQUIRED COMPONENTS program_options filesystem system)
find_package(SourceHighlight REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC
	split_get_vars.cpp
	response.cpp
	submit_paste_response.cpp
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
	tiger.c
	error_response.cpp
	tawashi_exception.cpp
	http_header.cpp
	quick_submit_paste_response.cpp
	ip_utils.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${TAWASHI_GEN_INCLUDE_DIR}
	PUBLIC ${TAWASHI_SOURCE_ROOT}/lib/kakoune
	PUBLIC ${TAWASHI_SOURCE_ROOT}/lib/mstch/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(${PROJECT_NAME} SYSTEM
	PUBLIC ${Boost_INCLUDE_DIRS}
	PUBLIC ${TAWASHI_SOURCE_ROOT}/lib/better-enums
	PRIVATE ${SourceHighlight_INCLUDE_DIR}
	PRIVATE ${TAWASHI_SOURCE_ROOT}/lib/utf8_v2_3_4/source
	PUBLIC ${TAWASHI_SOURCE_ROOT}/lib/spdlog/include
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE ${Boost_LIBRARIES}
	PRIVATE incredis
	PRIVATE ${SourceHighlight_LIBRARIES}
	PUBLIC mstch
	PRIVATE houdini
	PRIVATE pthread
)
target_compile_definitions(${PROJECT_NAME}
	PRIVATE BOOST_SPIRIT_USE_PHOENIX_V3=1
	PUBLIC $<$<CONFIG:Debug>:SPDLOG_DEBUG_ON>
	PUBLIC $<$<CONFIG:Debug>:SPDLOG_TRACE_ON>
)
target_compile_options(${PROJECT_NAME}
	PRIVATE -fdiagnostics-color=always
)
