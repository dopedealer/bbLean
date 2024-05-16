# bbLean

A blackbox for windows up-to-date code with bug fixes
Copyright (©) 2004-2009 grischka
Copyright (©) 2024 kam1  ; xmalkut@gmail.com, kam1@jabber.ru

Project goals:
- keeping maximum stability and error-freeness as possible. No unstable code in main branch allowed.
- keeping fast work
- backward compatibility with original blackbox on configs & stytes level as possible
- binary compatibility with bblean 1.17 plugins and interface as possible
- providing compilation with most used modern windows compilers
- fixing known bugs
- increasing the quality of code
- minimal needed customisations
- adopting to new windows os versions

# Building

  Prerequisites:
    - A modern c++ compiler (usually MinGW or MSVC)
    - A googletest library or sources

  Build steps:
    mkdir b
    cd b
    cmake .. 
    cmake --build . -t package -j 16 

  End user build only allows not to use google test:
    cmake .. -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release
    cmake --build . -t package

Currently old build way is retained. Will be removed later.

bbLean is free software, released under the GNU General Public
License (GPL version 2). For details see:

http://www.fsf.org/licenses/gpl.html

# Original base code info:

Original source code for bbLean from:
Copyright © 2004-2009 grischka

http://bb4win.sourceforge.net/bblean
http://developer.berlios.de/projects/bblean 

# Old build info:

  Prerequisites:
    - A compiler (MinGW or MSC)
    - GNU make from MinGW

  Make:
    Change into the source directory and type:

        make COMPILER=xxx

    where xxx is one of:
        gcc         for MinGW (default)
        msc         for MSVC 6.0
        vc8         for MS VC8 Express
        msc64       for MS AMD64 compiler
        gcc64       for MinGW-w64 compiler

    Some options in the build/makefile-xxx.inc files might need
    tweaking (e.g. for compiler installation paths).

  Clean:
    make clean

  Install into the source tree
    make COMPILER=xxx INSTDIR=.

  Install into directory elsewhere
    make COMPILER=xxx INSTDIR=<directory>

  Tested (among others) with MinGW package versions:
    gcc/g++         3.4.5
    binutils        2.18.50
    mingw-runtime   3.15.1
    w32api          3.12
    make            3.81

  2009-05-20

  --- grischka
