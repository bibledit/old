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
#include "xmlutils.h"
#include <glib.h>
#include "constants.h"

void xml_sort_positions(vector < size_t > &positions, vector < size_t > &lengths)
/*
  This sorts positions in a ustring, and lengths of any word to operated on.
  It sorts on positions, and the lengths are kept attached to their positions.
*/
{
  // If there are zero or one positions, there is nothing to sort.
  if (positions.size() < 2)
    return;
  // Make combined numbers, made up of the positions and the lengths.
  vector < unsigned long >combination;
  for (unsigned int i = 0; i < positions.size(); i++) {
    combination.push_back((1000000 * positions[i]) + lengths[i]);
  }
  // Sort these combined numbers.  
  sort(combination.begin(), combination.end());
  // Split the combined numbers again into positions and lengths.  
  positions.clear();
  lengths.clear();
  for (unsigned int i = 0; i < combination.size(); i++) {
    positions.push_back(combination[i] / 1000000);
    lengths.push_back(combination[i] % 1000000);
  }
}


void xml_combine_overlaps(vector < size_t > &positions, vector < size_t > &lengths)
// If there are words in a line that are to be highlighted, and some of 
// these words overlap, combine these words into one string to be highlighted.
{
  // If there are less than 2 positions, there can not be any overlap. Return.
  if (positions.size() < 2)
    return;
  // We presume that the positions have been sorted. That's only how it can work.
  bool position_was_removed;
  do {
    position_was_removed = false;
    vector < size_t >::iterator iterpos(positions.begin());
    vector < size_t >::iterator iterlen(lengths.begin());
    for (unsigned int i = 0; i < positions.size() - 1; i++) {
      // Let the two iterators point to the second position that might have to be removed.
      iterpos++;
      iterlen++;
      // Get the two boundaries of the current position.
      unsigned int lower_bound = positions[i];
      unsigned int upper_bound = positions[i] + lengths[i];
      // Get the boundary of the following positions.
      unsigned int second_bound = positions[i + 1];
      // Does the following position start somewhere between the boundaries?
      if ((second_bound >= lower_bound) && (second_bound <= upper_bound)) {
        // There is overlap. Get the highest upper boundary of the two positions.
        unsigned int highest_bound = MAX(upper_bound, second_bound + lengths[i + 1]);
        lengths[i] = highest_bound - positions[i];
        positions.erase(iterpos);
        lengths.erase(iterlen);
        position_was_removed = true;
        break;
      }
    }
  } while (position_was_removed);
}

void xml_html_insert_emphasis(ustring & line, vector < size_t > &positions, vector < size_t > &lengths)
// This inserts the <b> tag to get the text bold.
{
  for (unsigned int i = 0; i < positions.size(); i++) {
    // Text to insert to highlight it.
    ustring taggedtext = "<b>" + line.substr(positions[i], lengths[i]) + "</b>";
    // Insert tag.
    line.replace(positions[i], lengths[i], taggedtext);
    // Push any following positions up.
    for (unsigned int i2 = i + 1; i2 < positions.size(); i2++) {
      positions[i2] = positions[i2] + 7;
    }
  }
}

void xml_handle_entities(ustring & line, vector < size_t > *positions)
/*
  Changes the < and the > in the text to the xml entities &lt; and &gt
  Changes the ampersand (&) to &amp;
  Any positions affected by this will be adjusted.
*/
{
  size_t offposition;
  // Deal with &. This one is done first, 
  // else the ampersands inserted later will get changed too.
  offposition = line.find("&");
  while (offposition != string::npos) {
    line.replace(offposition, 1, "&amp;");
    if (positions)
      xml_positions_push_up(offposition, 4, *positions);
    // Do not repeat on the & just removed and inserted, but start searching after it.
    offposition = line.find("&", offposition + 3);
  }
  // Deal with <
  offposition = line.find("<");
  while (offposition != string::npos) {
    line.replace(offposition, 1, "&lt;");
    if (positions)
      xml_positions_push_up(offposition, 3, *positions);
    offposition = line.find("<", offposition);
  }
  // Deal with >
  offposition = line.find(">");
  while (offposition != string::npos) {
    line.replace(offposition, 1, "&gt;");
    if (positions)
      xml_positions_push_up(offposition, 3, *positions);
    offposition = line.find(">", offposition);
  }
}

void xml_positions_push_up(size_t position, size_t up, vector < size_t > &positions)
// Pushes the positions in variable "positions" up with "up" steps,
// when the position is greater than "position"
{
  for (unsigned int i = 0; i < positions.size(); i++) {
    if (positions[i] > position) {
      positions[i] = positions[i] + up;
    }
  }
}

ustring xml_text_embed_in_tags(int level, const ustring tag, const ustring & text)
{
  // Handle < and > and &.
  ustring text2(text);
  xml_handle_entities(text2, NULL);
  // Embed the text.
  ustring result;
  for (int i = 0; i < level; i++)
    result.append("  ");
  result.append("<" + tag + ">" + text2 + "</" + tag + ">");
  return result;
}

ustring xml_tag(int level, const ustring tag, bool closing)
{
  ustring result;
  for (int i = 0; i < level; i++)
    result.append("  ");
  result.append("<");
  if (closing)
    result.append("/");
  result.append(tag);
  result.append(">");
  return result;
}
