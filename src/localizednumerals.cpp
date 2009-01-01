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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/

#include "localizednumerals.h"
#include "utilities.h"
#include "directories.h"
#include "gwrappers.h"
#include <libxml/xmlreader.h>
#include "tiny_utilities.h"


ustring numeral_localization_filename (const ustring& language)
// Returns the filename that belongs to the localization of "language".
{
  ustring base_name = "numerals_" + lowerCase (language) + ".xml";

  ustring full_filename;

  // Try the templates that come with Bibledit.
  full_filename = gw_build_filename (directories_get_package_data(), base_name);
  if (!g_file_test(full_filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    full_filename.clear();
  }
  
  // Try the templates provided by the user.
  if (full_filename.empty()) {
    full_filename = gw_build_filename (directories_get_templates_user(), base_name);
    if (!g_file_test(full_filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
      full_filename.clear();
    }
  }

  // Give result.
  return full_filename;
}


NumeralLocalization::NumeralLocalization(const ustring& language)
{
  ustring filename = numeral_localization_filename (language);

  if (g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    gchar *contents;
    g_file_get_contents(filename.c_str(), &contents, NULL, NULL);
    xmlParserInputBufferPtr inputbuffer;
    inputbuffer = xmlParserInputBufferCreateMem(contents, strlen(contents), XML_CHAR_ENCODING_NONE);
    xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
    if (reader) {
      char *opening_element = NULL;
      ustring latin;
      ustring localization;

      while ((xmlTextReaderRead(reader) == 1)) {
        switch (xmlTextReaderNodeType(reader)) {
        case XML_READER_TYPE_ELEMENT:
          {
            opening_element = (char *)xmlTextReaderName(reader);
            if (!strcmp(opening_element, "numeral")) {
              latin.clear();
              localization.clear();
            }
            break;
          }
        case XML_READER_TYPE_TEXT:
          {
            char *text = (char *)xmlTextReaderValue(reader);
            if (text) {
              if (!strcmp(opening_element, "latin")) {
                latin = text;
              }
              if (!strcmp(opening_element, "localization")) {
                localization = text;
              }
              free(text);
            }
            break;
          }
        case XML_READER_TYPE_END_ELEMENT:
          {
            char *closing_element = (char *)xmlTextReaderName(reader);
            if (!strcmp(closing_element, "numeral")) {
              latins.push_back(latin);
              localizations.push_back(localization);
            }
            break;
          }
        }
      }
    }
    if (reader)
      xmlFreeTextReader(reader);
    if (inputbuffer)
      xmlFreeParserInputBuffer(inputbuffer);
    if (contents)
      g_free(contents);
  }
}


ustring NumeralLocalization::latin2localization (const ustring& latin)
// Given "latin" it returns the localization.
{
  // Process the full number first.
  for (unsigned int i = 0; i < latins.size(); i++) {
    if (latin == latins[i]) {
      return localizations[i];
    }
  }

  // Process the separate bits.
  ustring localization;
  for (unsigned int i = 0; i < latin.length(); i++) {
    bool found = false;
    for (unsigned int i2 = 0; i2 < latins.size(); i2++) {
      if (found) 
        continue;
      if (latin.substr (i, 1) == latins[i2]) {
        localization.append (localizations[i2]);
        found = true;
      }
    }
    if (!found) {
      localization.append (latin.substr (i, 1));
    }   
  }
  return localization;
}

