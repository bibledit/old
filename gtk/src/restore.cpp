/*
** Copyright (©) 2003-2012 Teus Benschop.
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
#include "tiny_utilities.h"
#include "snapshots.h"
#include "export_utils.h"
#include "notes_utils.h"
#include "resource_utils.h"


void restore_project (const ustring& unpack_directory, const ustring& bible, vector <ustring>& feedback)
// Restores a project.
{
  // Check file signature.
  if (!restore_file_present (unpack_directory, "data", feedback)) {
    return;
  }
  // Restore.
  project_create_restore (bible, unpack_directory);
  // Feedback to user.
  feedback.push_back ("The file was restored to Bible " + bible);
}


void restore_notes (const ustring& unpack_directory, vector <ustring>& feedback)
{
  // Check file signature.
  if (!restore_file_present (unpack_directory, "categories", feedback)) {
    return;
  }

  // If there are notes in the current database, ask whether these should be overwritten.
  unsigned int current_count = notes_count ();
  feedback.push_back ("Notes available before restore: " + convert_to_string (current_count));
  if (current_count > 0) {
    ustring question = "Currently Bibledit-Gtk has " + convert_to_string (current_count) + " notes. Would you like to overwrite these?";
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

  // Run upgrade.
  notes_storage_verify();
    
  // Give feedback about what has happened.
  feedback.push_back ("Notes were restored");
}


void restore_resource (const ustring& unpack_directory, vector <ustring>& feedback) 
{
  // Check file signature.
  if (!restore_file_present (unpack_directory, "resource-template.ini", feedback)) {
    return;
  }
 
  // Get the Resource's template filename.
  ustring filename = gw_build_filename (unpack_directory, resource_template_ini ());
  
  // Get the Resource's name.
  ustring resource = resource_get_title (filename);
  if (resource.empty ()) {
    feedback.push_back ("No resource found");
    return;
  }
  
  // Does this resource already exist?
  vector <ustring> filenames;
  vector <ustring> resources = resource_get_resources (filenames, false);
  set <ustring> resource_set (resources.begin (), resources.end ());
  if (resource_set.find (resource) != resource_set.end ()) {
    feedback.push_back ("This resource already exists");
    return;
  }

  // Find a non-existing directory where to restore the resource to.
  ustring restore_directory = gw_build_filename (directories_get_resources (), resource);
  while (g_file_test (restore_directory.c_str(), G_FILE_TEST_EXISTS)) {
    restore_directory.append ("1");
  }
  
  // Move the resource in place.
  unix_mv (unpack_directory, restore_directory);
  
  // Give feedback about what has happened.
  feedback.push_back ("Resource was restored");
}


void restore_all_stage_one (const ustring& unpack_directory, vector <ustring>& feedback)
// Restore everything: stage one.
{
  // Check file signature.
  if (!restore_file_present (unpack_directory, "projects", feedback)) {
    return;
  }
  
  // Move the directory to the temporal place.
  unix_rmdir (directories_get_restore ());
  unix_mv (unpack_directory, directories_get_restore ());
  
  // Give feedback about what has transpired.
  feedback.push_back ("Everything was restored");
  feedback.push_back ("Changes will only take effect after restart");
}


void restore_all_stage_two ()
// Restore everything: second and last stage.
{
  if (g_file_test (directories_get_restore ().c_str(), G_FILE_TEST_IS_DIR)) {
    gw_message ("Restoring everything");
    unix_rmdir (directories_get_root ());
    unix_mv (directories_get_restore (), directories_get_root ());
  }
}


bool restore_file_present (const ustring& directory, const gchar * filename, vector <ustring>& feedback)
// Returns true if a file or directory exists.
{
  ustring file = gw_build_filename (directory, filename);
  bool present = g_file_test (file.c_str(), G_FILE_TEST_EXISTS);
  if (!present) {
    feedback.push_back ("The file to restore does not contain valid data");
    feedback.push_back ("Nothing was restored");
  }
  return present;
}


