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

#include "check_count_usfms.h"
#include "projectutils.h"
#include "settings.h"
#include "stylesheetutils.h"
#include "utilities.h"
#include "usfmtools.h"
#include "books.h"
#include "checks.h"

CheckCountUsfms::CheckCountUsfms(const ustring & project, const vector < unsigned int >&books, CheckSortType sorttype, bool gui)
/*
It counts the USFM in the project.
project: project to check.
books: books to check; if empty it checks them all.
sorttype: if/how to sort.
gui: show graphical progressbar.
*/
{
  // Initialize variables.
  cancelled = false;
  // Get a list of the books to check. If no books were given, take them all.
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);
  // GUI.
  progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow("Counting markers", true);
    progresswindow->set_iterate(0, 1, mybooks.size());
  }
  // Check each book.
  for (unsigned int bk = 0; bk < mybooks.size(); bk++) {
    if (gui) {
      progresswindow->iterate();
      if (progresswindow->cancel) {
        cancelled = true;
        return;
      }
    }
    // Get text of the book and go through each line.
    vector < ustring > lines = project_retrieve_book(project, mybooks[bk]);
    for (unsigned int ln = 0; ln < lines.size(); ln++) {
      // Extract the markers, and deal with them.
      ustring marker = usfm_extract_marker_within_line(lines[ln]);
      // Discard lines without a marker and get more markers per line.
      while (!marker.empty()) {
        // Look whether this particular USFM is already in the list.
        int found_position = -1;
        for (unsigned int i3 = 0; i3 < markers.size(); i3++) {
          if (marker == markers[i3]) {
            found_position = i3;
            break;
          }
        }
        if (found_position >= 0) {
          // This USfM has been encountered before: increase its count.
          ++counts[found_position];
        } else {
          // This is a new USFM: add it and set its count to one.
          markers.push_back(marker);
          counts.push_back(1);
        }
        marker = usfm_extract_marker_within_line(lines[ln]);
      }
    }
  }
  // Sorting, if requested.
  switch (sorttype) {
  case cstSort0:
    break;
  case cstSort1:
    quick_sort(markers, counts, 0, markers.size());
    break;
  case cstSort2:
    quick_sort(counts, markers, 0, counts.size());
    break;
  case cstSort3:
    break;
  }
}

CheckCountUsfms::~CheckCountUsfms()
{
  if (progresswindow)
    delete progresswindow;
}
