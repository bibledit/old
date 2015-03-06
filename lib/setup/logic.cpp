/*
 Copyright (©) 2003-2015 Teus Benschop.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include <setup/logic.h>
#include <webserver/request.h>
#include <config/logic.h>
#include <database/config/general.h>
#include <database/logs.h>
#include <database/commits.h>
#include <database/confirm.h>
#include <database/jobs.h>
#include <database/sprint.h>
#include <database/mail.h>
#include <database/navigation.h>
#include <database/mappings.h>
#include <database/noteactions.h>
#include <database/versifications.h>
#include <database/modifications.h>
#include <database/notes.h>
#include <database/volatile.h>
#include <styles/sheets.h>
#include <filter/string.h>
#include <filter/url.h>
#include <config/globals.h>
#include <demo/logic.h>


void setup_conditionally ()
{
  if (config_logic_version () == Database_Config_General::getInstalledDatabaseVersion ()) return;
    
  // Ensure write access to certain folders.
  setup_write_access ();
  
  // Create or upgrade the databases.
  setup_initialize_data ();
  
  // Update installed version.
  Database_Config_General::setInstalledDatabaseVersion (config_logic_version ());
}



void setup_write_access ()
{
  vector <string> folders = {"exports", "git", "revisions", "dyncss", "databases", "bibles", "downloads", "fonts", "logbook", "tmp"};
  for (auto folder : folders) {
    string path = filter_url_create_root_path (folder);
    if (!filter_url_get_write_permission (path)) {
      filter_url_set_write_permission (path);
    }
  }
}


void setup_initialize_data ()
{
  // Do the database setup.
  Webserver_Request request;
  request.database_users ()->create ();
  request.database_users ()->upgrade ();
  Database_Logs database_logs = Database_Logs ();
  database_logs.create ();
  request.database_styles ()->create ();
  request.database_search ()->create ();
  request.database_bibleactions ()->create ();
  request.database_check ()->create ();
  Database_Commits database_commits = Database_Commits ();
  database_commits.create ();
  Database_Confirm database_confirm = Database_Confirm ();
  database_confirm.create ();
  Database_Jobs database_jobs = Database_Jobs ();
  database_jobs.create ();
  Database_Sprint database_sprint = Database_Sprint ();
  database_sprint.create ();
  Database_Mail database_mail = Database_Mail (&request);
  database_mail.create ();
  Database_Navigation database_navigation = Database_Navigation ();
  database_navigation.create ();
  Database_Mappings database_mappings = Database_Mappings ();
  database_mappings.create1 ();
  database_mappings.defaults ();
  database_mappings.create2 ();
  database_mappings.optimize ();
  Database_NoteActions database = Database_NoteActions ();
  database.create ();
  Database_Versifications database_versifications = Database_Versifications ();
  database_versifications.create ();
  database_versifications.defaults ();
  Database_Modifications database_modifications = Database_Modifications ();
  database_modifications.create ();
  Database_Notes database_notes = Database_Notes (&request);
  database_notes.create ();
  Database_Volatile database_volatile = Database_Volatile ();
  database_volatile.create ();
  
  // Create stylesheets.
  styles_sheets_create_all ();
  
  // Create sample Bible if there's no Bible yet.
  vector <string> bibles = request.database_bibles()->getBibles ();
  if (bibles.empty ()) demo_create_sample_bible (&request);
}


