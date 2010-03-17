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

void view_parallel_bible_pdf()
{
  // Log.
  gw_message("Printing Parallel Bible");

  // Configuration
  extern Settings *settings;

  // Get the chapters and check them.
  vector < unsigned int >chapters = project_get_chapters(settings->genconfig.project_get(), settings->genconfig.book_get());
  if (chapters.empty()) {
    gtkw_dialog_info(NULL, books_id_to_english(settings->genconfig.book_get()) + "does not exist in this project");
    return;
  }
  // Progress system.
  ProgressWindow progresswindow("Parallel Bible", true);
  progresswindow.set_text("Collecting verses");
  progresswindow.set_iterate(0, 1, chapters.size());

  // Messages to be printed first.
  vector < ustring > messages;

  // All the projects to be put in this parallel Bible.
  // If the book exists in the project, add it, else give message.
  vector <ustring> project_s_raw;
  {
    vector <ustring> bibles = settings->genconfig.parallel_bible_projects_get();
    vector <bool> enabled = settings->genconfig.parallel_bible_enabled_get();
    if (bibles.size () == enabled.size()) {
      for (unsigned int i = 0; i < enabled.size(); i++) {
        if (enabled[i]) {
          project_s_raw.push_back (bibles[i]);
        }
      }
    }
  }
  vector < ustring > project_names;
  for (unsigned int i = 0; i < project_s_raw.size(); i++) {
    if (project_book_exists(project_s_raw[i], settings->genconfig.book_get())) {
      project_names.push_back(project_s_raw[i]);
    } else {
      messages.push_back("Project " + project_s_raw[i] + " was requested to be included, but it does not contain " + books_id_to_english(settings->genconfig.book_get()) + ". It was left out.");
    }
  }

  // References to print.
  vector < Reference > references;

  // Portion selection.
  WithinReferencesRange inrange;
  {
    vector < unsigned int >portions_chapter_from, portions_chapter_to;
    vector < ustring > portions_verse_from, portions_verse_to;
    select_portion_get_values(settings->genconfig.project_get(), settings->genconfig.book_get(), settings->genconfig.parallel_bible_chapters_verses_get(), portions_chapter_from, portions_verse_from, portions_chapter_to, portions_verse_to);
    inrange.add_portion(settings->genconfig.book_get(), portions_chapter_from, portions_verse_from, portions_chapter_to, portions_verse_to);
    inrange.set_book(settings->genconfig.book_get());
  }

  // Go through the chapters.
  for (unsigned int ch = 0; ch < chapters.size(); ch++) {

    progresswindow.iterate();

    inrange.set_chapter(chapters[ch]);

    // Go through the verse numbers in this chapter.
    vector < ustring > verses = project_get_verses(settings->genconfig.project_get(), settings->genconfig.book_get(), chapters[ch]);
    for (unsigned int vs = 0; vs < verses.size(); vs++) {

      inrange.set_verse(verses[vs]);
      if (!inrange.in_range())
        continue;

      // See whether to print verses zero.
      if (!settings->genconfig.parallel_bible_include_verse_zero_get())
        if (verses[vs] == "0")
          continue;

      // Store the reference.
      Reference reference(settings->genconfig.book_get(), chapters[ch], verses[vs]);
      references.push_back(reference);
    }
  }

  // Hide progeressbar.
  progresswindow.hide();

  // Do the printing.
  ProjectMemory projectmemory(settings->genconfig.project_get(), true);
  view_parallel_references_pdf(projectmemory, &project_names, references, settings->genconfig.parallel_bible_keep_verses_together_get(), &messages, false);

  // Log: ready.
  gw_message("Ready printing the Parallel Bible");
}
