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


#include "utilities.h"
#include "search_utils.h"
#include "bible.h"
#include "gtkwrappers.h"
#include "bibletime.h"
#include <sqlite3.h>
#include "sqlite_reader.h"
#include "gwrappers.h"
#include "gtkwrappers.h"
#include "projectutils.h"
#include "progresswindow.h"
#include "categorize.h"
#include "books.h"
#include "shell.h"
#include "directories.h"
#include "settings.h"
#include "tiny_utilities.h"


void search_string_basic(const ustring & project, bool use_book_selection, unsigned int currentchapter, vector <Reference> &results)
/*
Basic search for a string in the text.
project: project to search.
use_book_selection: whether to limit searches to the selected books only.
chapter: currently opened chapter in the editor.
results: will contain the search results.
*/
{
  // Settings.
  extern Settings *settings;
  // Case sensitive
  bool casesensitive = settings->session.search_case_sensitive;
  // The string to search for. 
  // We need to normalize the search expression when comparing strings.
  ustring localsearchword(settings->session.searchword);
  localsearchword = localsearchword.normalize();
  if (!casesensitive)
    localsearchword = localsearchword.casefold();
  // Book and chapter selection.
  bool search_current_chapter = settings->session.search_current_chapter;
  set <unsigned int> selectedbooks = settings->session.selected_books;
  // Get all books.
  vector <unsigned int> books = project_get_books(project);
  // Progress.
  ProgressWindow progresswindow("Searching", false);
  progresswindow.set_iterate(0, 1, books.size());
  // Go through all books.
  for (unsigned int bk = 0; bk < books.size(); bk++) {
    progresswindow.iterate();
    unsigned int book = books[bk];
    // Handle possible case of book selection.
    if (use_book_selection) {
      if (selectedbooks.find(book) == selectedbooks.end()) {
        continue;
      }
    }
    // Get all chapters and go through them.
    vector <unsigned int> chapters = project_get_chapters(project, book);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      unsigned int chapter = chapters[ch];
      // Do we search this chapter?
      if (search_current_chapter) {
        if (chapter != currentchapter)
          continue;
      }
      // Take a quick peek at the chapter if the word is there.
      ustring chapterfilename = project_data_filename_chapter(project, book, chapter, false);
      gchar *quickcontent;
      bool wordfound = false;
      if (g_file_get_contents(chapterfilename.c_str(), &quickcontent, NULL, NULL)) {
        gchar *foundlocation;
        if (casesensitive) {
          foundlocation = g_strrstr(quickcontent, localsearchword.c_str());
        } else {
          gchar *casefoldedcontent = g_utf8_casefold(quickcontent, -1);
          foundlocation = g_strrstr(casefoldedcontent, localsearchword.c_str());
          if (casefoldedcontent)
            g_free(casefoldedcontent);
        }
        if (foundlocation)
          wordfound = true;
      }
      if (quickcontent)
        g_free(quickcontent);
      if (wordfound) {
        ReadText rt(chapterfilename, true, false);
        CategorizeChapterVerse ccv(rt.lines);
        for (unsigned int i = 0; i < ccv.verse.size(); i++) {
          if (!casesensitive)
            ccv.line[i] = ccv.line[i].casefold();
          if (ccv.line[i].find(localsearchword) != string::npos) {
            Reference reference(book, chapter, ccv.verse[i]);
            results.push_back(reference);
          }
        }
      }
    }
  }
}


ustring search_in_bibledit_assemble_line(const ustring & input, AreaType areatype, bool area_id, bool area_intro, bool area_heading, bool area_chapter, bool area_study, bool area_notes, bool area_xref, bool area_verse)
// Assembles the line of text we have to search through, depending on the area
// selection.
{
  ustring line;
  switch (areatype) {
  case atRaw:
    line = input;
    break;
  case atAll:
    {
      CategorizeLine cl(input);
      line = cl.id;
      line.append(" ");
      line.append(cl.intro);
      line.append(" ");
      line.append(cl.head);
      line.append(" ");
      line.append(cl.chap);
      line.append(" ");
      line.append(cl.study);
      line.append(" ");
      line.append(cl.note);
      line.append(" ");
      line.append(cl.ref);
      line.append(" ");
      line.append(cl.verse);
      break;
    }
  case atSelection:
    {
      CategorizeLine cl(input);
      if (area_id) {
        line.append(cl.id);
        line.append(" ");
      }
      if (area_intro) {
        line.append(cl.intro);
        line.append(" ");
      }
      if (area_heading) {
        line.append(cl.head);
        line.append(" ");
      }
      if (area_chapter) {
        line.append(cl.chap);
        line.append(" ");
      }
      if (area_study) {
        line.append(cl.study);
        line.append(" ");
      }
      if (area_notes) {
        line.append(cl.note);
        line.append(" ");
      }
      if (area_xref) {
        line.append(cl.ref);
        line.append(" ");
      }
      if (area_verse) {
        line.append(cl.verse);
      }
      break;
    }
  }
  return line;
}


bool search_in_bibledit_word_boundaries_match(const ustring & text, const ustring & searchword, bool matchbeginning, bool matchending, bool globbing)
// Does the word boundary matching.
{
  /*
     Deal with matching the start of a word and/or of the end.

     There are four cases here.
     1. Match word start only.
     2. Match word end only.
     3. Both match start end end of a word, which implies "whole word".
     4. No matching at all.

     Boundary resolution is handled by pango_break(). Textual boundaries such 
     as word boundaries and line boundaries are determined for each item.
     In most cases a general algorithm suffices for this process, but in some
     cases a language module will override the generic algorithm with a more
     specific one.
     It seems to be easier programming to use GtkTextIter and GtkTextBuffer,
     rather than pango_break() directly.
   */

  // Whether the word matches.
  bool match = false;

  // Textbuffer for determining word boundaries.
  GtkTextBuffer *textbuffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(textbuffer, text.c_str(), -1);

  // Iterators needed.
  GtkTextIter startiter;
  GtkTextIter enditer;

  // Store segments of text to compare against.
  vector < ustring > segments;

  // Deal with case one: Match word start only.  
  if (matchbeginning && !matchending) {
    // Create a patternword for the glob-style pattern matching.
    ustring patternword = searchword + "*";
    // Collect all strings starting with a word.
    gtk_text_buffer_get_start_iter(textbuffer, &startiter);
    gtk_text_buffer_get_end_iter(textbuffer, &enditer);
    while (gtk_text_iter_forward_word_end(&startiter)) {
      gtk_text_iter_backward_word_start(&startiter);
      segments.push_back(gtk_text_iter_get_text(&startiter, &enditer));
      gtk_text_iter_forward_word_end(&startiter);
    }
    // See whether the word is in it.
    for (unsigned int i2 = 0; i2 < segments.size(); i2++) {
      if (globbing) {
        // Glob-style pattern matching.
        if (g_pattern_match_simple(patternword.c_str(), segments[i2].c_str())) {
          match = true;
          break;
        }
      } else {
        // Straight compare.
        if (segments[i2].find(searchword) == 0) {
          match = true;
          break;
        }
      }
    }
  }
  // Deal with case two: Match word end only.  
  if (!matchbeginning && matchending) {
    // Create a patternword for the glob-style pattern matching.
    ustring patternword = "*" + searchword;
    // Collect all strings ending with a word.
    gtk_text_buffer_get_start_iter(textbuffer, &startiter);
    gtk_text_buffer_get_start_iter(textbuffer, &enditer);
    while (gtk_text_iter_forward_word_end(&enditer)) {
      segments.push_back(gtk_text_iter_get_text(&startiter, &enditer));
    }
    // See whether the word is in it.
    for (unsigned int i2 = 0; i2 < segments.size(); i2++) {
      if (globbing) {
        // Glob-style pattern matching.
        if (g_pattern_match_simple(patternword.c_str(), segments[i2].c_str())) {
          match = true;
          break;
        }
      } else {
        // Straight compare.
        size_t matchposition;
        matchposition = segments[i2].length() - searchword.length();
        // Negative match positions cause a false match. Solve that here.
        matchposition = CLAMP(matchposition, 0, 99999999);
        if (segments[i2].find(searchword) == matchposition) {
          match = true;
          break;
        }
      }
    }
  }
  // Deal with case three: Match both word start and end.  
  // Interpreted as "match whole word".
  if (matchbeginning && matchending) {
    // Create a patternword for the glob-style pattern matching.
    ustring patternword = searchword;
    // Collect all whole words.
    gtk_text_buffer_get_start_iter(textbuffer, &enditer);
    while (gtk_text_iter_forward_word_end(&enditer)) {
      startiter = enditer;
      gtk_text_iter_backward_word_start(&startiter);
      segments.push_back(gtk_text_iter_get_text(&startiter, &enditer));
    }
    // See whether the word is in it.
    for (unsigned int i2 = 0; i2 < segments.size(); i2++) {
      if (globbing) {
        // Glob-style pattern matching.
        if (g_pattern_match_simple(patternword.c_str(), segments[i2].c_str())) {
          match = true;
          break;
        }
      } else {
        // Straight compare.
        if (segments[i2] == searchword) {
          match = true;
          break;
        }
      }
    }
  }
  // Case four: Nothing to test, so set found to true.
  if (!matchbeginning && !matchending)
    match = true;

  // Free memory.
  g_object_unref(textbuffer);

  // Return whether match.
  return match;
}


vector < Reference > search_in_bibledit()
// Advanced searching in Bibledit.
{
  // Configuration / session
  extern Settings *settings;
  // Set some variables in memory for higher speed.
  bool casesensitive = settings->session.search_case_sensitive;
  bool search_current_book = settings->session.search_current_book;
  bool search_current_chapter = settings->session.search_current_chapter;
  bool search_globbing = settings->session.search_globbing;
  bool search_start_word_match = settings->session.search_start_word_match;
  bool search_end_word_match = settings->session.search_end_word_match;
  set < unsigned int >selected_books = settings->session.selected_books;
  AreaType areatype = settings->session.area_type;
  bool area_id = settings->session.area_id;
  bool area_intro = settings->session.area_intro;
  bool area_heading = settings->session.area_heading;
  bool area_chapter = settings->session.area_chapter;
  bool area_study = settings->session.area_study;
  bool area_notes = settings->session.area_notes;
  bool area_xref = settings->session.area_xref;
  bool area_verse = settings->session.area_verse;

  // Progress information.
  ProgressWindow progresswindow("Searching", true);

  // The string to search for. 
  // Note any apostrophies need to be doubled for SQLite.
  // We need to normalize the search expression when comparing strings.
  ustring localsearchword(settings->session.searchword.normalize());
  if (!casesensitive)
    localsearchword = localsearchword.casefold();
  ustring localsearchword2(localsearchword);

  // Storage for references: search results.
  vector < Reference > results;

  // Get our position in the text.
  ustring project = settings->genconfig.project_get();
  ustring book = books_id_to_english(settings->genconfig.book_get());
  unsigned int chapter = convert_to_int(settings->genconfig.chapter_get());

  // Go through each book in the project. Progress information.
  vector < unsigned int >availablebooks = project_get_books(project);
  progresswindow.set_iterate(0, 1, availablebooks.size());
  for (unsigned int bk = 0; bk < availablebooks.size(); bk++) {
    progresswindow.iterate();
    if (progresswindow.cancel) {
      return results;
    }
    // If the book is not to be searched, skip it.
    if (search_current_book)
      if (book != books_id_to_english(availablebooks[bk]))
        continue;
    if (selected_books.find(availablebooks[bk]) == selected_books.end())
      continue;

    try {
      // Go through each chapter in the book.
      vector < unsigned int >chapters = project_get_chapters(project, availablebooks[bk]);
      for (unsigned int ch = 0; ch < chapters.size(); ch++) {

        // Do we search this chapter?
        if (search_current_chapter)
          if (chapter != chapters[ch])
            continue;

        // Read the chapter.
        vector < ustring > lines = project_retrieve_chapter(project, availablebooks[bk], chapters[ch]);
        CategorizeChapterVerse ccv(lines);

        // Go through the verses.
        for (unsigned int i = 0; i < ccv.verse.size(); i++) {

          // Verse number.
          ustring verse = ccv.verse[i];

          // Handle casesensitive and area selection.
          // Assemble text to search through.
          ustring input(ccv.line[i]);
          if (!casesensitive)
            input = input.casefold();
          ustring text = search_in_bibledit_assemble_line(input, areatype, area_id, area_intro, area_heading, area_chapter, area_study, area_notes, area_xref, area_verse);

          // Use glob-style pattern matching or straight match.
          if (search_globbing) {
            ustring patternword = "*" + localsearchword + "*";
            if (!g_pattern_match_simple(patternword.c_str(), text.c_str()))
              continue;
          } else {
            if (text.find(localsearchword) == string::npos)
              continue;
          }

          // Do the word boundary matching.
          if (!search_in_bibledit_word_boundaries_match(text, localsearchword, search_start_word_match, search_end_word_match, search_globbing))
            continue;

          // This verse "passed" all tests: a search result.  
          Reference reference(availablebooks[bk], chapters[ch], verse);
          results.push_back(reference);
        }
      }
    }
    catch(exception & ex) {
      gw_critical(ex.what());
    }
  }

  // Give the results.
  return results;
}


void search_load_references(WindowReferences * references_window, vector <Reference>& searchresults)
/*
This function takes the searchresults from a search, and depending on information
entered in the search dialog, loads this in the reference area, or merges it 
with the references that are already there.
*/
{
  // Session data.  
  extern Settings *settings;

  // Deal with how the current search results interact with the ones in the editor.
  SearchResultsType searchresultstype = settings->session.searchresultstype;
  // If the search page is zero, that means we are on basic search. And that 
  // means we always load the results in the editor, regardless of the setting
  // in the dialog.
  if (settings->session.search_page == 0)
    searchresultstype = sstLoad;
  switch (searchresultstype) {
  case sstLoad:
    {
      // Sort and load the references.
      sort_references(searchresults);
      references_window->set (searchresults, settings->genconfig.project_get(), NULL);
      break;
    }
  case sstAdd:
    {
      // Add the references to the ones already in the editor
      vector <Reference> loaded_references = references_window->get();
      for (unsigned int i = 0; i < searchresults.size(); i++) {
        bool add = true;
        for (unsigned int i2 = 0; i2 < loaded_references.size(); i2++) {
          if (searchresults[i].equals(loaded_references[i2])) {
            add = false;
            break;
          }
        }
        if (add)
          loaded_references.push_back(searchresults[i]);
      }
      searchresults = loaded_references;
      sort_references(searchresults);
      references_window->set (searchresults, settings->genconfig.project_get(), NULL);
      break;
    }
  case sstSubtract:
    {
      // Subtract the references from the ones already in the editor.
      vector <Reference> loaded_references = references_window->get();
      for (unsigned int i = 0; i < searchresults.size(); i++) {
        vector < Reference >::iterator iter(loaded_references.begin());
        for (unsigned int i2 = 0; i2 < loaded_references.size(); i2++) {
          if (searchresults[i].equals(loaded_references[i2])) {
            loaded_references.erase(iter);
            break;
          }
          iter++;
        }
      }
      searchresults = loaded_references;
      sort_references(searchresults);
      references_window->set (searchresults, settings->genconfig.project_get(), NULL);
      break;
    }
  case sstShare:
    {
      // Share the references with the ones already in the editor.
      // This means that the references that are already in the editor
      // and the ones that are the result of this search, will be loaded,
      // provided they are the same.
      // All other ones will be discarded.
      vector <Reference> loaded_references = references_window->get();
      vector <Reference> shared_references;
      for (unsigned int i = 0; i < searchresults.size(); i++) {
        for (unsigned int i2 = 0; i2 < loaded_references.size(); i2++) {
          if (searchresults[i].equals(loaded_references[i2])) {
            shared_references.push_back(searchresults[i]);
            break;
          }
        }
      }
      searchresults = shared_references;
      sort_references(searchresults);
      references_window->set (searchresults, settings->genconfig.project_get(), NULL);
      break;
    }
  }
}


void search_string(WindowReferences * references_window)
{
  // Storage for results;
  vector <Reference> searchresults;

  // Configuration & session.
  extern Settings *settings;

  switch (settings->session.search_page) {
  case 0:
    {
      // Basic search.

      // Save the current book selection in case we modify it.
      set <unsigned int> saved_book_selection = settings->session.selected_books;

      // In case we search in the current book only, modify the book selection.
      if (settings->session.search_current_book) {
        settings->session.selected_books.clear();
        settings->session.selected_books.insert(settings->genconfig.book_get());
      }

      // Search.
      unsigned int chapter = convert_to_int(settings->genconfig.chapter_get());
      search_string_basic(settings->genconfig.project_get(), true, chapter, searchresults);
      sort_references(searchresults);

      // Restore current book selection.
      settings->session.selected_books = saved_book_selection;

      // Done.
      break;
    }
  case 1:
    {
      // Advanced search.
      searchresults = search_in_bibledit();
      break;
    }
  case 2:
    {
      break;
    }
  }
  // Load the references in the editor.
  search_load_references(references_window, searchresults);
}

