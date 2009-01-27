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

#include "biblenotes.h"
#include "utilities.h"
#include "usfmtools.h"
#include "tiny_utilities.h"

ustring biblenotes_get_note_marker(NoteType notetype)
// Gets the opening and closing marker, e.g. "f".
{
  ustring marker;
  switch (notetype) {
  case indtFootnote:{
      marker = "f";
      break;
    }
  case indtEndnote:{
      marker = "fe";
      break;
    }
  case indtCrossreference:{
      marker = "x";
      break;
    }
  }
  return marker;
}

NoteType biblenotes_get_note_type(const ustring & text)
// Gets the note type, e.g. indtFootnote.
{
  ustring marker(text);
  marker = trim(marker);
  if (!marker.empty()) {
    if (marker.substr(0, 1) == "\\")
      marker.erase(0, 1);
    replace_text(marker, "*", "");
    if (marker == biblenotes_get_note_marker(indtEndnote))
      return indtEndnote;
    if (marker == biblenotes_get_note_marker(indtCrossreference))
      return indtCrossreference;
  }
  return indtFootnote;
}

ustring biblenotes_get_note_caller(NumberingType numberingtype, char *character)
// Gets the caller, e.g. "+".
{
  ustring caller;
  switch (numberingtype) {
  case ntAutomatic:{
      caller = "+";
      break;
    }
  case ntNone:{
      caller = "-";
      break;
    }
  case ntCharacter:{
      caller = character;
      break;
    }
  }
  return caller;
}

void biblenotes_get_note_numbering(const ustring & caller, NumberingType & numberingtype, ustring & character)
// Given the "caller", it returns:
// - the "NumberingType", and
// - any possible character used in custom numbering.
{
  if (caller == "+")
    numberingtype = ntAutomatic;
  else if (caller == "-")
    numberingtype = ntNone;
  else {
    numberingtype = ntCharacter;
    character = caller;
  }
}

void biblenotes_extract_note_opener_caller_closer(const ustring & text, ustring & opener, ustring & caller, ustring & closer)
// Takes the full note text, e.g.:
// \f + \fr 1.1: \fk zokuzidla: \fl Heb. \fq ezinkulu.\f*
// Returns the main body of it, e.g.: 
// \fr 1.1: \fk zokuzidla: \fl Heb. \fq ezinkulu.
{
  Parse parse(text, false);
  if (parse.words.size() < 3)
    return;
  opener = parse.words[0];
  caller = parse.words[1];
  closer = text;
  size_t pos = closer.find_last_of("\\");
  if (pos != string::npos)
    closer.erase(0, pos);
}

ustring biblenotes_extract_note_body(ustring text)
// Takes the full note text, e.g.:
// \f + \fr 1.1: \fk zokuzidla: \fl Heb. \fq ezinkulu.\f*
// Returns the main body of it, e.g.: 
// \fr 1.1: \fk zokuzidla: \fl Heb. \fq ezinkulu.
{
  // Remove the opener and the caller, e.g. the \f and the +.
  for (unsigned int i = 0; i < 2; i++) {
    size_t pos = text.find(" ");
    if (pos != string::npos)
      text.erase(0, ++pos);
  }
  // Remove the closer, e.g. \f*
  size_t pos = text.find_last_of("\\");
  if (pos != string::npos)
    text.erase(pos, 10);
  // Return result.
  return text;
}

void biblenotes_switch_markers(ustring & bodytext, gchar * from_marker, gchar * to_marker)
// Changes "bodytext", switching from "from_marker" to "to_marker".
// It changes both opening and closing markers.
{
  ustring oldmarker, newmarker;
  oldmarker = usfm_get_full_opening_marker(from_marker);
  newmarker = usfm_get_full_opening_marker(to_marker);
  replace_text(bodytext, oldmarker, newmarker);
  oldmarker = usfm_get_full_closing_marker(from_marker);
  newmarker = usfm_get_full_closing_marker(to_marker);
  replace_text(bodytext, oldmarker, newmarker);
}
