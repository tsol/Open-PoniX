/**************************************************************************
    Lunchbox Window Manager
    Copyright (C) 2008 Alysander Stanley

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/
#include "xcheck.h"

/**
@file     xcheck.c
@brief    Contains simple wrappers which enable calls to windows/pixmaps with an ID of 0 to be silently ignored.  This is useful for the theming system which does not require all pixmaps/windows to be present.
@author   Alysander Stanley
**/


Bool
xcheck_raisewin(Display *display, Window window) {
  if(window) {
    XRaiseWindow(display, window);
    return True;
  }
  return False;
}

Bool
xcheck_setpixmap (Display *display, Window window, Pixmap pixmap) {
  if(pixmap) {
    XSetWindowBackgroundPixmap(display, window, pixmap);
    return True;
  }
  return False;
}

Bool
xcheck_map(Display *display, Window window) {
  if(window) {
    XMapWindow(display, window);
    return True;
  }
  return False;
}
