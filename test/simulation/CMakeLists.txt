project(kamokan_simulation CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wpedantic -Wconversion -Werror")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG} -Wall -Wpedantic -Wconversion -Werror")

add_executable(${PROJECT_NAME}
	check.cpp
	test_index_response.cpp
	test_submit_pastie.cpp
	fake_storage.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${TAWASHI_SOURCE_ROOT}/lib/Catch/single_include
	PRIVATE ${TAWASHI_GEN_INCLUDE_DIR}
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE kamokan_impl
	PRIVATE duckhandy
)

add_test(
	NAME TawashiSimulationTest
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND ${PROJECT_NAME}
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/simulation_config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/include/simulation_config.h
)
