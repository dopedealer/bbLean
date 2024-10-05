cmake_minimum_required (VERSION 3.13)

# How to use:
# Compile, add it's libraries and headers.
# Exports variable to use:
# ${CXIMAGE_INCLUDE_DIRS}
# ${CXIMAGE_LIBRARIES}

set (PROJECT cximage) 
project (${PROJECT})

set (INCLUDE_DIRS
    .
    ./zlib/
    ) 

set (LINK_DIRS
    ) 

set (HEADERS
    CxImage/xfile.h
    CxImage/ximabmp.h
    CxImage/ximadefs.h
    CxImage/ximage-original.h
    CxImage/ximage.h
    CxImage/ximagif.h
    CxImage/ximaico.h
    CxImage/ximaiter.h
    CxImage/ximaj2k.h
    CxImage/ximajbg.h
    CxImage/ximajpg.h
    CxImage/ximamng.h
    CxImage/ximapcx.h
    CxImage/ximapng.h
    CxImage/ximatga.h
    CxImage/ximatif.h
    CxImage/ximawbmp.h
    CxImage/ximawmf.h
    CxImage/xiofile.h
    CxImage/xmemfile.h
    #
    jpeg/jchuff.h
    jpeg/jconfig.h
    jpeg/jdct.h
    jpeg/jdhuff.h
    jpeg/jerror.h
    jpeg/jinclude.h
    jpeg/jmemsys.h
    jpeg/jmorecfg.h
    jpeg/jpegint.h
    jpeg/jpeglib.h
    jpeg/jversion.h
    #
    png/png.h
    png/pngasmrd.h
    png/pngconf.h
    #
    zlib/deflate.h
    zlib/infblock.h
    zlib/infcodes.h
    zlib/inffast.h
    zlib/inffixed.h
    zlib/inftrees.h
    zlib/infutil.h
    zlib/trees.h
    zlib/zconf.h
    zlib/zlib.h
    zlib/zutil.h
    ) 

set (SOURCES
    # CxImage
    CxImage/ximabmp.cpp
    CxImage/ximadsp.cpp
    CxImage/ximaenc.cpp
    CxImage/ximaexif.cpp
    CxImage/ximage.cpp
    CxImage/ximagif.cpp
    CxImage/ximaico.cpp
    CxImage/ximaj2k.cpp
    CxImage/ximajbg.cpp
    CxImage/ximajpg.cpp
    CxImage/ximalpha.cpp
    CxImage/ximamng.cpp
    CxImage/ximapal.cpp
    CxImage/ximapcx.cpp
    CxImage/ximapng.cpp
    CxImage/ximasel.cpp
    CxImage/ximatga.cpp
    CxImage/ximatif.cpp
    CxImage/ximatran.cpp
    CxImage/ximawbmp.cpp
    CxImage/ximawmf.cpp
    CxImage/ximawnd.cpp
    CxImage/xmemfile.cpp
    #
    jpeg/jcapimin.c
    jpeg/jcapistd.c
    jpeg/jccoefct.c
    jpeg/jccolor.c
    jpeg/jcdctmgr.c
    jpeg/jchuff.c
    jpeg/jcinit.c
    jpeg/jcmainct.c
    jpeg/jcmarker.c
    jpeg/jcmaster.c
    jpeg/jcomapi.c
    jpeg/jcparam.c
    jpeg/jcphuff.c
    jpeg/jcprepct.c
    jpeg/jcsample.c
    jpeg/jctrans.c
    jpeg/jdapimin.c
    jpeg/jdapistd.c
    jpeg/jdatadst.c
    jpeg/jdatasrc.c
    jpeg/jdcoefct.c
    jpeg/jdcolor.c
    jpeg/jddctmgr.c
    jpeg/jdhuff.c
    jpeg/jdinput.c
    jpeg/jdmainct.c
    jpeg/jdmarker.c
    jpeg/jdmaster.c
    jpeg/jdmerge.c
    jpeg/jdphuff.c
    jpeg/jdpostct.c
    jpeg/jdsample.c
    jpeg/jdtrans.c
    jpeg/jerror.c
    jpeg/jfdctflt.c
    jpeg/jfdctfst.c
    jpeg/jfdctint.c
    jpeg/jidctflt.c
    jpeg/jidctfst.c
    jpeg/jidctint.c
    jpeg/jidctred.c
    jpeg/jmemmgr.c
    jpeg/jmemnobs.c
    jpeg/jquant1.c
    jpeg/jquant2.c
    jpeg/jutils.c
    #
    png/png.c
    png/pngerror.c
    png/pnggccrd.c
    png/pngget.c
    png/pngmem.c
    png/pngpread.c
    png/pngread.c
    png/pngrio.c
    png/pngrtran.c
    png/pngrutil.c
    png/pngset.c
    png/pngtrans.c
    png/pngvcrd.c
    png/pngwio.c
    png/pngwrite.c
    png/pngwtran.c
    png/pngwutil.c
    #
    zlib/adler32.c
    zlib/compress.c
    zlib/crc32.c
    zlib/deflate.c
    zlib/gzio.c
    zlib/infblock.c
    zlib/infcodes.c
    zlib/inffast.c
    zlib/inflate.c
    zlib/inftrees.c
    zlib/infutil.c
    #zlib/maketree.c
    #zlib/minigzip.c
    zlib/trees.c
    zlib/uncompr.c
    zlib/zutil.c
    ) 

set (LIBS
    )

if (WIN32)
    list(APPEND LIBS user32; kernel32; gdi32; shell32; comctl32; comdlg32; shlwapi; advapi32; version; ole32; winmm) 
endif() 

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES}) 

set(CXIMAGE_INCLUDE_DIRS 
    ${CMAKE_CURRENT_LIST_DIR}/CxImage;
    ${CMAKE_CURRENT_LIST_DIR}/jpeg;
    ${CMAKE_CURRENT_LIST_DIR}/png;
    ${CMAKE_CURRENT_LIST_DIR}/zlib;
    CACHE INTERNAL ""
    )

set(CXIMAGE_LIBRARIES cximage CACHE INTERNAL "")


if(UNIX AND (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) )
  list(APPEND LIBS dl) 
endif()

# library 
add_library (${PROJECT} STATIC ${HEADERS} ${SOURCES})
target_link_directories(${PROJECT} PRIVATE ${LINK_DIRS})
target_include_directories (${PROJECT} PRIVATE ${INCLUDE_DIRS} )
target_link_libraries (${PROJECT} PRIVATE ${LIBS})
target_compile_definitions(${PROJECT} PRIVATE WIN32)

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


