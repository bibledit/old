/*
** Copyright (©) 2003-2008 Teus Benschop.
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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

enum Markup { StrikeThrough, Bold };

void compare_chapter_internal(const vector < ustring > &originalusfms, const vector < ustring > &originaltext, const vector < ustring > &secondusfms, const vector < ustring > &secondtext, vector < ustring > &outputchapter, bool optimize, unsigned int look_forward_lines, int &number_of_changes);
ustring insert_markup_in_line(const ustring & usfm, const ustring & text, Markup markup);
int compare_word_by_word(const ustring & newline, const ustring & oldline, ustring & outputline);
void compare_word_by_word_internal(ustring newline, ustring oldline, ustring & outputline, bool optimize, int look_forward_words, unsigned int &number_of_changes);

void compare_with(References * referencesgui, const ustring & project, const ustring & secondproject, bool print_changes_only)
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
  vector < ustring > additions_deletions;
  compare_get_additions_deletions(projectmemory, secondprojectmemory, additions_deletions);
  // See what to print.
  if (print_changes_only) {
    // Print changes only, and load them in the references.
    vector < Reference > references;
    compare_get_changes(comparedprojectmemory, references);
    referencesgui->set_references(references);
    extern Settings *settings;
    referencesgui->fill_store(settings->projectconfig(project)->language_get());
    comparedprojectmemory.name = project;
    view_parallel_references_pdf(comparedprojectmemory, NULL, references, true, &additions_deletions, true);
  } else {
    // Print whole modified project, plus additions / deletions.
    PrintProject printproject(&comparedprojectmemory);
    printproject.portion_project(project);
    for (unsigned int i = 0; i < additions_deletions.size(); i++) {
      printproject.comment(additions_deletions[i]);
    }
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
      compare_chapter(originallines, secondlines, outputlines);
      outputprojectchapter->set_data(outputlines);
    }
  }
  // Done: ok.
  return true;
}

void compare_chapter_remove_notes(ustring & text)
{
  replace_text_between(text, "\\f ", "\\f*", "");
  replace_text_between(text, "\\fe ", "\\fe*", "");
  replace_text_between(text, "\\x ", "\\x*", "");
}

void compare_chapter(vector < ustring > &originalchapter, vector < ustring > &secondchapter, vector < ustring > &outputchapter)
/*
Compares original chapter with second chapter, and outputs the differences
in output chapter.
It is assumed that the chapters differ.
*/
{
  // Split all text into usfms and text. Do it once, and speed up things.
  // E.g. "\v 1 In the beginning ..." is split into:
  // - \v 1 
  // - In the beginning ...
  // At the same time remove the text of the foot- and endnotes and the xrefs.
  vector < ustring > originalusfms;
  vector < ustring > originaltext;
  for (unsigned int i = 0; i < originalchapter.size(); i++) {
    ustring marker;
    ustring text;
    split_line_into_usfm_and_text(originalchapter[i], marker, text);
    originalusfms.push_back(marker);
    compare_chapter_remove_notes(text);
    originaltext.push_back(text);
  }
  vector < ustring > secondusfms;
  vector < ustring > secondtext;
  for (unsigned int i = 0; i < secondchapter.size(); i++) {
    ustring marker;
    ustring text;
    split_line_into_usfm_and_text(secondchapter[i], marker, text);
    secondusfms.push_back(marker);
    compare_chapter_remove_notes(text);
    secondtext.push_back(text);
  }
  // Find out how many lines the comparison needs to look forward to obtain best results.
  int number_of_changes;
  int lowest_number_of_changes = INT_MAX;
  int look_forward_length = 0;
  for (unsigned int i = 0; i < 6; i++) {
    compare_chapter_internal(originalusfms, originaltext, secondusfms, secondtext, outputchapter, true, i, number_of_changes);
    if (number_of_changes < lowest_number_of_changes) {
      look_forward_length = i;
      lowest_number_of_changes = number_of_changes;
    }
  }
  // Using the optimised parameter, make the comparison.
  outputchapter.clear();
  compare_chapter_internal(originalusfms, originaltext, secondusfms, secondtext, outputchapter, false, look_forward_length, number_of_changes);
}

void compare_chapter_internal(const vector < ustring > &originalusfms, const vector < ustring > &originaltext, const vector < ustring > &secondusfms, const vector < ustring > &secondtext, vector < ustring > &outputchapter, bool optimize, unsigned int look_forward_lines, int &number_of_changes)
/*
This one is used by "compare_chapter", but it is used in the optimization 
process also, to find out which algorithm detects the lowest number of changes.
*/
{
  // Variables and initialization of them.
  number_of_changes = 0;
  // Save the second chapter to a temporal variable.
  vector < ustring > usfm2(secondusfms.begin(), secondusfms.end());
  vector < ustring > text2(secondtext.begin(), secondtext.end());
  // Go through all the original lines.
  for (unsigned int c1c = 0; c1c < originalusfms.size(); c1c++) {
    // Still text left in second chapter?
    if (usfm2.size() > 0) {
      if (originalusfms[c1c].find(usfm2[0]) != string::npos) {
        // Usfm found at right position: check this verse word by word.
        if (!optimize) {
          ustring outputline;
          number_of_changes += compare_word_by_word(originaltext[c1c], text2[0], outputline);
          outputchapter.push_back(originalusfms[c1c] + " " + outputline);
        }
        // Remove this line as we're through with it.
        usfm2.erase(usfm2.begin());
        text2.erase(text2.begin());
      } else {
        bool sfm_found = false;
        int i = -1;
        // Check max n lines ahead. This variable is optimized for best results.
        for (unsigned int fc = 0; fc <= look_forward_lines; fc++) {
          if ((!sfm_found) && (fc < usfm2.size())) {
            if (originalusfms[c1c].find(usfm2[fc]) != string::npos) {
              sfm_found = true;
              i = fc;
            }
          }
        }
        if (i >= 0) {
          // Old text has extra lines, add them with strikeout.
          for (int fc = 0; fc < i; fc++) {
            number_of_changes++;
            if (!optimize) {
              outputchapter.push_back(insert_markup_in_line(usfm2[0], text2[0], StrikeThrough));
            }
            usfm2.erase(usfm2.begin());
            text2.erase(text2.begin());
          }
          // After adding extra lines, add the current in normal text.
          if (!optimize) {
            ustring outputline;
            number_of_changes += compare_word_by_word(originaltext[c1c], text2[0], outputline);
            outputchapter.push_back(originalusfms[c1c] + " " + outputline);
          }
          usfm2.erase(usfm2.begin());
          text2.erase(text2.begin());
        } else if (i < 0) {
          // Usfm not found, add new text with bold.
          number_of_changes++;
          if (!optimize) {
            outputchapter.push_back(insert_markup_in_line(originalusfms[c1c], originaltext[c1c], Bold));
          }
        }
      }
    } else {
      // Old text is finished, add remaining line with bold.
      number_of_changes++;
      if (!optimize) {
        outputchapter.push_back(insert_markup_in_line(originalusfms[c1c], originaltext[c1c], Bold));
      }
    }
  }
  // After comparison, what is left in the old text, add it with strikeout.
  for (unsigned int i = 0; i < usfm2.size(); i++) {
    number_of_changes++;
    if (!optimize) {
      outputchapter.push_back(insert_markup_in_line(usfm2[i], text2[i], StrikeThrough));
    }
  }
}

ustring insert_markup_in_line(const ustring & usfm, const ustring & text, Markup markup)
/*
// The tags to format text to be shown in Strike-Through are <strike> and </strike>.
// We do not yet insert them here, because the html formatter will 
// transfer this text converted into the output, so that <strike> shows as
// <strike> in the browser.
// Instead placeholders are inserted, which will be converted to the right
// tags in the printing engine.
The markup for strike-through or bold cannot just be inserted in a line like
so:
MARKUP_BEGIN\v 1 In the beginning ... earth.MARKUP_END
This would disturb the processing of the USFMs.
Instead it should be inserted so:
\v 1 MARKUP_BEGINIn the beginning ... earth.MARKUP_END
This function cares for that.
*/
{
  string markupbegin;
  string markupend;
  if (markup == StrikeThrough) {
    markupbegin = usfm_get_full_opening_marker(DELETION_MARKER);
    markupend = usfm_get_full_closing_marker(DELETION_MARKER);
  } else if (markup == Bold) {
    markupbegin = usfm_get_full_opening_marker(INSERTION_MARKER);
    markupend = usfm_get_full_closing_marker(INSERTION_MARKER);
  }
  ustring s;
  if (!text.empty())
    s = usfm + " " + markupbegin + text + markupend;
  else
    s = usfm;
  return s;
}

int compare_word_by_word(const ustring & newline, const ustring & oldline, ustring & outputline)
{
  // Find out how many words the comparison needs to look forward
  // to obtain best results.
  unsigned int lowest_number_of_changes = INT_MAX;
  unsigned int look_forward_length = 0;
  unsigned int number_of_changes = 0;
  for (unsigned int i = 0; i <= 10; i++) {
    compare_word_by_word_internal(newline, oldline, outputline, true, 3 * i, number_of_changes);
    if (number_of_changes < lowest_number_of_changes) {
      look_forward_length = 3 * i;
      lowest_number_of_changes = number_of_changes;
    }
  }
  // Using the optimised parameter, make the comparison.
  number_of_changes = 0;
  compare_word_by_word_internal(newline, oldline, outputline, false, look_forward_length, number_of_changes);
  return number_of_changes;
}

void compare_word_by_word_internal(ustring newline, ustring oldline, ustring & outputline, bool optimize, int look_forward_words, unsigned int &number_of_changes)
{
  // Initialize.
  number_of_changes = 0;
  // Fill "words1" with the words from the new line.
  // Spaces disturb the comparison process: leave them out.
  vector < ustring > words1;
  Parse parse1(newline, false);
  words1 = parse1.words;
  // Fill "words2" with the words from the old line.
  // Spaces disturb the comparison process: leave them out.
  vector < ustring > words2;
  Parse parse2(oldline, false);
  words2 = parse2.words;
  // Go through all the words, and compare them.
  for (unsigned int w1c = 0; w1c < words1.size(); w1c++) {
    ustring newword(words1[w1c]);
    // Spaces are not dealt with in the comparison, because they only mix it up.
    // But they are put back here in the output line.
    if (!outputline.empty())
      if (!optimize)
        outputline.append(" ");
    // See whether a word can be found that is the same, in the second text.
    if (words2.size() > 0) {
      // Look for that word. Look forward only "look_forward_words" positions,
      // or less, depending on how many words are still left in the line.
      int index = -1;
      unsigned int highlimit = look_forward_words;
      highlimit = CLAMP(highlimit, 0, words2.size());
      for (unsigned int i = 0; i < highlimit; i++) {
        if (newword == words2[i]) {
          index = i;
          break;
        }
      }
      if (index == 0) {
        // Word found at right position: add it to the output.
        if (!optimize)
          outputline.append(newword);
        words2.erase(words2.begin());
      } else if (index > 0) {
        // Equal word is near.
        // Old sentence has extra words, add them with strikeout.
        for (int fc = 0; fc < index; fc++) {
          number_of_changes++;
          if (!optimize)
            outputline.append(usfm_get_full_opening_marker(DELETION_MARKER) + words2[0] + usfm_get_full_closing_marker(DELETION_MARKER) + " ");
          words2.erase(words2.begin());
        }
        // After adding extra lines, add the current in normal text.
        if (!optimize)
          outputline.append(newword);
        words2.erase(words2.begin());
      } else if (index < 0) {
        // Word not found near current position, add new words with bold.
        number_of_changes++;
        if (!optimize)
          outputline.append(usfm_get_full_opening_marker(INSERTION_MARKER) + newword + usfm_get_full_closing_marker(INSERTION_MARKER));
      }
    } else {
      // Old text is finished, add remaining new words with bold.
      number_of_changes++;
      if (!optimize)
        outputline.append(usfm_get_full_opening_marker(INSERTION_MARKER) + newword + usfm_get_full_closing_marker(INSERTION_MARKER));
    }
  }
  // After comparison, what is left in the old text, add it with strikeout.
  for (unsigned int i = 0; i < words2.size(); i++) {
    number_of_changes++;
    if (!optimize) {
      ustring s;
      s.append(" ");
      s.append(usfm_get_full_opening_marker(DELETION_MARKER));
      s.append(words2[i]);
      s.append(usfm_get_full_closing_marker(DELETION_MARKER));
      outputline.append(s);
    }
  }
  if (!optimize) {
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
        if (line.find(usfm_get_full_opening_marker(DELETION_MARKER)) != string::npos) {
          add_this_line = true;
        } else if (line.find(usfm_get_full_closing_marker(DELETION_MARKER)) != string::npos) {
          add_this_line = true;
        } else if (line.find(usfm_get_full_opening_marker(INSERTION_MARKER)) != string::npos) {
          add_this_line = true;
        } else if (line.find(usfm_get_full_closing_marker(INSERTION_MARKER)) != string::npos) {
          add_this_line = true;
        }
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
