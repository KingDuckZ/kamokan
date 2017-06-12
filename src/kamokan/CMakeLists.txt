project(kamokan LANGUAGES CXX)

set(KAMOKAN_CONFIG_PATH "etc" CACHE STRING "Path where config file will be located, absolute or relative to the install prefix")
set(KAMOKAN_CONFIG_FILE "kamokan.ini" CACHE STRING "Filename of the config file in KAMOKAN_CONFIG_PATH")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
	main.cpp
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE kamokan_impl
	#hack - add duckhandy to the project instead of picking from inside redis
	PRIVATE duckhandy
)

set_target_properties(
	${PROJECT_NAME}
	PROPERTIES SUFFIX .cgi
)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)
