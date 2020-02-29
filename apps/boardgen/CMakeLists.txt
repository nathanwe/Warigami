
add_executable (boardgen main.cpp)

target_link_libraries(boardgen PRIVATE core)



if (UNIX)
    target_link_libraries(boardgen PRIVATE PRIVATE X11)
endif (UNIX)

if (CMAKE_DL_LIBS)
    target_link_libraries(boardgen PRIVATE ${CMAKE_DL_LIBS})
endif ()
