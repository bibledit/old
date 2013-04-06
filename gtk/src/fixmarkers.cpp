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

#include "fixmarkers.h"
#include "settings.h"
#include "projectutils.h"
#include "gtkwrappers.h"
#include "progresswindow.h"
#include "utilities.h"
#include "usfmtools.h"
#include "tiny_utilities.h"

void fix_markers_insert_space_after_opener()
/*
Using Check / Marker / Validate in CEV, John 1:5 has:
\q1\x - \xo 1.6: \xt Mt 3.1; Mk 1.4; Lk 3.1,2.\x*
and BE complains about the unknown USFM \q1\x.
Same with John 4:45:
\p\x - \xo 4.45: \xt Jn 2.23.\x*
And John 15:20:
\pi\x - \xo 15.20: \xt Mt 10.24; Lk 6.40; Jn 13.16.\x*
The text given violates the USFM 2 standard, and therefore bibledit
correctly complains about it.

This tool inserts a space after an opening marker that is not followed by 
a space. 
But after a closing marker it does not insert spaces, because the * is enough.
*/
{
  // Settings.
  extern Settings *settings;

  // Progress.
  ProgressWindow progresswindow("Inserting missing spaces after opening markers", false);

  // Statistics.
  unsigned int insertcount = 0;

  // Go through the books in the project.
  ustring project = settings->genconfig.project_get();
  vector < unsigned int >books = project_get_books(project);
  progresswindow.set_iterate(0, 1, books.size());
  for (unsigned int bk = 0; bk < books.size(); bk++) {

    // Progress.
    progresswindow.iterate();

    // Go through the chapters in this book.
    vector < unsigned int >chapters = project_get_chapters(project, books[bk]);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {

      // Go through the verses in this chapter.
      vector < ustring > verses = project_get_verses(project, books[bk], chapters[ch]);
      for (unsigned int vs = 0; vs < verses.size(); vs++) {

        // Retrieve each verse and process it.
        ustring text = project_retrieve_verse(project, books[bk], chapters[ch], verses[vs]);
        unsigned int previous_insertcount = insertcount;

        vector < ustring > markers = usfm_get_all_markers(text);
        for (unsigned int i = 0; i < markers.size(); i++) {
          if (markers[i].find("\\") != string::npos) {
            size_t position = text.find(markers[i]);
            if (position != string::npos) {
              ustring marker = text.substr(position, markers[i].length());
              replace_text(marker, "\\", " \\");
              insertcount++;
              text.replace(position, markers[i].length(), marker);
            }
          }
        }

        // If there were changes, store the verse.
        if (insertcount != previous_insertcount) {
          project_store_verse(project, books[bk], chapters[ch], verses[vs], text);
        }
      }
    }
  }

  // Give statistics.
  ustring message("Missing spaces inserted: " + convert_to_string(insertcount));
  gtkw_dialog_info(NULL, message);
}
