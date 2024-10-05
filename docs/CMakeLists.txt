cmake_minimum_required (VERSION 3.20)

set (PROJECT docs) 
project (${PROJECT})

set(FUZZY fuzzydoc)

# Generates target and commands to build fuzzydocs
# param targetName The name of created target.
# param pregenFiles Input files list for fuzzydoc. [!] The first one in list is the
#                   one that passed to fuzzydoc utility
# param generatedFiles Output files list that conrols by their presence the generate event
# param outputDir The directory where pregen files and output files will be placed
function(add_fuzzydocs_target
        targetName pregenFiles generatedFiles outputDir
        # auxDeps [optional]    # ${ARGV4}
    )
    set (auxDeps)
    if(NOT "${ARGV4}" STREQUAL "")
        set (auxDeps "${ARGV4}")
    endif()

    if (("${targetName}" STREQUAL "")
        OR ("${pregenFiles}" STREQUAL "")
        OR ("${generatedFiles}" STREQUAL "")
        OR ("${outputDir}" STREQUAL ""))
        message( FATAL_ERROR "All  add_fuzzydocs_target() parameters must not be empty!" )
    endif()

    # add 'copy' command for each resource file
    set(COPIED_RESFILES)
    foreach(RESFILE IN LISTS pregenFiles)
        cmake_path(ABSOLUTE_PATH RESFILE NORMALIZE OUTPUT_VARIABLE FULLRESPATH)
        cmake_path(GET RESFILE FILENAME FNAMEWITHEXT)

        set(COPYDEST_PATH ${outputDir}/${FNAMEWITHEXT})

        add_custom_command(
            OUTPUT ${COPYDEST_PATH}
            POST_BUILD
            COMMAND
            ${CMAKE_COMMAND} -E copy
            ${FULLRESPATH}        # src
            ${COPYDEST_PATH} # dest
        )

        list(APPEND COPIED_RESFILES ${COPYDEST_PATH})
    endforeach()

    # [~] maybe need to pass 'input file' parameter as function arg
    # Here we're getting first pregen file as input and first generated as
    # output control
    list(GET pregenFiles 0 FUZZYINPUT_FILE)
    list(GET generatedFiles 0 FIRSTGEN_FILE)

    cmake_path(ABSOLUTE_PATH FUZZYINPUT_FILE NORMALIZE OUTPUT_VARIABLE FUZZYINPUT_FILE_FULLPATH)
    cmake_path(GET FIRSTGEN_FILE FILENAME FIRSTGEN_FNAMEEXT)

    # generation itself. Generated files are added to cleanup list manually
    add_custom_command(OUTPUT ${outputDir}/${FIRSTGEN_FNAMEEXT}
        DEPENDS ${FUZZY} ${COPIED_RESFILES}
        COMMAND ${FUZZY} ${FUZZYINPUT_FILE_FULLPATH}
        WORKING_DIRECTORY ${outputDir}
    )

    add_custom_target(${targetName} ALL
        DEPENDS  ${COPIED_RESFILES} ${outputDir}/${FIRSTGEN_FNAMEEXT} ${auxDeps}
        VERBATIM
    )

    set(GENERATED_FULL_PATHS)
    foreach(GENFILE IN LISTS generatedFiles)
        cmake_path(GET GENFILE FILENAME FNAMEWITHEXT)
        list(APPEND GENERATED_FULL_PATHS "${outputDir}/${FNAMEWITHEXT}")
    endforeach()

    # [~] cmake bug: Currently works only on makefile build cases
    set_property(
        TARGET ${targetName}
        APPEND
        PROPERTY ADDITIONAL_CLEAN_FILES ${GENERATED_FULL_PATHS}
    )
endfunction()

set (RESOURCE_FILES_BBOX 
    bblean.txt
    bblean_logo.png
    bblean_style.css
    ${CMAKE_CURRENT_LIST_DIR}/../src/blackbox/nls-c.txt
    ${CMAKE_CURRENT_LIST_DIR}/../src/blackbox/Menu/menu-bullets.bmp
    ${CMAKE_CURRENT_LIST_DIR}/../src/tools/fuzzydoc/figure1.jpg
)
set (GENERATED_FILES_BBOX
    bblean.htm
    bblean_toc.htm
    bblean_overview.htm
    bblean_details.htm
    bblean_info.htm
)

get_current_output_dir(ARTIFACTS_OUTPUT_DIR)
add_fuzzydocs_target(GenerateDocsBbox "${RESOURCE_FILES_BBOX}" "${GENERATED_FILES_BBOX}" "${ARTIFACTS_OUTPUT_DIR}")


# bsetroot docs
set (RESOURCE_FILES_BSETROOT
    ${CMAKE_CURRENT_LIST_DIR}/../src/tools/bsetroot/bsetroot.txt
)
set (GENERATED_FILES_BSETROOT
    bsetroot.htm
)

add_fuzzydocs_target(GenerateDocsBsetroot "${RESOURCE_FILES_BSETROOT}"
    "${GENERATED_FILES_BSETROOT}" "${ARTIFACTS_OUTPUT_DIR}" GenerateDocsBbox)


# bbnote docs
set (RESOURCE_FILES_BBNOTE
    ${CMAKE_CURRENT_LIST_DIR}/../src/tools/bbnote/app/bbnote.txt
)
set (GENERATED_FILES_BBNOTE
    bbnote.htm
)

add_fuzzydocs_target(GenerateDocsBBnote "${RESOURCE_FILES_BBNOTE}"
    "${GENERATED_FILES_BBNOTE}" "${ARTIFACTS_OUTPUT_DIR}" GenerateDocsBbox)

# fuzzydoc docs
set (RESOURCE_FILES_BBNOTE
    ${CMAKE_CURRENT_LIST_DIR}/../src/tools/fuzzydoc/fuzzydoc.txt
)
set (GENERATED_FILES_BBNOTE
    fuzzydoc.htm
)

add_fuzzydocs_target(GenerateDocsFuzzydoc "${RESOURCE_FILES_BBNOTE}"
    "${GENERATED_FILES_BBNOTE}" "${ARTIFACTS_OUTPUT_DIR}" GenerateDocsBbox)
