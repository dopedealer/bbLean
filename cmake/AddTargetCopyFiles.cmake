
# copy resource, config files
function(add_target_copy_files targetName resFiles outputDir)
    if ("${outputDir}" STREQUAL "")
        message( FATAL_ERROR "The output dir MUST NOT be empty!" )
    endif()

    foreach(RESFILE IN LISTS resFiles)
        cmake_path(ABSOLUTE_PATH RESFILE NORMALIZE OUTPUT_VARIABLE FULLRESPATH)
        cmake_path(GET RESFILE FILENAME FNAMEWITHEXT)
        set(COPYDEST_PATH ${outputDir}/${FNAMEWITHEXT})

        add_custom_command(
            TARGET ${targetName}
            POST_BUILD
            COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            ${FULLRESPATH}
            ${COPYDEST_PATH}
        )
    endforeach()
endfunction()

# example of usage:
# set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "a")
# set_output_subdir(CMAKE_RUNTIME_OUTPUT_DIRECTORY "")
function(set_output_subdir outputVariable subdirName)
    get_property(USED_MULTICONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (USED_MULTICONFIG)
        if ("${subdirName}" STREQUAL "")
            set (${outputVariable} "${CMAKE_BINARY_DIR}/$<CONFIG>/" PARENT_SCOPE)
        else()
            set (${outputVariable} "${CMAKE_BINARY_DIR}/$<CONFIG>/${subdirName}" PARENT_SCOPE)
        endif()
    else()
        if ("${subdirName}" STREQUAL "")
            set (${outputVariable} "${CMAKE_BINARY_DIR}/" PARENT_SCOPE)
        else()
            set (${outputVariable} "${CMAKE_BINARY_DIR}/${subdirName}" PARENT_SCOPE)
        endif()
    endif()
endfunction()

function(get_current_output_dir outDir)
    get_property(USED_MULTICONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (USED_MULTICONFIG)
        if (DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
            # questionable. Maybe there is a better way to get per-config output dir
            #message("--> defined CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
            #set (${outDir} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG> PARENT_SCOPE)
            set (${outDir} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} PARENT_SCOPE)
        else()
            #message("--> NOT defined CMAKE_RUNTIME_OUTPUT_DIRECTORY")
            set (${outDir} ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
        endif()
    else()
        if (DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
            #message("--> defined CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
            set (${outDir} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} PARENT_SCOPE)
        else()
            #message("--> NOT defined CMAKE_RUNTIME_OUTPUT_DIRECTORY")
            set (${outDir} ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
        endif()
    endif()
endfunction()

# copy resource, config files
function(add_target_copy_files_to_output_dir targetName resFiles)
    get_current_output_dir(ARTIFACTS_OUTPUT_DIR)
    if ("${ARTIFACTS_OUTPUT_DIR}" STREQUAL "")
        message( FATAL_ERROR "The output dir MUST NOT be empty!" )
    endif()

    foreach(RESFILE IN LISTS resFiles)
        cmake_path(ABSOLUTE_PATH RESFILE NORMALIZE OUTPUT_VARIABLE FULLRESPATH)
        cmake_path(GET RESFILE FILENAME FNAMEWITHEXT)
        set(COPYDEST_PATH ${ARTIFACTS_OUTPUT_DIR}/${FNAMEWITHEXT})

        add_custom_command(
            TARGET ${targetName}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FULLRESPATH} ${COPYDEST_PATH}
        )
    endforeach()
endfunction()

# gets 'normalized' PATH environment variable that can be used in resolve
# runtime dependencies functions
function(get_path_normalized_dirs outVariable)
    set(DIRS_VAR $ENV{PATH})
    # assume that cmake's normalize is enought and it is does not corrupt pathes
    cmake_path(NORMAL_PATH DIRS_VAR OUTPUT_VARIABLE DIRS_VAR)
    set (${outVariable} ${DIRS_VAR} PARENT_SCOPE)
endfunction()
