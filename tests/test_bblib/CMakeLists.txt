cmake_minimum_required (VERSION 3.13)

set (PROJECT test_bblib) 
project (${PROJECT})


set (INCLUDE_DIRS
    ${GTEST_INCLUDE_DIRS}
    ${BBLIB_INCLUDE_DIR}
    ) 


set (LINK_DIRS
    ../../libs/
    ) 

set (HEADERS
) 

set (SOURCES
    test_main.cpp
    test_core.cpp
) 

set (LIBS
    ${BBLIB_LIBRARY}
    ${GTEST_LIBRARIES}
    )

if (WIN32)
    set(LIBS ${LIBS} Ws2_32 kernel32 user32)
endif ()

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES}) 

set (GUI_TYPE )


if(UNIX AND (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) )
  set(LIBS ${LIBS} dl)
endif()

# main executable
add_executable (${PROJECT} ${GUI_TYPE} ${HEADERS} ${SOURCES}) 
target_link_directories(${PROJECT} PUBLIC ${LINK_DIRS})
target_include_directories (${PROJECT} PUBLIC ${INCLUDE_DIRS} )
target_link_libraries (${PROJECT} ${LIBS})

add_test(
    NAME ${PROJECT}
    COMMAND ${PROJECT}
    )
