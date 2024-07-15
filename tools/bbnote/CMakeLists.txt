cmake_minimum_required (VERSION 3.13)

set (PROJECT bbnote) 
project (${PROJECT})

set (INCLUDE_DIRS
    .
    ${BLACKBOX_INCLUDES}
    ) 

set (LINK_DIRS
    ) 

set (HEADERS
    bbstyle.h
    eddef.h
    edstruct.h
    vk.h
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
    ${BBCORE_INCLUDE_DIR}/Settings.cpp     # hack!! remove
    ${BBCORE_INCLUDE_DIR}/BImage.cpp       # hack!! remove
    resource.rc
) 

set (LIBS
    ${BBLIB_LIBRARY}
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
target_compile_definitions(${PROJECT} PRIVATE BBSETTING_STYLEREADER_ONLY BBSETTING_NOWINDOW)

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

# bbnote.dll

set (PROJECT_BBNOTEDLL bbnote_proxy) 

set (INCLUDE_DIRS_BBNOTEDLL
    .
    ${BLACKBOX_INCLUDES}
    ) 

set (SOURCES_BBNOTEDLL
    bbnote-proxy.cpp    ##
) 

set (LIBS_BBNOTEDLL
    #bblib
    ${BBCORE_LIBRARY}
)

if (WIN32)
    list(APPEND LIBS_BBNOTEDLL user32; kernel32; gdi32; shell32; comctl32; comdlg32; shlwapi; advapi32; version; ole32; winmm) 
endif() 


add_library (${PROJECT_BBNOTEDLL} SHARED ${HEADERS} ${SOURCES_BBNOTEDLL})
target_link_directories(${PROJECT_BBNOTEDLL} PRIVATE ${LINK_DIRS})
target_include_directories (${PROJECT_BBNOTEDLL} PRIVATE ${INCLUDE_DIRS_BBNOTEDLL} )
target_link_libraries (${PROJECT_BBNOTEDLL} PRIVATE ${LIBS_BBNOTEDLL})
#target_compile_definitions(${PROJECT_BBNOTEDLL} PRIVATE BBSETTING_STYLEREADER_ONLY BBSETTING_NOWINDOW)
target_compile_definitions(${PROJECT_BBNOTEDLL} PRIVATE)

# add optional compile options
if( MSVC ) 
    # common options
    target_compile_options(${PROJECT_BBNOTEDLL} PRIVATE /nologo )
    # add latest c++ standard support
    target_compile_options(${PROJECT_BBNOTEDLL} PRIVATE /std:c++latest /source-charset:utf-8)
    # add pdb to realease build too
    target_compile_options(${PROJECT_BBNOTEDLL} PRIVATE /Zi)
    target_link_options(${PROJECT_BBNOTEDLL} PRIVATE $<$<CONFIG:RELEASE>:/OPT:ICF /OPT:REF /DEBUG /O1>)
elseif ( MINGW )
    # common options
    target_compile_options(${PROJECT_BBNOTEDLL} PRIVATE -Os -Wall -fno-strict-aliasing)
    # add latest c++ standard support
    #target_compile_options(${PROJECT_BBNOTEDLL} PRIVATE -std=c++2b)
    target_link_options(${PROJECT_BBNOTEDLL} PRIVATE $<$<CONFIG:RELEASE>:-s>)
endif ()

# determine architecture - bitness
if(CMAKE_SIZEOF_VOID_P EQUAL 8)  # means x64
    target_compile_definitions(${PROJECT_BBNOTEDLL} PRIVATE _WIN64 WIN64)
endif() 
