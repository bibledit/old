/*
** Copyright (©) 2003-2012 Teus Benschop.
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


bool NumeralLocalization::available ()
{
  return !localizations.empty();
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


ustring NumeralLocalization::convert_usfm (ustring line)
// Converts Latin numerals to their localized equivalents, 
// and keeps Latin numerals that are part of a USFM code.
// Also keeps the \c <latin numeral> and the \v <latin numeral>.
{
  ustring conversion;
  ustring number = number_in_string(line);
  while (!number.empty()) {
    bool convert = true;
    size_t number_pos = line.find (number);
    // If the number is preceded by \v or \c, do not convert it, e.g. "\c 1".
    if (number_pos >= 3) {
      ustring preceding_bit = line.substr (number_pos - 3, 2);
      if ((preceding_bit == "\\v") || (preceding_bit == "\\c")) {
        convert = false;
      }
    }
    // If the number belongs to an id line, e.g. \id 1JN, do not convert it.
    if (convert && (number_pos >= 4)) {
      ustring preceding_bit = line.substr (number_pos - 4, 3);
      if (preceding_bit == "\\id") {
        convert = false;
      }
    }
    // Look back for a space. If it is found then the numeral does not belong to a USFM code, and the conversion can go ahead.
    // If no space is found, but a backslash instead, then the conversion cannot go ahead since the numeral is part of a USFM code.
    if (convert) {
      for (int i = number_pos; i >= 0; i--) {
        ustring preceding_character = line.substr (i, 1);
        if (preceding_character == " ") {
          break;
        }
        if (preceding_character == "\\") {
          convert = false;
          break;
        }
      }
    }
    // Do the text transfer and optionally the conversion.
    conversion.append (line.substr (0, number_pos));
    line.erase (0, number_pos + number.length());
    if (convert) {
      number = latin2localization (number);
    }
    conversion.append (number);
    number = number_in_string(line);
  }
  conversion.append (line);
  return conversion;
}

