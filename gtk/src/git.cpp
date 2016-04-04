/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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


#include "git.h"
#include "gwrappers.h"
#include "directories.h"
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>
#include "utilities.h"
#include "projectutils.h"
#include "shell.h"
#include "progresswindow.h"
#include "books.h"
#include "tiny_utilities.h"
#include "settings.h"
#include "date_time_utils.h"
#include "unixwrappers.h"
#include "gtkwrappers.h"
#include "generalconfig.h"
#include "maintenance.h"
#include "snapshots.h"
#include "vcs.h"
#include <glib/gi18n.h>

void git_upgrade ()
// Upgrades the git system.
{
  // Go through the projects that have their git repository enabled.
  extern Settings * settings;
  vector <ustring> projects = projects_get_all ();
  for (unsigned int i = 0; i < projects.size(); i++) {
    ProjectConfiguration * projectconfig = settings->projectconfig (projects[i]);
    ustring git_directory = gw_build_filename (project_data_directory_project (projects[i]), ".git");
    if (projectconfig->git_use_remote_repository_get()) {
      // At times there's a stale index.lock file that prevents any collaboration.
      // This is to be removed.
      ustring index_lock = gw_build_filename (git_directory, "index.lock");
      if (g_file_test (index_lock.c_str(), G_FILE_TEST_IS_REGULAR)) {
        gw_message (_("Cleaning out index lock ") + index_lock);
        unix_unlink (index_lock.c_str());
      }
      // Get the data directory for the project
      ustring datadirectory = tiny_project_data_directory_project(projects[i]);
      // On most machines git can determine the user's name from the system services. 
      // On the XO machine, it can't. It is set here manually. 
      // On more recent versions of git, like version 1.8.3 and younger,
      // although git may determine the user's name from the system, 
      // it still requires it to be set manually.
      ustring command;
      command = "git config user.email \"";
      command.append(g_get_user_name());
      command.append("@");
      command.append(g_get_host_name());
      command.append("\"");
      maintenance_register_shell_command (datadirectory, command);
      command = "git config user.name \"";
      command.append(g_get_real_name());
      command.append("\"");
      maintenance_register_shell_command (datadirectory, command);
      // (Re)initialize the repository. This can be done repeatedly without harm.
      // Note that this is done on shutdown.
      maintenance_register_shell_command (datadirectory, "git init");
    } else {
      if (g_file_test (git_directory.c_str(), G_FILE_TEST_IS_DIR)) {
        gw_message (_("Cleaning out folder ") + git_directory);
        ProgressWindow progresswindow (_("Tidying up project ") + projects[i], false);
        progresswindow.set_fraction (0.5);
        unix_rmdir (git_directory);
      }
    }
  }
}


void git_revert_to_internal_repository(const ustring & project)
// This reverts the repository to the internal one, if that is not yet the case.
{
  // Set in the configuration that we're using a local repository only.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  projectconfig->git_use_remote_repository_set(false);
}


void git_resolve_conflict_chapter(const ustring & project, unsigned int book, unsigned int chapter)
// This solves a conflicting chapter.
{
  // Log message
  gw_message(_("Resolving conflict for project ") + project + _(" book ") + books_id_to_english(book) + " " + convert_to_string(chapter));

  // Directory of the chapter in the data.
  ustring directory = project_data_directory_chapter(project, book, chapter);

  // Data filename.
  ustring datafile = project_data_filename_chapter(project, book, chapter, false);

  /* Read the datafile. If there is a conflict it will look like the example below:

     \c 1
     <<<<<<< HEAD:3 John/1/data
     \v 1 my text.
     =======
     \v 1 server's text.
     >>>>>>> a62f843ce41ed2d0325c8a2767993df6acdbc933:3 John/1/data
     \v 2

   */
  ReadText rt(datafile, true);

  // Set about to resolve the conflict.
  vector < ustring > newdata;
  bool withinmine = false;
  bool withinserver = false;
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    // Find out when we've a marker, no actual data.    
    bool minemarker = rt.lines[i].find(git_mine_conflict_marker()) != string::npos;
    bool separatormarker = rt.lines[i] == "=======";
    bool servermarker = rt.lines[i].find(">>>>>>> ") == 0;
    // Do conflict management only if we've actual data.
    bool takeit = false;
    if (!minemarker && !separatormarker && !servermarker) {
      if (withinmine) {
      } else if (withinserver) {
        takeit = true;
      } else {
        takeit = true;
      }
    }
    if (takeit) {
      newdata.push_back(rt.lines[i]);
    } else {
    }
    // Set whether we're within a conflict, my data, or the server's data.
    // This setting applies to the next line of data.
    if (minemarker) {
      withinmine = true;
      withinserver = false;
    }
    if (separatormarker) {
      withinmine = false;
      withinserver = true;
    }
    if (servermarker) {
      withinmine = false;
      withinserver = false;
    }
  }
  write_lines(datafile, newdata);
  // Next time that a timed pull and push is done, the conflict will show up as resolved.
}


ustring git_mine_conflict_marker()
/*
 If there is a conflicting merge, we may have data like this:

 <<<<<<< HEAD:3 John/1/data
 \v 1 xFrom the church leader.
 =======
 \v 1 xxFrom the church leader.
 >>>>>>> a62f843ce41ed2d0325c8a2767993df6acdbc933:3 John/1/data

 */
{
  return "<<<<<<< HEAD";
}


void git_resolve_conflicts(const ustring & project, const vector < ustring > &errors)
/*
 This function reads through the "errors" to see if any conflicting merge occurred.
 If that happened, it resolves the conflicts.
 */
{
  /*
     See if the errors indicate a conflict. If not, bail out.

     The first time that a "git pull" is done, and there is a conflict, it says this:

     Auto-merged 3 John/1/data
     CONFLICT (content): Merge conflict in 3 John/1/data
     Automatic merge failed; fix conflicts and then commit the result.  

     The second time that a "git pull" would be done, it would say this:

     You are in the middle of a conflicted merge.

   */
  bool conflict = false;
  for (unsigned int i = 0; i < errors.size(); i++) {
    if (errors[i].find("conflict") != string::npos) {
      conflict = true;
    }
  }
  if (!conflict)
    return;

  // Get the working directory.
  ustring directory = project_data_directory_project(project);

  // Run a "git status" to find the books and chapters that have a merge conflict.
  vector < Reference > conflicted_chapters;
  {
    GwSpawn spawn("git status");
    spawn.workingdirectory(directory);
    spawn.read();
    spawn.run();
    for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
      ustring line = spawn.standardout[i];
      size_t pos = line.find("unmerged:");
      if (pos != string::npos) {
        line.erase(0, pos + 10);
        line = trim(line);
        Parse parse(line, false, G_DIR_SEPARATOR_S);
        if (parse.words.size() >= 2) {
          unsigned int book = books_english_to_id(parse.words[0]);
          if (book) {
            unsigned int chapter = convert_to_int(parse.words[1]);
            Reference reference(book, chapter, "");
            conflicted_chapters.push_back(reference);
          }
        }
      }
    }
  }

  // Resolve all conflicts according to the settings of the user.
  for (unsigned int i = 0; i < conflicted_chapters.size(); i++) {
    git_resolve_conflict_chapter(project, conflicted_chapters[i].book, conflicted_chapters[i].chapter);
  }

  // Commit the changes.
  {
    GwSpawn spawn("git commit");
    spawn.workingdirectory(directory);
    spawn.arg("-m");
    spawn.arg("Resolved conflict");
    spawn.arg("-a");
    spawn.run();
  }
}


void git_process_feedback (const ustring& project, const vector <ustring>& feedback, unsigned int watched_book, unsigned int watched_chapter, bool& watch_updated)
{
  // Bail out if there's not enough feedback.
  if (feedback.size() < 3) {
    return;
  }

  // Examine the output and take appropriate actions.
  // A normal action is when a chapter is updated as a result of a git pull. 
  // Example:
  // Updating 4807e84..0afa0e3
  // Fast forward
  // 3 John/1/data |    2 +-
  // 1 files changed, 1 insertions(+), 1 deletions(-)
  for (unsigned int i = 0; i < feedback.size(); i++) {
    ustring line = feedback[i];
    if (line.length() > 5) {
      Parse parse(line, false, G_DIR_SEPARATOR_S);
      if (parse.words.size() >= 2) {
        unsigned int book = books_english_to_id(parse.words[0]);
        if (book) {
          unsigned int chapter = convert_to_int(parse.words[1]);
          snapshots_shoot_chapter (project, book, chapter, 0, false);
          // Update the watch.
          if (book == watched_book) {
            if (chapter == watched_chapter) {
              watch_updated = true;
            }
          }
        }
      }
    }
  }
}


/*

Some information is available about the secure git repository at sourceforge.net.

Shell access: ssh shell.sourceforge.net
Then follow instructions.

Creating Multple Repositories
To create a new repository, you need to access the Shell service, then follow these steps:
   1. Navigate to your repository
         1. cd /home/scm_git/P/PR/PROJECTUNIXNAME
                * PROJECTUNIXNAME is the UNIX name of your project
                * P represents the first letter of that name, and PR the first two letters of the name. 
   2. Create a new directory with the name you want for the repository, eg mkdir DIRNAME.
   3. Run git --git-dir=DIRNAME init --shared=all --bare (where DIRNAME represents the name of the repository to be created)
          * This will initialize a new repository at that directory 
Notes: Developers should not nest directories / repositories. Directories should only be created the top level directory of repository. 

*/
