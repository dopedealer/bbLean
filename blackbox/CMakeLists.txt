cmake_minimum_required (VERSION 3.21)

# Exported variables:
# Includes:
# BBLIB_INCLUDE_DIR - dir with helper lib headers
# BLACKBOX_INCLUDE_DIR - dir with blackbox core headers
# BLACKBOX_INCLUDES - both dirs with headers of bblib and blackbox
#
# Libraries:
# BBLIB_LIBRARY - helper library
# BLACKBOX_LIBRARY - blackbox core library
# BLACKBOX_LIBRARIES - contains both bblib and blackbox libs

set (PROJECT blackbox) 
project (${PROJECT})

set (INCLUDE_DIRS
    .
    ${BBLIB_INCLUDE_DIR}
    ) 

set (LINK_DIRS
    ) 

set (HEADERS
    BB.h
    BBApi.h
    BBSendData.h
    BBVWM.h
    BImage.h
    Desk.h
    MessageManager.h
    PluginManager.h
    Settings.h
    Stylestruct.h
    Toolbar.h
    Tray.h
    win0x500.h
    Workspaces.h
    Menu/Menu.h
    Menu/MenuMaker.h
) 
set (SOURCES
    BBApi.cpp
    BBRoot.cpp
    BBVWM.cpp
    BImage.cpp
    Blackbox.cpp
    ConfigMenu.cpp
    Desk.cpp
    DesktopMenu.cpp
    MessageManager.cpp
    PluginManager.cpp
    Settings.cpp
    Toolbar.cpp
    Tray.cpp
    Utils.cpp
    Workspaces.cpp
    Menu/CommandItem.cpp
    Menu/Contextmenu.cpp
    Menu/Dragsource.cpp
    Menu/Droptarget.cpp
    Menu/FolderItem.cpp
    Menu/Menu.cpp
    Menu/MenuItem.cpp
    Menu/MenuMaker.cpp
    Menu/SpecialFolder.cpp
    Menu/TitleItem.cpp
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

# export include variables 
set(BLACKBOX_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/. CACHE INTERNAL "") 
set(BLACKBOX_INCLUDES ${BLACKBOX_INCLUDE_DIR} ${BBLIB_INCLUDE_DIR} CACHE INTERNAL "") 

# export libs variables
set(BLACKBOX_LIBRARY ${PROJECT} CACHE INTERNAL "") 
set(BLACKBOX_LIBRARIES ${BBLIB_LIBRARY} ${BLACKBOX_LIBRARY} CACHE INTERNAL "") 

# main executable
add_executable (${PROJECT} ${GUI_TYPE} ${HEADERS} ${SOURCES}) 
target_link_directories(${PROJECT} PRIVATE ${LINK_DIRS})
target_include_directories (${PROJECT} PRIVATE ${INCLUDE_DIRS} )
target_link_libraries (${PROJECT} PRIVATE ${LIBS})
target_compile_definitions(${PROJECT} PRIVATE __BBCORE__ BBCORE_STATIC)

## It is a dirty hack but existing compiled plugins do import functions
## directly from blackbox.exe. So we just cannot to remove it without breaking
## compatibility with existing plugins.
set_property(TARGET ${PROJECT} PROPERTY ENABLE_EXPORTS 1)

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

# copying resources
if (NEED_COPY_FILES)
    set (RESOURCE_FILES_TOCOPY
        ${CMAKE_CURRENT_LIST_DIR}/../menu.rc
        ${CMAKE_CURRENT_LIST_DIR}/../blackbox.rc
        ${CMAKE_CURRENT_LIST_DIR}/../extensions.rc
        ${CMAKE_CURRENT_LIST_DIR}/../plugins.rc
        ${CMAKE_CURRENT_LIST_DIR}/../shellfolders.rc
        ${CMAKE_CURRENT_LIST_DIR}/../stickywindows.ini
    )
    add_target_copy_files_to_output_dir(${PROJECT} "${RESOURCE_FILES_TOCOPY}")

    if (USE_BUILTIN_STYLES)
        get_current_output_dir(ARTIFACTS_OUTPUT_DIR)
        add_custom_command(
            TARGET ${PROJECT}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ARTIFACTS_OUTPUT_DIR}/styles/3colours"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/../styles/3colours/blue" "${ARTIFACTS_OUTPUT_DIR}/styles/3colours/blue"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/../styles/3colours/green" "${ARTIFACTS_OUTPUT_DIR}/styles/3colours/green"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/../styles/3colours/red" "${ARTIFACTS_OUTPUT_DIR}/styles/3colours/red"
        )
        add_custom_command(
            TARGET ${PROJECT}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ARTIFACTS_OUTPUT_DIR}/backgrounds/3colours"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/../backgrounds/3colours/blue.jpg" "${ARTIFACTS_OUTPUT_DIR}/backgrounds/3colours/blue.jpg"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/../backgrounds/3colours/red.jpg" "${ARTIFACTS_OUTPUT_DIR}/backgrounds/3colours/red.jpg"
        )
    endif()
endif()
