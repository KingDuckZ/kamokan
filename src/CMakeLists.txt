project(tawashi CXX)

find_package(Boost 1.53.0 REQUIRED COMPONENTS program_options filesystem system)
find_package(CURL REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
	curl_wrapper.cpp
	index_response.cpp
)

target_include_directories(${PROJECT_NAME} SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
	PRIVATE ${TAWASHI_SOURCE_ROOT}/lib/bette-enums
	PRIVATE ${CURL_INCLUDE_DIR}
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE ${Boost_LIBRARIES}
	PRIVATE incredis
	${CURL_LIBRARIES}
)
target_compile_definitions(${PROJECT_NAME}
	PRIVATE BOOST_SPIRIT_USE_PHOENIX_V3=1
)
