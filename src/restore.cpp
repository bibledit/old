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


#include "restore.h"
#include "gwrappers.h"
#include "statistics.h"
#include "utilities.h"
#include "projectutils.h"
#include "books.h"
#include "unixwrappers.h"
#include "directories.h"
#include "shell.h"
#include "gtkwrappers.h"
#include "htmlbrowser.h"
#include "tiny_utilities.h"
#include "snapshots.h"
#include "export_utils.h"
#include "notes_utils.h"


void restore_notes (const ustring& unpack_directory, vector <ustring>& feedback)
{
  // Run an upgrade on the unpacked directory.
  notes_database_verify(unpack_directory);
  
  // Bail out if there's nothing to import.
  unsigned int restore_count = notes_count (unpack_directory);
  feedback.push_back ("Notes to restore: " + convert_to_string (restore_count));
  if (restore_count == 0) {
    feedback.push_back ("Nothing to restore");
    return;
  }

  // If there are notes in the current database, ask whether these should be overwritten.
  unsigned int current_count = notes_count ();
  feedback.push_back ("Notes available before restore: " + convert_to_string (current_count));
  if (current_count > 0) {
    ustring question = "Currently Bibledit has " + convert_to_string (current_count) + " notes. Would you like to overwrite these?";
    if (gtkw_dialog_question(NULL, question, GTK_RESPONSE_NO) == GTK_RESPONSE_YES) {
      feedback.push_back ("Restore has overwritten the existing notes");
    } else {
      feedback.push_back ("Restore was cancelled");
      return;
    }
  }
  
  // If so, move everything into place.
  unix_rmdir (directories_get_notes ());
  unix_mv (unpack_directory, directories_get_notes ());
  
  // Give feedback about what has happened.
  feedback.push_back ("Notes were restored");
}


void restore_all_stage_one (const ustring& unpack_directory, vector <ustring>& feedback) // Todo
// Restore everything: stage one.
{
  // Move the directory to the temporal place.
  unix_rmdir (directories_get_restore ());
  unix_mv (unpack_directory, directories_get_restore ());
  
  // Give feedback about what has transpired.
  feedback.push_back ("Everything was restored");
  feedback.push_back ("Changes will only take effect after restart");
}


void restore_all_stage_two () // Todo
{
  if (g_file_test (directories_get_restore ().c_str(), G_FILE_TEST_IS_DIR)) {
    gw_message ("Restoring everything");
    unix_rmdir (directories_get_root ());
    unix_mv (directories_get_restore (), directories_get_root ());
  }
}

