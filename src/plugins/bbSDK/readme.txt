  ----------------------------------------------------------------------

  bbSDK - Example plugin for Blackbox for Windows.
  Copyright � 2004,2009 grischka

  This program is free software, released under the GNU General Public
  License (GPL version 2). See:

  http://www.fsf.org/licenses/gpl.html

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.


  Description:
  ------------
  This is an example Plugin for Blackbox for Windows. It displays
  a little stylized window with an inscription.

  Left mouse:
    - with the control key held down: moves the plugin
    - with alt key held down: resizes the plugin

  Right mouse click:
    - shows the plugin menu with some standard plugin configuration
      options. Also the inscription text can be set.

  bbSDK is compatible with all current bb4win versions:
    - bblean 1.12 or later
    - xoblite bb2 or later
    - bb4win 0.90 or later


  Files:
  ======

  bbSDK.cpp                 Source code (works as C or C++)
  readme.txt                This file 

  History:
  ========

  20-May-2009 - bbSDK 0.2
  -----------------------
  - Replaced all "bbSDK" strings by macros such that they can be
    changed more easily.
  - Function 'edit_rc(file)' works also under xoblite
  - Also works as plain C source (rename bbSDK.cpp to bbSDK.c)

  01-Aug-2004 - bbSDK 0.1
  -----------------------
  - initial release


  Enjoy!
  --- grischka
