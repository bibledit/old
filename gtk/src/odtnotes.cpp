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
#include "odtnotes.h"
#include "date_time_utils.h"
#include "gwrappers.h"
#include "settings.h"
#include "usfmtools.h"
#include "tiny_utilities.h"
#include <glib/gi18n.h>

OdtFootnote::OdtFootnote(const Usfm & usfm)
// Stores the properties for all the footnote related styles.
{
  // Store and initialize variables.
  show = true;
  NoteNumberingType note_numbering_type = nntNumerical;
  note_numbering_restart = nnrtChapter;
  ustring note_numbering_user_sequence;
  standardparagraph = NULL;
  extraparagraph = NULL;
  note_id = 0;

  // Go through all the styles.
  for (unsigned int i = 0; i < usfm.styles.size(); i++) {
    if (usfm.styles[i].type == stFootEndNote) {
      // Check subtype.
      FootEndNoteType footnotetype = (FootEndNoteType) usfm.styles[i].subtype;
      switch (footnotetype) {
      case fentFootnote:
        {
          // Store data for the markers and the anchor.
          opening_markers.push_back (usfm_get_full_opening_marker(usfm.styles[i].marker));
          closing_markers.push_back (usfm_get_full_closing_marker(usfm.styles[i].marker));
          note_numbering_type = (NoteNumberingType) usfm.styles[i].userint1;
          note_numbering_restart = (NoteNumberingRestartType) usfm.styles[i].userint2;
          note_numbering_user_sequence = usfm.styles[i].userstring1;
          break;
        }
      case fentEndnote:
        {
          // Endnotes not dealt with here.
          break;
        }
      case fentStandardContent:
        {
          // Standard content.
          if (!standardparagraph)
            standardparagraph = new XslFoFootnoteParagraph(usfm.styles[i].marker, usfm.styles[i].fontsize, usfm.styles[i].italic, usfm.styles[i].bold, usfm.styles[i].underline, usfm.styles[i].smallcaps, usfm.styles[i].justification, usfm.styles[i].spacebefore, usfm.styles[i].spaceafter, usfm.styles[i].leftmargin, usfm.styles[i].rightmargin, usfm.styles[i].firstlineindent, usfm.styles[i].userbool1);
          note_markers.insert(usfm.styles[i].marker);
          break;
        }
      case fentContent:
      case fentContentWithEndmarker:
        {
          // Store data for the footnote body.
          content_marker.push_back(usfm_get_full_opening_marker(usfm.styles[i].marker));
          content_apocrypha.push_back(usfm.styles[i].userbool1);
          note_markers.insert(usfm.styles[i].marker);
          break;
        }
      case fentParagraph:
        {
          // Store relevant data for a paragraph marker.
          if (!extraparagraph)
            extraparagraph = new XslFoFootnoteParagraph(usfm.styles[i].marker, usfm.styles[i].fontsize, usfm.styles[i].italic, usfm.styles[i].bold, usfm.styles[i].underline, usfm.styles[i].smallcaps, usfm.styles[i].justification, usfm.styles[i].spacebefore, usfm.styles[i].spaceafter, usfm.styles[i].leftmargin, usfm.styles[i].rightmargin, usfm.styles[i].firstlineindent, usfm.styles[i].userbool1);
          note_markers.insert(usfm.styles[i].marker);
          break;
        }
      }
    }
  }
  // Ensure that both of the paragraph styles are there.
  if (!standardparagraph)
    standardparagraph = new XslFoFootnoteParagraph("ft", 11, OFF, OFF, OFF, OFF, JUSTIFIED, 0, 0, 3, 0, 0, false);
  if (!extraparagraph)
    extraparagraph = new XslFoFootnoteParagraph("fp", 11, OFF, OFF, OFF, OFF, JUSTIFIED, 0, 0, 3, 0, 3, false);
  // Create footnote caller object.
  notecaller = new NoteCaller(note_numbering_type, note_numbering_user_sequence);
}


OdtFootnote::~OdtFootnote()
{
  delete notecaller;
  delete standardparagraph;
  delete extraparagraph;
}


void OdtFootnote::new_book()
{
  if (note_numbering_restart == nnrtBook)
    notecaller->reset();
  new_chapter();
}


void OdtFootnote::new_chapter()
{
  if (note_numbering_restart == nnrtChapter)
    notecaller->reset();
}


void OdtFootnote::transform(ustring & line)
// Replace all footnote related content with corresponding OpenDocument code.
{
  // Transform all of the possible markers for the note.
  for (unsigned int i = 0; i < opening_markers.size(); i++) {
    transform2 (line, opening_markers[i], closing_markers[i]);
  }
}


void OdtFootnote::transform2 (ustring& line, const ustring& opening_marker, const ustring& closing_marker)
// Replace all footnote related content with corresponding OpenDocument code.
{
  // Variables.
  size_t opening_position;

  // Look for footnotes, but only deal with them if they have the endmarker too.
  opening_position = line.find(opening_marker);
  while (opening_position != string::npos) {

    // Look for the endmarker.
    size_t closing_position;
    closing_position = line.find(closing_marker, opening_position);
    if (closing_position == string::npos) {
      gw_warning(_("Missing endmarker: ") + line);
      return;
    }
    // Take out this bit of the line, transform it, and insert it again.
    ustring footnote;
    footnote = line.substr(opening_position + opening_marker.length(), closing_position - opening_position - closing_marker.length());
    line.erase(opening_position, closing_position - opening_position + closing_marker.length());
    if (show) {
      footnote = transform_main_parts(footnote);
      line.insert(opening_position, footnote);
    }
    // Search for another footnote.
    opening_position = line.find(opening_marker, opening_position);
  }
}


ustring OdtFootnote::transform_main_parts(const ustring & line)
{
  // Variables.
  ustring odtcode;
  ustring s;

  // Work on a copy.
  ustring footnote(line);

  // Add first bit of code.
  odtcode.append("<text:note text:id=\"ftnxx\" text:note-class=\"footnote\">");
  replace_text(odtcode, "xx", convert_to_string(note_id));
  note_id++;

  // Extract the footnote caller.    
  ustring caller;
  if (footnote.length() > 0) {
    caller = footnote.substr(0, 1);
    caller = trim(caller);
    if (caller == "+") {
      caller = notecaller->get_caller();
      // if (note_numbering_restart == nnrtPage); See how to work with the various callers.
    } else if (caller == "-") {
      caller.clear();
    }
    footnote.erase(0, 1);
    footnote = trim(footnote);
  }
  // Insert the odt code.
  s = "<text:note-citation text:label=\"xx\">xx</text:note-citation>";
  replace_text(s, "xx", caller);
  odtcode.append(s);

  // We now come to the footnote body.
  odtcode.append("<text:note-body><text:p text:style-name=\"Footnote\">");

  // Divide the footnote into paragraphs, if there are more than one.
  vector < ustring > paragraphs;
  size_t pos;
  pos = footnote.find(extraparagraph->marker_open);
  while (pos != string::npos) {
    paragraphs.push_back(footnote.substr(0, pos));
    footnote.erase(0, pos + extraparagraph->marker_open.length());
    pos = footnote.find(extraparagraph->marker_open);
  }
  paragraphs.push_back(footnote);

  // Deal with each paragraph.
  for (unsigned int i = 0; i < paragraphs.size(); i++) {
    // Choose the right paragraph.
    XslFoFootnoteParagraph *paragraph;
    if (i == 0)
      paragraph = standardparagraph;
    else
      paragraph = extraparagraph;
    // Format actual text, and comply with footnote nesting, see USFM standard.
    paragraphs[i] = usfm_notes_handle_nesting(paragraphs[i], paragraph->marker_open, paragraph->marker_close, note_markers);

    // Insert the paragraph
    odtcode.append(paragraphs[i]);
  }

  // Close footnote and foot notebody.
  odtcode.append("</text:p></text:note-body></text:note>");

  // Return the code.
  return odtcode;
}


OdtEndnote::OdtEndnote(const Usfm & usfm)
// Stores the properties for all the endnote related styles.
{
  // Store and initialize variables.
  show = true;
  NoteNumberingType note_numbering_type = nntNumerical;
  note_numbering_restart = nnrtNever;
  ustring note_numbering_user_sequence;
  standardparagraph = NULL;
  extraparagraph = NULL;
  note_id = 0;

  // Go through all the styles.
  for (unsigned int i = 0; i < usfm.styles.size(); i++) {
    if (usfm.styles[i].type == stFootEndNote) {
      // Check subtype.
      FootEndNoteType footnotetype = (FootEndNoteType) usfm.styles[i].subtype;
      switch (footnotetype) {
      case fentFootnote:
        {
          // Footnotes not dealt with here.
          break;
        }
      case fentEndnote:
        {
          // Store data for the markers and the anchor.
          opening_marker = usfm_get_full_opening_marker(usfm.styles[i].marker);
          closing_marker = usfm_get_full_closing_marker(usfm.styles[i].marker);
          // note_numbering_type = (NoteNumberingType) usfm.styles[i].userint1;
          // note_numbering_restart = (NoteNumberingRestartType) usfm.styles[i].userint2;
          // note_numbering_user_sequence = usfm.styles[i].userstring1;
          break;
        }
      case fentStandardContent:
        {
          // Standard content.
          if (!standardparagraph)
            standardparagraph = new XslFoFootnoteParagraph(usfm.styles[i].marker, usfm.styles[i].fontsize, usfm.styles[i].italic, usfm.styles[i].bold, usfm.styles[i].underline, usfm.styles[i].smallcaps, usfm.styles[i].justification, usfm.styles[i].spacebefore, usfm.styles[i].spaceafter, usfm.styles[i].leftmargin, usfm.styles[i].rightmargin, usfm.styles[i].firstlineindent, usfm.styles[i].userbool1);
          note_markers.insert(usfm.styles[i].marker);
          break;
        }
      case fentContent:
      case fentContentWithEndmarker:
        {
          // Store data for the footnote body.
          content_marker.push_back(usfm_get_full_opening_marker(usfm.styles[i].marker));
          content_apocrypha.push_back(usfm.styles[i].userbool1);
          note_markers.insert(usfm.styles[i].marker);
          break;
        }
      case fentParagraph:
        {
          // Store relevant data for a paragraph marker.
          if (!extraparagraph)
            extraparagraph = new XslFoFootnoteParagraph(usfm.styles[i].marker, usfm.styles[i].fontsize, usfm.styles[i].italic, usfm.styles[i].bold, usfm.styles[i].underline, usfm.styles[i].smallcaps, usfm.styles[i].justification, usfm.styles[i].spacebefore, usfm.styles[i].spaceafter, usfm.styles[i].leftmargin, usfm.styles[i].rightmargin, usfm.styles[i].firstlineindent, usfm.styles[i].userbool1);
          note_markers.insert(usfm.styles[i].marker);
          break;
        }
      }
    }
  }
  // Ensure that both of the paragraph styles are there.
  if (!standardparagraph)
    standardparagraph = new XslFoFootnoteParagraph("ft", 11, OFF, OFF, OFF, OFF, JUSTIFIED, 0, 0, 3, 0, 0, false);
  if (!extraparagraph)
    extraparagraph = new XslFoFootnoteParagraph("fp", 11, OFF, OFF, OFF, OFF, JUSTIFIED, 0, 0, 3, 0, 3, false);
  // Create footnote caller object.
  notecaller = new NoteCaller(note_numbering_type, note_numbering_user_sequence);
}


OdtEndnote::~OdtEndnote()
{
  delete notecaller;
  delete standardparagraph;
  delete extraparagraph;
}


void OdtEndnote::new_book()
{
  if (note_numbering_restart == nnrtBook)
    notecaller->reset();
  new_chapter();
}


void OdtEndnote::new_chapter()
{
  if (note_numbering_restart == nnrtChapter)
    notecaller->reset();
}


void OdtEndnote::transform(ustring & line)
// Replace all endnote related content with corresponding OpenDocument code.
{
  // If no opening marker in stylesheet, bail out.
  if (opening_marker.empty())
    return;

  // Variables.
  size_t opening_position;

  // Look for endnotes, but only deal with them if they have the endmarker too.
  opening_position = line.find(opening_marker);
  while (opening_position != string::npos) {

    // Look for the endmarker.
    size_t closing_position;
    closing_position = line.find(closing_marker, opening_position);
    if (closing_position == string::npos) {
      gw_warning(_("Missing endmarker: ") + line);
      return;
    }
    // Take out this bit of the line, transform it, and insert it again.
    ustring note;
    note = line.substr(opening_position + opening_marker.length(), closing_position - opening_position - closing_marker.length());
    line.erase(opening_position, closing_position - opening_position + closing_marker.length());
    if (show) {
      note = transform_main_parts(note);
      line.insert(opening_position, note);
    }
    // Search for another footnote.
    opening_position = line.find(opening_marker, opening_position);
  }
}


ustring OdtEndnote::transform_main_parts(const ustring & line)
{
  // Variables.
  ustring odtcode;
  ustring s;

  // Work on a copy.
  ustring endnote(line);

  // Add first bit of code.
  odtcode.append("<text:note text:id=\"ednxx\" text:note-class=\"endnote\">");
  replace_text(odtcode, "xx", convert_to_string(note_id));
  note_id++;

  // Extract the footnote caller.    
  ustring caller;
  if (endnote.length() > 0) {
    caller = endnote.substr(0, 1);
    caller = trim(caller);
    if (caller == "+") {
      caller = notecaller->get_caller();
      // if (note_numbering_restart == nnrtPage); See how to work with the various callers.
    } else if (caller == "-") {
      caller.clear();
    }
    endnote.erase(0, 1);
    endnote = trim(endnote);
  }
  // Insert the odt code.
  s = "<text:note-citation text:label=\"xx\">xx</text:note-citation>";
  replace_text(s, "xx", caller);
  odtcode.append(s);

  // We now come to the footnote body.
  odtcode.append("<text:note-body><text:p text:style-name=\"Endnote\">");

  // Divide the footnote into paragraphs, if there are more than one.
  vector < ustring > paragraphs;
  size_t pos;
  pos = endnote.find(extraparagraph->marker_open);
  while (pos != string::npos) {
    paragraphs.push_back(endnote.substr(0, pos));
    endnote.erase(0, pos + extraparagraph->marker_open.length());
    pos = endnote.find(extraparagraph->marker_open);
  }
  paragraphs.push_back(endnote);

  // Deal with each paragraph.
  for (unsigned int i = 0; i < paragraphs.size(); i++) {
    // Choose the right paragraph.
    XslFoFootnoteParagraph *paragraph;
    if (i == 0)
      paragraph = standardparagraph;
    else
      paragraph = extraparagraph;
    // Format actual text, and comply with footnote nesting, see USFM standard.
    paragraphs[i] = usfm_notes_handle_nesting(paragraphs[i], paragraph->marker_open, paragraph->marker_close, note_markers);

    // Insert the paragraph
    odtcode.append(paragraphs[i]);
  }

  // Close footnote and foot notebody.
  odtcode.append("</text:p></text:note-body></text:note>");

  // Return the code.
  return odtcode;
}


OdtXref::OdtXref(const Usfm & usfm)
// Stores the properties for all the xref related styles.
{
  // Store and initialize variables.
  show = true;
  NoteNumberingType note_numbering_type = nntAlphabetical;
  note_numbering_restart = nnrtChapter;
  ustring note_numbering_user_sequence;
  standardparagraph = NULL;
  note_id = 0;
  // Go through all the styles.
  for (unsigned int i = 0; i < usfm.styles.size(); i++) {
    if (usfm.styles[i].type == stCrossreference) {
      // Check subtype.
      CrossreferenceType xreftype = (CrossreferenceType) usfm.styles[i].subtype;
      switch (xreftype) {
      case ctCrossreference:
        {
          // Store data for the markers and the anchor.
          opening_marker = usfm_get_full_opening_marker(usfm.styles[i].marker);
          closing_marker = usfm_get_full_closing_marker(usfm.styles[i].marker);
          note_numbering_type = (NoteNumberingType) usfm.styles[i].userint1;
          note_numbering_restart = (NoteNumberingRestartType) usfm.styles[i].userint2;
          note_numbering_user_sequence = usfm.styles[i].userstring1;
          break;
        }
      case ctStandardContent:
        {
          // Standard content.
          if (!standardparagraph)
            standardparagraph = new XslFoFootnoteParagraph(usfm.styles[i].marker, usfm.styles[i].fontsize, usfm.styles[i].italic, usfm.styles[i].bold, usfm.styles[i].underline, usfm.styles[i].smallcaps, usfm.styles[i].justification, usfm.styles[i].spacebefore, usfm.styles[i].spaceafter, usfm.styles[i].leftmargin, usfm.styles[i].rightmargin, usfm.styles[i].firstlineindent, usfm.styles[i].userbool1);
          note_markers.insert(usfm.styles[i].marker);
          break;
        }
      case ctContent:
      case ctContentWithEndmarker:
        {
          // Store data for the footnote body.
          content_marker.push_back(usfm_get_full_opening_marker(usfm.styles[i].marker));
          content_apocrypha.push_back(usfm.styles[i].userbool1);
          note_markers.insert(usfm.styles[i].marker);
          break;
        }
      }
    }
  }
  // Ensure that the paragraph style is there.
  if (!standardparagraph)
    standardparagraph = new XslFoFootnoteParagraph("xt", 11, OFF, OFF, OFF, OFF, JUSTIFIED, 0, 0, 3, 0, 0, false);
  // Create caller object.
  notecaller = new NoteCaller(note_numbering_type, note_numbering_user_sequence);
}


OdtXref::~OdtXref()
{
  delete notecaller;
  delete standardparagraph;
}


void OdtXref::new_book()
{
  if (note_numbering_restart == nnrtBook)
    notecaller->reset();
  new_chapter();
}


void OdtXref::new_chapter()
{
  if (note_numbering_restart == nnrtChapter)
    notecaller->reset();
}


void OdtXref::transform(ustring & line)
// Replace all xref related content with corresponding OpenDocument code.
{
  // If no opening marker in stylesheet, bail out.
  if (opening_marker.empty())
    return;

  // Variables.
  size_t opening_position;

  // Look for notes, but only deal with them if they have the endmarker too.
  opening_position = line.find(opening_marker);
  while (opening_position != string::npos) {

    // Look for the endmarker.
    size_t closing_position;
    closing_position = line.find(closing_marker, opening_position);
    if (closing_position == string::npos) {
      gw_warning(_("Missing endmarker: ") + line);
      return;
    }
    // Take out this bit of the line, transform it, and insert it again.
    ustring footnote;
    footnote = line.substr(opening_position + opening_marker.length(), closing_position - opening_position - closing_marker.length());
    line.erase(opening_position, closing_position - opening_position + closing_marker.length());
    if (show) {
      footnote = transform_main_parts(footnote);
      line.insert(opening_position, footnote);
    }
    // Search for another footnote.
    opening_position = line.find(opening_marker, opening_position);
  }
}


ustring OdtXref::transform_main_parts(const ustring & line)
{
  // Variables.
  ustring odtcode;
  ustring s;

  // Work on a copy.
  ustring footnote(line);

  // Add first bit of code.
  odtcode.append("<text:note text:id=\"xrnxx\" text:note-class=\"crossreference\">");
  replace_text(odtcode, "xx", convert_to_string(note_id));
  note_id++;

  // Extract the note caller.    
  ustring caller;
  if (footnote.length() > 0) {
    caller = footnote.substr(0, 1);
    caller = trim(caller);
    if (caller == "+") {
      caller = notecaller->get_caller();
      // if (note_numbering_restart == nnrtPage); See how to work with the various callers.
    } else if (caller == "-") {
      caller.clear();
    }
    footnote.erase(0, 1);
    footnote = trim(footnote);
  }
  // Insert the odt code.
  s = "<text:note-citation text:label=\"xx\">xx</text:note-citation>";
  replace_text(s, "xx", caller);
  odtcode.append(s);

  // We now come to the footnote body.
  odtcode.append("<text:note-body><text:p text:style-name=\"Crossreference\">");

  // Deal with the xref.
  {
    // Format actual text, and comply with footnote nesting, see USFM standard.
    footnote = usfm_notes_handle_nesting(footnote, standardparagraph->marker_open, standardparagraph->marker_close, note_markers);

    // Insert the paragraph
    odtcode.append(footnote);
  }

  // Close footnote and foot notebody.
  odtcode.append("</text:p></text:note-body></text:note>");

  // Return the code.
  return odtcode;
}
