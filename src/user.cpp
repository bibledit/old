/*
** Copyright (©) 2003-2009 Teus Benschop.
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/

#include "libraries.h"
#include "gtkwrappers.h"

bool runs_as_root()
{
#ifndef WIN32
  bool root = (getuid() == 0);
  if (root) {
    ustring message = "Bibledit has not been designed to run as user root.\n" "Please run it as a normal user.";
    cout << message << endl;
    gtkw_dialog_error(NULL, message);
  }
  return root;
#endif
#ifdef WIN32
  return false;
#endif
}
