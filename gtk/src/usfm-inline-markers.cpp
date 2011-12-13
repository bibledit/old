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

#include "libraries.h"
#include "utilities.h"
#include "usfm.h"
#include "usfmtools.h"
#include "constants.h"
#include "color.h"
#include "tiny_utilities.h"

UsfmInlineMarkers::UsfmInlineMarkers(const Usfm & usfm)
// Creates a list of all inline markers, with their properties.
{
  // Go through all the styles and look for inline ones.
  for (unsigned int i = 0; i < usfm.styles.size(); i++) {
    bool is_inline = false;
    if (usfm.styles[i].type == stInlineText)
      is_inline = true;
    if (usfm.styles[i].type == stFootEndNote) {
      if (usfm.styles[i].subtype == fentContent)
        is_inline = true;
      if (usfm.styles[i].subtype == fentContentWithEndmarker)
        is_inline = true;
    }
    if (usfm.styles[i].type == stCrossreference) {
      if (usfm.styles[i].subtype == ctContent)
        is_inline = true;
      if (usfm.styles[i].subtype == ctContentWithEndmarker)
        is_inline = true;
    }
    if (is_inline) {
      // Inline marker found. Store opening marker, basic and full.
      markers.push_back(usfm.styles[i].marker);
      string s;
      s = usfm_get_full_opening_marker(usfm.styles[i].marker);
      opening_markers.push_back(s);
      // Store closing marker.      
      s = usfm_get_full_closing_marker(usfm.styles[i].marker);
      closing_markers.push_back(s);
      // Store the relevant properties of this style.
      italic.push_back(usfm.styles[i].italic);
      bold.push_back(usfm.styles[i].bold);
      underline.push_back(usfm.styles[i].underline);
      smallcaps.push_back(usfm.styles[i].smallcaps);
      superscript.push_back(usfm.styles[i].superscript);
      color.push_back(usfm.styles[i].color);
    }
  }
  // One and the same closing marker for everything.
  closing_odt_markup = "</text:span>";
}

UsfmInlineMarkers::~UsfmInlineMarkers()
{
}

string UsfmInlineMarkers::opening_sword_markup(int index)
// This one generates the opening sword markup.
// It uses 'index' for the index in the opening and closing markers, as there 
// can be many.
/*
From OSIS User Manual 2006:
The hi element provides a simple text highlighting mechanism. Here are the types:
 acrostic
 bold 
 emphasis 
 illuminated 
 italic 
 line-through 
 normal
 small-caps 
 sub 
 super 
 underline
Example: <hi type="italic">the child with his mother Mary</hi>
*/
{
  unsigned int opener_count = 0;
  string opening_sword_markup;
  // Fontpercentage not dealt with.
  // Deal with italics.
  if ((italic[index] == ON) || (italic[index] == TOGGLE)) {
    opening_sword_markup.append("<hi type=\"italic\">");
    opener_count++;
  } else if (italic[index] == OFF) ;
  else if (italic[index] == INHERIT) ;
  // Deal with bold.
  if ((bold[index] == ON) || (bold[index] == TOGGLE)) {
    opening_sword_markup.append("<hi type=\"bold\">");
    opener_count++;
  } else if (bold[index] == OFF) ;
  else if (bold[index] == INHERIT) ;
  // Deal with underline.
  if ((underline[index] == ON) || (underline[index] == TOGGLE)) {
    opening_sword_markup.append("<hi type=\"underline\">");
    opener_count++;
  } else if (underline[index] == OFF) ;
  else if (underline[index] == INHERIT) ;
  // Deal with smallcaps.
  if ((smallcaps[index] == ON) || (smallcaps[index] == TOGGLE)) {
    opening_sword_markup.append("<hi type=\"small-caps\">");
    opener_count++;
  } else if (smallcaps[index] == OFF) ;
  else if (smallcaps[index] == INHERIT) ;
  // Deal with superscript.
  if (superscript[index]) {
    opening_sword_markup.append("<hi type=\"super\">");
    opener_count++;
  }
  // Depending on opening markup, generate closing one.
  closing_sword_markup.clear();
  for (unsigned int i = 0; i < opener_count; i++)
    closing_sword_markup.append("</hi>");
  // Return markup.
  return opening_sword_markup;
}

ustring UsfmInlineMarkers::opening_odt_markup(int index)
// This generates the opening OpenDocument markup.
// It uses 'index' for the index in the markers.
{
  ustring markup = "<text:span text:style-name=\"xx\">";
  replace_text(markup, "xx", markers[index]);
  return markup;
}
