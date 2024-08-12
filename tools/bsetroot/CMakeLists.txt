cmake_minimum_required (VERSION 3.13)

option(USING_CXIMAGELIB "-- using cximage library instead of FreeImage library" ON)

set (PROJECT bsetroot) 
project (${PROJECT})

set (INCLUDE_DIRS
    .
    ${BLACKBOX_INCLUDES}
    ) 

if (USING_CXIMAGELIB)
    list(APPEND INCLUDE_DIRS ${CXIMAGE_INCLUDE_DIRS})
endif()

set (LINK_DIRS
    ) 

set (HEADERS
) 
set (SOURCES
    bsetroot.cpp
    ${BLACKBOX_INCLUDE_DIR}/BImage.cpp # hack!! remove in future !
    bsrt-rsc.rc
) 

if (USING_CXIMAGELIB)
    list(APPEND SOURCES image_cx.cpp)
endif()

set (LIBS
    ${BBLIB_LIBRARY}
    ${CXIMAGE_LIBRARIES}
)

if (WIN32)
    list(APPEND LIBS user32; kernel32; gdi32; shell32; comctl32; comdlg32; shlwapi; advapi32; version; ole32; winmm) 
endif() 

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES}) 

# suppress console window in gui apps
if(WIN32)
  set (GUI_TYPE WIN32)
else()
  set (GUI_TYPE )
endif() 
# console type

if(UNIX AND (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) )
  list(APPEND LIBS dl) 
endif()

# bsetroot executable
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

# copy resource files
if (NEED_COPY_FILES)
    set (RESOURCE_FILES_TOCOPY
        bsetroot.rc
    )
    add_target_copy_files_to_output_dir(${PROJECT} "${RESOURCE_FILES_TOCOPY}")
endif()


# bsetbg executable
set (PROJECT_BSETBG bsetbg) 

set (SOURCES_BSETBG
    bsetbg.cpp
    bsbg-rsc.rc
) 

set (LIBS_BSETBG
    #bblib
    #${CXIMAGE_LIBRARIES}
)
if (WIN32)
    list(APPEND LIBS_BSETBG user32; kernel32; gdi32; shell32; comctl32; comdlg32; shlwapi; advapi32; version; ole32; winmm) 
endif() 

add_executable (${PROJECT_BSETBG} ${GUI_TYPE} ${HEADERS} ${SOURCES_BSETBG}) 
target_link_libraries (${PROJECT_BSETBG} PRIVATE ${LIBS_BSETBG})

# add optional compile options
if( MSVC ) 
    # common options
    target_compile_options(${PROJECT_BSETBG} PRIVATE /nologo )
    # add latest c++ standard support
    target_compile_options(${PROJECT_BSETBG} PRIVATE /std:c++latest /source-charset:utf-8)
    # add pdb to realease build too
    target_compile_options(${PROJECT_BSETBG} PRIVATE /Zi)
    target_link_options(${PROJECT_BSETBG} PRIVATE $<$<CONFIG:RELEASE>:/OPT:ICF /OPT:REF /DEBUG /O1>)
elseif ( MINGW )
    # common options
    target_compile_options(${PROJECT_BSETBG} PRIVATE -Os -Wall -fno-strict-aliasing)
    # add latest c++ standard support
    #target_compile_options(${PROJECT_BSETBG} PRIVATE -std=c++2b)
    target_link_options(${PROJECT_BSETBG} PRIVATE $<$<CONFIG:RELEASE>:-s>)
endif ()

# determine architecture - bitness
if(CMAKE_SIZEOF_VOID_P EQUAL 8)  # means x64
    target_compile_definitions(${PROJECT_BSETBG} PRIVATE _WIN64 WIN64)
endif() 
