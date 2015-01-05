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
#include "compareutils.h"
#include "usfmtools.h"
#include "usfm.h"
#include "printproject.h"
#include "projectutils.h"
#include "settings.h"
#include "books.h"
#include "print_parallel_references.h"
#include "tiny_utilities.h"
#include "merge_utils.h"
#include "directories.h"
#include "gwrappers.h"
#include "shell.h"
#include "constants.h"


void compare_with(WindowReferences * references_window, const ustring & project, const ustring & secondproject, bool print_changes_only)
{
  // Load the project, and the second project.
  ProjectMemory projectmemory(project, true);
  if (projectmemory.cancelled)
    return;
  ProjectMemory secondprojectmemory(secondproject, true);
  if (secondprojectmemory.cancelled)
    return;
  // The project to store the comparison.
  ProjectMemory comparedprojectmemory = projectmemory;
  comparedprojectmemory.name = project + " compared with " + secondproject;
  // Do the actual comparison.
  if (!compare_projects(projectmemory, secondprojectmemory, comparedprojectmemory))
    return;
  // Look for added or deleted books / chapters / verses.
  vector <ustring> additions_deletions;
  compare_get_additions_deletions(projectmemory, secondprojectmemory, additions_deletions);
  // See what to print.
  if (print_changes_only) {
    // Print changes only, and load them in the references.
    vector <Reference> references;
    compare_get_changes(comparedprojectmemory, references);
    references_window->set (references, project, NULL);
    comparedprojectmemory.name = project;
    view_parallel_references_pdf(comparedprojectmemory, NULL, references, true, &additions_deletions, true);
  } else {
    // Print whole modified project, plus additions / deletions.
    // No bold, as bold might interfere with the marking of additions, which are in bold too.
    PrintProject printproject(&comparedprojectmemory);
    printproject.portion_project(project);
    for (unsigned int i = 0; i < additions_deletions.size(); i++) {
      printproject.comment(additions_deletions[i]);
    }
    printproject.no_bold();
    printproject.print();
  }
}


bool compare_projects(ProjectMemory & originalproject, ProjectMemory & secondproject, ProjectMemory & outputproject)
// Compares originalproject with secondproject. Differences go in outputproject.
{
  // Progress information.
  ProgressWindow progresswindow("Comparing", true);

  // Open the Scriptures.
  vector < unsigned int >originalscripture_books = originalproject.get_books();
  vector < unsigned int >secondscripture_books = secondproject.get_books();
  vector < unsigned int >outputscripture = outputproject.get_books();

  // Portions.
  ScripturePortions scriptureportions(originalproject.name);

  // Progress information.
  progresswindow.set_iterate(0, 1, originalscripture_books.size());

  // Go through the original scripture.
  for (unsigned int ib = 0; ib < originalscripture_books.size(); ib++) {
    // Progress.
    progresswindow.iterate();

    // Skip book if it is not included in the portion.
    if (!scriptureportions.included(books_id_to_english(originalscripture_books[ib]))) {
      continue;
    }
    // Get a pointer to the book in the second project.
    ProjectBook *secondprojectbook = secondproject.get_book_pointer(originalscripture_books[ib]);
    // If that book does not exists in the second project, skip it.
    if (secondprojectbook == NULL)
      continue;
    // Go through each chapter of the original book.
    vector < unsigned int >originalchapters = originalproject.data[ib].get_chapters();
    for (unsigned int ic = 1; ic < originalchapters.size(); ic++) {
      // Progress info.
      ustring info = books_id_to_english(originalscripture_books[ib]) + " " + convert_to_string(originalchapters[ic]);
      progresswindow.set_text(info);
      if (progresswindow.cancel) {
        return false;
      }
      // Get chapter text of the original.
      vector < ustring > originallines = originalproject.data[ib].data[ic].get_data();
      // Get a pointer to the chapter in the second project.
      ProjectChapter *secondprojectchapter = secondprojectbook->get_chapter_pointer(originalchapters[ic]);
      // Skip if the chapter isn't there.
      if (secondprojectchapter == NULL)
        continue;
      // Get chapter text of the second.
      vector < ustring > secondlines = secondprojectchapter->get_data();
      // If both chapters are the same, continue to the next one.
      ustring originalbook_chapter_contents;
      for (unsigned int i = 0; i < originallines.size(); i++)
        originalbook_chapter_contents.append(originallines[i]);
      ustring secondbook_chapter_contents;
      for (unsigned int i = 0; i < secondlines.size(); i++)
        secondbook_chapter_contents.append(secondlines[i]);
      if (originalbook_chapter_contents == secondbook_chapter_contents)
        continue;
      // At this stage the chapters are different. Compare them more thoroughly.
      ProjectChapter *outputprojectchapter = &outputproject.data[ib].data[ic];
      vector < ustring > outputlines = outputprojectchapter->get_data();
      compare_usfm_text(originallines, secondlines, outputlines, false);
      outputprojectchapter->set_data(outputlines);
    }
  }
  // Done: ok.
  return true;
}


void compare_usfm_text_remove_notes (ustring& text)
{
  extern Settings * settings;
  bool disregard_notes = settings->genconfig.compare_disregard_notes_get();
  if (disregard_notes) {
    replace_text_between (text, "\\f ", "\\f*", "");    
    replace_text_between (text, "\\fe ", "\\fe*", "");    
    replace_text_between (text, "\\x ", "\\x*", "");
  }
}


void compare_get_changes(ProjectMemory & project, vector < Reference > &changed_references)
// This produces all changed references in a project.
{
  // Go through the whole scripture.
  vector < unsigned int >scripture_books = project.get_books();
  for (unsigned int i = 0; i < scripture_books.size(); i++) {
    vector < unsigned int >chapters = project.data[i].get_chapters();
    for (unsigned int chapter = 1; chapter < chapters.size(); chapter++) {
      vector < ustring > lines = project.data[i].data[chapter].get_data();
      ustring currentverse = "1";
      Reference last_reference_stored(0);
      for (unsigned int i2 = 0; i2 < lines.size(); i2++) {
        ustring line(lines[i2]);
        // Extract the verse number.
        ustring marker;
        marker = usfm_extract_marker(line);
        if (usfm_is_verse(marker)) {
          currentverse = number_in_string(line);
        }
        // See if this line contains a change.
        bool add_this_line = false;
        if (line.find(DELETION_FLAG) != string::npos)
          add_this_line = true;
        if (line.find(INSERTION_FLAG) != string::npos)
          add_this_line = true;
        if (add_this_line) {
          Reference ref(scripture_books[i], chapter, currentverse);
          if (!ref.equals(last_reference_stored)) {
            changed_references.push_back(ref);
            last_reference_stored = ref;
          }
        }
      }
    }
  }
}


void compare_get_additions_deletions_verses(ProjectMemory & originalproject, ProjectMemory & secondproject, unsigned int book, unsigned int chapter, vector < ustring > &results)
{
  // Get the verses of the chapter of the book of the two projects.
  vector < ustring > originalverses;
  ProjectBook *originalbook = originalproject.get_book_pointer(book);
  if (originalbook) {
    ProjectChapter *originalchapter = originalbook->get_chapter_pointer(chapter);
    if (originalchapter) {
      originalverses = originalchapter->get_verses();
    }
  }
  vector < ustring > secondverses;
  ProjectBook *secondbook = secondproject.get_book_pointer(book);
  if (secondbook) {
    ProjectChapter *secondchapter = secondbook->get_chapter_pointer(chapter);
    if (secondchapter) {
      secondverses = secondchapter->get_verses();
    }
  }
  set < ustring > originalverses_set(originalverses.begin(), originalverses.end());
  set < ustring > secondverses_set(secondverses.begin(), secondverses.end());

  // Go through the original verses to see which were added.
  for (unsigned int b = 0; b < originalverses.size(); b++) {
    if (secondverses_set.find(originalverses[b]) == secondverses_set.end()) {
      results.push_back("Added: " + books_id_to_english(book) + " " + convert_to_string(chapter) + ":" + originalverses[b]);
    }
  }

  // Go through the second verses to see which were deleted.
  for (unsigned int b = 0; b < secondverses.size(); b++) {
    if (originalverses_set.find(secondverses[b]) == originalverses_set.end()) {
      results.push_back("Deleted: " + books_id_to_english(book) + " " + convert_to_string(chapter) + ":" + secondverses[b]);
    }
  }
}


void compare_get_additions_deletions_chapters(ProjectMemory & originalproject, ProjectMemory & secondproject, unsigned int book, vector < ustring > &results)
{
  // Get the chapters of the book of the two projects.
  vector < unsigned int >originalchapters;
  ProjectBook *originalbook = originalproject.get_book_pointer(book);
  if (originalbook)
    originalchapters = originalbook->get_chapters();
  vector < unsigned int >secondchapters;
  ProjectBook *secondbook = secondproject.get_book_pointer(book);
  if (secondbook)
    secondchapters = secondbook->get_chapters();
  set < unsigned int >originalchapters_set(originalchapters.begin(), originalchapters.end());
  set < unsigned int >secondchapters_set(secondchapters.begin(), secondchapters.end());

  // Go through the original chapters to see which chapters were added.
  // Chapters that are there will be checked on their verses.
  for (unsigned int b = 0; b < originalchapters.size(); b++) {
    if (secondchapters_set.find(originalchapters[b]) != secondchapters_set.end()) {
      compare_get_additions_deletions_verses(originalproject, secondproject, book, originalchapters[b], results);
    } else {
      results.push_back("Added: " + books_id_to_english(book) + " " + convert_to_string(originalchapters[b]));
    }
  }

  // Go through the second chapters to see which chapters were deleted.
  // Chapters that are there will be checked on their verses.
  for (unsigned int b = 0; b < secondchapters.size(); b++) {
    if (originalchapters_set.find(secondchapters[b]) != originalchapters_set.end()) {
      compare_get_additions_deletions_verses(originalproject, secondproject, book, secondchapters[b], results);
    } else {
      results.push_back("Deleted: " + books_id_to_english(book) + " " + convert_to_string(secondchapters[b]));
    }
  }
}


void compare_get_additions_deletions(ProjectMemory & originalproject, ProjectMemory & secondproject, vector < ustring > &results)
/*
Compares the projects, and finds if books, chapters or verses were added or 
deleted. If anything was found, it puts human readable text in "results".
*/
{
  // Get the books of the two projects.
  vector < unsigned int >originalbooks = originalproject.get_books();
  vector < unsigned int >secondbooks = secondproject.get_books();
  set < unsigned int >originalbooks_set(originalbooks.begin(), originalbooks.end());
  set < unsigned int >secondbooks_set(secondbooks.begin(), secondbooks.end());

  // Progress information.
  ProgressWindow progresswindow("Finding additions and deletions", false);
  progresswindow.set_iterate(0, 1, originalbooks.size() + secondbooks.size());

  // Go through the original books to see which books were added.
  // Books that are there will be checked on their chapters.
  for (unsigned int b = 0; b < originalbooks.size(); b++) {
    progresswindow.iterate();
    if (secondbooks_set.find(originalbooks[b]) != secondbooks_set.end()) {
      compare_get_additions_deletions_chapters(originalproject, secondproject, originalbooks[b], results);
    } else {
      results.push_back("Added: " + books_id_to_english(originalbooks[b]));
    }
  }

  // Go through the second books to see which books were deleted.
  // Books that are there will be checked on their chapters.
  for (unsigned int b = 0; b < secondbooks.size(); b++) {
    progresswindow.iterate();
    if (originalbooks_set.find(secondbooks[b]) != originalbooks_set.end()) {
      compare_get_additions_deletions_chapters(originalproject, secondproject, secondbooks[b], results);
    } else {
      results.push_back("Deleted: " + books_id_to_english(secondbooks[b]));
    }
  }

  // Remove double ones.
  set < ustring > results_set(results.begin(), results.end());
  results.assign(results_set.begin(), results_set.end());
}


void compare_insert_marked_text (ustring& line, const ustring& text, bool addition)
// Insert marked text into the comparison line.
// line: where to insert the text into.
// text: text to insert.
// addition: whether this is flagged as an addition.
{
  for (unsigned int i = 0; i < text.length(); i++) {
    if (addition)
      line.append (INSERTION_FLAG);
    else
      line.append (DELETION_FLAG);
    line.append (text.substr (i, 1));
  }
}


ustring disassemble_usfm_text_for_comparison (vector <ustring>& text)
// Disassembles USFM text making it ready for comparison.
{
  ustring data;
  for (unsigned int i = 0; i < text.size(); i++) {
    ustring line = text[i];
    compare_usfm_text_remove_notes (line);
    while (!line.empty()) {
      // Normally we have one character per line.
      // Thus the diff command will compare at character level.
      ustring character = line.substr (0, 1);
      line.erase (0, 1);
      data.append (character);
      // USFM markers go completely on one line, so that the diff command compares complete USFM markers.
      if (character == "\\") {
        size_t pos = line.find (" ");
        if (pos != string::npos) {
          character = line.substr (0, pos);
          line.erase (0, pos);
          data.append (character);
        } else {
          data.append (line);
          line.clear();
        }
      }
      data.append ("\n");
    }
    data.append ("linefeed\n");
  }
  return data;
}


void compare_usfm_text (vector<ustring>& original, vector<ustring>& edit, vector<ustring>& output, bool mark_new_line)
// Compares the "original" usfm text with the "edit"ed usfm text.
// Result goes into "output".
// mark_new_line: whether to mark differences in places where a new line starts.
{
  // Store the original to disk.
  ustring original_line = disassemble_usfm_text_for_comparison (original);
  ustring originalfile = gw_build_filename (Directories->get_temp(), "compare-original");
  g_file_set_contents (originalfile.c_str(), original_line.c_str(), -1, NULL);
  
  // Store the edit to disk.
  ustring edit_line = disassemble_usfm_text_for_comparison (edit);
  ustring editfile = gw_build_filename (Directories->get_temp(), "compare-edit");
  g_file_set_contents (editfile.c_str(), edit_line.c_str(), -1, NULL);
  
  // The output file.
  ustring outputfile = gw_build_filename (Directories->get_temp(), "compare-output");

  // Clear output.
  output.clear ();
  
  // Run diff.  
  ustring command = "diff -U 1000000" + shell_quote_space(originalfile) + shell_quote_space(editfile) + ">" + shell_quote_space(outputfile);
  if (system(command.c_str()));
  // GwSpawn needs a change before it can be used, because ::standardout trims the text.
 
  // Read the raw output.
  ReadText rt (outputfile, true, false);

  // If there's no output, there were no differences. Handle that.
  if (rt.lines.size() < 2) {
    output = original;
  }
  
  // Go through the output, skipping the first lines as these are irrelevant.
  ustring output_line;
  for (unsigned int i = 3; i < rt.lines.size(); i++) {
    ustring line = rt.lines[i];
    if (!line.empty()) {
      // Two special characters mark insertions and deletions, 
      // inserted right before the character it applies to. 
      ustring first_character = line.substr (0, 1);
      bool is_insertion = (first_character == "+");
      bool is_deletion = (first_character == "-");
      line.erase (0, 1);
      bool is_marker = (line.find ("\\") == 0);
      bool is_new_line = (line == "linefeed");
      
      // Deal with a marker.
      if (is_marker) {
        // The marker that is inserted becomes the new marker,
        // and the one deleted is removed and a message about that is inserted.
        if (!is_deletion)
          output_line.append (line);
        if (is_insertion || is_deletion) {
          output_line.append (" ");
          line.erase (0, 1);
          line.insert (0, "Marker ");
          compare_insert_marked_text (output_line, line, is_insertion);
        }
        line.clear();
      } 
      
      // Deal with a new line.
      else if (is_new_line) {
        if (is_insertion || is_deletion) {
          if (mark_new_line) {
            compare_insert_marked_text (output_line, "New line", is_insertion);
          }
        }
        line.clear();
      } 
      
      // Deal with other cases.
      else {
        if (is_insertion) {
          output_line.append (INSERTION_FLAG);
        }
        if (is_deletion) {
          output_line.append (DELETION_FLAG);
        }
      }

      if (is_new_line) {
        output.push_back (output_line);
        output_line.clear();
      } else {
        output_line.append (line);
      }
      
    }
  }
  if (!output_line.empty()) {
    output.push_back (output_line);
    output_line.clear();
  }
  
}

