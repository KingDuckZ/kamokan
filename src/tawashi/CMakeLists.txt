project(tawashi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
	main.cpp
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE tawashi_implem
)

set_target_properties(
	${PROJECT_NAME}
	PROPERTIES SUFFIX .cgi
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${TAWASHI_GEN_INCLUDE_DIR}
	#hack - add duckhandy to the project instead of picking from inside redis
	PRIVATE ${TAWASHI_SOURCE_ROOT}/lib/incredis/lib/duckhandy/include
)
install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib/static
)