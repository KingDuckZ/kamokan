project(kamokan_impl VERSION 0.3.0 LANGUAGES CXX)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(SourceHighlight REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC
	response.cpp
	submit_paste_response.cpp
	index_response.cpp
	pastie_response.cpp
	ini_file.cpp
	pathname/pathname.cpp
	response_factory.cpp
	highlight_functions.cpp
	settings_bag.cpp
	error_response.cpp
	quick_submit_paste_response.cpp
	storage.cpp
	string_conv.cpp
	edit_response.cpp
	general_pastie_response.cpp
	${CMAKE_CURRENT_BINARY_DIR}/include/lua_scripts_for_redis.cpp
	redis_to_error_reason.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include
	PUBLIC ${TAWASHI_SOURCE_ROOT}/lib/kakoune
	PUBLIC ${TAWASHI_SOURCE_ROOT}/lib/mstch/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(${PROJECT_NAME} SYSTEM
	PUBLIC ${Boost_INCLUDE_DIRS}
	PUBLIC ${TAWASHI_SOURCE_ROOT}/lib/better-enums
	PRIVATE ${TAWASHI_SOURCE_ROOT}/lib/utf8_v2_3_4/source
	PUBLIC ${TAWASHI_SOURCE_ROOT}/lib/spdlog/include
	PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE ${Boost_LIBRARIES}
	PRIVATE incredis
	PRIVATE SourceHighlight::SourceHighlight
	PUBLIC mstch
	PRIVATE Threads::Threads
	PUBLIC tawashi
)
target_compile_definitions(${PROJECT_NAME}
	PRIVATE BOOST_SPIRIT_USE_PHOENIX_V3=1
	PUBLIC $<$<CONFIG:Debug>:SPDLOG_DEBUG_ON>
	PUBLIC $<$<CONFIG:Debug>:SPDLOG_TRACE_ON>
	PUBLIC $<$<BOOL:${BUILD_TESTING}>:kamokan_virtual_testing=virtual>
	PUBLIC $<$<NOT:$<BOOL:${BUILD_TESTING}>>:kamokan_virtual_testing=>
	PUBLIC $<$<BOOL:${BUILD_TESTING}>:KAMOKAN_WITH_TESTING>
)
target_compile_options(${PROJECT_NAME}
	PRIVATE -fdiagnostics-color=always
)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/kamokan_config.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/include/kamokan_config.h"
)

add_custom_command(
	OUTPUT include/lua_scripts_for_redis.cpp
	COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" -P ${CMAKE_CURRENT_SOURCE_DIR}/lua_to_cpp.cmake
	DEPENDS
		${CMAKE_CURRENT_SOURCE_DIR}/retrieve_pastie.lua
		${CMAKE_CURRENT_SOURCE_DIR}/save_pastie.lua
		${CMAKE_CURRENT_SOURCE_DIR}/add_highlighted_pastie.lua
	COMMENT "Embedding save/load lua scripts into the c++ code"
)
