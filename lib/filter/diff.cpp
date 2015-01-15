/*
 Copyright (©) 2003-2015 Teus Benschop.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include <filter/diff.h>
#include <filter/string.h>
#include <dtl/dtl.hpp>
using dtl::Diff;


// This filter returns the diff of two input strngs.
// $oldstring: The old string for input.
// $newstring: The new string for input.
// The function returns the differences marked.
string filter_diff_diff (string oldstring, string newstring)
{
  // Type definitions for the diff template engine.
  typedef string elem;
  typedef vector <string> sequence;

  // Split the input up into words.
  // It compares with word granularity.
  sequence oldvector = filter_string_explode (oldstring, ' ');
  sequence newvector = filter_string_explode (newstring, ' ');
  
  // Run the diff engine.
  Diff <elem> d (oldvector, newvector);
  d.compose();
  
  // Get the shortest edit distance.
  stringstream result;
  d.printSES (result);
  
  // Add html markup for bold and strikethrough.
  vector <string> output = filter_string_explode (result.str(), '\n');
  for (auto & line : output) {
    if (line.empty ()) continue;
    char indicator = line.front ();
    line.erase (0, 1);
    if (indicator == '+') {
      line.insert (0, "<span style=\"font-weight: bold;\">");
      line.append ("</span>");
    }
    if (indicator == '-') {
      line.insert (0, "<span style=\"text-decoration: line-through;\">");
      line.append ("</span>");
    }
  }
  
  // Result.
  string html = filter_string_implode (output, " ");
  return html;
}


// This calculates the similarity between the old and new strings.
// It returns the similarity as a percentage.
// 100% means that the text is completely similar.
// And 0% means that the text is completely different.
// The output ranges from 0 to 100%.
int filter_diff_similarity (string oldstring, string newstring)
{
  // Type definitions for the diff template engine.
  typedef string elem;
  typedef vector <string> sequence;
  
  // Split the input up into unicode characers.
  sequence oldvector;
  sequence newvector;
  size_t oldlength = unicode_string_length (oldstring);
  for (size_t i = 0; i < oldlength; i++) {
    oldvector.push_back (unicode_string_substr (oldstring, i, 1));
  }
  size_t newlength = unicode_string_length (newstring);
  for (size_t i = 0; i < newlength; i++) {
    newvector.push_back (unicode_string_substr (newstring, i, 1));
  }
  
  // Run the diff engine.
  Diff <elem> d (oldvector, newvector);
  d.compose();
  
  // Get the shortest edit distance.
  stringstream result;
  d.printSES (result);
  
  // Calculate the total elements compared, and the total differences found.
  int element_count = 0;
  int similar_count = 0;
  vector <string> output = filter_string_explode (result.str(), '\n');
  for (auto & line : output) {
    if (line.empty ()) continue;
    element_count++;
    char indicator = line.front ();
    if (indicator == ' ') similar_count++;
  }
  
  // Calculate the percentage similarity.
  int percentage = round (100 * ((float) similar_count / (float) element_count));
  return percentage;
}