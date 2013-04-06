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

#ifndef INCLUDED_REPORTING_H
#define INCLUDED_REPORTING_H

#include "libraries.h"
#include <glib.h>
#include "types.h"

class ProjectStatusRecord
{
public:
  ProjectStatusRecord(unsigned int tasks_size);
  vector <int> tasks_done;
  unsigned int calculate_percentage(const vector <unsigned int>& percentages);
  void print();
private:
};

const gchar * reporting_status_filename();
ustring reporting_status_filename(const ustring& project, unsigned int book);
void reporting_check_tasks_and_durations(vector <ustring>& tasks, vector <double> * durations);
void reporting_durations_to_percentages(vector <double>& durations, vector <unsigned int>& percentages);
unsigned int reporting_calculate_percentage_ready(vector <int>& tasks_done, const vector <unsigned int>& percentages);
unsigned int reporting_status_string_to_int(const ustring& status, vector <ustring> * allstatus);
void reporting_derive_parent_status_from_children(unsigned int taskcount, ProjectStatusRecord * parent, vector <ProjectStatusRecord *> children);
void reporting_merge_child_status_into_parent(unsigned int taskcount, unsigned int childnumber, ProjectStatusRecord& parent, const ProjectStatusRecord& child);
void reporting_derive_children_status_from_parent(unsigned int taskcount, vector <ProjectStatusRecord *> children, ProjectStatusRecord * parent);

class ProjectStatusChapter
{
public:
  ProjectStatusChapter(unsigned int chapter_in, unsigned int highestverse, GKeyFile * keyfile, vector <ustring> * alltasks_in);
  unsigned int chapter;
  vector <ProjectStatusRecord> status;
  ProjectStatusRecord get();
  void set(ProjectStatusRecord& state);
  ProjectStatusRecord get_verse(unsigned int verse);
  void set_verse(unsigned int verse, const ProjectStatusRecord& state);
  void save(GKeyFile * keyfile);
  void print();
private:
  vector <ustring> * alltasks;
  const gchar * chapter_key();
};

class ProjectStatusBook
{
public:
  ProjectStatusBook(const ustring& project_in, unsigned int book_in, vector <ustring> * alltasks_in);
  ~ProjectStatusBook();
  unsigned int book;
  vector <ProjectStatusChapter *> chapters;
  ProjectStatusRecord get();
  void set(ProjectStatusRecord& state);
  void save();
  void print();
private:
  ustring project;
  vector <ustring> * alltasks;
};

class ProjectStatus
{
public:
  ProjectStatus(const ustring& project_in, const vector <ustring>& alltasks_in, bool gui);
  ~ProjectStatus();
  vector <ProjectStatusBook *> books;
  ProjectStatusRecord get();
  void set(ProjectStatusRecord& state);
  void save();
  void print();
private:
  ustring project;
  vector <ustring> alltasks;
};

unsigned int reporting_get_percentage_ready_project(ProjectStatus * projectstatus);
unsigned int reporting_get_percentage_ready_book(ProjectStatusBook * projectstatusbook);
ustring reporting_get_task_done_sign(int state);
void reporting_get_tasks_done_per_book(ProjectStatus * projectstatus, vector <ustring>& headers, vector <VectorUstring>& texts);
void reporting_get_tasks_done_per_chapter(ProjectStatusBook * projectstatusbook, vector <ustring>& headers, vector <VectorUstring>& texts);
void reporting_produce_status_report(const ustring& project, bool planning, bool perc_done_project, bool perc_done_book, bool tasks_book, bool tasks_chapter, bool csv_export);
void reporting_get_tasks_undone(ProjectStatus * projectstatus, vector <unsigned int>& books, vector <ustring>& tasks);
unsigned int reporting_get_task_percentage_complete(ProjectStatus * projectstatus, unsigned int book, const ustring& task);

#endif
