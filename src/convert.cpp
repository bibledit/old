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


#include "convert.h"
#include "tiny_utilities.h"


gchar *unicode_convert(gchar * data, gchar * encoding)
{
  gchar *output;
  output = g_convert(data, -1, "UTF-8", encoding, NULL, NULL, NULL);
  return output;

}

gchar *unicode_convert_automatic(gchar * data)
{
  // Storage for the output.
  gchar *output;

  // If the data is already utf8, return a copy of it and that's it.
  if (g_utf8_validate(data, -1, NULL)) {
    output = g_strdup(data);
    return output;
  }
  // Convert from known encoding(s).
  // Note that these known one(s) can be expanded if need be.
  vector < ustring > encodings = encodings_get();
  for (unsigned int i = 0; i < encodings.size(); i++) {
    output = unicode_convert(data, (gchar *) encodings[i].c_str());
    if (output) {
      if (g_utf8_validate(data, -1, NULL)) {
        return output;
      }
      g_free(output);
    }
  }

  // None found.
  return NULL;
}


vector < ustring > encodings_get()
// Get the available character encodings.
{
  vector < ustring > encodings;
  FILE *stream = popen("iconv -l", "r");
  char buf[1024];
  while (fgets(buf, sizeof(buf), stream)) {
    ustring output = buf;
    output = trim(output);
    if (g_str_has_suffix(output.c_str(), "//")) {
      output = output.substr(0, output.length() - 2);
    }
    encodings.push_back(output);
  }
  pclose(stream);
  return encodings;
}
