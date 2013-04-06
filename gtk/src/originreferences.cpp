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
#include <glib.h>
#include "originreferences.h"
#include "utilities.h"
#include "projectutils.h"
#include "settings.h"
#include "progresswindow.h"
#include "gtkwrappers.h"
#include "gwrappers.h"
#include "usfmtools.h"
#include "help.h"
#include "usfm.h"
#include "books.h"
#include "tiny_utilities.h"
#include "stylesheetutils.h"


OriginReferences::OriginReferences(const ustring & project, bool affectfootnotes, bool affectendnotes, bool affectxrefs, OriginReferencesActionType action, bool includebook, bool includechapter, const ustring & separator, bool includeverse, const ustring & suffix, bool gui)
{
  // Progress.
  ProgressWindow *progresswindow = NULL;
  if (gui)
    progresswindow = new ProgressWindow("Working...", false);

  // Initialize variables.
  myaction = action;
  myincludebook = includebook;
  myincludechapter = includechapter;
  myincludeverse = includeverse;
  myseparator = separator;
  mysuffix = suffix;
  notescount = 0;
  affectedcount = 0;
  finecount = 0;
  errorcount = 0;
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  stylesheet = stylesheet_get_actual ();
  language = projectconfig->language_get();

  // Get the relevant markers.
  get_relevant_markers();

  // Go through the books in the project.
  vector < unsigned int >books = project_get_books(project);
  if (gui)
    progresswindow->set_iterate(0, 1, books.size());
  for (unsigned int bk = 0; bk < books.size(); bk++) {

    // Progress.
    if (gui)
      progresswindow->iterate();

    // Go through the chapters in this book.
    vector < unsigned int >chapters = project_get_chapters(project, books[bk]);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {

      // Go through the verses in this chapter.
      vector < ustring > verses = project_get_verses(project, books[bk], chapters[ch]);
      for (unsigned int vs = 0; vs < verses.size(); vs++) {

        // Retrieve each verse and process it.
        ustring text = project_retrieve_verse(project, books[bk], chapters[ch], verses[vs]);
        unsigned int previous_affected_count = affectedcount;
        if (affectfootnotes)
          handle_notes(text, books[bk], chapters[ch], verses[vs], footnote_opener, footnote_closer, foot_end_note_origin_reference_opener, foot_end_note_origin_reference_closer, foot_end_note_text_opener, foot_end_note_text_closer);
        if (affectendnotes)
          handle_notes(text, books[bk], chapters[ch], verses[vs], endnote_opener, endnote_closer, foot_end_note_origin_reference_opener, foot_end_note_origin_reference_closer, foot_end_note_text_opener, foot_end_note_text_closer);
        if (affectxrefs)
          handle_notes(text, books[bk], chapters[ch], verses[vs], xref_opener, xref_closer, xref_origin_reference_opener, xref_origin_reference_closer, xref_text_opener, xref_text_closer);
        if (affectedcount != previous_affected_count) {
          project_store_verse(project, books[bk], chapters[ch], verses[vs], text);
        }
      }
    }
  }

  // Give statistics.
  ustring message;
  message.append("Relevant notes found: " + convert_to_string(notescount) + "\n");
  if (action != oratNothing) {
    if (action == oratAddReference)
      message.append("Origin references added: ");
    if (action == oratRemoveReferences)
      message.append("Origin references removed: ");
    if (action == oratTextLabels)
      message.append("Text labels added: ");
    message.append(convert_to_string(affectedcount) + "\n");
    if (finecount)
      message.append("Notes not affected because they already were fine: " + convert_to_string(finecount) + "\n");
  }
  if (errorcount)
    message.append("Notes with structural errors: " + convert_to_string(errorcount) + "\n");
  if (gui)
    gtkw_dialog_info(NULL, message);
  else
    cout << message;

  // Destroy progress.
  if (gui)
    delete progresswindow;
}

OriginReferences::~OriginReferences()
{
}

void OriginReferences::get_relevant_markers()
{
  // Set default values in case they cannot be retrieved from the stylesheet.
  footnote_opener = usfm_get_full_opening_marker("f");
  footnote_closer = usfm_get_full_closing_marker("f");
  endnote_opener = usfm_get_full_opening_marker("fe");
  endnote_closer = usfm_get_full_opening_marker("fe");
  xref_opener = usfm_get_full_opening_marker("x");
  xref_closer = usfm_get_full_opening_marker("x");
  foot_end_note_origin_reference_opener = usfm_get_full_opening_marker("fr");
  foot_end_note_origin_reference_closer = usfm_get_full_closing_marker("fr");
  xref_origin_reference_opener = usfm_get_full_opening_marker("xo");
  xref_origin_reference_closer = usfm_get_full_closing_marker("xo");
  foot_end_note_text_opener = usfm_get_full_opening_marker("ft");
  foot_end_note_text_closer = usfm_get_full_closing_marker("ft");
  xref_text_opener = usfm_get_full_opening_marker("xt");
  xref_text_closer = usfm_get_full_opening_marker("xt");

  // Retrieve some values from the stylesheet.  
  Usfm usfm(stylesheet);
  for (unsigned int i = 0; i < usfm.styles.size(); i++) {
    if (usfm.styles[i].type == stFootEndNote) {
      FootEndNoteType footnotetype = (FootEndNoteType) usfm.styles[i].subtype;
      switch (footnotetype) {
      case fentFootnote:
        {
          footnote_opener = usfm_get_full_opening_marker(usfm.styles[i].marker);
          footnote_closer = usfm_get_full_closing_marker(usfm.styles[i].marker);
          break;
        }
      case fentEndnote:
        {
          endnote_opener = usfm_get_full_opening_marker(usfm.styles[i].marker);
          endnote_closer = usfm_get_full_closing_marker(usfm.styles[i].marker);
          break;
        }
      case fentStandardContent:
        {
          foot_end_note_text_opener = usfm_get_full_opening_marker(usfm.styles[i].marker);
          foot_end_note_text_closer = usfm_get_full_closing_marker(usfm.styles[i].marker);
          break;
        }
      case fentContent:
      case fentContentWithEndmarker:
      case fentParagraph:;
      }
    }
    if (usfm.styles[i].type == stCrossreference) {
      CrossreferenceType xreftype = (CrossreferenceType) usfm.styles[i].subtype;
      switch (xreftype) {
      case ctCrossreference:
        {
          xref_opener = usfm_get_full_opening_marker(usfm.styles[i].marker);
          xref_closer = usfm_get_full_closing_marker(usfm.styles[i].marker);
          break;
        }
      case ctStandardContent:
        {
          xref_text_opener = usfm_get_full_opening_marker(usfm.styles[i].marker);
          xref_text_closer = usfm_get_full_closing_marker(usfm.styles[i].marker);
          break;
        }
      case ctContent:
      case ctContentWithEndmarker:;
      }
    }
  }
}

void OriginReferences::handle_notes(ustring & line, int book, int chapter, const ustring & verse, const ustring & note_opener, const ustring & note_closer, const ustring & origin_ref_opener, const ustring & origin_ref_closer, const ustring & text_opener, const ustring & text_closer)
// Extract a note, call the handler, insert the changed note again.
{
  // Variables.
  size_t opening_position;

  // Look for notes, but only deal with them if they have the endmarker too.
  opening_position = line.find(note_opener);
  while (opening_position != string::npos) {

    // Statistics.
    notescount++;

    // Look for the endmarker.
    size_t closing_position;
    closing_position = line.find(note_closer, opening_position);
    if (closing_position == string::npos) {
      gw_warning("Missing endmarker: " + line);
      errorcount++;
      return;
    }
    // Take out this bit of the line, transform it, and insert it again.
    ustring footnote;
    footnote = line.substr(opening_position + note_opener.length(), closing_position - opening_position - note_closer.length());
    line.erase(opening_position, closing_position - opening_position + note_closer.length());

    switch (myaction) {
    case oratNothing:
      {
        finecount++;
        break;
      }
    case oratRemoveReferences:
      {
        remove_reference(footnote, origin_ref_opener, origin_ref_closer);
        break;
      }
    case oratAddReference:
      {
        ustring reference;
        reference = origin_reference_produce(myincludebook, book, language, myincludechapter, chapter, myseparator, myincludeverse, verse, mysuffix, false);
        add_reference(footnote, origin_ref_opener, reference);
        break;
      }
    case oratTextLabels:
      {
        add_text_marker(footnote, origin_ref_opener, origin_ref_closer, text_opener, text_opener);
        break;
      }
    }
    line.insert(opening_position, note_opener + footnote + note_closer);

    // Search for another footnote.
    opening_position = line.find(note_opener, opening_position + 1);
  }
}

void OriginReferences::remove_reference(ustring & line, const ustring & opener, const ustring & closer)
{
  size_t opener_position = line.find(opener);
  if (opener_position == string::npos) {
    finecount++;
    return;
  }

  size_t next_space_position = line.find(" ", opener_position + 4);
  if (next_space_position != string::npos) {
    line.erase(opener_position, next_space_position - opener_position + 1);
  }

  replace_text(line, closer, "");

  affectedcount++;
}

void OriginReferences::add_reference(ustring & line, const ustring & opener, const ustring & reference)
{
  // If the opener is already in, the note is fine. Bail out.
  size_t position = line.find(opener);
  if (position != string::npos) {
    finecount++;
    return;
  }
  // Data to insert.
  ustring insert = opener + reference;

  // Insert the reference after the first space, if there, else at the end.
  position = line.find(" ");
  if (position == string::npos)
    line.append(insert);
  else
    line.insert(++position, insert);
  affectedcount++;
}

void OriginReferences::add_text_marker(ustring & line, const ustring & referenceopener, const ustring & referencecloser, const ustring & textopener, const ustring & textcloser)
// Adds a text marker, only the opening marker, if it is not yet in the line.
{
  // If the text opener or closer is already in, things appear to be fine.
  size_t position = line.find(textopener);
  if (position != string::npos) {
    finecount++;
    return;
  }
  position = line.find(textcloser);
  if (position != string::npos) {
    finecount++;
    return;
  }
  // If a reference closer is in, insert the text opener straight after it.
  position = line.find(referencecloser);
  if (position != string::npos) {
    position += referencecloser.length();
    line.insert(position, textopener);
    affectedcount++;
    return;
  }
  // If a reference opener is in, insert the text opener shortly after it.
  // E.g.: \fr 1.3 The word house is used in a figurative sense.
  position = line.find(referenceopener);
  if (position != string::npos) {
    position += referenceopener.length();
    position++;
    if (position < line.length()) {
      position = line.find(" ", position);
      position++;
      if (position < line.length()) {
        line.insert(position, textopener);
        affectedcount++;
      }
    }
    return;
  }
  // If no known opener is in, insert the reference after the first space.
  position = line.find(" ");
  if (position != string::npos) {
    line.insert(++position, textopener);
    affectedcount++;
    return;
  }
}

ustring origin_reference_produce(bool includebook, unsigned int book, const ustring & language, bool includechapter, int chapter, const ustring & separator, bool includeverse, const ustring & verse, const ustring & suffix, bool example)
// Produces the text of the origin references, as can be inserted in a Bible note.
// If "example" is set, then the reference will be surrounded by brackets.
{
  ustring result;
  if (includebook)
    result.append(books_id_to_abbreviation(language, book) + " ");
  if (includechapter)
    result.append(convert_to_string(chapter));
  if (includechapter && includeverse)
    result.append(separator);
  if (includeverse)
    result.append(verse);
  if (includechapter || includeverse)
    result.append(suffix);
  if (example) {
    result.insert(0, "(");
    result.append(")");
  }
  return result;
}
