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


#include "libraries.h"
#include "utilities.h"
#include "vcs.h"
#include <glib/gthread.h>
#include "bible.h"
#include "gwrappers.h"
#include "shell.h"
#include "unixwrappers.h"
#include "uname.h"
#include "books.h"
#include "settings.h"
#include "tiny_utilities.h"
#include "git-exec.h"


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
  // Wait until thread has exited.
  while (thread_running)
    g_usleep(10000);
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
        switch (tasks[0].task) {
        case gttPushPull:
          {
            vector <ustring> feedback = git_exec_update_project(tasks[0].project);
            git_process_feedback (tasks[0].project, feedback);
            break;
          }
        }
        erase (tasks[0]);
      }
    }
    g_usleep(300000);
  }
  thread_running = false;
}


void VCS::schedule(GitTaskType task, const ustring & project)
// This schedules a git task.
{
  GitTask gittask(task, project);
  tasks.push_back(gittask);
}


void VCS::erase (const GitTask& task)
{
  vector <GitTask> newtasks;
  for (unsigned int i = 0; i < tasks.size(); i++) {
    if (task.task != tasks[i].task || task.project != tasks[i].project)
      newtasks.push_back(tasks[i]);
  }
  tasks = newtasks;
}


void VCS::move_bible (const ustring& old, const ustring& nw)
// Moves any pending tasks that refer to the old Bible to the new one.
{
  for (unsigned int i = 0; i < tasks.size(); i++) {
    if (tasks[i].project == old) {
      tasks[i].project = nw;
    }
  }
}


unsigned int VCS::tasks_for_bible(const ustring& name)
// Counts the number of tasks for this Bible.
{
  unsigned int count = 0;
  for (unsigned int i = 0; i < tasks.size(); i++) {
    if (tasks[i].project == name)
      count++;
  }
  return count;
}


void VCS::remove_bible (const ustring & name)
// Remove all pending operations for this Bible.
{
  vector <GitTask> newtasks;
  for (unsigned int i = 0; i < tasks.size(); i++) {
    if (tasks[i].project != name)
      newtasks.push_back(tasks[i]);
  }
  tasks = newtasks;
}


void VCS::pause (bool value)
// Whether operations should be paused.
{
  mypause = value;
}


bool VCS::paused ()
// Whether operations are paused.
{
  return mypause;
}


