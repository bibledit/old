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

#include "git-exec.h"
#include "tiny_utilities.h"
#include <glib.h>
#ifdef WIN32
#include <windows.h> // CreateProcess
#endif

ustring git_exec_change_dir(const ustring& dir) {
#ifdef WIN32
  return("cd \"" + dir + "\" && ");
#else
  return("cd '" + dir + "' ; ");
#endif
}

void git_exec_initialize_project(const ustring & project, bool health)
{
  // Get the data directory for this project
  ustring datadirectory = tiny_project_data_directory_project(project);
  git_exec_message(datadirectory, true);

  // If the .svn subdirectory exists under the data directory remove them all
  ustring svndirectory = tiny_gw_build_filename(datadirectory, ".svn");
  if (g_file_test(svndirectory.c_str(), G_FILE_TEST_IS_DIR)) {
    ustring command = "find . -name .svn -print0 | xargs -0 rm -rf";
    git_exec_message("All .svn traces were removed", true);
    git_exec_command(command, datadirectory);
  }

  // On most machines git can determine the user's name from the system services. 
  // But on the XO machine, it can't.
  // Therefore it is set here manually.
  ustring command0 = "git-config user.email \"";
  command0.append(g_get_user_name());
  command0.append("@");
  command0.append(g_get_host_name());
  command0.append("\"");
  git_exec_command(command0, datadirectory);
  command0 = "git-config user.name \"";
  command0.append(g_get_real_name());
  command0.append("\"");
  git_exec_command(command0, datadirectory);

  // (Re)initialize the repository. This can be done repeatedly without harm,
  // and it ensures that anything that was put in by hand will be seen by git,
  // thus making the system more robust.
  // At times health-related commands are ran too.
  ustring command1 = "git-init-db";
  git_exec_command(command1, datadirectory);
  if (health) {
    ustring command = "git-gc --prune";
    git_exec_command(command, datadirectory);
  }
  ustring command2 = "git-add .";
  git_exec_command(command2, datadirectory);
  git_exec_commit_directory(datadirectory);
}

void git_exec_store_chapter(const ustring & project, unsigned int book, unsigned int chapter)
// Stores a chapter. Returns whether things worked out.
{
  // Log message
  git_exec_message("Storing book " + convert_to_string(book) + " chapter " + convert_to_string(chapter), true);

  // Get the data directory for this project
  ustring datadirectory = tiny_project_data_directory_project(project);

  // The chapter may have been added when it wasn't there before.
  // Just to be sure, add anything under the data directory.  
  ustring command = "git-add .";
  git_exec_command(command, datadirectory);

  // Show status, and commit changes.
  git_exec_commit_directory(datadirectory);
}

void git_exec_commit_project(const ustring & project)
// Commits a project.
{
  ustring datadirectory = tiny_project_data_directory_project(project);
  git_exec_commit_directory(datadirectory);
}

void git_exec_commit_directory(const ustring & directory)
{
  // Show status, and commit changes.
  ustring command1 = "git-status -a";
  git_exec_command(command1, directory);

  ustring command2 = "git-add .";
  git_exec_command(command2, directory);

  ustring command3 = "git-commit -m Commit -a";
  git_exec_command(command3, directory);
}

vector < ustring > git_exec_update_project(const ustring & project, const ustring & data)
/*
 Pulls all changes from the remote repository.
 Pushes all changes to the remote repository.
 It disregards errors, because at times a remote repository can be offline.
 */
{
  // Log.
  git_exec_message("Updating project " + project, true);

  // The data directory for this project
  ustring datadirectory = tiny_project_data_directory_project(project);

  // Pull changes from the remote repository.
  // Some git installations need the source and destination branches as well.
  ustring command1 = "git-pull '" + data + "'";
  git_exec_command(command1, datadirectory);

  // Push changes to the remote repository.
  ustring command2 = "git-push '" + data + "'";
  git_exec_command(command2, datadirectory);

  // An update can fail in cases that the remote repository is not available 
  // at this time. In case of failure it would keep trying too often.
  // Therefore it is better to simulate that it worked out, rather than
  // returning the error.
  vector < ustring > lines;
  return lines;
}

void git_exec_message(const ustring & message, bool eol)
{
  if (write(1, message.c_str(), strlen(message.c_str()))) ;
  if (eol)
    if (write(1, "\n", 1)) ;
}

unsigned long git_exec_command(const ustring& cmd, const ustring& dir)
{
  unsigned long retval = -1; // assume failure

  //cout << "git_exec_command: " << cmd << " wd: " << dir << " ";

#ifdef WIN32
  // If you don't use CreateProcess(), you get one window for every system() call
  // ...tends to slow down the git process and produce flashing windows... :(
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  memset(&si, 0, sizeof(si));
  memset(&pi, 0, sizeof(pi));
  char* c = (char*)calloc(cmd.length() + 1, sizeof(char));
  cmd.copy(c, cmd.length());
  if(!CreateProcess(NULL, c, NULL, NULL, FALSE, DETACHED_PROCESS, NULL, dir.c_str(), &si, &pi)) {
    cerr << "CreateProcess failed: " << GetLastError() << endl;
    return(retval);
  }
  free(c);
  WaitForSingleObject(pi.hProcess, INFINITE);
  GetExitCodeProcess(pi.hProcess, &retval);
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
#else
  ustring c = git_exec_change_dir(dir) + cmd;
  retval = system(c.c_str());
#endif
  if(retval != 0)
    cout << "returned error code: " << retval << endl;
  else {
    //cout << endl;
  }

  return(retval);
}

