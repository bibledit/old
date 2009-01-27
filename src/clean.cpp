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

#include "utilities.h"
#include "clean.h"
#include "usfmtools.h"
#include "tiny_utilities.h"

const char *markers_that_dont_split[] = {
  "th1", "th2", "th3", "th4", "thr1", "thr2", "thr3", "thr4",
  "tc1", "tc2", "tc3", "tc4", "tcr1", "tcr2", "tcr3", "tcr4",
  "f", "fe", "fm", "fr", "fk", "fq", "fqa", "fl", "fp", "fv", "ft", "fdc",
  "f*", "fe*", "fm*", "fr*", "fk*", "fq*", "fqa*", "fl*", "fp*", "fv*", "ft*", "fdc*",
  "x", "xo", "xk", "xq", "xt", "xdc",
  "x*", "xo*", "xk*", "xq*", "xt*", "xdc*",
  "qt", "nd", "tl", "dc", "bk", "sig", "pn", "wj", "k", "sls", "ord", "add", "lit",
  "qt*", "nd*", "tl*", "dc*", "bk*", "sig*", "pn*", "wj*", "k*", "sls*", "ord*", "add*", "lit*",
  "no", "bd", "it", "bdit", "em", "sc",
  "no*", "bd*", "it*", "bdit*", "em*", "sc*",
  "pro", "w", "wh", "wg", "ndx",
  "pro*", "w*", "wh*", "wg*", "ndx*",
  "n", "n*"                     // These two are used in old projects.
};

const char *markers_that_join_to_previous_line[] = {
  "f*", "fe*", "x*", "wj*", "add*", "it", "it*"
};

CleanUsfm::CleanUsfm(vector < ustring > &input)
// Clean the lines provided: each main USFM starts a new line.
{
  // Markers that don't split a line.
  set < ustring > nonsplitters;
  for (unsigned int i = 0; i < sizeof(markers_that_dont_split) / sizeof(*markers_that_dont_split); i++)
    nonsplitters.insert(markers_that_dont_split[i]);

  // Markers that join to the previous line.
  set < ustring > joiners;
  for (unsigned int i = 0; i < sizeof(markers_that_join_to_previous_line) / sizeof(*markers_that_join_to_previous_line); i++)
    joiners.insert(markers_that_join_to_previous_line[i]);

  // Go through all lines.
  for (unsigned int i = 0; i < input.size(); i++) {

    // Change tabs to spaces.
    size_t position;
    position = input[i].find("\t");
    while (position != string::npos) {
      input[i].replace(position, 1, " ");
      position = input[i].find("\t", position);
    }

    // Change multiple spaces to one space.
    position = input[i].find("  ");
    while (position != string::npos) {
      input[i].replace(position, 2, " ");
      position = input[i].find("  ");
    }

    // Cut into handsome lines, according to the USFM standard.
    vector < ustring > markers;
    markers = usfm_get_all_markers(input[i]);
    for (unsigned int i2 = 0; i2 < markers.size(); i2++) {
      if (nonsplitters.find(markers[i2]) == nonsplitters.end()) {
        split_line_on_marker(markers[i2], input[i]);
      }
    }

    // Add a line that does not start with a USFM to the previous one, or one
    // that starts with a marker supposed to be joined to the previous line.
    ustring line(input[i]);
    ustring marker = usfm_extract_marker(line);
    if ((marker.empty()) || (joiners.find(marker) != joiners.end())) {
      lines[lines.size() - 1].append(" ");
      lines[lines.size() - 1].append(input[i]);
      continue;
    }
    // Separate and trim these lines.
    position = input[i].find("\n");
    while (position != string::npos) {
      ustring line = input[i].substr(0, position);
      line = trim(line);
      if (!line.empty())
        lines.push_back(line);
      input[i].erase(0, ++position);
      position = input[i].find("\n");
    }

    // Add to output.
    if (!input[i].empty())
      lines.push_back(trim(input[i]));
  }
}
