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
#include "print_parallel_bible.h"
#include "settings.h"
#include "print_parallel_references.h"
#include "gwrappers.h"
#include "gtkwrappers.h"
#include "projectutils.h"
#include "progresswindow.h"
#include "books.h"
#include "portion_utils.h"


void view_parallel_bible_pdf ()
{
  // Log.
  gw_message ("Printing Parallel Bible");

  // Configuration
  extern Settings * settings;
  
  // Get the chapters and check them.
  vector<unsigned int> chapters = project_get_chapters (settings->genconfig.project_get(), settings->genconfig.book_get ());
  if (chapters.empty()) {
    gtkw_dialog_info (NULL, books_id_to_english (settings->genconfig.book_get ()) + "does not exist in this project");
    return;
  }
    
  // Progress system.
  ProgressWindow progresswindow ("Parallel Bible", true);  
  progresswindow.set_text ("Collecting verses");
  progresswindow.set_iterate (0, 1, chapters.size());
  
  // Messages to be printed first.
  vector<ustring> messages;
  
  // All the projects to be put in this parallel Bible.
  // If the book exists in the project, add it, else give message.
  vector<ustring> project_s_raw = settings->genconfig.parallel_bible_projects_get ();
  vector<ustring> project_names;
  for (unsigned int i = 0; i < project_s_raw.size(); i++) {
    if (project_book_exists(project_s_raw[i], settings->genconfig.book_get ())) {
      project_names.push_back(project_s_raw[i]);
    } else {
      messages.push_back ("Project "  + project_s_raw[i] + " was requested to be included, but it does not contain " + books_id_to_english (settings->genconfig.book_get ()) + ". It was left out.");
    }
  }
          
  // References to print.
  vector <Reference> references;

  // Variables for portion selection. todo multiple portions don't work yet.
  bool portion_print = false;
  bool portion_print_next_verse_off = false;
  unsigned int portion_chapter_from, portion_chapter_to;
  ustring portion_verse_from, portion_verse_to;
  {
    vector <unsigned int> portions_chapter_from, portions_chapter_to;
    vector <ustring> portions_verse_from, portions_verse_to;
    select_portion_get_values (settings->genconfig.project_get(), settings->genconfig.book_get (), settings->genconfig.parallel_bible_chapters_verses_get (),
                               portions_chapter_from, portions_verse_from, portions_chapter_to, portions_verse_to);
    portion_chapter_from = portions_chapter_from[0];
    portion_chapter_to = portions_chapter_to[0];
    portion_verse_from = portions_verse_from[0];
    portion_verse_to = portion_verse_to[0];
  }
  // Go through the chapters.
  for (unsigned int ch = 0; ch < chapters.size(); ch++) {
    // Update progress bar.
    progresswindow.iterate();
    // Go through the verse numbers in this chapter.
    vector<ustring> verses = project_get_verses (settings->genconfig.project_get(), settings->genconfig.book_get (), chapters[ch]);
    for (unsigned int vs = 0; vs < verses.size(); vs++) {
      // See whether this chapter.verse is within our portion to print.
      if ((chapters[ch] == portion_chapter_from) && (verses[vs] == portion_verse_from))
        portion_print = true;
      if (portion_print_next_verse_off)
        portion_print = false;
      if ((chapters[ch] == portion_chapter_to) && (verses[vs] == portion_verse_to))
        portion_print_next_verse_off = true;
      if (!portion_print)
        continue;
      // See whether to print verses zero.
      if (!settings->genconfig.parallel_bible_include_verse_zero_get ())
        if (verses[vs] == "0")
          continue;
      // Store the reference.
      Reference reference (settings->genconfig.book_get (), chapters[ch], verses[vs]);
      references.push_back (reference);
    }
  }

  // Hide progeressbar.
  progresswindow.hide ();

  // Do the printing.
  ProjectMemory projectmemory (settings->genconfig.project_get(), true);
  view_parallel_references_pdf (projectmemory, 
                                &project_names, references, 
                                settings->genconfig.parallel_bible_keep_verses_together_get (), 
                                &messages, false);
  
  // Log: ready.
  gw_message ("Ready printing the Parallel Bible");
}
