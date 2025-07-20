cmake_minimum_required (VERSION 3.20)

set (PROJECT bbcore) 
project (${PROJECT})

set(BBOX_COMMON_INCLUDES_DIR ${CMAKE_CURRENT_LIST_DIR}/../common/) 

set (INCLUDE_DIRS
    .
    ${CMAKE_CURRENT_LIST_DIR}/Menu/
    ${CMAKE_CURRENT_LIST_DIR}/Menu/items/
    ${CMAKE_CURRENT_LIST_DIR}/Menu/menus/
    ${BBLIB_INCLUDE_DIR}
    ${BBOX_COMMON_INCLUDES_DIR}
    ) 

set (LINK_DIRS
    ) 

set (HEADERS
    application.h
    BB.h
    BBSendData.h
    BBVWM.h
    BBRootImage.h
    BImage.h
    Blackbox.h
    commands.h
    Desk.h
    Menu/MenuInfo.h
    Menu/ShellContext.h
    Menu/MenuMaker.h
    Menu/Dragsource.h
    Menu/Droptarget.h
    Menu/menus/ConfigMenu.h
    Menu/menus/Contextmenu.h
    Menu/menus/DesktopMenu.h
    Menu/menus/Menu.h
    Menu/menus/SpecialFolderMenu.h
    Menu/items/ContextItem.h
    Menu/items/CommandItem.h
    Menu/items/FolderItem.h
    Menu/items/IntegerItem.h
    Menu/items/MenuItem.h
    Menu/items/SeparatorItem.h
    Menu/items/SFInsertItem.h
    Menu/items/SpecialFolderItem.h
    Menu/items/StringItem.h
    Menu/items/TitleItem.h
    MessageManager.h
    nls.h
    PluginManager.h
    Settings.h
    SettingsBbox.h
    Stylestruct.h
    Toolbar.h
    Tray.h
    Utils.h
    Workspaces.h
    bbcore.h
    parse.h
    rcfile.h
    system.h
    win0x500.h
) 

set (SOURCES
    application.cpp
    BBRootImage.cpp
    BBVWM.cpp
    BImage.cpp
    Blackbox.cpp
    commands.cpp
    Desk.cpp
    Menu/Dragsource.cpp
    Menu/Droptarget.cpp
    Menu/MenuMaker.cpp
    Menu/ShellContext.cpp
    Menu/menus/Menu.cpp
    Menu/menus/SpecialFolderMenu.cpp
    Menu/menus/Contextmenu.cpp
    Menu/menus/ConfigMenu.cpp
    Menu/menus/DesktopMenu.cpp
    Menu/items/CommandItem.cpp
    Menu/items/ContextItem.cpp
    Menu/items/FolderItem.cpp
    Menu/items/IntegerItem.cpp
    Menu/items/MenuItem.cpp
    Menu/items/SFInsertItem.cpp
    Menu/items/SpecialFolderItem.cpp
    Menu/items/SeparatorItem.cpp
    Menu/items/StringItem.cpp
    Menu/items/TitleItem.cpp
    MessageManager.cpp
    nls.cpp
    PluginManager.cpp
    Settings.cpp
    SettingsBbox.cpp
    Toolbar.cpp
    Tray.cpp
    Utils.cpp
    Workspaces.cpp
    parse.cpp
    rcfile.cpp
    system.cpp
) 

set (LIBS
    ${BBLIB_LIBRARY}
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
set(BBCORE_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/. CACHE INTERNAL "") 
set(BBCORE_INCLUDES 
    ${BBCORE_INCLUDE_DIR} 
    ${BBCORE_INCLUDE_DIR}/Menu/
    ${BBCORE_INCLUDE_DIR}/Menu/menus/
    ${BBCORE_INCLUDE_DIR}/Menu/items/
    ${BBOX_COMMON_INCLUDES_DIR}
    CACHE INTERNAL "") 

set(BBCORE_LIBRARY ${PROJECT} CACHE INTERNAL "") 

# library 
add_library (${PROJECT} STATIC ${HEADERS} ${SOURCES})
target_link_directories(${PROJECT} PRIVATE ${LINK_DIRS})
target_include_directories (${PROJECT} PRIVATE ${INCLUDE_DIRS} )
target_link_libraries (${PROJECT} PRIVATE ${LIBS})
target_compile_definitions(${PROJECT} PRIVATE __BBCORE__)

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

