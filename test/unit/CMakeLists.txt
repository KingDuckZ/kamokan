project(tawashi_unittest CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
	check.cpp
	test_settings_bag.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${TAWASHI_SOURCE_ROOT}/lib/Catch/single_include
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE tawashi_implem
)

add_test(
	NAME TawashiUnitTest
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND ${PROJECT_NAME}
)
