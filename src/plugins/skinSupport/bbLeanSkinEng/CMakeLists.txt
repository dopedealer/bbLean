cmake_minimum_required (VERSION 3.13)

set (PROJECT bbLeanSkinEng) 
project (${PROJECT})

set(BBSKIN_COMMON_INCLUDES_DIR ${CMAKE_CURRENT_LIST_DIR}/../common/) 

set (INCLUDE_DIRS
    ${BLACKBOX_INCLUDES}
    ${BBSKIN_COMMON_INCLUDES_DIR}
    ${BBCORE_INCLUDES}
    ${BBPLUGIN_INCLUDE_DIR}
    ) 

set (LINK_DIRS
    ) 

set (HEADERS
    ${BBSKIN_COMMON_INCLUDES_DIR}/engine/hookinfo.h
    ${BBSKIN_COMMON_INCLUDES_DIR}/engine/subclass.h
) 
set (SOURCES
    hookctl.cpp
    subclass.cpp
) 

set (LIBS
    ${BBCORE_LIBRARY}
    ${BBPLUGIN_LIBRARY}
)

if (WIN32)
    list(APPEND LIBS user32; kernel32; gdi32; shell32; comctl32; comdlg32; shlwapi; advapi32; version; ole32; winmm) 
endif() 

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES}) 

if(UNIX AND (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) )
  list(APPEND LIBS dl) 
endif()

# secondary dll bbLeanSkinEng

add_library (${PROJECT} SHARED ${HEADERS} ${SOURCES})
target_link_directories(${PROJECT} PRIVATE ${LINK_DIRS})
target_include_directories (${PROJECT} PRIVATE ${INCLUDE_DIRS} )
target_link_libraries (${PROJECT} PRIVATE ${LIBS})
# removing "lib" prefix
set_target_properties(${PROJECT} PROPERTIES PREFIX "")

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


# copy resource, config files
if (NEED_COPY_FILES)
    set (RESOURCE_FILES_TOCOPY
        exclusions.rc
    )
    add_target_copy_files_to_output_dir(${PROJECT} "${RESOURCE_FILES_TOCOPY}")
endif()

get_current_output_dir(ARTIFACTS_OUTPUT_DIR)

if ("${ARTIFACTS_OUTPUT_DIR}" STREQUAL "")
    message( FATAL_ERROR "The output dir MUST NOT be empty!" )
endif()

# copy buttons.bmp to created dir with rename
add_custom_command(
    TARGET ${PROJECT}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ARTIFACTS_OUTPUT_DIR}/buttons"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/buttons/buttons_11x11.bmp" "${ARTIFACTS_OUTPUT_DIR}/buttons/buttons_11x11.bmp"
)
