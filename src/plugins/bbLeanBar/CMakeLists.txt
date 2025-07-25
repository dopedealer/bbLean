cmake_minimum_required (VERSION 3.13)

set (PROJECT bbLeanBar) 
project (${PROJECT})

set (INCLUDE_DIRS
    .
    ${BLACKBOX_INCLUDES}
    ../bbPlugin/
    ) 

set (LINK_DIRS
    ) 

set (HEADERS
    baritem.h
    barinfo.h
    baritemlist.h
    barlabel.h
    bar_button.h
    BuffBmp.h
    bbLeanBar.h
    clock_displ.h
    leanbar.h
    spacer.h
    taskentry.h
    taskitemlist.h
    TinyDropTarg.h
    trayentry.h
    trayitemlist.h
    window_label.h
    workspace_label.h
    ) 

set (SOURCES
    baritem.cpp
    barinfo.cpp
    barlabel.cpp
    bar_button.cpp
    baritemlist.cpp
    bbLeanBar.cpp
    BuffBmp.cpp
    clock_displ.cpp
    leanbar.cpp
    spacer.cpp
    taskentry.cpp
    taskitemlist.cpp
    TinyDropTarg.cpp
    trayentry.cpp
    trayitemlist.cpp
    window_label.cpp
    workspace_label.cpp
    ) 

set (LIBS
    ${BLACKBOX_LIBRARIES}
    bbPlugin
)

if (WIN32)
    list(APPEND LIBS user32; kernel32; gdi32; shell32; comctl32; comdlg32; shlwapi; advapi32; version; ole32; winmm) 
endif() 

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES}) 

if(UNIX AND (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) )
  list(APPEND LIBS dl) 
endif()

# main executable
add_library (${PROJECT} SHARED ${HEADERS} ${SOURCES})
target_link_directories(${PROJECT} PRIVATE ${LINK_DIRS})
target_include_directories (${PROJECT} PRIVATE ${INCLUDE_DIRS} )
target_link_libraries (${PROJECT} PRIVATE ${LIBS})
# removing "lib" prefix
set_target_properties(${PROJECT} PROPERTIES PREFIX "")

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
    #target_compile_options(${PROJECT} PRIVATE -std=c++2b)
    target_link_options(${PROJECT} PRIVATE $<$<CONFIG:RELEASE>:-s>)
endif ()

# determine architecture - bitness
if(CMAKE_SIZEOF_VOID_P EQUAL 8)  # means x64
    target_compile_definitions(${PROJECT} PRIVATE _WIN64 WIN64)
endif() 

if (NEED_COPY_FILES)
    set (RESOURCE_FILES_BBLEANBAR
        readme.txt
        bbLeanBar.rc
    )
    add_target_copy_files_to_output_dir(${PROJECT} "${RESOURCE_FILES_BBLEANBAR}")
endif()
