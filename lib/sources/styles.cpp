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


string sources_style_parse_expand_key (string key)
{
  return "/* " + key + " */";
}


bool sources_style_parse_cpp_element (string & line, string signature)
{
  signature = sources_style_parse_expand_key (signature);
  size_t pos = line.find (signature);
  if (pos == string::npos) return false;
  line.erase (0, pos + signature.length ());
  line = filter_string_trim (line);
  if (line [line.size () - 1] == ',') line = line.erase (line.size () - 1);
  if (line [line.size () - 1] == '"') line = line.erase (line.size () - 1);
  if (line [0] == '"') line.erase (0, 1);
  return true;
}


// Parses usfm.sty (see https://github.com/ubsicap/usfm) and updates values in the default stylesheet.
void sources_styles_parse ()
{

#ifdef HAVE_CLOUD

  // Read the C++ source files with the default style definitions.
  string cpp_path = filter_url_create_root_path ("database", "styles.cpp");
  string contents = filter_url_file_get_contents (cpp_path);
  string cpp_start = "style_record styles_table [] =";
  string cpp_end = "};";
  size_t pos = contents.find (cpp_start);
  if (pos == string::npos) {
    cout << "Cannot find default styles signature: " << cpp_start << endl;
    return;
  }
  contents.erase (0, pos + cpp_start.size() + 1);
  pos = contents.find (cpp_end);
  if (pos == string::npos) {
    cout << "Cannot find default styles signature: " << cpp_end << endl;
    return;
  }
  contents.erase (pos);
  vector <string> cpp_lines = filter_string_explode (contents, '\n');

  // Parsing signatures.
  const char * marker_key = "marker";
  const char * name_key = "name";

  // Parse the default style definitions.
  typedef map <string, string> definition_type;
  definition_type style_definition;
  map <string, definition_type> style_definitions;
  string marker;
  for (auto line : cpp_lines) {
    if (sources_style_parse_cpp_element (line, marker_key)) {
      marker = line;
      style_definitions [marker] [marker_key] = marker;
    }
    if (sources_style_parse_cpp_element (line, name_key)) {
      style_definitions [marker] [name_key] = line;
    }
  }
  
  
  // Read the default stylesheet for Paratext projects.
  string path = filter_url_create_root_path ("sources", "usfm.sty");
  contents = filter_url_file_get_contents (path);
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
      //cout << paratext_marker << endl;
      continue;
    }
    
  }
  

  // Build the C++ fragment with the default styles.
  cpp_lines.clear ();
  for (auto element : style_definitions) {
    definition_type style_definition = element.second;
    cpp_lines.push_back ("  {");
    string line = "    " + sources_style_parse_expand_key (marker_key) + " \"" + style_definition [marker_key] + "\",";
    cpp_lines.push_back (line);
    line = "    " + sources_style_parse_expand_key (name_key) + " \"" + style_definition [name_key] + "\",";
    cpp_lines.push_back (line);
    cpp_lines.push_back ("  },");
  }
  
  
  // Insert the C++ fragment into the source code.
  for (auto line : cpp_lines) {
    cout << line << endl; // Todo
  }
  
  
  cout << "Finished parsing style values from the usfm.sty file" << endl;

#endif

}
