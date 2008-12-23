/*
 ** Copyright (©) 2003-2008 Teus Benschop.
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
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **  
 */

#include "git-exec.h"
#include "tiny_utilities.h"
#include <glib.h>

void git_exec_initialize_project(const ustring & project, bool health)
{
  // Get the data directory for this project
  ustring datadirectory = tiny_project_data_directory_project(project);
  git_exec_message(datadirectory, true);

  // If the .svn subdirectory exists under the data directory remove them all
  ustring svndirectory = tiny_gw_build_filename(datadirectory, ".svn");
  if (g_file_test(svndirectory.c_str(), G_FILE_TEST_IS_DIR)) {
    ustring command = "cd '" + datadirectory + "'; ";
    command.append("find . -name .svn -print0 | xargs -0 rm -rf");
    git_exec_message("All .svn traces were removed", true);
    if (system(command.c_str())) ;
  }
  // On most machines git can determine the user's name from the system services. 
  // But on the XO machine, it can't.
  // Therefore it is set here manually.
  ustring command0("cd '" + datadirectory + "'; ");
  command0.append("git-config user.email \"");
  command0.append(g_get_user_name());
  command0.append("@");
  command0.append(g_get_host_name());
  command0.append("\"");
  if (system(command0.c_str())) ;
  command0 = ("cd '" + datadirectory + "'; ");
  command0.append("git-config user.name \"");
  command0.append(g_get_real_name());
  command0.append("\"");
  if (system(command0.c_str())) ;

  // (Re)initialize the repository. This can be done repeatedly without harm,
  // and it ensures that anything that was put in by hand will be seen by git,
  // thus making the system more robust.
  // At times health-related commands are ran too.
  ustring command1("cd '" + datadirectory + "'; ");
  command1.append("git-init-db");
  if (system(command1.c_str())) ;
  if (health) {
    ustring command("cd '" + datadirectory + "'; ");
    command.append("git-gc --prune");
    if (system(command.c_str())) ;
  }
  ustring command2("cd '" + datadirectory + "'; ");
  command2.append("git-add .");
  if (system(command2.c_str())) ;
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
  ustring command("cd '" + datadirectory + "'; ");
  command.append("git-add .");
  if (system(command.c_str())) ;

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
  ustring command1("cd '" + directory + "'; ");
  command1.append("git-status -a");
  if (system(command1.c_str())) ;

  ustring command2("cd '" + directory + "'; ");
  command2.append("git-add .");
  if (system(command2.c_str())) ;

  ustring command3("cd '" + directory + "'; ");
  command3.append("git-commit -m Commit -a");
  if (system(command3.c_str())) ;
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
  ustring command1("cd '" + datadirectory + "'; ");
  command1.append("git-pull '" + data + "'");
  if (system(command1.c_str())) ;

  // Push changes to the remote repository.
  ustring command2("cd '" + datadirectory + "'; ");
  command2.append("git-push '" + data + "'");
  if (system(command2.c_str())) ;

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
