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


#include <collaboration/link.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/git.h>
#include <filter/url.h>
#include <locale/translate.h>
#include <access/bible.h>
#include <database/config/bible.h>
#include <database/config/general.h>
#include <database/jobs.h>
#include <git2.h>


void collaboration_link (string object, int jobid, string direction)
{
  // Repository details for local and remote.
  string url = Database_Config_Bible::getRemoteRepositoryUrl (object);
  string path = filter_git_directory (object);
  bool result = true;
  vector <string> success;
  string error;

  Database_Jobs database_jobs;

  // Generate the initial page.
  string page = collaboration_link_header ();
  Assets_View view = Assets_View ();
  view.set_variable ("object", object);
  view.set_variable ("url", url);
  if (direction == "me") view.enable_zone ("takeme");
  if (direction == "repo") view.enable_zone ("takerepo");
  page += view.render ("collaboration", "link");
  page += Assets_Page::footer ();
  database_jobs.setStart (jobid, page);

  // Some checks on input values.
  if (result) {
    if (object.empty ()) {
      error = gettext ("No Bible given");
      result = false;
    }
  }
  if (result) {
    if (direction != "repo" && direction != "me") {
      error = gettext ("It is unclear which data to copy to where");
      result = false;
    }
  }

  // Try read access.
  result = filter_git_remote_read (url, error);
  if (result) {
    success.push_back (gettext("Read access to the repository is successful."));
  } else {
    error.append (" " + gettext ("Read access failed."));
  }
  
  // Clone the remote repository, with feedback about progress.
  if (result) {
    success.push_back ("Clone remote repository");
    result = filter_git_remote_clone (url, path, jobid, error);
  }

  // Set some configuration values.
  if (result) {
    success.push_back (gettext ("Configure cloned repository"));

    // Switch rename detection off.
    // This is necessary for the consultation notes, since git has been seen to "detect" spurious renames.
    filter_git_config_set_int (path, "diff.renamelimit", 0);
    filter_git_config_set_bool (path, "diff.renames", false);

    // Recent versions of git ask the user to set the default pushing method.
    filter_git_config_set_string (path, "push.default", "matching");
    
    // On some machines the mail name and address are not set properly; therefore these are set here.
    string mail_name = Database_Config_General::getSiteMailName();
    if (mail_name.empty ()) mail_name = "Bibledit";
    filter_git_config_set_string (path, "user.name", mail_name);
    string mail_address = Database_Config_General::getSiteMailAddress();
    if (mail_address.empty ()) mail_address = "bibledit-web@bibledit.org";
    filter_git_config_set_string (path, "user.email", mail_address);
  }
  
  // Store a temporal file for trying whether Bibledit has write access.
  string temporal_file_name = filter_url_create_path (path, "test_repository_writable");
  if (result) {
    filter_url_file_put_contents (temporal_file_name, "contents");
  }
  
  // Add this file.
  if (result) {
    result = filter_git_add_all (path, error);
    if (result) {
      success.push_back (gettext("A file was added to the cloned repository successfully."));
    } else {
      error.append (" " + gettext("Failure adding a file to the cloned repository."));
    }
    
  }
  
  // Commit the file locally.
  if (result) {
    result = filter_git_commit (path, "Bibledit", "bibledit@bibledit.org", "Write test", error);
    if (result) {
      success.push_back (gettext("The file was committed to the cloned repository successfully."));
    } else {
      error.append (" " + gettext("Failure committing a file to the cloned repository."));
    }
  }
  

  
  
  /* Todo
   
   echo gettext("Step: Committing the above changes locally") . "\n";
   
   echo gettext("Step: Pulling changes from the remote repository") . "\n";
   
   // Pull changes.
   // We cannot look at the exit code here in case the repository is empty,
   // because in such cases the exit code is undefined.
   $command = "cd $escapedDir; git pull 2>&1";
   echo "$command\n";
   passthru ($command, $exit_code);
   echo gettext("Ok: Changes were pulled from the repository successfully.");
   echo "\n";
   
   echo gettext("Step: Pushing changes to the remote repository") . "\n";
   
   // Push the changes to see if there is write access.
   // Notice the --all switch needed when the remote repository is empty.
   $command = "cd $escapedDir; git push --all 2>&1";
   echo "$command\n";
   passthru ($command, $exit_code);
   if ($exit_code == 0) {
   echo gettext("Ok: Changes were pushed to the repository successfully.");
   } else {
   echo gettext("Error: Pushing changes to the repository failed.");
   }
   echo "\n";
   
   echo gettext("Step: Removing the temporal file from the local repository") . "\n";
   
   // Remove the temporal file from the cloned repository.
   unlink ($temporal_file_name);
   $command = "cd $escapedDir; git commit -a -m write-test 2>&1";
   echo "$command\n";
   passthru ($command, $exit_code);
   if ($exit_code == 0) {
   echo gettext("Ok: The temporal file was removed from the local repository successfully.");
   } else {
   echo gettext("Error: Removing the temporal file from the local repository failed.");
   }
   echo "\n";
   
   echo gettext("Step: Pushing the changes to the remote repository") . "\n";
   
   // Push changes to the remote repository.
   $command = "cd $escapedDir; git push 2>&1";
   echo "$command\n";
   passthru ($command, $exit_code);
   if ($exit_code == 0) {
   echo gettext("Ok: Changes were pushed to the remote repository successfully.");
   } else {
   echo gettext("Error: Pushing changes to the remote repository failed.");
   }
   echo "\n";
   
   exec ("sync");
   
   $database_shell = Database_Shell::getInstance ();
   $database_shell->stopProcess ("collaboration_repo_write", 0);
   
   */

 
 
// Todo add more linking steps.
  
  page = collaboration_link_header ();
  view = Assets_View ();
  view.set_variable ("object", object);
  view.set_variable ("url", url);
  if (direction == "me") view.enable_zone ("takeme");
  if (direction == "repo") view.enable_zone ("takerepo");
  if (result) {
    view.enable_zone ("okay");
    view.set_variable ("success", filter_string_implode (success, "<br>\n"));
  } else {
    view.enable_zone ("error");
    view.set_variable ("error", error);
  }
  page += view.render ("collaboration", "link");
  page += Assets_Page::footer ();
  database_jobs.setResult (jobid, page);
}


string collaboration_link_header ()
{
  return "<h1>" + gettext ("Link repository") + "</h1>\n";
}
