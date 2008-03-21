/*
** Copyright (©) 2003-2008 Teus Benschop.
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#include "restart.h"
#include "settings.h"
#include "gtkwrappers.h"
#include "gwrappers.h"


void restart ()
// Sets the environment for restarting Bibledit.
{
  // Ask whether bibledit should restart. Bail out if not.
  if (gtkw_dialog_question (NULL, 
                            "The changes will take effect after Bibledit has been restarted\n"
                            "Would you like to restart now?",
                            GTK_RESPONSE_YES) != GTK_RESPONSE_YES) {
    return;
  }
  // Set the restart-flag in the filesystem.
  ustring filename = gw_build_filename (g_get_home_dir (), ".bibledit-restart-flag");
  g_file_set_contents (filename.c_str(), "", -1, NULL);
  // Set the restart flag in the session data.
  extern Settings * settings;
  settings->session.restart = true;
}
