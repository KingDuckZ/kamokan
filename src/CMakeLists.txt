project(tawashi CXX)

find_package(Boost 1.53.0 REQUIRED COMPONENTS program_options filesystem system)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
	main.cpp
	split_get_vars.cpp
	response.cpp
	submit_form_response.cpp
	get_env.cpp
	envy.cpp
)

target_include_directories(${PROJECT_NAME} SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
	PRIVATE ${TAWASHI_SOURCE_ROOT}/lib/bette-enums
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE ${Boost_LIBRARIES}
	PRIVATE incredis
)
