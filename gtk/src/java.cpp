/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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
#include "java.h"
#include "gwrappers.h"
#include "gtkwrappers.h"
#include <glib/gi18n.h>

bool java_runtime_present (bool gui)
// Returns whether a java runtime is present on the system.
// gui: if true, it gives an error in case java is not there.
{
  bool present = gw_find_program_in_path ("java");
  if (gui) {
    if (!present) {
      gtkw_dialog_error (NULL, _("A Java Runtime Environment was not found.\nPlease install one at your convenience."));
    }
  }
  return present;
}

