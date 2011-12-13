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

#include "planning.h"
#include "reporting.h"
#include "date_time_utils.h"
#include "progresswindow.h"
#include "htmlwriter.h"
#include "settings.h"
#include "tiny_utilities.h"
#include <math.h>
#include "dialogeditlist.h"
#include "books.h"
#include "utilities.h"
#include "versification.h"


void planning_edit(const ustring & project)
// Edits the planning.
{
  extern Settings *settings;
  vector < ustring > tasks = settings->projectconfig(project)->planning_tasks_get();
  vector < ustring > addables;
  {
    vector < ustring > alltasks = settings->genconfig.project_tasks_names_get();
    reporting_check_tasks_and_durations(alltasks, NULL);
    ProjectStatus projectstatus(project, alltasks, true);
    vector < unsigned int >addable_books;
    vector < ustring > addable_tasks;
    reporting_get_tasks_undone(&projectstatus, addable_books, addable_tasks);
    for (unsigned int i = 0; i < addable_books.size(); i++) {
      addables.push_back(planning_assemble_task(addable_books[i], addable_tasks[i]));
    }
  }
  EditListDialog dialog(&tasks, "Planned Tasks", "of planned tasks: add, remove or reorder them", true, true, false, false, false, false, true, &addables);
  if (dialog.run() == GTK_RESPONSE_OK) {
    settings->projectconfig(project)->planning_tasks_set(tasks);
  }
}


ustring planning_assemble_task(unsigned int book, const ustring & task)
{
  return books_id_to_english(book) + " | " + task;
}


void planning_disassemble_task(ustring assembled_task, unsigned int &book, ustring & task)
{
  book = 0;
  task.clear();
  Parse parse(assembled_task, false, "|");
  if (parse.words.size() == 2) {
    book = books_english_to_id(trim(parse.words[0]));
    task = trim(parse.words[1]);
  }
}


void planning_produce_report(const ustring & project, HtmlWriter & htmlwriter)
// This produces a planning report.
{
  // Bail out if there's no project.
  if (project.empty())
    return;

  // Heading.
  htmlwriter.heading(1, "Planning " + project);

  // Current date.
  htmlwriter.paragraph("Produced on " + date_time_julian_human_readable(date_time_julian_day_get_current(), true) + ".");

  // Settings.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  vector < ustring > alltasks = settings->genconfig.project_tasks_names_get();
  vector < double >durations;
  reporting_check_tasks_and_durations(alltasks, &durations);
  ProjectStatus projectstatus(project, alltasks, true);

  // Date project finishes based on starting date and work completed.
  {
    int startdate = projectconfig->planning_project_start_get();
    int currentdate = date_time_julian_day_get_current();
    ustring paragraphtext = "The project started on " + date_time_julian_human_readable(startdate, false) + ", ";
    unsigned int percentage = reporting_get_percentage_ready_project(&projectstatus);
    paragraphtext.append("and " + convert_to_string(percentage) + " percent of the work has been done. ");
    bool calculation_possible = ((currentdate > startdate + 30) && (percentage > 1));
    if (calculation_possible) {
      double days_done = currentdate - startdate;
      double percentage_done_per_day = percentage / days_done;
      double percentage_todo = 100 - percentage;
      int days_todo = (int)round(percentage_todo / percentage_done_per_day);
      int enddate = currentdate + days_todo;
      guint year_complete, month_complete, day_complete;
      date_time_normal_get_year_month_day(enddate, year_complete, month_complete, day_complete);
      paragraphtext.append("If the work is carried on with the same pace it will be completed in the year " + convert_to_string(year_complete) + ", God willing.");
    } else {
      paragraphtext.append("At this stage it is not yet possible to estimate when it will finish.");
    }
    htmlwriter.paragraph(paragraphtext);
  }

  // List of planned tasks, including their dates.
  {
    vector < ustring > column_headers;
    column_headers.push_back("Book");
    column_headers.push_back("Task");
    column_headers.push_back("Start");
    column_headers.push_back("Duration (days)");
    column_headers.push_back("Complete (%)");
    column_headers.push_back("Finish");

    vector < bool > centers;
    centers.push_back(false);
    centers.push_back(false);
    centers.push_back(false);
    centers.push_back(true);
    centers.push_back(true);
    centers.push_back(false);

    unsigned int day = date_time_julian_day_get_current();

    vector < VectorUstring > cell_texts;

    vector < ustring > assembled_tasks = projectconfig->planning_tasks_get();
    for (unsigned int i = 0; i < assembled_tasks.size(); i++) {
      vector < ustring > row;
      unsigned int book;
      ustring task;
      planning_disassemble_task(assembled_tasks[i], book, task);
      unsigned int percentage_complete = reporting_get_task_percentage_complete(&projectstatus, book, task);
      double duration = 0;
      {
        for (unsigned int i = 0; i < alltasks.size(); i++) {
          if (task == alltasks[i])
            duration = durations[i];
        }
        vector < unsigned int >chapters = versification_get_chapters(projectconfig->versification_get(), book);
        duration *= (chapters.size() + 1);
      }
      row.push_back(books_id_to_english(book));
      row.push_back(task);
      day += 1;
      if (percentage_complete)
        row.push_back("Already started");
      else
        row.push_back(date_time_julian_get_month_and_year(day));
      row.push_back(convert_to_string(duration));
      row.push_back(convert_to_string(percentage_complete));
      day += (unsigned int)(round(duration * 7 / 5 * (100 - percentage_complete) / 100));
      if (percentage_complete == 100)
        row.push_back("Already finished");
      else
        row.push_back(date_time_julian_get_month_and_year(day));
      cell_texts.push_back(row);
    }

    htmlwriter.table("Planned tasks", column_headers, cell_texts, "", &centers, 0);
  }

}
