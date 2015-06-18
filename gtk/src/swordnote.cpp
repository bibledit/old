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
#include "swordnote.h"
#include "usfmtools.h"
#include "constants.h"
#include "gwrappers.h"
#include "tiny_utilities.h"
#include <glib/gi18n.h>

SwordNote::SwordNote(const Usfm & usfm, bool show)
// Stores the properties for all the footnote related styles.
{
  // Store and initialize variables.
  myshow = show;
  NoteNumberingType footnote_numbering_type = nntNumerical;
  NoteNumberingType endnote_numbering_type = nntNumerical;
  NoteNumberingType xref_numbering_type = nntAlphabetical;
  footnote_numbering_restart = nnrtChapter;
  endnote_numbering_restart = nnrtNever;
  xref_numbering_restart = nnrtChapter;
  ustring footnote_numbering_user_sequence;
  ustring endnote_numbering_user_sequence;
  ustring xref_numbering_user_sequence;

  // Go through all the styles.
  for (unsigned int i = 0; i < usfm.styles.size(); i++) {
    if (usfm.styles[i].type == stFootEndNote) {
      // Check subtype.
      FootEndNoteType footnotetype = (FootEndNoteType) usfm.styles[i].subtype;
      switch (footnotetype) {
      case fentFootnote:
        {
          // Store data.
          footnote_opener = usfm_get_full_opening_marker(usfm.styles[i].marker);
          footnote_closer = usfm_get_full_closing_marker(usfm.styles[i].marker);
          footnote_numbering_type = (NoteNumberingType) usfm.styles[i].userint1;
          footnote_numbering_restart = (NoteNumberingRestartType) usfm.styles[i].userint2;
          footnote_numbering_user_sequence = usfm.styles[i].userstring1;
          break;
        }
      case fentEndnote:
        {
          // Store data.
          endnote_opener = usfm_get_full_opening_marker(usfm.styles[i].marker);
          endnote_closer = usfm_get_full_closing_marker(usfm.styles[i].marker);
          endnote_numbering_type = (NoteNumberingType) usfm.styles[i].userint1;
          endnote_numbering_restart = nnrtNever;
          EndnotePositionType endnoteposition = (EndnotePositionType) usfm.styles[i].userint2;
          if (endnoteposition == eptAfterBook)
            endnote_numbering_restart = nnrtBook;
          endnote_numbering_user_sequence = usfm.styles[i].userstring1;
          break;
        }
      case fentStandardContent:
        {
          // Standard content.
          standardparagraph_marker_open = usfm_get_full_opening_marker(usfm.styles[i].marker);
          standardparagraph_marker_close = usfm_get_full_closing_marker(usfm.styles[i].marker);
          footnote_markers.insert(usfm.styles[i].marker);
          break;
        }
      case fentContent:
      case fentContentWithEndmarker:
        {
          // Store data for the footnote body.
          content_marker.push_back(usfm_get_full_opening_marker(usfm.styles[i].marker));
          footnote_markers.insert(usfm.styles[i].marker);
          break;
        }
      case fentParagraph:
        {
          extraparagraph_marker_open = usfm_get_full_opening_marker(usfm.styles[i].marker);
          extraparagraph_marker_close = usfm_get_full_closing_marker(usfm.styles[i].marker);
          footnote_markers.insert(usfm.styles[i].marker);
          break;
        }
      }
    }
    if (usfm.styles[i].type == stCrossreference) {
      // Check subtype.
      CrossreferenceType xreftype = (CrossreferenceType) usfm.styles[i].subtype;
      switch (xreftype) {
      case ctCrossreference:
        {
          // Store data for the markers and the anchor.
          xref_opener = usfm_get_full_opening_marker(usfm.styles[i].marker);
          xref_closer = usfm_get_full_closing_marker(usfm.styles[i].marker);
          xref_numbering_type = (NoteNumberingType) usfm.styles[i].userint1;
          xref_numbering_restart = (NoteNumberingRestartType) usfm.styles[i].userint2;
          xref_numbering_user_sequence = usfm.styles[i].userstring1;
          break;
        }
      case ctStandardContent:
        {
          // Standard content.
          standardparagraph_marker_open = usfm_get_full_opening_marker(usfm.styles[i].marker);
          standardparagraph_marker_close = usfm_get_full_closing_marker(usfm.styles[i].marker);
          footnote_markers.insert(usfm.styles[i].marker);
          break;
        }
      case ctContent:
      case ctContentWithEndmarker:
        {
          // Store data for the footnote body.
          content_marker.push_back(usfm_get_full_opening_marker(usfm.styles[i].marker));
          footnote_markers.insert(usfm.styles[i].marker);
          break;
        }
      }
    }
  }
  // Ensure that both of the paragraph styles are there.
  if (standardparagraph_marker_open.empty()) {
    standardparagraph_marker_open = usfm_get_full_opening_marker("ft");
    standardparagraph_marker_close = usfm_get_full_closing_marker("ft");
  }
  if (extraparagraph_marker_open.empty()) {
    extraparagraph_marker_open = usfm_get_full_opening_marker("fp");
    extraparagraph_marker_close = usfm_get_full_closing_marker("fp");
  }
  // Create note caller objects.
  footnotecaller = new NoteCaller(footnote_numbering_type, footnote_numbering_user_sequence);
  endnotecaller = new NoteCaller(endnote_numbering_type, endnote_numbering_user_sequence);
  xrefcaller = new NoteCaller(xref_numbering_type, xref_numbering_user_sequence);
}

SwordNote::~SwordNote()
{
  delete footnotecaller;
  delete endnotecaller;
  delete xrefcaller;
}

void SwordNote::new_book()
{
  if (footnote_numbering_restart == nnrtBook)
    footnotecaller->reset();
  if (endnote_numbering_restart == nnrtBook)
    endnotecaller->reset();
  if (xref_numbering_restart == nnrtBook)
    xrefcaller->reset();
  new_chapter();
}

void SwordNote::new_chapter()
{
  if (footnote_numbering_restart == nnrtChapter)
    footnotecaller->reset();
  if (endnote_numbering_restart == nnrtChapter)
    endnotecaller->reset();
  if (xref_numbering_restart == nnrtChapter)
    xrefcaller->reset();
}

void SwordNote::transform(ustring & line)
{
  // Deal with the three note types we recognize.
  transform2(footnote_opener, footnote_closer, footnotecaller, line);
  transform2(endnote_opener, endnote_closer, endnotecaller, line);
  transform2(xref_opener, xref_closer, xrefcaller, line);
}

void SwordNote::transform2(ustring & opener, ustring & closer, NoteCaller * caller, ustring & line)
// Replace all note related content with corresponding osis xml code.
{
  // Variables.
  size_t opening_position;

  // Look for notes, but only deal with them if they have the endmarker too.
  opening_position = line.find(opener);
  while (opening_position != string::npos) {

    // Look for the endmarker.
    size_t closing_position;
    closing_position = line.find(closer, opening_position);
    if (closing_position == string::npos) {
      gw_warning(_("Missing endmarker: ") + line);
      return;
    }
    // Take out this bit of the line, transform it, and insert it again.
    ustring note;
    note = line.substr(opening_position + opener.length(), closing_position - opening_position - closer.length());
    line.erase(opening_position, closing_position - opening_position + closer.length());
    if (myshow) {
      note = transform_main_parts(caller, note);
      line.insert(opening_position, note);
    }
    // Search for another note.
    opening_position = line.find(opener, opening_position);
  }
}

ustring SwordNote::transform_main_parts(NoteCaller * caller, const ustring & line)
{
  // Variables.
  ustring sword_code;

  // Work on a copy.
  ustring note(line);

  // Extract the footnote caller.    
  ustring callertext;
  if (note.length() > 0) {
    callertext = note.substr(0, 1);
    callertext = trim(callertext);
    if (callertext == "+") {
      callertext = caller->get_caller();
    } else if (callertext == "-") {
      callertext.clear();
    }
    note.erase(0, 1);
    note = trim(note);
  }
  // Add first bit of code.
  sword_code.append("<note type=\"");
  if (caller == xrefcaller) {
    sword_code.append("crossReference");
  } else {
    sword_code.append("translation");
  }
  sword_code.append("\"");
  if (!callertext.empty())
    sword_code.append(" n=\"" + callertext + "\"");
  sword_code.append(">");

  // As Sword content, as displayed in BibleTime, does not format notes, just remove the formatting.  
  ustring marker;
  size_t position, length;
  bool opening_marker;
  while (usfm_search_marker(note, marker, position, length, opening_marker)) {
    note.erase(position, length);
  }

  // Add note.
  sword_code.append(note);

  // Add last bit of code.
  sword_code.append("</note>");

  // Return the code.
  return sword_code;
}
