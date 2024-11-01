cmake_minimum_required (VERSION 3.13)

# TODO: if these dll will ever be used this makefile must be split on two
# separate ones.

set (PROJECT Hooks) 
project (${PROJECT})


include_directories (AFTER ../.
    .
    ../.
    )

set (HEADERS
    ) 

set (SOURCES
    )

set (LIBS
    )

if (WIN32)
    list(APPEND LIBS user32; kernel32; gdi32; shell32; comctl32; comdlg32; shlwapi; advapi32; version; ole32; winmm) 
endif() 

# for VIsual studio output only
source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES})

#add_library (${PROJECT} SHARED ${HEADERS} ${SOURCES})

# TODO: must be split to separate cmake files and rewritten
add_library (deskhook SHARED ${HEADERS} deskhook.cpp)
target_link_directories(deskhook PRIVATE ${LINK_DIRS})
target_include_directories (deskhook PRIVATE ${INCLUDE_DIRS} )
target_link_libraries (deskhook PRIVATE ${LIBS})
#target_compile_definitions(deskhook PRIVATE)

# add optional compile options
if( MSVC ) 
    # common options
    target_compile_options(deskhook PRIVATE /nologo )
    # add latest c++ standard support
    target_compile_options(deskhook PRIVATE /std:c++latest /source-charset:utf-8)
    # add pdb to realease build too
    target_compile_options(deskhook PRIVATE /Zi)
    target_link_options(deskhook PRIVATE $<$<CONFIG:RELEASE>:/OPT:ICF /OPT:REF /DEBUG /O1>)
elseif ( MINGW )
    # common options
    target_compile_options(deskhook PRIVATE -Os -Wall -fno-strict-aliasing)
    # add latest c++ standard support
    #target_compile_options(deskhook PRIVATE -std=c++2b)
    target_link_options(deskhook PRIVATE $<$<CONFIG:RELEASE>:-s>)
endif ()

# determine architecture - bitness
if(CMAKE_SIZEOF_VOID_P EQUAL 8)  # means x64
    target_compile_definitions(deskhook PRIVATE _WIN64 WIN64)
endif() 


add_library (trayhook SHARED ${HEADERS} trayhook.cpp)
target_link_directories(trayhook PRIVATE ${LINK_DIRS})
target_include_directories (trayhook PRIVATE ${INCLUDE_DIRS} )
target_link_libraries (trayhook ${LIBS})
#target_compile_definitions(trayhook PRIVATE)

# add optional compile options
if( MSVC ) 
    # common options
    target_compile_options(trayhook PRIVATE /nologo )
    # add latest c++ standard support
    target_compile_options(trayhook PRIVATE /std:c++latest /source-charset:utf-8)
    # add pdb to realease build too
    target_compile_options(trayhook PRIVATE /Zi)
    target_link_options(trayhook PRIVATE $<$<CONFIG:RELEASE>:/OPT:ICF /OPT:REF /DEBUG /O1>)
elseif ( MINGW )
    # common options
    target_compile_options(trayhook PRIVATE -Os -Wall -fno-strict-aliasing)
    # add latest c++ standard support
    #target_compile_options(trayhook PRIVATE -std=c++2b)
    target_link_options(trayhook PRIVATE $<$<CONFIG:RELEASE>:-s>)
endif ()

# determine architecture - bitness
if(CMAKE_SIZEOF_VOID_P EQUAL 8)  # means x64
    target_compile_definitions(trayhook PRIVATE _WIN64 WIN64)
endif() 

# removing "lib" prefix
#set_target_properties(${PROJECT} PROPERTIES PREFIX "")
set_target_properties(deskhook PROPERTIES PREFIX "")
set_target_properties(trayhook PROPERTIES PREFIX "")
