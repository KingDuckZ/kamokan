project(duckbin VERSION 0.1.0 LANGUAGES CXX)

set(DUCKBIN_CONFIG_PATH "etc" CACHE STRING "Path where config file will be located, absolute or relative to the install prefix")
set(DUCKBIN_CONFIG_FILE "duckbin.ini" CACHE STRING "Filename of the config file in DUCKBIN_CONFIG_PATH")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
	main.cpp
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE tawashi
	#hack - add duckhandy to the project instead of picking from inside redis
	PRIVATE duckhandy
)

set_target_properties(
	${PROJECT_NAME}
	PROPERTIES SUFFIX .cgi
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include
)
install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/duckbin_config.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/include/duckbin_config.h"
)
