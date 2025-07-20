# bbLean

## A blackbox for windows project

Copyright © 2004-2009 grischka

Copyright © 2024-2025 kam1  ; xmalkut@gmail.com, kam1@jabber.ru

## Project goals:
- keeping maximum stability and error-freeness as possible. No unstable code in main branch allowed.
- keeping fast work
- backward compatibility with original blackbox on configs & stytes level as possible
- binary compatibility with bblean 1.17 plugins and interface as possible
- providing compilation with most used modern windows compilers
- fixing known bugs
- increasing the quality of code
- minimal needed customisations
- adopting to new windows os versions
- work out of the box without additional setup

# Build

Prerequisites:

    - A modern c++ compiler (usually MinGW or MSVC)
    - A googletest library or sources

  Build steps:
  
    mkdir b
    cd b
    cmake .. 
    cmake --build . --parallel

  End user build only allows not to use google test:
  
    cmake .. -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release
    cmake --build . -t package

bbLean is free software, released under the GNU General Public
License (GPL version 2). For details see:

http://www.fsf.org/licenses/gpl.html

# Code style rules
- camel case style. If legacy subproject uses snake case then snake case
  retained.
- allowed only pure C export. No c++ classes or mangled functions allowed.
  Exception: Allowed only c++ classes that are not exported, but fully contained in only
  one hpp file in bblib
- Not changing public interfaces for retaining compatibility with already built
  plugins

# Original base code info:

Original source code for bbLean from:
Copyright © 2004-2009 grischka

http://bb4win.sourceforge.net/bblean

http://developer.berlios.de/projects/bblean 
