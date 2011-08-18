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
#include "gwrappers.h"


vector <ustring> git_exec_update_folder(const ustring & folder)
/*
 Commits local changes.
 Pulls all changes from the remote repository.
 Pushes all changes to the remote repository.
 */
{
  git_exec_message("Updating data in folder " + folder, true);

  vector <ustring> feedback;

  // Tell git about the default method for pushing.
  {
    GwSpawn spawn ("git");
    spawn.arg ("config");
    spawn.arg ("push.default");
    spawn.arg ("matching");
    spawn.workingdirectory (folder);
    spawn.run ();
  }

  // Add everything because things could have been added or changed.
  {
    GwSpawn spawn ("git");
    spawn.arg ("add");
    spawn.arg (".");
    spawn.workingdirectory (folder);
    spawn.run ();
  }

  // Show status.
  {
    GwSpawn spawn ("git");
    spawn.arg ("status");
    spawn.workingdirectory (folder);
    spawn.run ();
  }

  // Commit changes locally.
  {
    GwSpawn spawn ("git");
    spawn.arg ("commit");
    spawn.arg ("-a");
    spawn.arg ("-m");
    spawn.arg ("commit");
    spawn.workingdirectory (folder);
    spawn.run ();
  }

  // Pull changes from the remote repository.
  bool pulled_ok = false;
  {
    GwSpawn spawn ("git");
    spawn.arg ("pull");
    spawn.workingdirectory (folder);
    spawn.read ();
    spawn.run ();
    pulled_ok = spawn.exitstatus == 0;
    feedback.push_back (convert_to_string (spawn.exitstatus));
    for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
      git_exec_message (spawn.standardout[i], true);
      feedback.push_back (spawn.standardout[i]);
    }
    for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
      git_exec_message (spawn.standarderr[i], true);
      feedback.push_back (spawn.standarderr[i]);
    }
  }

  // Push changes to the remote repository.
  if (pulled_ok) {
    GwSpawn spawn ("git");
    spawn.arg ("push");
    spawn.workingdirectory (folder);
    spawn.run ();
  }
  
  // Indicate ready in logbook.
  gw_message ("Ready");
  
  return feedback;
}


void git_exec_message(const ustring & message, bool eol)
{
  if (write(1, message.c_str(), strlen(message.c_str()))) ;
  if (eol)
    if (write(1, "\n", 1)) ;
}

