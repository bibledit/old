/*
 Copyright (©) 2003-2016 Teus Benschop.
 
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


#include <sources/styles.h>
#include <filter/string.h>
#include <filter/url.h>


// Parses usfm.sty (see https://github.com/ubsicap/usfm) and updates values in the default stylesheet.
void sources_styles_parse ()
{
#ifdef HAVE_CLOUD
  
  // Read the default stylesheet for Paratext projects.
  string path = filter_url_create_root_path ("sources", "usfm.sty");
  string contents = filter_url_file_get_contents (path);
  vector <string> paratext_lines = filter_string_explode (contents, '\n');
  
  // Parse state variables.
  string paratext_marker;

  // Parser signatures.
  string backslash_marker = "\\Marker ";
  
  // Parse the stylesheet.
  for (auto paratext_line : paratext_lines) {
    
    // An empty line: End of style block reached.
    paratext_line = filter_string_trim (paratext_line);
    if (paratext_line.empty ()) {
      paratext_marker.clear ();
      continue;
    }
    
    // Look for the start of a style block, like:
    // \Marker id
    if (paratext_line.find (backslash_marker) == 0) {
      paratext_line.erase (0, backslash_marker.length ());
      string marker = filter_string_trim (paratext_line);
      // Skip markers in the z-area.
      if (marker [0] == 'z') continue;
      // A new style block starts here.
      paratext_marker = marker;
      cout << paratext_marker << endl;
      continue;
    }
    
  }
  cout << "Finished parsing style values from the usfm.sty file" << endl;
#endif
}
