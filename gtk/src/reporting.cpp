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


#include "reporting.h"
#include "projectutils.h"
#include "gwrappers.h"
#include "tiny_utilities.h"
#include "settings.h"
#include "versification.h"
#include <math.h>
#include "progresswindow.h"
#include "htmlwriter.h"
#include "date_time_utils.h"
#include "books.h"
#include "directories.h"
#include "utilities.h"
#include "planning.h"


const gchar *reporting_status_filename()
// Gives the base filename of the status file.
{
  return "status";
}

ustring reporting_status_filename(const ustring & project, unsigned int book)
// Gives the full filename of the filename that has the status information.
// project: project
// book: book - there is one status file per book.
{
  return gw_build_filename(project_data_directory_book(project, book), reporting_status_filename());
}

void reporting_check_tasks_and_durations(vector < ustring > &tasks, vector < double >*durations)
// This is a function designed to initialize an empty vector of tasks
// and accompanying durations.
// If the vector are empty, they get initialized with a defaults.
// If the vectors are not of the same size, their sizes will be adjusted.
{
  struct {
    const char *task;
    double duration;
  } default_values[] = { {
  "First Draft", 2}, {
  "Self Check", 0.4}, {
  "Team Check", 0.4}, {
  "Back Translation", 0.4}, {
  "Advisor Check", 0.1}, {
  "First Revision", 0.4}, {
  "First Village Check", 0.5}, {
  "Second Revision", 0.4}, {
  "Second Village Check", 0.5}, {
  "Third Revision", 0.4}, {
  "Consultant Check", 0.1}, {
  "Fourth Revision", 0.4}, {
  "First Read Through", 0.2}, {
  "Second Read Through", 0.2}, {
  "Publish", 0}, {
  "Post Publish Revision", 0}};

  if (tasks.empty()) {
    vector < ustring > values;
    for (unsigned int i = 0; i < (sizeof(default_values) / sizeof(*default_values)); i++) {
      values.push_back(default_values[i].task);
    }
    tasks = values;
  }

  if (durations) {
    if (tasks.size() != durations->size()) {
      durations->clear();
      for (unsigned int i = 0; i < tasks.size(); i++) {
        double duration = 0.5;
        for (unsigned int i2 = 0; i2 < (sizeof(default_values) / sizeof(*default_values)); i2++) {
          if (tasks[i] == default_values[i2].task) {
            duration = default_values[i2].duration;
          }
        }
        durations->push_back(duration);
      }
    }
  }
}

void reporting_durations_to_percentages(vector < double >&durations, vector < unsigned int >&percentages)
// Takes durations, and creates a list of percentages out of that.
{
  // Bail out if there's nothing.
  if (durations.empty())
    return;

  // Calculate the total duration.
  double duration = 0;
  for (unsigned int i = 0; i < durations.size(); i++) {
    duration += durations[i];
  }

  // Calculate the factor to multiply with to get percentages.
  double factor = 100 / duration;

  // Calculate the percentages belonging to each duration.
  // Calculate the total percentage too.
  unsigned int total_percentage = 0;
  percentages.clear();
  for (unsigned int i = 0; i < durations.size(); i++) {
    unsigned int percentage = (unsigned int)round(factor * durations[i]);
    percentages.push_back(percentage);
    total_percentage += percentage;
  }

  // Correct the list of percentages if the total differs from 100.
  if (total_percentage == 100)
    return;
  if (total_percentage < 100) {
    for (unsigned int i = 0; i < 100 - total_percentage; i++) {
      percentages[i % percentages.size()]++;
    }
  }
  if (total_percentage > 100) {
    for (unsigned int i = 0; i < total_percentage - 100; i++) {
      unsigned int offset = i % percentages.size();
      if (percentages[offset] > 0) {
        percentages[offset]--;
      } else {
        total_percentage++;
      }
    }
  }
}

unsigned int reporting_calculate_percentage_ready(vector < int >&tasks_done, const vector < unsigned int >&percentages)
// Calculates the percentage ready
{
  unsigned int percentage = 0;
  if (tasks_done.size() == percentages.size()) {
    for (unsigned int i = 0; i < tasks_done.size(); i++) {
      if (tasks_done[i] > 0) {
        percentage += percentages[i];
      }
    }
  }
  return percentage;
}

unsigned int reporting_status_string_to_int(const ustring & status, vector < ustring > *allstatus)
// Takes string "status", and returns the offset this string has in "allstatus".
{
  for (unsigned int i = 0; i < allstatus->size(); i++) {
    if (status == allstatus->at(i)) {
      return i;
    }
  }
  return 0;
}

void reporting_derive_parent_status_from_children(unsigned int taskcount, ProjectStatusRecord * parent, vector < ProjectStatusRecord * >children)
// Takes pointers to existing status records, and calculates the parent's status
// from the children.
{
  // Go through all children and derive the parent's status for each task.
  for (unsigned int i = 0; i < taskcount; i++) {
    bool inconsistent = false;
    int initialstatus = children[0]->tasks_done[i];
    for (unsigned int i2 = 1; i2 < children.size(); i2++) {
      if (children[i2]->tasks_done[i] != initialstatus)
        inconsistent = true;
    }
    if (inconsistent) {
      parent->tasks_done[i] = -1;
    } else {
      if (initialstatus)
        parent->tasks_done[i] = 1;
      else
        parent->tasks_done[i] = 0;
    }
  }
}

void reporting_merge_child_status_into_parent(unsigned int taskcount, unsigned int childnumber, ProjectStatusRecord & parent, const ProjectStatusRecord & child)
// Merges a child's status record into a parent.
{
  for (unsigned int i = 0; i < taskcount; i++) {
    if (childnumber == 0) {
      parent.tasks_done[i] = child.tasks_done[i];
    } else {
      if (child.tasks_done[i] < 0)
        parent.tasks_done[i] = -1;
      if (child.tasks_done[i] != parent.tasks_done[i])
        parent.tasks_done[i] = -1;
    }
  }
}

void reporting_derive_children_status_from_parent(unsigned int taskcount, vector < ProjectStatusRecord * >children, ProjectStatusRecord * parent)
// Takes pointers to an existing status record, and derives the children's status
// from their parent.
{
  // Go through each task of the parent and propagate it to all the children.
  for (unsigned int i = 0; i < taskcount; i++) {

    // Get status of this particular task.
    int status = parent->tasks_done[i];
    // If it is inconsistent, do not propagate it.
    if (status < 0)
      continue;
    // Propagate it to all children.
    for (unsigned int i2 = 0; i2 < children.size(); i2++) {
      children[i2]->tasks_done[i] = status;
    }
  }
}

ProjectStatusRecord::ProjectStatusRecord(unsigned int tasks_size)
// Status record of a verse (or chapter / book / project).
//  0 : task not done.
//  1 : task done.
// -1 : some children have the task done, others have not.
{
  for (unsigned int i = 0; i < tasks_size; i++) {
    tasks_done.push_back(0);
  }
}

unsigned int ProjectStatusRecord::calculate_percentage(const vector < unsigned int >&percentages)
{
  return reporting_calculate_percentage_ready(tasks_done, percentages);
}

void ProjectStatusRecord::print()
{
  for (unsigned int i = 0; i < tasks_done.size(); i++)
    cout << tasks_done[i];
  cout << endl;
}

ProjectStatusChapter::ProjectStatusChapter(unsigned int chapter_in, unsigned int highestverse, GKeyFile * keyfile, vector < ustring > *alltasks_in)
/*
 Retrieves the chapter status from the keyfile.
 An example file would look so:

 [0]
 (Indicates chapter 0 in a book).

 [1]
 1=Draft;Read Through;
 (Indicates that verse 1 has completed tasks "Draft" and "Read Through").

 [2]
 chapter=Draft;
 (Indicates that all the verses of the chapter have completed task "Draft".)
 */
{
  // Save variables.
  chapter = chapter_in;
  alltasks = alltasks_in;

  // Get the status for the whole chapter if available.
  ProjectStatusRecord chapterrecord(alltasks->size());
  bool chapterrecord_available = false;
  {
    gsize length;
    gchar **stringlist = g_key_file_get_string_list(keyfile, convert_to_string(chapter).c_str(), chapter_key(), &length, NULL);
    if (stringlist) {
      for (unsigned int i2 = 0; i2 < length; i2++) {
        for (unsigned int i3 = 0; i3 < alltasks->size(); i3++) {
          if (stringlist[i2] == alltasks->at(i3)) {
            chapterrecord.tasks_done[i3] = 1;
          }
        }
      }
      g_strfreev(stringlist);
      chapterrecord_available = true;
    }
  }

  // Get status of all verses in the chapter.
  for (unsigned int i = 0; i <= highestverse; i++) {
    ProjectStatusRecord statusrecord(alltasks->size());
    if (chapterrecord_available) {
      for (unsigned int i2 = 0; i2 < alltasks->size(); i2++) {
        statusrecord.tasks_done[i2] = chapterrecord.tasks_done[i2];
      }
    } else {
      gsize length;
      gchar **stringlist = g_key_file_get_string_list(keyfile, convert_to_string(chapter).c_str(), convert_to_string(i).c_str(), &length, NULL);
      if (stringlist) {
        for (unsigned int i2 = 0; i2 < length; i2++) {
          for (unsigned int i3 = 0; i3 < alltasks->size(); i3++) {
            if (stringlist[i2] == alltasks->at(i3)) {
              statusrecord.tasks_done[i3] = 1;
            }
          }
        }
        g_strfreev(stringlist);
      }
    }
    status.push_back(statusrecord);
  }
}

ProjectStatusRecord ProjectStatusChapter::get()
// Gets the status of the entire chapter.
// This status is derived from all verses.
{
  // Empty status for the chapter.
  ProjectStatusRecord statusrecord(alltasks->size());

  // Collect pointers to the status record for the children.
  vector < ProjectStatusRecord * >children_records;
  for (unsigned int i = 0; i < status.size(); i++) {
    children_records.push_back(&status[i]);
  }

  // Get the chapter's status.
  reporting_derive_parent_status_from_children(alltasks->size(), &statusrecord, children_records);

  // Return the chapter's status.
  return statusrecord;
}

void ProjectStatusChapter::set(ProjectStatusRecord & state)
// Propagates the "state" to all the verses in the chapter.
{
  vector < ProjectStatusRecord * >verses_status;
  for (unsigned int i = 0; i < status.size(); i++) {
    verses_status.push_back(&status[i]);
  }
  reporting_derive_children_status_from_parent(alltasks->size(), verses_status, &state);
}

ProjectStatusRecord ProjectStatusChapter::get_verse(unsigned int verse)
// Gets the status of a verse.
{
  if (verse < status.size())
    return status[verse];
  ProjectStatusRecord statusrecord(alltasks->size());
  return statusrecord;
}

void ProjectStatusChapter::set_verse(unsigned int verse, const ProjectStatusRecord & state)
// Sets the status of a verse.
{
  if (verse < status.size())
    status[verse] = state;
}

void ProjectStatusChapter::save(GKeyFile * keyfile)
// Save the status of the chapter to the keyfile.
{
  // The name of the group.
  ustring group = convert_to_string(chapter);

  // Get the whole chapter's status.
  ProjectStatusRecord chapterrecord = get();

  // See if the parent status is consistent.
  bool chapter_consistent = true;
  for (unsigned int i = 0; i < chapterrecord.tasks_done.size(); i++) {
    if (chapterrecord.tasks_done[i] < 0)
      chapter_consistent = false;
  }

  if (chapter_consistent) {

    // Set the values for the whole chapter at once.

    // Assemble values to set at this key.
    vector < ustring > values;
    for (unsigned int i2 = 0; i2 < chapterrecord.tasks_done.size(); i2++) {
      if (chapterrecord.tasks_done[i2] > 0) {
        values.push_back(alltasks->at(i2));
      }
    }
    // Continue if there is something to save.
    if (!values.empty()) {
      // Write the values to the keyfile.
      gchar **strs = NULL;
      strs = g_new(gchar *, values.size());
      for (unsigned int i2 = 0; i2 < values.size(); i2++) {
        strs[i2] = g_strdup(values[i2].c_str());
      }
      g_key_file_set_string_list(keyfile, group.c_str(), chapter_key(), (const gchar ** const)strs, values.size());
      for (unsigned int i2 = 0; i2 < values.size(); i2++) {
        g_free(strs[i2]);
      }
      g_free(strs);
    }
  } else {

    // Set the values for the individual verses.
    for (unsigned int i = 0; i < status.size(); i++) {
      // Key name.
      ustring name = convert_to_string(i);
      // Assemble values to set at this key.
      vector < ustring > values;
      for (unsigned int i2 = 0; i2 < status[i].tasks_done.size(); i2++) {
        if (status[i].tasks_done[i2] > 0) {
          values.push_back(alltasks->at(i2));
        }
      }
      // Continue if there's nothing to save.
      if (values.empty())
        continue;
      // Write the values to the keyfile.
      gchar **strs = NULL;
      strs = g_new(gchar *, values.size());
      for (unsigned int i2 = 0; i2 < values.size(); i2++) {
        strs[i2] = g_strdup(values[i2].c_str());
      }
      g_key_file_set_string_list(keyfile, group.c_str(), name.c_str(), (const gchar ** const)strs, values.size());
      for (unsigned int i2 = 0; i2 < values.size(); i2++) {
        g_free(strs[i2]);
      }
      g_free(strs);
    }

  }
}

const gchar *ProjectStatusChapter::chapter_key()
/*
 The chapter key has been introduced to save space.
 Without it the status of each verse would be saved per verse.
 But with this key, if the status of a chapter is consistent, that is,
 if all verses have completed the same tasks, then the status of the whole
 chapter is saved at once, using this key. This requires much less space
 than saving per chapter. And it is much faster reading and writing the
 status.
 */
{
  return "chapter";
}

void ProjectStatusChapter::print()
{
  cout << "Status for chapter " << chapter << endl;
  for (unsigned int i = 0; i < status.size(); i++) {
    status[i].print();
  }
}

ProjectStatusBook::ProjectStatusBook(const ustring & project_in, unsigned int book_in, vector < ustring > *alltasks_in)
{
  // Save variables.
  project = project_in;
  book = book_in;
  alltasks = alltasks_in;

  // Versification.
  extern Settings *settings;
  ustring versification = settings->projectconfig(project)->versification_get();

  // Load the keyfile.
  GKeyFile *keyfile = g_key_file_new();
  g_key_file_load_from_file(keyfile, reporting_status_filename(project, book).c_str(), G_KEY_FILE_NONE, NULL);

  // Create the chapters in this book.
  vector < unsigned int >chps = project_get_chapters(project, book);
  for (unsigned int i = 0; i < chps.size(); i++) {
    unsigned int highestverse = convert_to_int(versification_get_last_verse(versification, book, chps[i]));
    if (highestverse == 1)
      highestverse = 0;
    ProjectStatusChapter *chapter = new ProjectStatusChapter(chps[i], highestverse, keyfile, alltasks);
    chapters.push_back(chapter);
  }

  // Free the keyfile.
  g_key_file_free(keyfile);
}

ProjectStatusBook::~ProjectStatusBook()
{
  for (unsigned int i = 0; i < chapters.size(); i++) {
    delete chapters[i];
  }
}

ProjectStatusRecord ProjectStatusBook::get()
// Gets the status of the entire book.
// This status is derived from all chapters.
{
  // Empty status for the book.
  ProjectStatusRecord statusrecord(alltasks->size());

  // Collect pointers to the status record for the children.
  vector < ProjectStatusRecord > children_records_objects;
  for (unsigned int i = 0; i < chapters.size(); i++) {
    ProjectStatusRecord statusrecord = chapters[i]->get();
    children_records_objects.push_back(statusrecord);
  }
  vector < ProjectStatusRecord * >children_records_pointers;
  for (unsigned int i = 0; i < children_records_objects.size(); i++) {
    children_records_pointers.push_back(&children_records_objects[i]);
  }

  // Get the book's status.
  reporting_derive_parent_status_from_children(alltasks->size(), &statusrecord, children_records_pointers);

  // Return the book's status.
  return statusrecord;
}

void ProjectStatusBook::set(ProjectStatusRecord & state)
// Sets the status of the entire book.
// This status is propagated to all chapters in the book.
{
  for (unsigned int i = 0; i < chapters.size(); i++) {
    chapters.at(i)->set(state);
  }
}

void ProjectStatusBook::save()
// Save the status of the book.
{
  // Create new keyfile.
  GKeyFile *keyfile = g_key_file_new();
  // Let every chapter store its own bit in that keyfile.
  for (unsigned int i = 0; i < chapters.size(); i++) {
    chapters[i]->save(keyfile);
  }
  // Save the file to disk.
  gchar *data = g_key_file_to_data(keyfile, NULL, NULL);
  if (data) {
    g_file_set_contents(reporting_status_filename(project, book).c_str(), data, -1, NULL);
    g_free(data);
  }
  // Free memory.
  g_key_file_free(keyfile);
}

void ProjectStatusBook::print()
{
  cout << "Printing status for book " << book << endl;
  for (unsigned int i = 0; i < chapters.size(); i++) {
    chapters[i]->print();
  }
}

ProjectStatus::ProjectStatus(const ustring & project_in, const vector < ustring > &alltasks_in, bool gui)
{
  // Save project.
  project = project_in;
  alltasks = alltasks_in;

  // Progress window.
  ProgressWindow *progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow("Loading status", false);
  }
  // Load books in project.
  vector < unsigned int >bks = project_get_books(project);
  if (gui)
    progresswindow->set_iterate(0, 1, bks.size());
  for (unsigned int i = 0; i < bks.size(); i++) {
    if (gui)
      progresswindow->iterate();
    ProjectStatusBook *book = new ProjectStatusBook(project, bks[i], &alltasks);
    books.push_back(book);
  }

  // Destroy possible progresswindow.
  if (gui)
    delete progresswindow;
}

ProjectStatus::~ProjectStatus()
{
  for (unsigned int i = 0; i < books.size(); i++) {
    delete books[i];
  }
}

ProjectStatusRecord ProjectStatus::get()
// Gets the status of the entire project.
// This status is derived from all individual books.
{
  // Empty status for the project.
  ProjectStatusRecord statusrecord(alltasks.size());

  // Collect pointers to the status record for the children.
  vector < ProjectStatusRecord > children_records_objects;
  for (unsigned int i = 0; i < books.size(); i++) {
    ProjectStatusRecord statusrecord = books[i]->get();
    children_records_objects.push_back(statusrecord);
  }
  vector < ProjectStatusRecord * >children_records_pointers;
  for (unsigned int i = 0; i < children_records_objects.size(); i++) {
    children_records_pointers.push_back(&children_records_objects[i]);
  }

  // Get the project's status.
  reporting_derive_parent_status_from_children(alltasks.size(), &statusrecord, children_records_pointers);

  // Return the project's status.
  return statusrecord;
}

void ProjectStatus::set(ProjectStatusRecord & state)
// Sets the status of the entire project.
// This status is set in all books of the project.
{
  for (unsigned int i = 0; i < books.size(); i++) {
    books.at(i)->set(state);
  }
}

void ProjectStatus::save()
{
  for (unsigned int i = 0; i < books.size(); i++) {
    books[i]->save();
  }
}

void ProjectStatus::print()
{
  cout << "Printing project status " << this << endl;
  for (unsigned int i = 0; i < books.size(); i++) {
    books[i]->print();
  }
}

unsigned int reporting_get_percentage_ready_project(ProjectStatus * projectstatus)
// Calculates the percentage ready for the whole project.
{
  extern Settings *settings;
  vector < ustring > tasks = settings->genconfig.project_tasks_names_get();
  vector < double >durations;
  reporting_check_tasks_and_durations(tasks, &durations);
  vector < unsigned int >percentages;
  reporting_durations_to_percentages(durations, percentages);

  unsigned int verse_count = 0;
  unsigned int accumulated_percentage = 0;
  for (unsigned int i = 0; i < projectstatus->books.size(); i++) {
    ProjectStatusBook *projectstatusbook = projectstatus->books[i];
    for (unsigned int i = 0; i < projectstatusbook->chapters.size(); i++) {
      ProjectStatusChapter *projectstatuschapter = projectstatusbook->chapters[i];
      for (unsigned int i = 0; i < projectstatuschapter->status.size(); i++) {
        unsigned int percentage = projectstatuschapter->status[i].calculate_percentage(percentages);
        verse_count++;
        accumulated_percentage += percentage;
      }
    }
  }

  return accumulated_percentage / verse_count;
}

unsigned int reporting_get_percentage_ready_book(ProjectStatusBook * projectstatusbook)
{
  extern Settings *settings;
  vector < ustring > tasks = settings->genconfig.project_tasks_names_get();
  vector < double >durations;
  reporting_check_tasks_and_durations(tasks, &durations);
  vector < unsigned int >percentages;
  reporting_durations_to_percentages(durations, percentages);

  unsigned int verse_count = 0;
  unsigned int accumulated_percentage = 0;
  for (unsigned int i = 0; i < projectstatusbook->chapters.size(); i++) {
    ProjectStatusChapter *projectstatuschapter = projectstatusbook->chapters[i];
    for (unsigned int i = 0; i < projectstatuschapter->status.size(); i++) {
      unsigned int percentage = projectstatuschapter->status[i].calculate_percentage(percentages);
      verse_count++;
      accumulated_percentage += percentage;
    }
  }

  return accumulated_percentage / verse_count;
}

ustring reporting_get_task_done_sign(int state)
{
  ustring sign;
  if (state > 0)
    sign = "✔";
  if (state == 0)
    sign = "✗";
  if (state < 0)
    sign = "✗ ✔";
  return sign;
}

void reporting_get_tasks_done_per_book(ProjectStatus * projectstatus, vector < ustring > &headers, vector < VectorUstring > &texts)
// Produces the tasks completed / undone / partially done for each book.
{
  // Get the available tasks to be done.
  extern Settings *settings;
  vector < ustring > tasks = settings->genconfig.project_tasks_names_get();
  reporting_check_tasks_and_durations(tasks, NULL);

  // Put the tasks in the headers.
  headers = tasks;
  headers.insert(headers.begin(), "");

  for (unsigned int i = 0; i < projectstatus->books.size(); i++) {
    ProjectStatusBook *projectstatusbook = projectstatus->books[i];
    ProjectStatusRecord statusrecord(tasks.size());
    statusrecord = projectstatusbook->get();
    vector < ustring > cells;
    cells.push_back(books_id_to_english(projectstatusbook->book));
    for (unsigned int i = 0; i < tasks.size(); i++) {
      int state = statusrecord.tasks_done[i];
      cells.push_back(reporting_get_task_done_sign(state));
    }
    texts.push_back(cells);
  }
}

void reporting_get_tasks_done_per_chapter(ProjectStatusBook * projectstatusbook, vector < ustring > &headers, vector < VectorUstring > &texts)
{
  // Get the available tasks to be done.
  extern Settings *settings;
  vector < ustring > tasks = settings->genconfig.project_tasks_names_get();
  reporting_check_tasks_and_durations(tasks, NULL);

  // Put the tasks in the headers.
  headers = tasks;
  headers.insert(headers.begin(), "");

  for (unsigned int i = 0; i < projectstatusbook->chapters.size(); i++) {
    ProjectStatusChapter *projectstatuschapter = projectstatusbook->chapters[i];
    ProjectStatusRecord statusrecord(tasks.size());
    statusrecord = projectstatuschapter->get();
    vector < ustring > cells;
    cells.push_back(convert_to_string(projectstatuschapter->chapter));
    for (unsigned int i = 0; i < tasks.size(); i++) {
      int state = statusrecord.tasks_done[i];
      cells.push_back(reporting_get_task_done_sign(state));
    }
    texts.push_back(cells);
  }
}


void reporting_produce_status_report(const ustring & project, bool planning, bool perc_done_project, bool perc_done_book, bool tasks_book, bool tasks_chapter, bool csv_export)
// This produces a status report.
{
  // Bail out if there's no project.
  if (project.empty())
    return;

  // Progress window.
  ProgressWindow progresswindow("Creating report", false);
  progresswindow.set_fraction(0.1);

  // Title and heading.
  HtmlWriter htmlwriter("Status Report", false, false, true);
  htmlwriter.heading(1, "Status Report " + project);

  // Current date.
  htmlwriter.paragraph("Produced on " + date_time_julian_human_readable(date_time_julian_day_get_current(), true) + ".");

  // Planning.
  if (planning) {
    planning_produce_report(project, htmlwriter);
  }

  // Load status data.
  extern Settings *settings;
  vector < ustring > tasks = settings->genconfig.project_tasks_names_get();
  reporting_check_tasks_and_durations(tasks, NULL);
  ProjectStatus projectstatus(project, tasks, false);

  // Percentage complete for whole project.
  progresswindow.set_fraction(0.2);
  if (perc_done_project) {
    // Bar graph.
    vector < ustring > texts;
    vector < unsigned int >percentages;
    texts.push_back(project);
    unsigned int percentage = reporting_get_percentage_ready_project(&projectstatus);
    percentages.push_back(percentage);
    htmlwriter.bargraph("Project Percentage Complete", texts, percentages, "");
    htmlwriter.paragraph("");

    // CSV export.
    if (csv_export) {
      vector < ustring > csv;
      csv.push_back("\"project\"," + convert_to_string(percentage));
      ustring filename = gw_build_filename(directories_get_temp(), "project_complete.csv");
      write_lines(filename, csv);
      htmlwriter.hyperlinkedparagraph("The above data in csv format", filename);
      htmlwriter.paragraph("");
    }
  }
  // Percentage complete for each book.
  progresswindow.set_fraction(0.3);
  if (perc_done_book) {
    vector < ustring > texts;
    vector < unsigned int >percentages;
    for (unsigned int i = 0; i < projectstatus.books.size(); i++) {
      ProjectStatusBook *projectstatusbook = projectstatus.books[i];
      texts.push_back(books_id_to_english(projectstatusbook->book));
      percentages.push_back(reporting_get_percentage_ready_book(projectstatusbook));
    }
    htmlwriter.bargraph("Books Percentage Complete", texts, percentages, "");
    htmlwriter.paragraph("");

    // CSV export.
    if (csv_export) {
      vector < ustring > csv;
      for (unsigned int i = 0; i < texts.size(); i++) {
        csv.push_back("\"" + texts[i] + "\"," + convert_to_string(percentages[i]));
      }
      ustring filename = gw_build_filename(directories_get_temp(), "books_complete.csv");
      write_lines(filename, csv);
      htmlwriter.hyperlinkedparagraph("The above data in csv format", filename);
      htmlwriter.paragraph("");
    }
  }
  // Show tasks per book.
  progresswindow.set_fraction(0.4);
  if (tasks_book) {
    vector < ustring > column_headers;
    vector < VectorUstring > cell_texts;
    reporting_get_tasks_done_per_book(&projectstatus, column_headers, cell_texts);
    vector < bool > centers;
    for (unsigned int i = 0; i < column_headers.size(); i++) {
      if (i == 0)
        centers.push_back(false);
      else
        centers.push_back(true);
    }
    vector < ustring > shortened_column_headers;
    for (unsigned int i = 0; i < column_headers.size(); i++) {
      if (i)
        shortened_column_headers.push_back(convert_to_string(i));
      else
        shortened_column_headers.push_back("");
    }
    vector < VectorUstring > legend;
    for (unsigned int i = 0; i < column_headers.size(); i++) {
      vector < ustring > s;
      s.push_back(column_headers[i]);
      s.push_back(shortened_column_headers[i]);
      legend.push_back(s);
    }
    column_headers.clear();
    htmlwriter.table("Tasks per book - Legend", column_headers, legend, "", NULL, -1);
    htmlwriter.paragraph("");
    htmlwriter.table("Tasks per book - Data", shortened_column_headers, cell_texts, "", &centers, -1);
    htmlwriter.paragraph("");
  }
  // Show tasks per chapter.
  progresswindow.set_fraction(0.5);
  if (tasks_chapter) {
    for (unsigned int i = 0; i < projectstatus.books.size(); i++) {
      ProjectStatusBook *projectstatusbook = projectstatus.books[i];
      vector < ustring > column_headers;
      vector < VectorUstring > cell_texts;
      reporting_get_tasks_done_per_chapter(projectstatusbook, column_headers, cell_texts);
      vector < bool > centers;
      for (unsigned int i2 = 0; i2 < column_headers.size(); i2++) {
        if (i2 == 0)
          centers.push_back(false);
        else
          centers.push_back(true);
      }
      htmlwriter.table("Tasks per chapter for " + books_id_to_english(projectstatus.books[i]->book), column_headers, cell_texts, "", &centers, -1);
      htmlwriter.paragraph("");
    }
  }

}

void reporting_get_tasks_undone(ProjectStatus * projectstatus, vector < unsigned int >&books, vector < ustring > &tasks)
// Produces the tasks not yet (fully) done for all books.
{
  // Get the available tasks to be done.
  extern Settings *settings;
  vector < ustring > alltasks = settings->genconfig.project_tasks_names_get();
  reporting_check_tasks_and_durations(alltasks, NULL);

  // Get the undone (or partially done) tasks.
  for (unsigned int i = 0; i < projectstatus->books.size(); i++) {
    ProjectStatusBook *projectstatusbook = projectstatus->books[i];
    ProjectStatusRecord statusrecord(tasks.size());
    statusrecord = projectstatusbook->get();
    for (unsigned int i = 0; i < alltasks.size(); i++) {
      int state = statusrecord.tasks_done[i];
      if (state < 1) {
        books.push_back(projectstatusbook->book);
        tasks.push_back(alltasks[i]);
      }
    }
  }
}

unsigned int reporting_get_task_percentage_complete(ProjectStatus * projectstatus, unsigned int book, const ustring & task)
// Returns the percentage complete of a particular task of a book.
{
  unsigned int percentage_complete = 0;

  extern Settings *settings;
  vector < ustring > alltasks = settings->genconfig.project_tasks_names_get();
  vector < double >durations;
  reporting_check_tasks_and_durations(alltasks, &durations);

  for (unsigned int bk = 0; bk < projectstatus->books.size(); bk++) {
    ProjectStatusBook *projectstatusbook = projectstatus->books[bk];
    if (projectstatusbook->book == book) {
      unsigned int verse_count = 0;
      unsigned int accumulated_percentage = 0;
      for (unsigned int i = 0; i < projectstatusbook->chapters.size(); i++) {
        ProjectStatusChapter *projectstatuschapter = projectstatusbook->chapters[i];
        for (unsigned int i = 0; i < projectstatuschapter->status.size(); i++) {
          ProjectStatusRecord *statusrecord = &projectstatuschapter->status[i];
          for (unsigned int i = 0; i < alltasks.size(); i++) {
            if (task == alltasks[i]) {
              if (statusrecord->tasks_done[i] > 0)
                accumulated_percentage += 100;
              verse_count++;
            }
          }
        }
      }
      percentage_complete = accumulated_percentage / verse_count;
    }
  }

  return percentage_complete;
}
