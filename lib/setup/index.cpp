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


#include <setup/index.h>
#include <config.h>
#include <assets/view.h>
#include <assets/page.h>
#include <webserver/request.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/url.h>
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
#include <config/globals.h>
#include <index/index.h>
#include <styles/sheets.h>
#include <demo/logic.h>


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
  // If the setup is already running, bail out.
  // This situation may happen if a user keeps refreshing the setup page.
  if (config_globals_setup_running) return;
  // Set flag that setup is now running.
  config_globals_setup_running = true;

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

  // Clear setup flag.
  config_globals_setup_running = false;
}


string setup_index (void * webserver_request)
{
  // Ensure write access to certain folders.
  setup_write_access ();
  
  // Create or upgrade the databases.
  new thread (setup_initialize_data);
  
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  Assets_View view = Assets_View ();

  // Get the existing Administrators.
  vector <string> admins = request->database_users ()->getAdministrators ();

  // Admins do not yet exist: Allow to enter an admin.
  if (admins.empty ()) {
    if (!request->post ["Submit"].empty ()) {
      string admin_username = request->post ["admin_username"];
      string admin_password = request->post ["admin_password"];
      string admin_email = request->post ["admin_email"];
      vector <string> errors;
      if (admin_username.length() < 5) errors.push_back ("Choose a longer username.");
      if (admin_password.length() < 7) errors.push_back ("Choose a longer password.");
      if (admin_email.length() < 5) errors.push_back ("Enter a valid email address.");
      if (errors.empty()) {
        request->database_users ()->removeUser (admin_username);
        request->database_users ()->addNewUser (admin_username, admin_password, Filter_Roles::admin (), admin_email);
        Database_Config_General::setInstalledVersion (VERSION);
        redirect_browser (request, index_index_url ());
      } else {
        view.enable_zone ("errors");
        view.set_variable ("error", filter_string_implode (errors, " "));
      }
    }
  }

  // Enable appropriate zones: Either enter admin's details, or else display the details.
  if (admins.empty ()) {
    view.enable_zone ("enteradmin");
    view.enable_zone ("enteruser");
    view.enable_zone ("enterpass");
    view.enable_zone ("entermail");
    view.enable_zone ("displaysubmit");
  } else {
    string usernames;
    string emails;
    for (unsigned int i = 0; i < admins.size(); i++) {
      if (i) {
        usernames.append (" / ");
        emails.append (" / ");
      }
      usernames.append (admins[i]);
      emails.append (request->database_users ()->getUserToEmail (admins[i]));
    }
    view.set_variable ("usernames", usernames);
    view.set_variable ("emails", emails);
    view.enable_zone ("displayok");
    view.set_variable ("readonly", "readonly");
    // If the admin's are already there, then the setup has completed.
    // The automatic page refresh will kick in, and navigate to the main screen.
    Database_Config_General::setInstalledVersion (VERSION);
  }

  return view.render ("setup", "index");
}


