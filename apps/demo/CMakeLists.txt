include(cotire)


add_executable (demo main.cpp)

target_link_libraries(demo PRIVATE
	ecs
	transforms
	rendering
	glbinding
	imgui
	audio
	asset
	core)

add_custom_target(copy_demo_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)

add_dependencies (demo copy_demo_assets)

if (UNIX)
    target_link_libraries(demo PRIVATE PRIVATE X11)
endif (UNIX)

if (CMAKE_DL_LIBS)
    target_link_libraries(demo PRIVATE ${CMAKE_DL_LIBS})
endif ()

set_target_properties(demo PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(demo)