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

#include "html.h"
#include "utilities.h"
#include "tiny_utilities.h"

ustring html_remove_code_between_less_than_and_greater_than_signs(const ustring & text)
{
  ustring clean_line(text);
  unsigned int infinite_loop_counter = 0;
  while (true) {
    infinite_loop_counter++;
    if (infinite_loop_counter > 100000)
      break;
    size_t open_position = clean_line.find("<");
    if (open_position == string::npos)
      break;
    size_t close_position = clean_line.find(">", open_position);
    if (close_position == string::npos)
      break;
    clean_line.erase(open_position, close_position - open_position + 1);
  }
  return clean_line;
}

ustring html_create_anchor(const ustring & anchor, const ustring & text)
{
  ustring result;
  result.append("<a name=\"");
  result.append(anchor);
  result.append("\">");
  result.append(text);
  result.append("</a>");
  return result;
}

ustring html_extract_title(const ustring & filename)
// Extracts the title from the file: <title>Page Title</title>
{
  ustring title;
  ReadText rt(filename, true);
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    ustring line = rt.lines[i].casefold();
    size_t pos = line.find("<title>");
    if (pos != string::npos) {
      line = rt.lines[i];
      line.erase(0, pos + 7);
      pos = line.find("<");
      if (pos == string::npos)
        title = line;
      else
        title = line.substr(0, pos);
    }
  }
  return title;
}

void html_entities_to_utf8(ustring & code)
// Change html entities to utf8 code, e.g. &#949; becomes ε.
// The entities for < and > and & remain because they are supposed to be given like &amp;, etc.
{
  size_t ampersand_pos = 0;
  while (true) {
    ampersand_pos = code.find("&", ampersand_pos);
    if (ampersand_pos == string::npos)
      break;
    size_t hash_pos = code.find("#", ampersand_pos);
    if (hash_pos == string::npos)
      break;
    if (hash_pos != ampersand_pos + 1) {
      ampersand_pos++;
      continue;
    }
    size_t semicolon_pos = code.find(";", ampersand_pos);
    if (semicolon_pos == string::npos)
      break;
    ustring number = code.substr(hash_pos + 1, semicolon_pos - hash_pos - 1);
    if (number_in_string(number) != number) {
      ampersand_pos++;
      continue;
    }
    gunichar unichar = convert_to_int(number);
    char *buffer = g_ucs4_to_utf8(&unichar, 1, NULL, NULL, NULL);
    if (buffer) {
      code.erase(ampersand_pos, semicolon_pos - ampersand_pos + 1);
      code.insert(ampersand_pos, buffer);
      g_free(buffer);
    }
    ampersand_pos++;
  }
}


int html_hexit(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return 0;
}


void html_url_decode(char *buf)
// Decode string %xx -> char (in place)
{
  int v;
  char *p, *s, *w;

  w = p = buf;
  while (*p) {
    v = 0;

    if (*p == '%') {
      s = p;
      s++;

      if (isxdigit((int)s[0]) && isxdigit((int)s[1])) {
        v = html_hexit(s[0]) * 16 + html_hexit(s[1]);
        if (v) {                /* do not decode %00 to null char */
          *w = (char)v;
          p = &s[1];
        }
      }

    }

    if (!v)
      *w = *p;
    p++;
    w++;
  }
  *w = '\0';
}


