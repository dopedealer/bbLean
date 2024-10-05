cmake_minimum_required (VERSION 3.20)

set (PROJECT fonts) 
project (${PROJECT})

set (CUSTOM_FONTS_FOLDER "artwiz-latin1-1.0-src")

get_current_output_dir(FONTS_OUTPUT_DIR)

set (FONT_NAMES 
    anorexia
    aqui
    cure
    drift
    edges
    fkp
    gelly
    glisp
    kates
    lime
    mints-mild
    mints-strong
    nu
    snap
)


cmake_path(ABSOLUTE_PATH CUSTOM_FONTS_FOLDER  NORMALIZE OUTPUT_VARIABLE CUSTOM_FONTS_FOLDER_ABS)

# add 'generate' command for each font file
set(FONT_NAMES_FNTEXT)
foreach(FONTNAME IN LISTS FONT_NAMES)
    set(BDF_FILE_PATH ${CUSTOM_FONTS_FOLDER_ABS}/${FONTNAME}.bdf)
    cmake_path(ABSOLUTE_PATH BDF_FILE_PATH NORMALIZE OUTPUT_VARIABLE BDF_FILE_PATH)
    cmake_path(NATIVE_PATH BDF_FILE_PATH NORMALIZE BDF_FILE_PATH)

    set(FON_FILE_OUTPUT_PATH ${FONTS_OUTPUT_DIR}/${FONTNAME}.fon)
    cmake_path(ABSOLUTE_PATH FON_FILE_OUTPUT_PATH NORMALIZE OUTPUT_VARIABLE FON_FILE_OUTPUT_PATH)
    cmake_path(NATIVE_PATH FON_FILE_OUTPUT_PATH NORMALIZE FON_FILE_OUTPUT_PATH)

    add_custom_command(OUTPUT ${FON_FILE_OUTPUT_PATH}
        DEPENDS bdf2fnt
        COMMAND bdf2fnt ${BDF_FILE_PATH} ${FONTNAME}.fnt 
        COMMAND fnt2fon ${FONTNAME}.fnt ${FON_FILE_OUTPUT_PATH}
    )
    list(APPEND FONT_NAMES_FNTEXT ${FON_FILE_OUTPUT_PATH})
endforeach()

add_custom_target(GenerateFonts ALL
                  DEPENDS  ${FONT_NAMES_FNTEXT}
                  VERBATIM
                  )

if (NEED_COPY_FILES)
    get_property(USED_MULTICONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (NOT USED_MULTICONFIG)
        get_current_output_dir(ARTIFACTS_OUTPUT_DIR)
        file(MAKE_DIRECTORY ${ARTIFACTS_OUTPUT_DIR})
    endif()

    set (RESOURCE_FILES_TOCOPY
        README.TXT
    )
    add_target_copy_files_to_output_dir(GenerateFonts "${RESOURCE_FILES_TOCOPY}")
endif()
