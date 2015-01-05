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


#include "libraries.h"
#include "utilities.h"
#include <glib.h>
#include "constants.h"
#include "projectutils.h"
#include "directories.h"
#include "gwrappers.h"
#include "shell.h"
#include "sqlite3.h"
#include "books.h"
#include "bible.h"
#include "progresswindow.h"
#include "import.h"
#include "unixwrappers.h"
#include "statistics.h"
#include "settings.h"
#include "dialoglistview.h"
#include <gdk/gdkkeysyms.h>
#include "git.h"
#include "tiny_utilities.h"
#include "notes_utils.h"
#include "scripts.h"
#include "statistics.h"
#include "snapshots.h"
#include "vcs.h"


void project_store_sanitize_line(ustring & line)
/* 
Joins lines, without starting usfm, to a previous one. 
Makes changes in-place.
*/
{
  // Look for a newline.
  size_t position = line.find("\n");
  while (position != string::npos) {
    position++;
    if (position >= line.size())
      break;
    // If found, and the next character is not a backslash (\), then put a space 
    // instead of the newline.
    if (line.substr(position, 1) != "\\") {
      line.replace(position - 1, 1, " ");
    }
    // Keep going till through.
    position = line.find("\n", position);
  }
}

void project_store_chapter_internal(const ustring & project, unsigned int book, unsigned int chapter, CategorizeChapterVerse & ccv, unsigned int first, unsigned int last, unsigned int timestamp)
{
  // Bail out if the project does not exist.
  if (!project_exists(project))
    return;

  // Store statistics.
  statistics_record_store_chapter(project, book, chapter, ccv, first, last);

  // Store data.
  try {
    ustring chapterdirectory = project_data_directory_chapter(project, book, chapter);
    if (!g_file_test(chapterdirectory.c_str(), G_FILE_TEST_IS_DIR))
      gw_mkdir_with_parents(chapterdirectory);
    ustring filename = project_data_filename_chapter(project, book, chapter, false);
    vector < ustring > data;
    for (unsigned int i = first; i < last; i++) {
      ustring text = ccv.line[i];
      project_store_sanitize_line(text);
      data.push_back(text);
    }
    write_lines(filename, data);
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }

  // Store a snapshot of this chapter.
  snapshots_shoot_chapter (project, book, chapter, 0, false);
}

void projects_initial_check()
// Does some initial checks on the projects we have.
// Upgrades projects of an older version to the current version.
{
  // Ensure that we create separate routines for each Bible, as this routine will need to be called upon restore as well.
}

vector <ustring> projects_get_all()
// Gets sorted list of all projects.
{
  ReadDirectories rd(Directories->get_projects(), "", "");
  sort(rd.directories.begin(), rd.directories.end());
  return rd.directories;
}

ustring project_data_directory_project(const ustring & project)
// Returns the data directory for the project, e.g.:
// ~/.bibledit/projects/testproject/data
{
  return tiny_project_data_directory_project(project);
}

ustring project_data_directory_book(const ustring & project, unsigned int book)
// Returns the data directory for the book, e.g.:
// ~/.bibledit/projects/testproject/data/Genesis
{
  ustring directory = gw_build_filename(project_data_directory_project(project), books_id_to_english(book));
  return directory;
}

ustring project_data_directory_chapter(const ustring & project, unsigned int book, unsigned int chapter)
// Returns the data directory for the chapter, e.g.:
// ~/.bibledit/projects/testproject/data/Genesis/1
{
  ustring directory = gw_build_filename(project_data_directory_book(project, book), convert_to_string(chapter));
  return directory;
}

ustring project_data_filename_chapter(const ustring & project, unsigned int book, unsigned int chapter, bool checkexists)
// Returns the data filename of the chapter, e.g.:
// ~/.bibledit/projects/testproject/data/Genesis/1/data
{
  ustring filename = gw_build_filename(project_data_directory_chapter(project, book, chapter), "data");
  if (checkexists) {
    if (!g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR))
      throw runtime_error(books_id_to_english(book) + " " + convert_to_string(chapter) + " does not exist");
  }
  return filename;
}

bool project_exists(const ustring & project)
{
  return g_file_test(gw_build_filename(Directories->get_projects(), project).c_str(), G_FILE_TEST_IS_DIR);
}


void project_create_restore (const ustring& project, const ustring& restore_from)
{
  if (restore_from.empty()) {
    // Create project directory, and directory for data.
    gw_mkdir_with_parents(project_data_directory_project(project));
  } else {
    // Restore project.
    ustring project_directory = tiny_project_directory (project);
    unix_mv (restore_from, project_directory);
  }
  // Create statistics system.
  statistics_initial_check_project(project, false);
  // Initialize snapshots.
  snapshots_initialize_project (project);
}


void project_delete(const ustring & project)
// Deletes a project and associated data.
{
  // No project: bail out.
  if (project.empty())
    return;
  // Delete the whole project, including all databases and the repository.
  ustring directory = gw_build_filename(Directories->get_projects(), project);
  unix_rmdir(directory);
}

void project_copy(const ustring & project, const ustring & newproject)
// Copies "project" to "newproject.
{
  ustring olddirectory = gw_build_filename(Directories->get_projects(), project);
  ustring newdirectory = gw_build_filename(Directories->get_projects(), newproject);
  unix_cp_r(olddirectory, newdirectory);
  // Statistics have been copied too. Do initial check.
  statistics_initial_check_project(newproject, false);
  // Use of a remote repository is switched off so as not to mess with our server.
  git_revert_to_internal_repository(newproject);
  // Statistics database will have been copied already.
}

void project_move(const ustring & project, const ustring & newproject)
{
  // This moves the whole project, including the databases.
  ustring oldname = (gw_build_filename(Directories->get_projects(), project));
  ustring newname = (gw_build_filename(Directories->get_projects(), newproject));
  unix_mv(oldname, newname);
  // Move project notes.
  notes_change_project(project, newproject);
}

void project_store_book(const ustring & project, unsigned int book, CategorizeChapterVerse & ccv)
{
  // If nothing to save, bail out.
  if (ccv.chapter.empty())
    return;
  // Timestamp.
  unsigned int timestamp = time(0);
  // Make pointers to each chapter.
  vector < unsigned int >chapters;
  vector < unsigned int >firsts;
  vector < unsigned int >lasts;
  unsigned int currentchapter = ccv.chapter[0];
  unsigned int first = 0;
  for (unsigned int i = 0; i < ccv.chapter.size(); i++) {
    if (ccv.chapter[i] != currentchapter) {
      chapters.push_back(currentchapter);
      currentchapter = ccv.chapter[i];
      firsts.push_back(first);
      first = i;
      lasts.push_back(i);
    }
  }
  chapters.push_back(currentchapter);
  firsts.push_back(first);
  lasts.push_back(ccv.chapter.size());
  // Store each chapter.
  for (unsigned int i = 0; i < chapters.size(); i++) {
    project_store_chapter_internal(project, book, chapters[i], ccv, firsts[i], lasts[i], timestamp);
  }
}

void project_remove_book(const ustring & project, unsigned int book)
{
  // Collect the chapter numbers in this book.
  vector <unsigned int> chapters = project_get_chapters (project, book);
  // Store statistics.
  statistics_record_remove_book(project, book);
  // Remove book directory.
  unix_rmdir(project_data_directory_book(project, book));
  // Store snapshots for the chapters we collected earlier.
  for (unsigned int i = 0; i < chapters.size(); i++) {
    snapshots_shoot_chapter (project, book, chapters[i], 0, false);
  }
}

void project_store_chapter(const ustring & project, unsigned int book, CategorizeChapterVerse & ccv)
{
  // If there's nothing to save, bail out.
  if (ccv.chapter.size() == 0)
    return;
  // Do the actual storing.
  unsigned int chapter = ccv.chapter[0];
  unsigned int timestamp = time(0);
  project_store_chapter_internal(project, book, chapter, ccv, 0, ccv.chapter.size(), timestamp);
}

void project_remove_chapter(const ustring & project, unsigned int book, unsigned int chapter)
{
  // Statistics.
  statistics_record_remove_chapter(project, book, chapter);
  // Actual removal.
  ustring directory = project_data_directory_chapter(project, book, chapter);
  unix_rmdir(directory);
  // Store empty snapshot.
  snapshots_shoot_chapter (project, book, chapter, 0, false);
}

void project_store_verse(const ustring & project, unsigned int book, unsigned int chapter, const ustring & verse, const ustring & data)
{
  // If there's nothing to save, bail out.
  if (data.empty())
    return;
  // Statistics.
  statistics_record_store_verse(project, book, chapter, verse, data);
  // Retrieve current data, categorize it, store new verse at right place, store it.
  vector < ustring > lines = project_retrieve_chapter(project, book, chapter);
  CategorizeChapterVerse ccv(lines);
  for (unsigned int i = 0; i < ccv.verse.size(); i++) {
    if (ccv.verse[i] == verse) {
      ustring text(data);
      project_store_sanitize_line(text);
      ccv.line[i] = text;
    }
  }
  unsigned int timestamp = time(0);
  project_store_chapter_internal(project, book, chapter, ccv, 0, ccv.chapter.size(), timestamp);
}

ustring project_retrieve_verse_extended(const ustring & project, unsigned int book, unsigned int chapter, const ustring & verse)
/*
This function will be called when the exact verse was not found in the text.
E.g. it requested verse "1-2", and the text happens to have verse "1" and
verse "2", but not the combined verse. This extended function handles these
and similar cases.
This is roughly the strategy to be followed to get the right verse.
- The requested verse may have the a or b suffix, e.g. verse 4a.
- Therefore use a system of "half" verses.
- Create a vector with all the half verses requested.
- Create sets with all the half verses that are in a particular verse, e.g.
  v 1-2 will contain 1a and 1b, and v 3-5 contains 3a, 3b, 4a, 4b, 5a, 5b.
- Take any set that contains any of the requested half verses and get the 
  text(s) of the verse(s).
*/
{
  // Holds the verse text we will retrieve.
  ustring line;

  // Get the requested half-verses.
  vector < int >requested_verses = verses_encode(verse);

  // Get all verses in this chapter.
  vector < ustring > all_verses = project_get_verses(project, book, chapter);

  // Get the sets with the half-verses of each available verse in this chapter.
  vector < set < int > > available_verses_sets;
  for (unsigned int i = 0; i < all_verses.size(); i++) {
    vector < int >verses_v = verses_encode(all_verses[i]);
    set < int >verses_s(verses_v.begin(), verses_v.end());
    available_verses_sets.push_back(verses_s);
  }

  // Retrieve any verse that relates to the verse we look for.
  // Use a mechanism that prevents retrieving the same verse more than once.
  set < ustring > retrieved_verses;
  for (unsigned int a = 0; a < available_verses_sets.size(); a++) {
    for (unsigned int r = 0; r < requested_verses.size(); r++) {
      if (available_verses_sets[a].find(requested_verses[r]) != available_verses_sets[a].end()) {
        if (retrieved_verses.find(all_verses[a]) == retrieved_verses.end()) {
          retrieved_verses.insert(all_verses[a]);
          if (!line.empty())
            line.append("\n");
          line.append(project_retrieve_verse(project, book, chapter, all_verses[a]));
        }
      }
    }
  }

  // Return whatever we got.
  return line;
}

ustring project_retrieve_verse(const ustring & project, unsigned int book, unsigned int chapter, const ustring & verse)
{
  // Retrieve the verse.
  ustring line;
  vector < ustring > lines = project_retrieve_chapter(project, book, chapter);
  CategorizeChapterVerse ccv(lines);
  for (unsigned int i = 0; i < ccv.verse.size(); i++) {
    if (ccv.verse[i] == verse) {
      line = ccv.line[i];
    }
  }
  // If the verse was not found, use extended retrieval mechanism to get it.
  if (line.empty()) {
    line = project_retrieve_verse_extended(project, book, chapter, verse);
  }
  // Return the data.
  return line;
}

vector < ustring > project_retrieve_chapter(const ustring & project, unsigned int book, unsigned int chapter)
// Get the chapter from disk.
{
  // Get the filename of the chapter to retrieve.
  ustring filename = project_data_filename_chapter(project, book, chapter, false);

  // If the project depends upon another project we need to make sure that we 
  // retrieve the information from that project in case that project was updated.
  // If the file stamp of the source project is newer, then that means it was updated.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project, false);
  if (projectconfig->depending_on_switch_get()) {
    ustring source_project = projectconfig->depending_on_project_get();
    if (!source_project.empty()) {
      ustring script = projectconfig->depending_on_script_get();
      unsigned int myfiledate = file_get_modification_time(filename);
      ustring sourcefilename = project_data_filename_chapter(source_project, book, chapter, false);
      unsigned int sourcefiledate = file_get_modification_time(sourcefilename);
      if (sourcefiledate >= myfiledate) {
        script_filter(script, script == scripts_straight_through(), sourcefilename, script_temporal_output_file());
        ReadText rt(script_temporal_output_file(), true);
        CategorizeChapterVerse ccv(rt.lines);
        project_store_chapter(project, book, ccv);
      }
    }
  }
  // Get the data.
  ReadText rt(filename, true);

  // If the data shows that there is a conflict, solve it, then reload.
  bool solve_conflict = false;
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    if (rt.lines[i].find(git_mine_conflict_marker()) != string::npos) {
      solve_conflict = true;
    }
  }
  if (solve_conflict) {
    git_resolve_conflict_chapter(project, book, chapter);
    ReadText rt2(filename, true);
    return rt2.lines;
  }
  // Return the data.
  return rt.lines;
}

vector < ustring > project_retrieve_book(const ustring & project, unsigned int book)
// Retrieves the text of the whole book.
{
  // Storage for the data.
  vector < ustring > lines;
  // All chapters.
  vector < unsigned int >chapters = project_get_chapters(project, book);
  // Get text of each chapter.
  for (unsigned int i = 0; i < chapters.size(); i++) {
    vector < ustring > ln = project_retrieve_chapter(project, book, chapters[i]);
    for (unsigned int i2 = 0; i2 < ln.size(); i2++) {
      lines.push_back(ln[i2]);
    }
  }
  // Return data.
  return lines;
}

vector < unsigned int >project_get_books(const ustring & project)
// Gives all books in this project.
// Reorders the book according to settings made by user.
{
  vector < unsigned int >books;
  ReadDirectories rd(project_data_directory_project(project), "", "");
  for (unsigned int i = 0; i < rd.directories.size(); i++) {
    unsigned int book = books_english_to_id(rd.directories[i]);
    if (book)
      books.push_back(book);
  }
  books_order(project, books);
  return books;
}

vector < unsigned int >project_get_chapters(const ustring & project, unsigned int book)
// Gives all chapters in a book of a project.
{
  vector < unsigned int >chapters;
  ustring project_data_directory = project_data_directory_book(project, book);
  if (g_file_test(project_data_directory.c_str(), G_FILE_TEST_IS_DIR)) {
    ReadDirectories rd(project_data_directory, "", "");
    for (unsigned int i = 0; i < rd.directories.size(); i++) {
      // It also reads the .svn directory. Filter that out.
      ustring uchapter = number_in_string(rd.directories[i]);
      if (!uchapter.empty()) {
        unsigned int chapter = convert_to_int(rd.directories[i]);
        chapters.push_back(chapter);
      }
    }
    sort(chapters.begin(), chapters.end());
  }
  return chapters;
}

vector < ustring > project_get_verses(const ustring & project, unsigned int book, unsigned int chapter)
// Returns the verses in a chapter.
{
  vector < ustring > verses;
  vector < ustring > lines = project_retrieve_chapter(project, book, chapter);
  CategorizeChapterVerse ccv(lines);
  verses = ccv.verse;
  return verses;
}

bool project_book_exists(const ustring & project, unsigned int book)
{
  ustring directory = project_data_directory_book(project, book);
  return g_file_test(directory.c_str(), G_FILE_TEST_IS_DIR);
}

bool project_select(ustring & newproject)
// Shows the select project dialog. 
// newproject: gets the new project if one was chosen.
// Returns true if a project was chosen.
{
  // Divide the projects into editable and non-editable ones.
  vector < ustring > editable_projects;
  vector < ustring > non_editable_projects;
  {
    vector < ustring > projects = projects_get_all();
    for (unsigned int i = 0; i < projects.size(); i++) {
      extern Settings *settings;
      ProjectConfiguration *projectconfig = settings->projectconfig(projects[i]);
      if (projectconfig->editable_get())
        editable_projects.push_back(projects[i]);
      else
        non_editable_projects.push_back(projects[i]);
    }
  }
  // Build/run dialog.    
  ustring focusedproject = newproject;
  if (focusedproject.empty()) {
    extern Settings *settings;
    focusedproject = settings->genconfig.project_get();
  }
  ListviewDialog dialog("Open project", editable_projects, focusedproject, true, NULL);
  dialog.two_pages("Editable", "Non-editable", non_editable_projects, true);
  dialog.tab_switcher(GDK_KEY_O, GDK_CONTROL_MASK, "Ctrl-O", "to display the editable projects", "to display the non-editable projects");
  if (dialog.run() == GTK_RESPONSE_OK) {
    newproject = dialog.focus;
    return true;
  } else {
    return false;
  }
}
