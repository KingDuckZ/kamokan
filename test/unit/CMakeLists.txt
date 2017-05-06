project(tawashi_unittest CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
	check.cpp
	fake_cgi_env.cpp
	test_ini_file.cpp
	test_settings_bag.cpp
	test_index_response.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${TAWASHI_SOURCE_ROOT}/lib/Catch/single_include
	PRIVATE ${TAWASHI_GEN_INCLUDE_DIR}
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE tawashi_implem
	PRIVATE duckhandy
)

add_test(
	NAME TawashiUnitTest
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND ${PROJECT_NAME}
)
