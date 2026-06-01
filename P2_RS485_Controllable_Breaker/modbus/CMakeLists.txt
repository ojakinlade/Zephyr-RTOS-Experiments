target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_sources(app PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src/modbus_server.c
)
