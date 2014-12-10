/*
Copyright (©) 2003-2014 Teus Benschop.

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
#include <database/history.h>
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


void setup_create_databases ()
{
  Webserver_Request request;
  request.database_users ()->create ();
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
  Database_History database_history = Database_History ();
  database_history.create ();
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
}


string setup_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  // Create or upgrade the databases.
  new thread (setup_create_databases);
  
  Assets_View view = Assets_View (0);

  if (!request->post ["Submit"].empty ()) {
  
    string admin_username = request->post ["admin_username"];
    string admin_password = request->post ["admin_password"];
    string admin_email = request->post ["admin_email"];
  
    vector <string> errors;
    
    if (admin_username.length() < 5) {
      errors.push_back ("Choose a longer username.");
    }
    
    if (admin_password.length() < 7) {
      errors.push_back ("Choose a longer password.");
    }
    
    if (admin_email.length() < 5) {
      errors.push_back ("Enter a valid email address.");
    }
  
    if (errors.empty()) {
      request->database_users ()->removeUser (admin_username);
      request->database_users ()->addNewUser (admin_username, admin_password, Filter_Roles::admin (), admin_email);
      Database_Config_General::setInstalledVersion (VERSION);
      redirect_browser ("/index/index", request);
    } else {
      view.enable_zone ("errors");
      view.set_variable ("error", filter_string_implode (errors, " "));  
    }

  }

  return view.render ("setup", "index");
}


