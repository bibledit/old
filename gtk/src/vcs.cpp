/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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


#include "libraries.h"
#include "utilities.h"
#include "vcs.h"
#include <glib.h>
#include "bible.h"
#include "gwrappers.h"
#include "shell.h"
#include "unixwrappers.h"
#include "uname.h"
#include "books.h"
#include "settings.h"
#include "tiny_utilities.h"
#include "git-exec.h"
#include "notes_utils.h"


VCS::VCS(bool dummy)
// This controls the git calls.
{
  mypause = false;
  thread_running = false;
  thread_run = true;
  g_thread_create(GThreadFunc(thread_start), gpointer(this), false, NULL);
}


VCS::~VCS()
{
  // Indicate to the thread we want to stop.
  thread_run = false;
  // Wait until thread has exited, but do not wait more than, say, 10 seconds.
  // The reason that we don't want to wait too long is that at times when a git operations cannot complete in time,
  // bibledit won't shut down till that operations has been done. This may take a long time, therefore a cap should be in place.
  unsigned int waitcounter = 0;
  while (thread_running) {
    g_usleep(10000);
    waitcounter++;
    if (waitcounter > 1000) {
      break;
    }
  }
}


void VCS::thread_start(gpointer data)
{
  ((VCS *) data)->thread_main();
}


void VCS::thread_main()
{
  thread_running = true;
  while (thread_run) {
    if (!mypause) {
      if (!tasks.empty()) {
        vector <ustring> feedback = git_exec_update_folder(tasks[0]);
        if (tasks[0] == notes_shared_storage_folder ()) {
          for (unsigned int i = 0; i < feedback.size(); i++) {
            notes_handle_vcs_feedback (notes_shared_storage_folder (), feedback[i]);
          }
        }
        erase (tasks[0]);
      }
    }
    g_usleep(300000);
  }
  thread_running = false;
}


void VCS::schedule(const ustring & folder)
// This schedules a git folder.
{
  tasks.push_back(folder);
}


void VCS::erase (const ustring& task)
{
  vector <ustring> newtasks;
  for (unsigned int i = 0; i < tasks.size(); i++) {
    if (task != tasks[i])
      newtasks.push_back(tasks[i]);
  }
  tasks = newtasks;
}


