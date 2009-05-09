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


#ifndef INCLUDED_GIT_H
#define INCLUDED_GIT_H


#include "libraries.h"
#include "types.h"
#include "reference.h"


class GitTask
{
public:
  GitTask (GitTaskType task_in, const ustring& project_in, unsigned int book_in, unsigned int chapter_in, unsigned int failures_in, const ustring& data_in);
  GitTaskType task;
  ustring project;
  unsigned int book;
  unsigned int chapter;
  unsigned int failures;
  ustring data;
};


class GitChapterState
{
public:
  GitChapterState (const ustring& project, unsigned int book, unsigned int chapter);
  bool changed ();
private:
  ustring state;
  ustring myproject;
  unsigned int mybook;
  unsigned int mychapter;
};


// Possible results of running git update.
enum GitUpdateType {gutOk, gutUpdated, gutAdded, gutDeleted, gutReplaced, gutMerged, gutConflict, gutError};
// Conflict handling.
enum GitConflictHandlingType {gchtTakeMe, gchtTakeServer};


void git_initialize_subsystem ();
void git_finalize_subsystem ();
void git_upgrade ();
void git_schedule (GitTaskType task, const ustring& project, unsigned int book, unsigned int chapter, const ustring& data);
unsigned int git_tasks_count ();
void git_get_tasks (vector <unsigned int>& tasks, vector <ustring>& projects, vector <unsigned int>& books, vector <unsigned int>& chapters, vector <unsigned int>& fails);
vector <ustring> git_get_next_task ();
void git_erase_task_done ();
void git_fail_task_done ();
void git_erase_task (GitTaskType task, const ustring& project, unsigned int book, unsigned int chapter);
void git_move_project (const ustring& project, const ustring& newproject);
void git_remove_project (const ustring& project);
int git_count_tasks_project (const ustring& project);
void git_revert_to_internal_repository (const ustring& project);
void git_resolve_conflict_chapter (const ustring& project, unsigned int book, unsigned int chapter);
ustring git_mine_conflict_marker ();
Reference git_execute_retrieve_reference ();
void git_command_pause (bool pause);
void git_command_set_update_frequency (unsigned int frequency);
void git_resolve_conflicts (const ustring& project, const vector <ustring>& errors);
vector<ustring> git_retrieve_chapter_commit (const ustring& project, unsigned int book, unsigned int chapter, const ustring& commit);


#endif
