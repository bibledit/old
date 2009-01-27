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

#include "bibledit-git.h"
#include "ipc.h"
#include "types.h"
#include "git-exec.h"
#include "tiny_utilities.h"

InterprocessCommunication *ipc;

int main(int argc, char *argv[])
{
  // Do not allow to run as root.
  if (getuid() == 0)
    return 1;

  // Thread support.
  g_thread_init(NULL);

  // IPC sytstem.
  InterprocessCommunication myipc(ipcstBibleditGit);
  ipc = &myipc;

  // Wait a reasonably long time before git activity starts.
  // This is because Bibledit itself is starting, and if git activity
  // were starting at the same time too, that would postpone
  // Bibledit getting reasonably responsive.
  g_usleep(5000000);

  // While the flag indicates that we ought to run, we keep going.
  while (ipc->get_payload(ipcctGitShutdown).empty()) {

    // Variables.
    vector < ustring > error;
    GitTaskType task;
    ustring project;
    unsigned int book, chapter;
    ustring data;

    // Get the next available task.
    vector < ustring > available_task;
    available_task = ipc->receive(ipcstBibleditBin, ipcctGitJobDescription, available_task);
    if (available_task.size() == 5) {

      // Okay, there is something to be done. Pop the job description.
      task = (GitTaskType) convert_to_int(available_task[0]);
      project = available_task[1];
      book = convert_to_int(available_task[2]);
      chapter = convert_to_int(available_task[3]);
      data = available_task[4];

      // Clear any previous error.
      error.clear();

      // Execute the right task.    
      switch (task) {
      case gttInitializeProject:
        {
          git_exec_initialize_project(project, book);
          break;
        }
      case gttCommitProject:
        {
          git_exec_commit_project(project);
          break;
        }
      case gttStoreChapter:
        {
          git_exec_store_chapter(project, book, chapter);
          break;
        }
      case gttUpdateProject:
        {
          error = git_exec_update_project(project, data);
          break;
        }
      }

      // Send done or fail.
      ipc->send(ipcstBibleditBin, ipcctGitTaskDone, error);

      // There are cases that one task hangs, and nearly blocks the program.
      // Introduce a delay to solve this.
      if (!error.empty()) {
        g_usleep(1000000);
      }

    }

  }

  // The job's been done.
  git_exec_message("Terminated normally", true);
  return 0;
}
