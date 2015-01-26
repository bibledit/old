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


#include <collaboration/write.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/git.h>
#include <filter/url.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <access/bible.h>
#include <database/config/bible.h>
#include <database/jobs.h>
#include <dialog/list.h>
#include <tasks/logic.h>
#include <jobs/index.h>
#include <collaboration/link.h>


string collaboration_write_url ()
{
  return "collaboration/write";
}


bool collaboration_write_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::admin ());
}


string collaboration_write (void * webserver_request) // Todo
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  
  string page;
  Assets_Header header = Assets_Header (gettext("Collaboration"), request);
  page = header.run ();
  Assets_View view = Assets_View ();
  
  
  // The Bible (object) to work with.
  string object = request->query ["object"];
  view.set_variable ("object", object);
  

  string url = Database_Config_Bible::getRemoteRepositoryUrl (object);
  view.set_variable ("url", url);

  
  vector <string> okaylines;
  vector <string> errorlines;
  if (!object.empty ()) {

    string directory = filter_git_directory (object);
    string error;
    string username = request->session_logic ()->currentUser ();
    string email = request->database_users ()->getUserToEmail (username);
    bool okay = true;
    
    // Temporal file for trying write access.
    string temporal_file_name = filter_url_create_path (directory, "test_repository_writable");
    filter_url_file_put_contents (temporal_file_name, "contents");
    
    // Add this file.
    okay = filter_git_add_all (directory, error);
    if (okay) { // Todo
      okaylines.push_back (gettext("A file was added to the cloned repository successfully."));
    } else {
      errorlines.push_back (gettext("Failure adding a file to the cloned repository."));
      errorlines.push_back (error);
    }
    
    // Commit the file locally.
    if (okay) {
      okay = filter_git_commit (directory, username, email, "Write text", error);
      if (okay) { // Todo
        okaylines.push_back (gettext("The file was committed to the cloned repository successfully."));
      } else {
        errorlines.push_back (gettext("Failure committing a file to the cloned repository."));
        errorlines.push_back (error);
      }
    }


    
    
    
  } else {
    errorlines.push_back (gettext ("No Bible given"));
  }

  
  if (!errorlines.empty ()) {
    view.enable_zone ("error");
  } else {
    view.enable_zone ("okay");
  }
  view.set_variable ("resultblock", filter_string_implode (okaylines, " "));
  view.set_variable ("error", filter_string_implode (errorlines, " "));
  
  
  page += view.render ("collaboration", "write");
  page += Assets_Page::footer ();
  return page;
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
