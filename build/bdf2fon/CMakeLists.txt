cmake_minimum_required (VERSION 3.13)

set (PROJECT fnt2fon) 
project (${PROJECT})

set (INCLUDE_DIRS
    .
    ) 

set (LINK_DIRS
    ) 

set (HEADERS
) 
set (SOURCES
    fnt2fon.c
) 

set (LIBS
    #bblib
)

if (WIN32)
    list(APPEND LIBS user32; kernel32; gdi32; shell32; comctl32; comdlg32; shlwapi; advapi32; version; ole32; winmm) 
endif() 

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES}) 

# console type
set (GUI_TYPE)

if(UNIX AND (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) )
  list(APPEND LIBS dl) 
endif()

# main executable
add_executable (${PROJECT} ${GUI_TYPE} ${HEADERS} ${SOURCES}) 
target_link_directories(${PROJECT} PRIVATE ${LINK_DIRS})
target_include_directories (${PROJECT} PRIVATE ${INCLUDE_DIRS} )
target_link_libraries (${PROJECT} PRIVATE ${LIBS})
#target_compile_definitions(${PROJECT} PRIVATE MYCOOLDEF)

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


# bdf2fnt.exe

set (PROJECT_BDF2FNT bdf2fnt) 
set (SOURCES_BDF2FNT
    bdf2fnt.c
) 

# main executable
add_executable (${PROJECT_BDF2FNT} ${GUI_TYPE} ${HEADERS} ${SOURCES_BDF2FNT}) 
target_link_directories(${PROJECT_BDF2FNT} PRIVATE ${LINK_DIRS})
target_include_directories (${PROJECT_BDF2FNT} PRIVATE ${INCLUDE_DIRS} )
target_link_libraries (${PROJECT_BDF2FNT} PRIVATE ${LIBS})
#target_compile_definitions(${PROJECT_BDF2FNT} PRIVATE MYCOOLDEF)

# add optional compile options
if( MSVC ) 
    # common options
    target_compile_options(${PROJECT_BDF2FNT} PRIVATE /nologo )
    # add latest c++ standard support
    target_compile_options(${PROJECT_BDF2FNT} PRIVATE /std:c++latest /source-charset:utf-8)
    # add pdb to realease build too
    target_compile_options(${PROJECT_BDF2FNT} PRIVATE /Zi)
    target_link_options(${PROJECT_BDF2FNT} PRIVATE $<$<CONFIG:RELEASE>:/OPT:ICF /OPT:REF /DEBUG /O1>)
elseif ( MINGW )
    # common options
    target_compile_options(${PROJECT_BDF2FNT} PRIVATE -Os -Wall -fno-strict-aliasing)
    # add latest c++ standard support
    #target_compile_options(${PROJECT_BDF2FNT} PRIVATE -std=c++2b)
    target_link_options(${PROJECT_BDF2FNT} PRIVATE $<$<CONFIG:RELEASE>:-s>)
endif ()

# determine architecture - bitness
if(CMAKE_SIZEOF_VOID_P EQUAL 8)  # means x64
    target_compile_definitions(${PROJECT_BDF2FNT} PRIVATE _WIN64 WIN64)
endif() 
