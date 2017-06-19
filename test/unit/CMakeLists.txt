project(tawashi_unittest CXX C)

find_package(GLIB 2.20 REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wpedantic -Wconversion -Werror")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG} -Wall -Wpedantic -Wconversion -Werror")

add_executable(${PROJECT_NAME}
	check.cpp
	test_ini_file.cpp
	test_settings_bag.cpp
	test_invalid_utf8_post.cpp
	../data/UTF-8-test.txt.c
	test_invalid_utf8_get.cpp
	test_mime_split.cpp
	test_html_escape.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${TAWASHI_SOURCE_ROOT}/lib/Catch/single_include
	PRIVATE ${TAWASHI_GEN_INCLUDE_DIR}
	PRIVATE ${GLIB_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE tawashi
	PRIVATE duckhandy
	PRIVATE kamokan_impl
	PRIVATE ${GLIB_LIBRARIES}
)

add_test(
	NAME TawashiUnitTest
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND ${PROJECT_NAME}
)
