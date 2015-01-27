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
#include <filter/shell.h>
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
  bool takeme = (direction == "me");
  bool takerepo = (direction == "repo");

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
    if (!takeme && !takerepo) {
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
    success.push_back ("Copy repository to Bibledit");
    result = filter_git_remote_clone (url, path, jobid, error);
  }

  // Set some configuration values.
  if (result) {
    success.push_back (gettext ("Configure copied data"));

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
    result = filter_git_add_remove_all (path, error);
    if (result) {
      success.push_back (gettext("A file was added to the data successfully."));
    } else {
      error.append (" " + gettext("Failure adding a file to the data."));
    }
  }
  
  // Commit the file locally.
  if (result) {
    result = filter_git_commit (path, "Bibledit", "bibledit@bibledit.org", "Write test 1", error);
    if (result) {
      success.push_back (gettext("The file was committed successfully."));
    } else {
      error.append (" " + gettext("Failure committing the file."));
    }
  }
  
  // Pull changes from the remote repository.
  // We cannot look at the exit code here in case the repository is empty,
  // because in such cases the exit code is undefined.
  if (result) {
    string out, err;
    filter_shell_run (path, "git", {"pull"}, out, err);
    success.push_back (out + " " + err);
    success.push_back (gettext("Changes were pulled from the repository successfully."));
  }
  
  // Push the changes to see if there is write access.
  // Notice the --all switch needed when the remote repository is empty.
  if (result) {
    string out, err;
    result = (filter_shell_run (path, "git", {"push", "--all"}, out, err) == 0);
    success.push_back (out + " " + err);
    if (result) {
      success.push_back (gettext("Changes were pushed to the repository successfully."));
    } else {
      error.append (" " + gettext("Pushing changes to the repository failed."));
    }
  }
  
  // Remove the temporal file from the cloned repository.
  filter_url_unlink (temporal_file_name);
  if (result) {
    result = filter_git_add_remove_all (path, error); // Todo fails for removed files.
    if (result) {
      success.push_back (gettext("The temporal file was removed from the data successfully."));
    } else {
      error.append (" " + gettext("Failure removing the temporal file from the data."));
    }
  }
  if (result) {
    result = filter_git_commit (path, "Bibledit", "bibledit@bibledit.org", "Write test 2", error);
    if (result) {
      success.push_back (gettext("The removed temporal file was committed successfully."));
    } else {
      error.append (" " + gettext("Failure committing the removed temporal file."));
    }
  }

  // Push changes to the remote repository.
  if (result) {
    string out, err;
    result = (filter_shell_run (path, "git", {"push"}, out, err) == 0);
    success.push_back (out + " " + err);
    if (result) {
      success.push_back (gettext("The changes were pushed to the repository successfully."));
    } else {
      error.append (" " + gettext("Pushing changes to the repository failed."));
    }
  }
  
  // If so requested by the user,
  // copy the data from the local cloned repository,
  // and store it in Bibledit's Bible given in $object,
  // overwriting the whole Bible that was there before.
  if (takerepo && result) {
    success.push_back (gettext ("Copying the data from the repository and storing it in Bibledit."));
    Webserver_Request request;
    filter_git_sync_git_to_bible (&request, path, object);
  }
  
  // If so requested by the user,
  // copy the data from Bibledit to the local cloned repository,
  // and then push it to the remote repository,
  // so that the data in the repository matches with Bibledit's local data.
  if (takeme && result) {

    // Bibledit's data goes into the local repository.
    Webserver_Request request;
    success.push_back (gettext("Storing the local Bible data to the staging area."));
    filter_git_sync_bible_to_git (&request, object, path);

    // Stage the data: add and remove it as needed.
    if (result) {
      result = filter_git_add_remove_all (path, error);
      if (result) {
        success.push_back (gettext("The local Bible data was staged successfully."));
      } else {
        error.append (" " + gettext("Failure staging the local Bible data."));
      }
    }
    if (result) {
      result = filter_git_commit (path, "Bibledit", "bibledit@bibledit.org", "Write test 2", error);
      if (result) {
        success.push_back (gettext("The local Bible data was committed successfully."));
      } else {
        error.append (" " + gettext("Failure committing the local Bible data."));
      }
    }

    // Push changes to the remote repository.
    if (result) {
      string out, err;
      result = (filter_shell_run (path, "git", {"push"}, out, err) == 0);
      success.push_back (out + " " + err);
      if (result) {
        success.push_back (gettext("The local Bible data was pushed to the repository successfully."));
      } else {
        error.append (" " + gettext("Pushing the local Bible data to the repository failed."));
      }
    }
  }
  
  // Just in case it uses a removable flash disk for the repository, flush any pending writes to disk.
  sync ();
 
  // Ready linking the repository.
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
