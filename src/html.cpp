/*
** Copyright (©) 2003-2008 Teus Benschop.
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#include "html.h"
#include "utilities.h"


ustring html_remove_code_between_less_than_and_greater_than_signs (const ustring& text)
{
  ustring clean_line (text);
  unsigned int infinite_loop_counter = 0;
  while (true) {
    infinite_loop_counter++;
    if (infinite_loop_counter > 100000) break;
    size_t open_position = clean_line.find ("<");
    if (open_position == string::npos) break;
    size_t close_position = clean_line.find (">", open_position);
    if (close_position == string::npos) break;
    clean_line.erase (open_position, close_position - open_position + 1); 
  }
  return clean_line;
}


ustring html_create_anchor (const ustring& anchor, const ustring& text)
{
  ustring result;
  result.append ("<a name=\"");
  result.append (anchor);
  result.append ("\">");
  result.append (text);
  result.append ("</a>");
  return result;
}


ustring html_extract_title (const ustring& filename)
// Extracts the title from the file: <title>Page Title</title>
{
  ustring title;
  ReadText rt (filename, true);
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    ustring line = rt.lines[i].casefold ();
    size_t pos = line.find ("<title>");
    if (pos != string::npos) {
      line = rt.lines[i];
      line.erase (0, pos + 7);
      pos = line.find ("<");
      if (pos == string::npos) title = line;
      else title = line.substr (0, pos);
    }
  }
  return title;
}
