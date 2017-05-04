project(tawashi_unittest CXX)

add_executable(${PROJECT_NAME}
	check.cpp
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
