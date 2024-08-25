cmake_minimum_required (VERSION 3.30)

# Options:
# BUILD_TESTS - building tests. Strictly needed for developing. Allowed
#               to turn of only in 'only user' cases. 'On' by default.

# Notes:
# To use google test thou must have installed gtest package or environment
# variable GTEST_SRC that points to google test source dir
# If you want to build without tests (strictly NOT recommendend while
# developing) then add command line argument -DBUILD_TESTS=OFF on confuguring step.
# Example: cmake .. -G "NMake Makefiles" -DBUILD_TESTS=OFF
# External env variable that may point to gtest source folder: GTEST_SRC

# enable 'new' compiler flags working. Link runtime statically in msvc
if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif() 
set (CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)

# add dir with modules
list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake) 
include(AddTargetCopyFiles) 

option(BUILD_TESTS "-- build tests" ON)
#option(BUILD_SHARED_LIBS "-- build using shared libraries" ON)
option(BUILD_SHARED_LIBS "-- build using static libraries" OFF)
option(USE_RES_GENERATION "-- use resource generation (docs, fonts, configs)" ON)
option(NEED_COPY_FILES "-- need to copy resource/config files on every subproj" ON)
option(USE_BUILTIN_STYLES "-- is build-in styles used" ON)

set (PROJECT bblean)
project (${PROJECT})
project(${PROJECT} VERSION 1.17.1)

if( MSVC ) 
    add_compile_options(/MP)
endif () 

set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "")
set_output_subdir(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")
set_output_subdir(CMAKE_LIBRARY_OUTPUT_DIRECTORY "lib")

add_subdirectory ("build/" "${CMAKE_BINARY_DIR}/i/inst")
add_subdirectory ("bblib/" "${CMAKE_BINARY_DIR}/i/bblib")
add_subdirectory ("blackbox/" "${CMAKE_BINARY_DIR}/i/blackbox")

set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "tools/fuzzydoc")
##add_subdirectory ("blackbox/Hooks/" "${CMAKE_BINARY_DIR}/i/blackbox/Hooks") # is it needed ?
add_subdirectory ("build/fuzzydoc/" "${CMAKE_BINARY_DIR}/i/fuzzydoc")

set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "docs")
if (USE_RES_GENERATION)
    add_subdirectory ("docs/" "${CMAKE_BINARY_DIR}/i/docs")
endif()

set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "plugins/bbKeys")
add_subdirectory ("plugins/bbKeys/" "${CMAKE_BINARY_DIR}/i/bbKeys")

set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "plugins/bbPlugin")
add_subdirectory ("plugins/bbPlugin/" "${CMAKE_BINARY_DIR}/i/bbPlugin")

set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "plugins/bbLeanBar")
add_subdirectory ("plugins/bbLeanBar/" "${CMAKE_BINARY_DIR}/i/bbLeanBar")

set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "plugins/bbLeanSkin")
add_subdirectory ("plugins/bbLeanSkin/" "${CMAKE_BINARY_DIR}/i/bbLeanSkin")

set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "plugins/bbSlit")
add_subdirectory ("plugins/bbSlit/" "${CMAKE_BINARY_DIR}/i/bbSlit")

set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "plugins/bbAnalog")
add_subdirectory ("plugins/bbAnalog/" "${CMAKE_BINARY_DIR}/i/bbAnalog")

set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "plugins/bbIconBox")
add_subdirectory ("plugins/bbIconBox/" "${CMAKE_BINARY_DIR}/i/bbIconBox")

set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "plugins/bbColor3dc")
add_subdirectory ("plugins/bbColor3dc/" "${CMAKE_BINARY_DIR}/i/bbColor3dc")

set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "plugins/bbWorkspaceWheel")
add_subdirectory ("plugins/bbWorkspaceWheel/" "${CMAKE_BINARY_DIR}/i/bbWorkspaceWheel")

set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "plugins/bbSDK")
add_subdirectory ("plugins/bbSDK/" "${CMAKE_BINARY_DIR}/i/bbSDK")

set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "")
add_subdirectory ("tools/bsetshell/" "${CMAKE_BINARY_DIR}/i/bsetshell")

add_subdirectory ("tools/bsetroot/CXIMAGE/" "${CMAKE_BINARY_DIR}/i/CxImage")
add_subdirectory ("tools/bsetroot/" "${CMAKE_BINARY_DIR}/i/bsetroot")
add_subdirectory ("tools/bsetbg/" "${CMAKE_BINARY_DIR}/i/bsetbg")
add_subdirectory ("tools/bbstylemaker/" "${CMAKE_BINARY_DIR}/i/bbstylemaker")
add_subdirectory ("tools/bbnote/app" "${CMAKE_BINARY_DIR}/i/bbnoteapp")
add_subdirectory ("tools/bbnote/proxylib" "${CMAKE_BINARY_DIR}/i/bbnoteproxylib")

set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "tools/fonttools")
add_subdirectory ("tools/fonttools/fnt2fon/" "${CMAKE_BINARY_DIR}/i/fnt2fon")
add_subdirectory ("tools/fonttools/bdf2fnt/" "${CMAKE_BINARY_DIR}/i/bdf2fnt")

if (USE_RES_GENERATION)
    set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "fonts")
    add_subdirectory ("fonts/" "${CMAKE_BINARY_DIR}/i/fonts")
endif()

# restore default path
set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "")

if (BUILD_TESTS)
    message("-- using tests")
    enable_testing()
    find_package(GTest)
    if(NOT GTEST_FOUND)
        message("-- trying to search google test source dir")
        if(DEFINED ENV{GTEST_SRC})
            message("-- found google test source dir in env variable")
            set (GTEST_SRC $ENV{GTEST_SRC})
            # Fake 'search' results setting
            set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
            add_subdirectory ("${GTEST_SRC}" gtest_build)
            set(GTEST_INCLUDE_DIR ${GTEST_SRC}/googletest/googletest/include) 
            set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR}) 
            set(GTEST_LIBRARY gtest)
            set(GTEST_MAIN_LIBRARY gtest_main)
            set(GTEST_LIBRARIES ${GTEST_LIBRARY} gmock)
            set(GTEST_MAIN_LIBRARIES ${GTEST_MAIN_LIBRARY})
            set(GTEST_BOTH_LIBRARIES ${GTEST_MAIN_LIBRARY} ${GTEST_LIBRARY}) 
            set(GTEST_FOUND TRUE)
        endif()
    endif() 

    if(NOT GTEST_FOUND)
        message( FATAL_ERROR "The test are used but cannot find gtest library
        or gtest source dir! Thou must have built googletest library that can
        be found by cmake OR have environment variable GTEST_SRC that points to
        folder with google test sources")
    endif() 

    add_subdirectory ("tests/dummy/")
    add_subdirectory ("tests/test_bblib/")
    add_subdirectory ("tests/test_blackbox/")
else()
    message("-- not using tests")
endif()


# install/package settings
get_current_output_dir(ARTIFACTS_OUTPUT_DIR)
set(CPACK_GENERATOR  "ZIP")

get_path_normalized_dirs(PATH_ENV_NORMALIZED) 

# add executables
install(TARGETS ${BLACKBOX_LIBRARY} bbnote bbstylemaker bsetroot bsetbg bsetshell inst
    RUNTIME_DEPENDENCIES
    PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
    POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
    DIRECTORIES ${PATH_ENV_NORMALIZED}
    RUNTIME DESTINATION . 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(TARGETS bdf2fnt fnt2fon
    RUNTIME_DEPENDENCIES
    PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
    POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
    DIRECTORIES ${PATH_ENV_NORMALIZED}
    RUNTIME DESTINATION tools/fonttools
)

install(TARGETS fuzzydoc
    RUNTIME_DEPENDENCIES
    PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
    POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
    DIRECTORIES ${PATH_ENV_NORMALIZED}
    RUNTIME DESTINATION tools/fuzzydoc
)
install(DIRECTORY ${ARTIFACTS_OUTPUT_DIR}/tools/fuzzydoc DESTINATION tools
    FILES_MATCHING
    PATTERN "*.htm"
    PATTERN "*.txt"
    )

# add static libs
install(TARGETS bblib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${ARTIFACTS_OUTPUT_DIR}/plugins DESTINATION .
    FILES_MATCHING
    PATTERN "*.dll"
    PATTERN "*.txt"
    PATTERN "*.rc"
    PATTERN "*.bmp"
    )

# copy each config by name in root dir. Probably there is a better way to do that
install (FILES 
    ${ARTIFACTS_OUTPUT_DIR}/stickywindows.ini
    ${ARTIFACTS_OUTPUT_DIR}/bbstylemaker.rc
    ${ARTIFACTS_OUTPUT_DIR}/blackbox.rc
    ${ARTIFACTS_OUTPUT_DIR}/bsetroot.rc
    ${ARTIFACTS_OUTPUT_DIR}/extensions.rc
    ${ARTIFACTS_OUTPUT_DIR}/menu.rc
    ${ARTIFACTS_OUTPUT_DIR}/plugins.rc
    ${ARTIFACTS_OUTPUT_DIR}/shellfolders.rc
    DESTINATION . 
    )

install(DIRECTORY ${ARTIFACTS_OUTPUT_DIR}/styles DESTINATION .)
install(DIRECTORY ${ARTIFACTS_OUTPUT_DIR}/backgrounds DESTINATION .)
install(DIRECTORY ${ARTIFACTS_OUTPUT_DIR}/fonts DESTINATION .)
install(DIRECTORY ${ARTIFACTS_OUTPUT_DIR}/docs DESTINATION .)
# add 'missing' docs
install (FILES ${ARTIFACTS_OUTPUT_DIR}/bbstylemaker.txt DESTINATION docs)

# plugin developement headers
install (FILES 
    blackbox/BBApi.h
    bblib/bblib.h
    DESTINATION include
    )

include(CPack)
