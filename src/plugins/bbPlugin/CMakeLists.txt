cmake_minimum_required (VERSION 3.13)

set (PROJECT bbPlugin) 
project (${PROJECT})

set (INCLUDE_DIRS
    .
    ${BBLIB_INCLUDE_DIR}
    ${BBCORE_INCLUDES}
    ) 

set (LINK_DIRS
    #../../libs/
    ) 

set (HEADERS
    bbPlugin.h
    tooltips.h
    ) 

set (SOURCES
    bbPlugin.cpp
    sysmenu.cpp
    tooltips.cpp
    ) 

set (LIBS
    ${BBLIB_LIBRARY}
    ${BBCORE_LIBRARY}
    )

if (WIN32)
    list(APPEND LIBS user32; kernel32; gdi32; shell32; comctl32; comdlg32; shlwapi; advapi32; version; ole32; winmm) 
endif() 

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES}) 

if(UNIX AND (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) )
  list(APPEND LIBS dl) 
endif()

# export variables
set(BBPLUGIN_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/. CACHE INTERNAL "") 
set(BBPLUGIN_LIBRARY ${PROJECT} CACHE INTERNAL "") 

# library 
add_library (${PROJECT} STATIC ${HEADERS} ${SOURCES})
target_link_directories(${PROJECT} PRIVATE ${LINK_DIRS})
target_include_directories (${PROJECT} PRIVATE ${INCLUDE_DIRS} )
target_link_libraries (${PROJECT} PRIVATE ${LIBS})
#target_compile_definitions(${PROJECT} PRIVATE MYDEF)

# add optional compile options
if( MSVC ) 
    # common options
    #target_compile_options(${PROJECT} PRIVATE /MD /O1 /W3 /nologo /GS- /EHsc)
    target_compile_options(${PROJECT} PRIVATE /nologo )
    # add latest c++ standard support
    target_compile_options(${PROJECT} PRIVATE /std:c++latest /source-charset:utf-8)
    # add pdb to realease build too
    target_compile_options(${PROJECT} PRIVATE /Zi)
    target_link_options(${PROJECT} PRIVATE $<$<CONFIG:RELEASE>:/OPT:ICF /OPT:REF /DEBUG /O1>)
elseif ( MINGW )
    # common options
    target_compile_options(${PROJECT} PRIVATE -Os -Wall -fno-strict-aliasing)
    # add latest c++ standard support
    target_compile_options(${PROJECT} PRIVATE -std=c++2b)
    target_link_options(${PROJECT} PRIVATE $<$<CONFIG:RELEASE>:-s>)
endif ()

# determine architecture - bitness
if(CMAKE_SIZEOF_VOID_P EQUAL 8)  # means x64
    target_compile_definitions(${PROJECT} PRIVATE _WIN64 WIN64)
endif() 
