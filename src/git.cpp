/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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
#include "ipc.h"
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
#include "git-exec.h"

vector < GitTask > gittasks;

GitTask::GitTask(GitTaskType task_in, const ustring & project_in, unsigned int book_in, unsigned int chapter_in, unsigned int failures_in, const ustring & data_in)
{
  task = task_in;
  project = project_in;
  book = book_in;
  chapter = chapter_in;
  failures = failures_in;
  data = data_in;
}

GitChapterState::GitChapterState(const ustring & project, unsigned int book, unsigned int chapter)
{
  // Store variables.
  myproject = project;
  mybook = book;
  mychapter = chapter;
  // Read and store the current state of the file.
  ustring filename = project_data_filename_chapter(project, book, chapter, false);
  gchar *contents;
  g_file_get_contents(filename.c_str(), &contents, NULL, NULL);
  if (contents) {
    state = contents;
    g_free(contents);
  }
}

bool GitChapterState::changed()
{
  // Compare the current state of the file with the previous one,
  // and see whether it differs.
  ustring filename = project_data_filename_chapter(myproject, mybook, mychapter, false);
  gchar *contents;
  g_file_get_contents(filename.c_str(), &contents, NULL, NULL);
  bool filechanged = false;
  if (contents) {
    filechanged = state != contents;
    g_free(contents);
  }
  return filechanged;
}

void git_initialize_subsystem()
{
}

void git_finalize_subsystem()
{
  // Send the shutdown command to bibledit-git.
  vector <ustring> payload;
  payload.push_back(" ");
  extern InterprocessCommunication *ipc;
  ipc->send(ipcstBibleditGit, ipcctGitShutdown, payload);
}

void git_upgrade ()
// Upgrades the git system.
{
  extern Settings * settings;
  vector <ustring> projects = projects_get_all ();
  for (unsigned int i = 0; i < projects.size(); i++) {
    ProjectConfiguration * projectconfig = settings->projectconfig (projects[i]);
    if (!projectconfig->git_use_remote_repository_get()) {
      ustring git_directory = gw_build_filename (project_data_directory_project (projects[i]), ".git");
      if (g_file_test (git_directory.c_str(), G_FILE_TEST_IS_DIR)) {
        gw_message ("Cleaning out folder " + git_directory);
        ProgressWindow progresswindow ("Tidying up project " + projects[i], false);
        progresswindow.set_fraction (0.5);
        unix_rmdir (git_directory);
      }
    }
  }
}


void git_schedule(GitTaskType task, const ustring & project, unsigned int book, unsigned int chapter, const ustring & data)
// This schedules a git task.
{
  GitTask gittask(task, project, book, chapter, 0, data);
  gittasks.push_back(gittask);
}

unsigned int git_tasks_count()
{
  return gittasks.size();
}

void git_get_tasks(vector < unsigned int >&tasks, vector < ustring > &projects, vector < unsigned int >&books, vector < unsigned int >&chapters, vector < unsigned int >&fails)
{
  for (unsigned int i = 0; i < gittasks.size(); i++) {
    tasks.push_back(gittasks[i].task);
    projects.push_back(gittasks[i].project);
    books.push_back(gittasks[i].book);
    chapters.push_back(gittasks[i].chapter);
    fails.push_back(gittasks[i].failures);
  }
}

vector < ustring > git_get_next_task()
{
  vector < ustring > task;
  if (!gittasks.empty()) {
    task.push_back(convert_to_string(gittasks[0].task));
    task.push_back(gittasks[0].project);
    task.push_back(convert_to_string(gittasks[0].book));
    task.push_back(convert_to_string(gittasks[0].chapter));
    task.push_back(gittasks[0].data);
  }
  return task;
}

void git_erase_task_done()
{
  // Erase the task on top of the stack, but also all other equal tasks.
  if (!gittasks.empty()) {
    GitTask task = gittasks[0];
    vector < GitTask > newtasks;
    for (unsigned int i = 0; i < gittasks.size(); i++) {
      if (task.task != gittasks[i].task || task.project != gittasks[i].project || task.book != gittasks[i].book || task.chapter != gittasks[i].chapter)
        newtasks.push_back(gittasks[i]);
    }
    gittasks = newtasks;
  }
}

void git_fail_task_done()
{
  if (!gittasks.empty()) {
    GitTask task = gittasks[0];
    git_erase_task_done();
    task.failures++;
    gittasks.push_back(task);

  }
}

void git_erase_task(GitTaskType task, const ustring & project, unsigned int book, unsigned int chapter)
{
  vector < GitTask > newtasks;
  for (unsigned int i = 0; i < gittasks.size(); i++) {
    if (task != gittasks[i].task || project != gittasks[i].project || book != gittasks[i].book || chapter != gittasks[i].chapter)
      newtasks.push_back(gittasks[i]);
  }
  gittasks = newtasks;
}

void git_move_project(const ustring & project, const ustring & newproject)
{
  // Update all pending operations for this project.
  for (unsigned int i = 0; i < gittasks.size(); i++) {
    if (gittasks[i].project == project)
      gittasks[i].project = newproject;
  }
}

void git_remove_project(const ustring & project)
{
  // Remove all pending operations for this project.
  vector < GitTask > newtasks;
  for (unsigned int i = 0; i < gittasks.size(); i++) {
    if (gittasks[i].project != project)
      newtasks.push_back(gittasks[i]);
  }
  gittasks = newtasks;
}

int git_count_tasks_project(const ustring & project)
{
  int count = 0;
  for (unsigned int i = 0; i < gittasks.size(); i++) {
    if (gittasks[i].project == project)
      count++;
  }
  return count;
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
  gw_message("Resolving conflict for project " + project + " book " + books_id_to_english(book) + " " + convert_to_string(chapter));

  // Directory of the chapter in the data.
  ustring directory = project_data_directory_chapter(project, book, chapter);

  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  GitConflictHandlingType conflicthandling = (GitConflictHandlingType) projectconfig->git_remote_repository_conflict_handling_get();

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
        if (conflicthandling == gchtTakeMe)
          takeit = true;
      } else if (withinserver) {
        if (conflicthandling == gchtTakeServer)
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

  // To inform git that the conflict has been resolved.
  git_exec_update_project(project);
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

Reference git_execute_retrieve_reference()
// Retrieves the editor's reference from the database.
{
  Reference reference(0);
  extern InterprocessCommunication *ipc;
  vector < ustring > payload = ipc->get_payload(ipcctGitJobDescription);
  if (payload.size() == 2) {
    reference.book = convert_to_int(payload[0]);
    reference.chapter = convert_to_int(payload[1]);
  }
  return reference;
}

void git_command_pause(bool pause)
// Sets a flag whether the git subsystem has to pause or resume.
{
  extern Settings *settings;
  settings->session.git_pause = pause;
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

/*

Todo new git

* To move the git health commands to the maintenance routines.
* The initialization of the git repositories to be done in the maintenance routines. It also cleans out the index.lock, if it ever exists.

The database gets a table that shows the maintenance routines to be done. It has a field "working directory", and a field "command".
The shutdown routine reads all the commands, and executes them in order. It also stores what has been done already, so one command is not done twice.
The database stores a numbered "index", or how this is called, so that the commands are done in the sequence they were entered.

Send/receive scriptures. Works on git only. Normally only once in so many minutes, can be set. 
Default every hour or so. The git system is only used when remote git is used as well, apart from that it is not used. 
This prevents a lot of disk churning on startup since nothing is initialized if git is not used.

When a remote update results in files locally changed, we need to make a snapshot of each of the changed chapters.
How to we know that local files were changed? We might need to roam through the snapshots and compare these with the actual 
state of the files so as to see where there was a change. This method might be more reliable than relying on git's output, we don't know yet.

*/


