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


#include "wordlist.h"
#include "settings.h"
#include "projectutils.h"
#include "utilities.h"
#include "books.h"
#include "usfmtools.h"
#include "tiny_utilities.h"
#include "styles.h"
#include "stylesheetutils.h"
#include <glib/gi18n.h>

Wordlist::Wordlist(WordlistType wordlist)
{
  extern Settings *settings;

  project = settings->genconfig.project_get();

  ustring marker = wordlist_get_entry_style(project, wordlist);
  entry_opener = usfm_get_full_opening_marker(marker);
  entry_closer = usfm_get_full_closing_marker(marker);

  switch (wordlist) {
  case wltGeneral:
    {
      list_opener = "\\zopenwordlist";
      list_closer = "\\zclosewordlist";
      use_asterisk = settings->genconfig.wordlist_general_asterisk_get();
      first_in_chapter = settings->genconfig.wordlist_general_asterisk_first_get();
      wordlistname = _("General word list");
      break;
    }
  case wltHebrew:
    {
      list_opener = "\\zopenhebrewwordlist";
      list_closer = "\\zclosehebrewwordlist";
      use_asterisk = settings->genconfig.wordlist_hebrew_asterisk_get();
      first_in_chapter = settings->genconfig.wordlist_hebrew_asterisk_first_get();
      wordlistname = _("Hebrew word list");
      break;
    }
  case wltGreek:
    {
      list_opener = "\\zopengreekwordlist";
      list_closer = "\\zclosegreekwordlist";
      use_asterisk = settings->genconfig.wordlist_greek_asterisk_get();
      first_in_chapter = settings->genconfig.wordlist_greek_asterisk_first_get();
      wordlistname = _("Greek word list");
      break;
    }
  case wltIndex:
    {
      list_opener = "\\zopenindex";
      list_closer = "\\zcloseindex";
      use_asterisk = settings->genconfig.wordlist_index_asterisk_get();
      first_in_chapter = settings->genconfig.wordlist_index_asterisk_first_get();
      wordlistname = _("Index");
      break;
    }
  }

  // Get markers that indicate a section.
  extern Styles * styles;
  Stylesheet * stylesheet = styles->stylesheet (stylesheet_get_actual ());
  for (unsigned int i = 0; i < stylesheet->styles.size(); i++) {
    StyleV2 * style = stylesheet->styles[i];
    if (style->type == stStartsParagraph) {
      if (style->subtype != ptNormalParagraph) {
        section_markers.insert (style->marker);
      }
    }
  }

  progresswindow = new ProgressWindow(wordlistname, false);

  wordcount = 0;
}

Wordlist::~Wordlist()
{
  delete progresswindow;
}

void Wordlist::run(vector < ustring > &allmessages)
{
  // No project given: bail out.
  if (project.empty()) {
    message(_("No project"));
    return;
  }
  // Pass 1: Collect words and handle asterisks.

  // Go through the books.
  vector < unsigned int >books = project_get_books(project);
  progresswindow->set_iterate(0, 1, books.size());
  for (unsigned int bk = 0; bk < books.size(); bk++) {
    progresswindow->iterate();

    // Go through the chapters.
    vector < unsigned int >chapters = project_get_chapters(project, books[bk]);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {

      // Go through the lines of the chapter, and process them.
      vector <ustring> lines = project_retrieve_chapter(project, books[bk], chapters[ch]);
      set <ustring> section_entries;
      bool chapter_content_was_changed = false;
      for (unsigned int i = 0; i < lines.size(); i++) {
        ustring line(lines[i]);
        process_line(line, section_entries);
        if (line != lines[i]) {
          chapter_content_was_changed = true;
          lines[i] = line;
        }
      }
      if (chapter_content_was_changed) {
        CategorizeChapterVerse ccv(lines);
        project_store_chapter(project, books[bk], ccv);
      }
    }
  }
  // Informative messages.
  message(_("Total entries: ") + convert_to_string(wordcount));
  message(_("Unique entries: ") + convert_to_string((unsigned int)words.size()));

  // Pass 2: Insert word lists.

  // Only proceed if there is something to insert.
  if (!words.empty()) {

    // Whether a list was inserted.
    bool inserted = false;

    // Go through the books.
    progresswindow->set_iterate(0, 1, books.size());
    for (unsigned int bk = 0; bk < books.size(); bk++) {
      progresswindow->iterate();

      // Go through the chapters.
      vector < unsigned int >chapters = project_get_chapters(project, books[bk]);
      for (unsigned int ch = 0; ch < chapters.size(); ch++) {

        // Go through the lines of the chapter to look for the position to insert the list.
        unsigned int opener_offset = 0;
        unsigned int closer_offset = 0;
        vector < ustring > lines = project_retrieve_chapter(project, books[bk], chapters[ch]);
        for (unsigned int ln = 0; ln < lines.size(); ln++) {
          if (lines[ln].find(list_opener) == 0) {
            opener_offset = ln;
          }
          if (lines[ln].find(list_closer) == 0) {
            closer_offset = ln;
          }
        }

        // If something like a position was found, process that.
        if (opener_offset || closer_offset) {
          if (opener_offset && closer_offset && (opener_offset < closer_offset)) {
            insert_list(lines, opener_offset, closer_offset);
            CategorizeChapterVerse ccv(lines);
            project_store_chapter(project, books[bk], ccv);
            message(_("Word list inserted in ") + books_id_to_english(books[bk]) + " " + convert_to_string(chapters[ch]));
            inserted = true;
          } else {
            message(_("Invalid word list location in ") + books_id_to_english(books[bk]) + " " + convert_to_string(chapters[ch]));
          }
        }
      }
    }

    // Message if the list was not inserted.
    if (!inserted)
      message(_("No place found to insert the word list"));
  }
  // Store messages.
  for (unsigned int i = 0; i < messages.size(); i++) {
    allmessages.push_back(messages[i]);
  }
}


void Wordlist::process_line(ustring & line, set <ustring> &section_entries)
// Processes one line of text:
// - deals with entries.
// - deals with asterisks.
// section_entries - has the entries already made in this section.
{
  // Handle section restart.
  {
    ustring s (line);
    ustring marker = usfm_extract_marker (s);
    if (section_markers.find (marker) != section_markers.end()) {
      section_entries.clear();
    }
  }
  // Remove the asterisk before and the asterisk after the closer, e.g.:
  // \w entry*\w* -> \w entry\w*
  // \w entry\w** -> \w entry\w*
  replace_text(line, "*" + entry_closer, entry_closer);
  replace_text(line, entry_closer + "*", entry_closer);
  // Go through the line looking for the opener.
  size_t startpos = line.find(entry_opener);
  while (startpos != string::npos) {
    // Look for the closer too, after the opener, not before.
    size_t endpos = line.find(entry_closer, startpos);
    if (endpos == string::npos)
      break;
    // Get the word.
    ustring word = line.substr(startpos + entry_opener.length(), endpos - startpos - entry_closer.length());
    if (!word.empty()) {
      // Store the word.      
      words.insert(word);
      wordcount++;
      // Handle asterisks.
      if (use_asterisk) {
        bool insert_asterisk = true;
        if (first_in_chapter)
          if (section_entries.find(word) != section_entries.end())
            insert_asterisk = false;
        if (insert_asterisk) {
          line.insert(endpos + entry_closer.length(), "*");
        }
        section_entries.insert(word);
      }
    }
    startpos = line.find(entry_opener, endpos);
  }
}


void Wordlist::insert_list(vector < ustring > &lines, unsigned int startlist, unsigned int endlist)
{
  // Split the available lines into three parts:
  // - the part before the insertion point,
  // - the already existing entries,
  // - and the part after the insertion point.
  vector < ustring > linesbefore;
  vector < ustring > entries;
  vector < ustring > linesafter;
  for (unsigned int i = 0; i < lines.size(); i++) {
    if (i <= startlist) {
      linesbefore.push_back(lines[i]);
    }
    if ((i > startlist) && (i < endlist)) {
      entries.push_back(lines[i]);
    }
    if (i >= endlist) {
      linesafter.push_back(lines[i]);
    }
  }

  // Any existing entry, put it in the \rem remark. 
  // This is done so that in case it is no longer referenced, it will get commented out.
  for (unsigned int i = 0; i < entries.size(); i++) {
    ustring marker = usfm_extract_marker(entries[i]);
    entries[i].insert(0, usfm_get_full_opening_marker("rem"));
  }

  // Get a list of entries.
  vector < ustring > mywords(words.begin(), words.end());

  // Go through all the entries, insert them in the list, 
  // preserving existing ones.
  for (unsigned int i = 0; i < mywords.size(); i++) {
    bool entry_exists = false;
    for (unsigned int i2 = 0; i2 < entries.size(); i2++) {
      ustring line(entries[i2]);
      usfm_extract_marker(line);
      ustring entry(line);
      size_t pos = entry.find(":");
      if (pos != string::npos) {
        entry.erase(pos, 100000);
      }
      if (entry == mywords[i]) {
        line.insert(0, usfm_get_full_opening_marker("p"));
        entries[i2] = line;
        entry_exists = true;
      }
    }
    if (!entry_exists) {
      entries.push_back(usfm_get_full_opening_marker("p") + mywords[i]);
    }
  }

  // Sort the entries.  
  sort(entries.begin(), entries.end());

  // Join everything together.
  lines.clear();
  for (unsigned int i = 0; i < linesbefore.size(); i++)
    lines.push_back(linesbefore[i]);
  for (unsigned int i = 0; i < entries.size(); i++)
    lines.push_back(entries[i]);
  for (unsigned int i = 0; i < linesafter.size(); i++)
    lines.push_back(linesafter[i]);
}


void Wordlist::message(const ustring & message)
{
  if (messages.empty())
    messages.push_back(wordlistname + ":");
  messages.push_back("- " + message);
}


ustring wordlist_get_entry_style(const ustring & project, WordlistType type)
{
  ustring stylesheet = stylesheet_get_actual ();
  extern Styles *styles;
  Usfm *usfm = styles->usfm(stylesheet);
  ustring style;
  switch (type) {
  case wltGeneral:
    style = "w";
    break;
  case wltHebrew:
    style = "wh";
    break;
  case wltGreek:
    style = "wg";
    break;
  case wltIndex:
    style = "ndx";
    break;
  }
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    if (usfm->styles[i].type == stWordlistElement) {
      switch (type) {
      case wltGeneral:
        if (usfm->styles[i].subtype == wltWordlistGlossaryDictionary)
          style = usfm->styles[i].marker;
        break;
      case wltHebrew:
        if (usfm->styles[i].subtype == wltHebrewWordlistEntry)
          style = usfm->styles[i].marker;
        break;
      case wltGreek:
        if (usfm->styles[i].subtype == wltGreekWordlistEntry)
          style = usfm->styles[i].marker;
        break;
      case wltIndex:
        if (usfm->styles[i].subtype == wltSubjectIndexEntry)
          style = usfm->styles[i].marker;
        break;
      }
    }
  }
  return style;
}


