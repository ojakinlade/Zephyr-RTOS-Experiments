target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_sources(app PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src/pd195z_e31.c
)
