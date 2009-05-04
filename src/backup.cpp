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

#include "backup.h"
#include "settings.h"
#include "settings.h"
#include "gwrappers.h"
#include "date_time_utils.h"
#include "statistics.h"
#include "utilities.h"
#include "projectutils.h"
#include "books.h"
#include "unixwrappers.h"
#include "directories.h"
#include "shell.h"
#include "gtkwrappers.h"
#include "htmlbrowser.h"
#include "tiny_utilities.h"
#include "snapshots.h"
#include "export_utils.h"

void backup_make_incremental()
{
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  backup_make(settings->genconfig.project_get(), false, projectconfig->backup_incremental_last_time_get());
  projectconfig->backup_incremental_last_time_set(date_time_seconds_get_current());
}

void backup_make_flexible()
{
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  backup_make(settings->genconfig.project_get(), projectconfig->backup_full_get(), projectconfig->backup_incremental_last_time_get());
  if (!projectconfig->backup_full_get())
    projectconfig->backup_incremental_last_time_set(date_time_seconds_get_current());
}

void backup_make(const ustring & project, bool full, int timefrom)
{
  // Working directory and zipped filename.
  ustring workingdirectory = gw_build_filename(directories_get_temp(), "backup");
  gw_mkdir_with_parents(workingdirectory);
  int currentseconds = date_time_seconds_get_current();
  int currentdays = date_time_julian_day_get_current();
  guint currentyear, currentmonth, currentday;
  int currenthour, currentminute, currentsecond;
  date_time_normal_get_year_month_day(currentdays, currentyear, currentmonth, currentday);
  date_time_normal_get_hour_minute_second(currentseconds, currenthour, currentminute, currentsecond);
  ustring currentsuffix = "-" + convert_to_string(currentyear) + "-" + convert_to_string(currentmonth) + "-" + convert_to_string(currentday) + "-" + convert_to_string(currenthour) + "-" + convert_to_string(currentminute) + "-" + convert_to_string(currentsecond);
  ustring filename;
  if (full) {
    filename = "full-backup" + currentsuffix;
  } else {
    guint fromyear, frommonth, fromday;
    int fromhour, fromminute, fromsecond;
    date_time_normal_get_year_month_day(date_time_seconds_to_julian(timefrom), fromyear, frommonth, fromday);
    date_time_normal_get_hour_minute_second(timefrom, fromhour, fromminute, fromsecond);
    ustring fromsuffix = "-from-" + convert_to_string(fromyear) + "-" + convert_to_string(frommonth) + "-" + convert_to_string(fromday) + "-" + convert_to_string(fromhour) + "-" + convert_to_string(fromminute) + "-" + convert_to_string(fromsecond);
    filename = "incremental-backup" + fromsuffix + "-to" + currentsuffix;
  }
  filename.append("-of-project-");
  filename.append(project);
  ustring username = g_get_real_name ();
  replace_text (username, " ", "-");
  filename.append ("-backed-up-by-");
  filename.append (username);
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (project);
  ustring comment = projectconfig->backup_comment_get();
  if (!comment.empty()) {
    filename.append("-comment-");
    replace_text (comment, " ", "-");
    filename.append (comment);
  }
  filename.append(".zip");
  replace_text(filename, " ", "_");
  filename = gw_build_filename(workingdirectory, filename);

  if (full) {
    // Handle full backup first.
    export_to_usfm(project, filename, true);
  } else {
    // Handle incremental backups.
    // Get the books/chapters changed since a given time.
    // Get them from git, because git also records changes made by others.
    // Locally produced tracking mechanisms would not so easily 
    // provide that information.
    vector < unsigned int >books;
    vector < unsigned int >chapters;
    snapshots_get_chapters_changed_since(project, timefrom, books, chapters);
    if (books.empty()) {
      gtkw_dialog_info(NULL, "There was nothing to backup");
      return;
    }
    quick_sort(books, chapters, 0, books.size());
    // Storage for the data of the chapters in each book.
    map < unsigned int, vector < ustring > >lines;
    // Always include chapter 0 as that has the id line.
    set < unsigned int >books_chapter_zero_done;
    for (unsigned int i = 0; i < books.size(); i++) {
      if (books_chapter_zero_done.find(books[i]) == books_chapter_zero_done.end()) {
        vector < ustring > data = project_retrieve_chapter(project, books[i], 0);
        for (unsigned int i2 = 0; i2 < data.size(); i2++) {
          lines[books[i]].push_back(data[i2]);
        }
        books_chapter_zero_done.insert(books[i]);
      }
    }
    // Go through the books/chapters with changes, and add the text to the map.
    // Skip chapter 0. Has been done already.
    for (unsigned int i = 0; i < books.size(); i++) {
      if (chapters[i] != 0) {
        vector < ustring > data = project_retrieve_chapter(project, books[i], chapters[i]);
        for (unsigned int i2 = 0; i2 < data.size(); i2++) {
          lines[books[i]].push_back(data[i2]);
        }
      }
    }
    // Go through all books, see which ones have data, save to file.
    vector < unsigned int >all_books = books_type_to_ids(btUnknown);
    for (unsigned int i = 0; i < all_books.size(); i++) {
      if (!lines[books[i]].empty()) {
        ustring usfmfilename = gw_build_filename(workingdirectory, books_id_to_english(books[i]) + ".usfm");
        write_lines(usfmfilename, lines[books[i]]);
      }
    }
    // Zip them.
#ifdef WIN32
    ustring command = "cd" + shell_quote_space(workingdirectory) + " && zip -r " + gw_path_get_basename(filename) + " *.usfm && del *.usfm";
#else
    ustring command = "cd" + shell_quote_space(workingdirectory) + "; zip -r " + gw_path_get_basename(filename) + " *.usfm; rm *.usfm";
#endif
    if (system(command.c_str())) ;
  }

  // Open web page with information: backup_is_ready.html.
  ustring filesize = convert_to_string(file_get_size(filename));
  ReadText rt(gw_build_filename(directories_get_package_data(), "backup_is_ready.html"), true, false);
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    replace_text(rt.lines[i], "100", filesize);
    replace_text(rt.lines[i], "location", filename);
    replace_text(rt.lines[i], "tempdir", workingdirectory);
  }
  ustring htmlfile = gw_build_filename(workingdirectory, "backup.html");
  write_lines(htmlfile, rt.lines);
  unix_cp(gw_build_filename(directories_get_package_data(), "bibledit.css"), gw_build_filename(workingdirectory, "bibledit.css"));
  htmlbrowser(htmlfile, false);
}

