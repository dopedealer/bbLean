cmake_minimum_required (VERSION 3.13)

set (PROJECT bbnote) 
project (${PROJECT})


set(BBNOTE_COMMON_INCLUDES_DIR ${CMAKE_CURRENT_LIST_DIR}/../common/) 
set (INCLUDE_DIRS
    .
    #../common/
    ${BBNOTE_COMMON_INCLUDES_DIR}
    ${BLACKBOX_INCLUDES}
    ) 

set (LINK_DIRS
    ) 

set (HEADERS
    ${BBNOTE_COMMON_INCLUDES_DIR}/bbstyle_bbnote.h
    ${BBNOTE_COMMON_INCLUDES_DIR}/eddef.h
    ${BBNOTE_COMMON_INCLUDES_DIR}/edstruct.h
    ${BBNOTE_COMMON_INCLUDES_DIR}/vk.h
) 
set (SOURCES
    bbapi-small.cpp
    bbnote.cpp
    edfiles.cpp
    edfunc.cpp
    edprint.cpp
    edproc.cpp
    match3.cpp
    readstyle.cpp
    resource.rc
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

## suppress console window in gui apps
if(WIN32)
  set (GUI_TYPE WIN32)
else()
  set (GUI_TYPE )
endif() 
# console type
#set (GUI_TYPE)

if(UNIX AND (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) )
  list(APPEND LIBS dl) 
endif()

# main executable
add_executable (${PROJECT} ${GUI_TYPE} ${HEADERS} ${SOURCES}) 
target_link_directories(${PROJECT} PRIVATE ${LINK_DIRS})
target_include_directories (${PROJECT} PRIVATE ${INCLUDE_DIRS} )
target_link_libraries (${PROJECT} PRIVATE ${LIBS})
#target_compile_definitions(${PROJECT} PRIVATE MYDEF)

# add optional compile options
if( MSVC ) 
    # common options
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
    #target_compile_options(${PROJECT} PRIVATE -std=c++2b)
    target_link_options(${PROJECT} PRIVATE $<$<CONFIG:RELEASE>:-s>)
endif ()

# determine architecture - bitness
if(CMAKE_SIZEOF_VOID_P EQUAL 8)  # means x64
    target_compile_definitions(${PROJECT} PRIVATE _WIN64 WIN64)
endif() 
