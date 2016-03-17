/*
 Copyright (©) 2003-2016 Teus Benschop.
 
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
#include <database/localization.h>
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
#include <database/state.h>
#include <database/login.h>
#include <database/privileges.h>
#include <styles/sheets.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <config/globals.h>
#include <demo/logic.h>
#include <locale/logic.h>
#include <tasks/logic.h>


void setup_conditionally (const char * package)
{
  string p (package);
  
  // When the package folder is the same as the document root folder,
  // it may mean that another program installs the data for us.
  // This is the case on Android.
  // In that case, wait till the most important data has been installed.
  if (p == config_globals_document_root) setup_main_folders_present ();
  
  // Run the setup if the versions differ.
  if (config_logic_version () != Database_Config_General::getInstalledDatabaseVersion ()) {
    
    vector <string> messages;

    // Copy the library into the destination place.
    if (p != config_globals_document_root) {
      messages.push_back ("Copy data from " + p + " to " + config_globals_document_root);
      setup_copy_library (package);
    }
    
    // Ensure write access to certain folders.
    setup_write_access ();
    
    // Create or upgrade the databases.
    messages.push_back ("Initializing and upgrading data");
    setup_initialize_data ();
    
    for (auto message : messages) {
      Database_Logs::log (message, Filter_Roles::admin());
    }
    
    // Cloud updates the available SWORD modules.
    if (!config_logic_client_prepared ()) tasks_logic_queue (REFRESHSWORDMODULES);
    
    // Update installed version.
    Database_Config_General::setInstalledDatabaseVersion (config_logic_version ());
  };

  if (config_logic_version () != Database_Config_General::getInstalledInterfaceVersion ()) {
    
    // In client mode or in demo mode do not display the page for entering the admin's details.
    if (config_logic_client_prepared ()) setup_complete_gui ();
    if (config_logic_demo_enabled ()) setup_complete_gui ();
    
    // When the admin's credentials are configured, enter them, and do not display them in the setup page.
    if (!config_logic_admin_username ().empty ()) {
      if (!config_logic_admin_password ().empty ()) {
        setup_set_admin_details (config_logic_admin_username (),
                                 config_logic_admin_password (),
                                 config_logic_admin_email ());
        setup_complete_gui ();
      }
    }
    
  }

  config_globals_data_initialized = true;
  
  // Run the indexers if a flag was set for it.
  // This mechanism is suitable for low power devices as Android and iOS.
  // If Bibles or Notes are scheduled to be indexed, since the tasks take a lot of time,
  // the app may shut down before the tasks have been completed.
  // Next time the app starts, the tasks will be restarted here, and they will run if a flag was set for them.
  // Once the tasks are really complete, they will clear the flag.
  tasks_logic_queue (REINDEXBIBLES);
  tasks_logic_queue (REINDEXNOTES);
  // Same for the resource downloader, for the client.
  if (config_logic_client_prepared ()) tasks_logic_queue (SYNCRESOURCES);
}


void setup_copy_library (const char * package)
{
  size_t package_length = strlen (package);
  filter_url_mkdir (config_globals_document_root);
  vector <string> package_paths;
  config_globals_setup_message = "scanning";
  filter_url_recursive_scandir (package, package_paths);
  for (auto package_path : package_paths) {
    string dest_path = config_globals_document_root + package_path.substr (package_length);
    config_globals_setup_message = dest_path;
    if (filter_url_is_dir (package_path)) {
      filter_url_mkdir (dest_path);
    } else {
      filter_url_file_cp (package_path, dest_path);
    }
  }
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


// Waits until the main folders for setup are present.
void setup_main_folders_present ()
{
  bool present;
  do {
    present = true;
    vector <string> folders = {"dyncss", "databases", "databases/config/general", "logbook", "bibles"};
    for (auto folder : folders) {
      string path = filter_url_create_root_path (folder);
      if (!file_exists (path)) {
        present = false;
      }
    }
    if (!present) this_thread::sleep_for (chrono::milliseconds (300));
  } while (!present);
}


void setup_initialize_data ()
{
  // Do the database setup.
  Webserver_Request request;
  // Display progress in text format.
  // That provides feedback to the user during installation.
  // This alerts the user that installation is in progress, and is not stuck,
  // as the user might think when the install takes longer than expected.
  config_globals_setup_message = "users";
  request.database_users ()->create ();
  request.database_users ()->upgrade ();
  config_globals_setup_message = "styles";
  request.database_styles ()->create ();
  config_globals_setup_message = "bible actions";
  request.database_bibleactions ()->create ();
  config_globals_setup_message = "checks";
  request.database_check ()->create ();
  setup_generate_locale_databases (false);
  config_globals_setup_message = "confirmations";
  Database_Confirm database_confirm = Database_Confirm ();
  database_confirm.create ();
  config_globals_setup_message = "jobs";
  Database_Jobs database_jobs = Database_Jobs ();
  database_jobs.create ();
  config_globals_setup_message = "sprint";
  Database_Sprint database_sprint = Database_Sprint ();
  database_sprint.create ();
  config_globals_setup_message = "mail";
  Database_Mail database_mail = Database_Mail (&request);
  database_mail.create ();
  config_globals_setup_message = "navigation";
  Database_Navigation database_navigation = Database_Navigation ();
  database_navigation.create ();
  config_globals_setup_message = "mappings";
  setup_generate_verse_mapping_databases ();
  config_globals_setup_message = "note actions";
  Database_NoteActions database = Database_NoteActions ();
  database.create ();
  config_globals_setup_message = "versifications";
  setup_generate_versification_databases ();
  config_globals_setup_message = "modifications";
  Database_Modifications database_modifications;
  database_modifications.create ();
  config_globals_setup_message = "notes";
  Database_Notes database_notes (&request);
  database_notes.create ();
  config_globals_setup_message = "volatile";
  Database_Volatile database_volatile = Database_Volatile ();
  database_volatile.create ();
  config_globals_setup_message = "state";
  Database_State::create ();
  config_globals_setup_message = "login";
  Database_Login::create ();
  Database_Login::optimize ();
  config_globals_setup_message = "privileges";
  Database_Privileges::create ();
  Database_Privileges::upgrade ();
  Database_Privileges::optimize ();

  // Create stylesheets.
  config_globals_setup_message = "stylesheets";
  styles_sheets_create_all ();
  
  // Schedule creation of sample Bible if there's no Bible yet.
  // In former versions of Bibledit, creation of the sample Bible was not scheduled,
  // but executed right away.
  // This led to very long app first-run times on low power devices.
  // The installation times were so long that user were tempted to think
  // that the install process was stuck.
  // To make installation fast, the creation of the sample Bible is now done in the background.
  config_globals_setup_message = "samples";
  vector <string> bibles = request.database_bibles()->getBibles ();
  if (bibles.empty ()) {
    tasks_logic_queue (CREATESAMPLEBIBLE);
  }
  
  // Schedule reindexing Bible search data.
  /*
   Re-indexing Bible search data was disabled again in Februari 2016,
   because it takes quite a while on low power devices,
   and the reason for the re-indexing is not clear.
  config_globals_setup_message = "indexes";
  Database_Config_General::setIndexBibles (true);
  tasks_logic_queue (REINDEXBIBLES);
  */
}


// Store the admin's details.
void setup_set_admin_details (string username, string password, string email)
{
  Database_Users database_users;
  database_users.removeUser (username);
  database_users.addNewUser (username, password, Filter_Roles::admin (), email);
}


// Set the GUI setup status as completed.
void setup_complete_gui ()
{
  Database_Config_General::setInstalledInterfaceVersion (config_logic_version ());
}


// Generate the locale databases.
void setup_generate_locale_databases (bool progress)
{
  // On Android, do not generate the locale databases.
  // On this low power device, generating them would take quite a while, as experience shows.
  // Instead of generating them, the builder and installer put the pre-generated databases into place.
  if (config_logic_android ()) return;
  // Same story for iOS.
  if (config_logic_ios ()) return;
  // Generate databases for all the localizations.
  map <string, string> localizations = locale_logic_localizations ();
  for (auto & element : localizations) {
    string localization = element.first;
    if (localization.empty ()) continue;
    config_globals_setup_message = "locale " + localization;
    if (progress) cout << config_globals_setup_message << endl;
    Database_Localization database_localization = Database_Localization (localization);
    string path = filter_url_create_root_path ("locale", localization + ".po");
    database_localization.create (path);
  }
}


// Generate the verse mapping databases.
void setup_generate_verse_mapping_databases () // Todo
{
  // On Android, do not generate the verse mapping databases.
  // On this low power device, generating them would take quite a while, as experience shows.
  // Instead of generating them, the builder and installer put the pre-generated databases into place.
  if (config_logic_android ()) return;
  // Same story for iOS.
  if (config_logic_ios ()) return;
  // Generate the verse mappings.
  Database_Mappings database_mappings;
  database_mappings.create1 ();
  database_mappings.defaults ();
  database_mappings.create2 ();
  database_mappings.optimize ();
}


// Generate the versification databases.
void setup_generate_versification_databases () // Todo
{
  // On Android, do not generate the verse mapping databases.
  // On this low power device, generating them would take quite a while, as experience shows.
  // Instead of generating them, the builder and installer put the pre-generated databases into place.
  if (config_logic_android ()) return;
  // Same story for iOS.
  if (config_logic_ios ()) return;
  // Generate the versifications.
  Database_Versifications database_versifications;
  database_versifications.create ();
  database_versifications.defaults ();
}