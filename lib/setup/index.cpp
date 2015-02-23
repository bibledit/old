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
#include <config/logic.h>


// Returns data for page indicating Bibledit is installing its data.
const char *  setup_installation_notice ()
{
  // Use http://base64online.org/encode/ to convert a png to inline graphic data.
  
  const char * html =
  "<!DOCTYPE html>\n"
  "<html>\n"
  "<head>\n"
  "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n"
  "<title>Bibledit</title>\n"
  "<link rel=\"stylesheet\" href=\"setup.css\" type=\"text/css\" />\n"
  "<style>\n"
  "html {\n"
  "  background: #f9f9f9;\n"
  "}\n"
  "body {\n"
  "  background: #fff;\n"
  "  color: #333;\n"
  "  font-family: sans-serif;\n"
  "  margin: 2em auto;\n"
  "  text-align: center;\n"
  "  padding: 1em 2em;\n"
  "  border-radius: 3px;\n"
  "  border: 1px solid #dfdfdf;\n"
  "}\n"
  "h1 {\n"
  "  border-bottom: 1px solid #dadada;\n"
  "  clear: both;\n"
  "  color: #666;\n"
  "  font: 24px Georgia, \"Times New Roman\", Times, serif;\n"
  "  padding: 0;\n"
  "  padding-bottom: 7px;\n"
  "}\n"
  "</style>\n"
  "</head>\n"
  "<body>\n"
  "<p><img src=\"data:image/png;base64,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\"></p>\n"
  "<h1>Preparing Bibledit</h1>\n"
  "</body>\n"
  "</html>\n";
  return html;
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
  
  // In client mode, do not display the page for entering the admin's details.
  if (config_logic_client_prepared ()) {
    redirect_browser (request, index_index_url ());
    Database_Config_General::setInstalledVersion (config_logic_version ());
    return "";
  }

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
        Database_Config_General::setInstalledVersion (config_logic_version ());
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
    Database_Config_General::setInstalledVersion (config_logic_version ());
  }

  return view.render ("setup", "index");
}


