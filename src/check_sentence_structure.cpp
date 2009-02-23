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

#include "check_sentence_structure.h"
#include "projectutils.h"
#include "progresswindow.h"
#include "settings.h"
#include "stylesheetutils.h"
#include "utilities.h"
#include "usfmtools.h"
#include "books.h"
#include "tiny_utilities.h"

CheckSentenceStructure::CheckSentenceStructure(const ustring & project, const vector < unsigned int >&books, bool gui)
/*
It performs checks of the sentence structure.
project: project to check.
books: books to check; if empty it checks them all.
gui: whether to show graphical progressbar.
*/
{
  // Init variables.
  cancelled = false;
  // Classify the styles.
  classify_styles(project);
  // Get a list of the books to check. If no books were given, take them all.
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);
  // GUI.
  ProgressWindow *progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow("Checking sentence structure", true);
    progresswindow->set_iterate(0, 1, mybooks.size());
  }
  // Go through each book.
  for (unsigned int bk = 0; bk < mybooks.size(); bk++) {
    if (gui) {
      progresswindow->iterate();
      progresswindow->set_text(books_id_to_english(mybooks[bk]));
      if (progresswindow->cancel) {
        cancelled = true;
        break;
      }
    }
    book = mybooks[bk];
    // Go through each chapter.
    vector < unsigned int >chapters = project_get_chapters(project, book);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      chapter = chapters[ch];
      vector < ustring > lines = project_retrieve_chapter(project, book, chapter);
      // Assemble the line with all data in it.
      ustring line;
      for (unsigned int i = 0; i < lines.size(); i++) {
        line.append(lines[i]);
        line.append(" ");
      }
      create_blocks(line);
    }
  }
  // Clean up.
  if (progresswindow)
    delete progresswindow;
}

CheckSentenceStructure::~CheckSentenceStructure()
{
}

void CheckSentenceStructure::classify_styles(const ustring & project)
// Classifies the markers in the project.
{
  ustring stylesheet = stylesheet_get_actual ();
  Usfm usfm(stylesheet);
  for (unsigned int i = 0; i < usfm.styles.size(); i++) {
    switch (usfm.styles[i].type) {
    case stIdentifier:
    case stNotUsedComment:
    case stNotUsedRunningHeader:
      {
        textdeleters.insert(usfm.styles[i].marker);
        break;
      }
    case stStartsParagraph:
      {
        switch (usfm.styles[i].subtype) {
        case ptMainTitle:
        case ptSubTitle:
        case ptSectionHeading:
          {
            headingstarters.insert(usfm.styles[i].marker);
            break;
          }
        case ptNormalParagraph:
          {
            ustring marker = usfm.styles[i].marker;
            bool poetry = false;
            // Markers starting with a "q" are poetry.
            if (g_str_has_prefix(marker.c_str(), "q"))
              poetry = true;
            if (g_str_has_prefix(marker.c_str(), "iq"))
              poetry = true;
            if (poetry) {
              markerdeleters.insert(marker);
            } else {
              paragraphstarters.insert(marker);
            }
            break;
          }
        }
        break;
      }
    case stInlineText:
      {
        markerdeleters.insert(usfm.styles[i].marker);
        break;
      }
    case stChapterNumber:
      {
        textdeleters.insert(usfm.styles[i].marker);
        break;
      }
    case stVerseNumber:
      {
        markerdeleters.insert(usfm.styles[i].marker);
        versemarkers.insert(usfm.styles[i].marker);
        break;
      }
    case stFootEndNote:
      {
        switch (usfm.styles[i].subtype) {
        case fentFootnote:
        case fentEndnote:
          {
            notestarters.insert(usfm.styles[i].marker);
            break;
          }
        case fentStandardContent:
        case fentContent:
        case fentContentWithEndmarker:
          {
            markerdeleters.insert(usfm.styles[i].marker);
            break;
          }
        case fentParagraph:
          {
            paragraphstarters.insert(usfm.styles[i].marker);
            break;
          }
        }
        break;
      }
    case stCrossreference:
      {
        switch (usfm.styles[i].subtype) {
        case ctCrossreference:
          {
            notestarters.insert(usfm.styles[i].marker);
            break;
          }
        case ctStandardContent:
        case ctContent:
        case ctContentWithEndmarker:
          {
            markerdeleters.insert(usfm.styles[i].marker);
            break;
          }
        }
        break;
      }
    case stPeripheral:
      {
        textdeleters.insert(usfm.styles[i].marker);
        break;
      }
    case stPicture:
      {
        textdeleters.insert(usfm.styles[i].marker);
        break;
      }
    case stPageBreak:
      {
        textdeleters.insert(usfm.styles[i].marker);
        break;
      }
    case stTableElement:
      {
        switch (usfm.styles[i].subtype) {
        case tetRow:
          {
            markerdeleters.insert(usfm.styles[i].marker);
            break;
          }
        case tetHeading:
          {
            headingstarters.insert(usfm.styles[i].marker);
            break;
          }
        case tetCell:
          {
            paragraphstarters.insert(usfm.styles[i].marker);
            break;
          }
        }
        break;
      }
    case stWordlistElement:
      {
        markerdeleters.insert(usfm.styles[i].marker);
        break;
      }
    }
  }
}

void CheckSentenceStructure::create_blocks(ustring & text)
// This creates complete units ready for checking.
{
  // Variables for defining the independent blocks of text, and for the verse numbers.
  ustring block;
  SentenceStructureType structuretype = sstParagraph;
  vector < ustring > versenumbers;
  versenumbers.push_back("0");
  vector < size_t > verseoffsets;
  verseoffsets.push_back(0);
  // Go through the available text till ready.
  while (!text.empty()) {
    // Variables for USFM markers.
    ustring marker;
    size_t marker_position;
    size_t marker_length;
    bool is_opener;
    bool marker_found = usfm_search_marker(text, marker, marker_position, marker_length, is_opener);
    bool processed = false;
    if (marker_found) {
      if (marker_position == 0) {
        if (versemarkers.find(marker) != versemarkers.end()) {
          // Extract the verse number and offset within the block.
          ustring verse(text.substr(0, 10));
          verse.erase(0, marker_length);
          size_t pos = verse.find(" ");
          if (pos != string::npos) {
            verse = verse.substr(0, pos);
            versenumbers.push_back(verse);
            verseoffsets.push_back(block.length());
          }
        }
        if (textdeleters.find(marker) != textdeleters.end()) {
          // Handle markers whose text should be deleted from the input text.
          get_erase_code_till_next_marker(text, marker_position, marker_length, false);
          processed = true;
        } else if (headingstarters.find(marker) != headingstarters.end()) {
          // Handle markers that start a heading.
          check(structuretype, block, versenumbers, verseoffsets);
          block.append(get_erase_code_till_next_marker(text, marker_position, marker_length, false));
          structuretype = sstHeading;
          processed = true;
        } else if (paragraphstarters.find(marker) != paragraphstarters.end()) {
          // Handle markers that start a normal paragraph.
          check(structuretype, block, versenumbers, verseoffsets);
          block.append(get_erase_code_till_next_marker(text, marker_position, marker_length, false));
          structuretype = sstParagraph;
          processed = true;
        } else if (markerdeleters.find(marker) != markerdeleters.end()) {
          // Handle markers to be deleted from the text.
          text.erase(0, marker_length);
          processed = true;
        } else if (notestarters.find(marker) != notestarters.end()) {
          // Handle markers that start a note.
          text.erase(0, marker_length);
          if (is_opener) {
            // Look for marker that closes the note as well.
            size_t pos = text.find(usfm_get_full_closing_marker(marker));
            if (pos != string::npos) {
              // Extract the note.
              ustring notetext = text.substr(0, pos);
              text.erase(0, pos + usfm_get_full_closing_marker(marker).length());
              // Update the verse numbering system for this note, taking the current verse.
              vector < ustring > versenumbers2;
              if (versenumbers.empty()) {
                versenumbers2.push_back("0");
              } else {
                ustring verse = versenumbers[versenumbers.size() - 1];
                versenumbers2.push_back(verse);
              }
              vector < size_t > verseoffsets2;
              verseoffsets2.push_back(0);
              // Clean up the note text: Remove the caller marker, trim it, remove all markers from it.
              notetext.erase(0, 1);
              notetext = trim(notetext);
              vector < ustring > allmarkers = usfm_get_all_markers(notetext);
              for (unsigned int i = 0; i < allmarkers.size(); i++) {
                ustring opener = usfm_get_full_opening_marker(allmarkers[i]);
                replace_text(notetext, opener, "");
                ustring closer = usfm_get_full_closing_marker(allmarkers[i]);
                replace_text(notetext, closer, "");
              }
              // Check this note.
              check(sstNote, notetext, versenumbers2, verseoffsets2);
            }
          }
          processed = true;
        }
      }
    }
    if (!processed) {
      // Fall-back processing.
      block.append(text.substr(0, 1));
      text.erase(0, 1);
    }
  }
  if (!block.empty()) {
    check(structuretype, block, versenumbers, verseoffsets);
  }
}

void CheckSentenceStructure::check(SentenceStructureType type, ustring text, vector < ustring > &versenumbers, vector < size_t > &verseoffsets)
{
  // Bail out if there's no text.
  if (text.empty())
    return;

  // Load text.
  GtkTextBuffer *buffer = gtk_text_buffer_new(NULL);
  text.append(" ");
  gtk_text_buffer_set_text(buffer, text.c_str(), -1);

  // Iterators.
  GtkTextIter iter;
  gtk_text_buffer_get_start_iter(buffer, &iter);

  // Variables for check: The start of the buffer is supposed to have a capital.
  // Skip this step if we're checking a note, as notes often start without capitals.
  bool initial_capital_done = (type == sstNote);
  // Variables for check: Sentence start and end.
  bool sentence_started = false;

  // Go through the whole buffer.
  do {

    // Get the character at the iterator.
    gunichar character = gtk_text_iter_get_char(&iter);

    // Check: The start of the buffer is supposed to have a capital.
    if (!initial_capital_done) {
      if (g_unichar_isalpha(character) || g_unichar_iscntrl(character)) {
        if (!g_unichar_isupper(character)) {
          message(versenumbers, verseoffsets, &iter, "The text does not start with a capital");
        }
        initial_capital_done = true;
      }
    }
    // Check: Sentence starts with capital.
    if (!sentence_started) {
      if (g_unichar_islower(character)) {
        message(versenumbers, verseoffsets, &iter, "The text does not start with a capital");
      }
    }
    // Check: Start / end sentence.
    if (gtk_text_iter_starts_sentence(&iter)) {
      if (sentence_started) {
        message(versenumbers, verseoffsets, &iter, "A new sentence starts within a sentence");
      }
      sentence_started = true;
    }
    if (gtk_text_iter_ends_sentence(&iter)) {
      if (!sentence_started) {
        message(versenumbers, verseoffsets, &iter, "A sentence ends where none was started");
      }
      sentence_started = false;
    }

  } while (gtk_text_iter_forward_char(&iter));

  // Check: That a heading does not end the sentence.
  if (type == sstHeading) {
    if (!sentence_started) {
      message(versenumbers, verseoffsets, &iter, "A heading usually starts a sentence but does not finish it");
    }
  }
  // Free buffer.
  g_object_unref(G_OBJECT(buffer));

  // Prepare input for next check.
  text.clear();
  ustring lastverse = versenumbers[versenumbers.size() - 1];
  versenumbers.clear();
  versenumbers.push_back(lastverse);
  verseoffsets.clear();
  verseoffsets.push_back(0);
}

void CheckSentenceStructure::message(vector < ustring > &versenumbers, vector < size_t > &verseoffsets, GtkTextIter * iter, const ustring & message)
{
  // Get the verse.
  ustring verse = "0";
  size_t offset = gtk_text_iter_get_offset(iter);
  for (unsigned int i = 0; i < versenumbers.size(); i++) {
    if (offset >= verseoffsets[i]) {
      verse = versenumbers[i];
    }
  }

  // Assemble the reference.
  ustring reference = books_id_to_english(book) + " " + convert_to_string(chapter) + ":" + verse;

  // Get the fragment of the text.
  GtkTextIter iter1 = *iter;
  GtkTextIter iter2 = *iter;
  gtk_text_iter_backward_word_starts(&iter1, 2);
  gtk_text_iter_forward_word_ends(&iter2, 2);
  ustring text = gtk_text_iter_get_text(&iter1, &iter2);

  // Assemble the comment.
  ustring comment(text + " - " + message);

  // Bail out of the message is already there.
  for (unsigned int i = 0; i < references.size(); i++)
    if (reference == references[i])
      if (message == messages[i])
        return;

  // Store the message.
  references.push_back(reference);
  comments.push_back(comment);
  messages.push_back(message);
}
