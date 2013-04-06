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

#include "localizedbooks.h"
#include "utilities.h"
#include "directories.h"
#include "gwrappers.h"
#include <libxml/xmlreader.h>
#include "bookdata.h"
#include "books.h"
#include "tiny_utilities.h"


extern typeof(book_record) books_table[];


BookLocalization::BookLocalization(const ustring & language_in, const ustring & filename)
{
  language = language_in;

  if (g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    gchar *contents;
    g_file_get_contents(filename.c_str(), &contents, NULL, NULL);
    xmlParserInputBufferPtr inputbuffer;
    inputbuffer = xmlParserInputBufferCreateMem(contents, strlen(contents), XML_CHAR_ENCODING_NONE);
    xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
    if (reader) {
      char *opening_element = NULL;
      unsigned int myid;
      ustring mylocalization;
      ustring myabbreviation;

      while ((xmlTextReaderRead(reader) == 1)) {
        switch (xmlTextReaderNodeType(reader)) {
        case XML_READER_TYPE_ELEMENT:
          {
            opening_element = (char *)xmlTextReaderName(reader);
            if (!strcmp(opening_element, "book")) {
              myid = 0;
              mylocalization.clear();
              myabbreviation.clear();
            }
            break;
          }
        case XML_READER_TYPE_TEXT:
          {
            char *text = (char *)xmlTextReaderValue(reader);
            if (text) {
              if (!strcmp(opening_element, "id")) {
                myid = convert_to_int(text);
              }
              if (!strcmp(opening_element, "localization")) {
                mylocalization = text;
              }
              if (!strcmp(opening_element, "abbreviation")) {
                myabbreviation = text;
              }
              free(text);
            }
            break;
          }
        case XML_READER_TYPE_END_ELEMENT:
          {
            char *closing_element = (char *)xmlTextReaderName(reader);
            if (!strcmp(closing_element, "book")) {
              if (myid == 0)
                break;
              if (mylocalization.empty())
                break;
              if (myabbreviation.empty())
                break;
              id.push_back(myid);
              name.push_back(mylocalization);
              name_casefold.push_back(mylocalization.casefold());
              abbreviation.push_back(myabbreviation);
              abbreviation_casefold.push_back(myabbreviation.casefold());
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
  // Add missing books, take values from English.
  vector < unsigned int >all_ids = books_type_to_ids(btUnknown);
  set < unsigned int >ids_done(id.begin(), id.end());
  for (unsigned int i = 0; i < all_ids.size(); i++) {
    if (ids_done.find(all_ids[i]) == ids_done.end()) {
      ustring book = books_id_to_english(all_ids[i]);
      id.push_back(all_ids[i]);
      name.push_back(book);
      name_casefold.push_back(book.casefold());
      abbreviation.push_back(book);
      abbreviation_casefold.push_back(book.casefold());
    }
  }
}

BookLocalizations::BookLocalizations(int dummy)
{
}

vector < ustring > BookLocalizations::localizations_get()
// Get all available localizations and store them in the object.
{
  if (available_localizations.empty()) {
    // Get the localizations from the templates that come with Bibledit.
    ReadFiles rf1(directories_get_package_data(), "language_", ".xml");
    for (unsigned int i = 0; i < rf1.files.size(); i++) {
      available_filenames.push_back(gw_build_filename(directories_get_package_data(), rf1.files[i]));
      available_localizations.push_back(filename_get_localization(rf1.files[i]));
    }
    // Get the localizations from the templates provided by the user.
    ReadFiles rf2(directories_get_templates_user(), "language_", ".xml");
    for (unsigned int i = 0; i < rf2.files.size(); i++) {
      available_filenames.push_back(gw_build_filename(directories_get_templates_user(), rf2.files[i]));
      available_localizations.push_back(filename_get_localization(rf2.files[i]));
    }
    // Sort everything on name.
    quick_sort(available_localizations, available_filenames, 0, available_localizations.size());
  }
  return available_localizations;
}

ustring BookLocalizations::filename_get_localization(ustring filename)
{
  filename.erase(0, 9);
  filename.erase(filename.length() - 4, 4);
  replace_text(filename, "_", " ");
  size_t pos = 0;
  while (pos != string::npos) {
    filename.replace(pos, 1, upperCase(filename.substr(pos, 1)));
    pos = filename.find(" ", pos);
    if (pos != string::npos)
      pos++;
  }
  return filename;
}

unsigned int BookLocalizations::language_pointer_get(ustring language)
/*
Get a pointer to the BookLocalization for "language".
If "language" does not exist, it takes "English".
If "language" is not yet loaded, it first loads it, then gives the pointer.
*/
{
  // Load the localizations.
  if (available_localizations.empty()) {
    localizations_get();
  }
  // Is the requested language available?
  bool available = false;
  for (unsigned int i = 0; i < available_localizations.size(); i++) {
    if (language == available_localizations[i]) {
      available = true;
      break;
    }
  }
  // If the language is not available, take "English".
  if (!available)
    language = "English";
  // Get a pointer to the requested language, if it is already loaded.
  for (unsigned int i = 0; i < loaded_localizations.size(); i++) {
    if (language == loaded_localizations[i].language)
      return i;
  }
  // Load the requested language from the filename, and return a pointer to it.
  ustring filename;
  for (unsigned int i = 0; i < available_localizations.size(); i++) {
    if (language == available_localizations[i]) {
      filename = available_filenames[i];
      break;
    }
  }
  BookLocalization booklocalization(language, filename);
  loaded_localizations.push_back(booklocalization);
  return loaded_localizations.size() - 1;
}

unsigned int BookLocalizations::name2id(const ustring & language, const ustring & name)
{
  ustring namecf = name.casefold();
  unsigned int pointer = language_pointer_get(language);
  for (unsigned int i = 0; i < loaded_localizations[pointer].name_casefold.size(); i++) {
    if (namecf == loaded_localizations[pointer].name_casefold[i])
      return loaded_localizations[pointer].id[i];
  }
  return 0;
}

unsigned int BookLocalizations::abbrev2id(const ustring & language, const ustring & abbrev)
{
  ustring abbrevcf = abbrev.casefold();
  unsigned int pointer = language_pointer_get(language);
  for (unsigned int i = 0; i < loaded_localizations[pointer].abbreviation_casefold.size(); i++) {
    if (abbrevcf == loaded_localizations[pointer].abbreviation_casefold[i])
      return loaded_localizations[pointer].id[i];
  }
  return 0;
}

unsigned int BookLocalizations::abbrev2id_loose(const ustring & language, const ustring & abbrev)
{
  ustring abbrevcf = abbrev.casefold();
  replace_text(abbrevcf, ".", "");
  unsigned int pointer = language_pointer_get(language);
  for (unsigned int i = 0; i < loaded_localizations[pointer].abbreviation_casefold.size(); i++) {
    if (loaded_localizations[pointer].abbreviation_casefold[i].find(abbrevcf) == 0)
      return loaded_localizations[pointer].id[i];
  }
  return 0;
}

ustring BookLocalizations::id2name(const ustring & language, unsigned int id)
{
  unsigned int pointer = language_pointer_get(language);
  for (unsigned int i = 0; i < loaded_localizations[pointer].id.size(); i++) {
    if (id == loaded_localizations[pointer].id[i])
      return loaded_localizations[pointer].name[i];
  }
  return "";
}

ustring BookLocalizations::id2abbrev(const ustring & language, unsigned int id)
{
  unsigned int pointer = language_pointer_get(language);
  for (unsigned int i = 0; i < loaded_localizations[pointer].id.size(); i++) {
    if (id == loaded_localizations[pointer].id[i])
      return loaded_localizations[pointer].abbreviation[i];
  }
  return "";
}


ustring general_adapted_booknames_filename ()
// Returns the name of the file that contains the general adapted book names.
{
  ustring filename;
  filename = gw_build_filename (directories_get_configuration(), "adapted_booknames");
  return filename;
}

map <unsigned int, ustring> general_adapted_booknames_read ()
// Reads the general adapted book names from file.
{
  map <unsigned int, ustring> books;
  ReadText rt (general_adapted_booknames_filename(), true, false);
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    ustring book = rt.lines[i];
    if (!book.empty()) {
      books[i] = book;
    }
  }
  return books;
}

void general_adapted_booknames_write (map <unsigned int, ustring>& booknames)
// Writes the general adapted book names to file.
{
  unsigned int highest_book_id = 0;  
  for (unsigned int i = 0; i < bookdata_books_count(); i++) {
    unsigned int id = books_table[i].id;
    if (id > highest_book_id) {
      highest_book_id = id;
    }
  }
  vector <ustring> books;
  for (unsigned int i = 0; i <= highest_book_id; i++) {
    books.push_back (booknames[i]);
  }
  write_lines (general_adapted_booknames_filename(), books);
}


map <ustring, unsigned int> general_adapted_booknames_fill_up (map <unsigned int, ustring>& mapping)
// The variable "mapping" contains adapted booknames.
// This function generates a full map of booknames. 
// If there are adapted names, it takes these, 
// and those books that have no adapted names get the English ones.
{
  map <ustring, unsigned int> swapped_mapping;
  for (unsigned int i = 0; i < bookdata_books_count(); i++) {
    ustring book = mapping[i];
    if (book.empty()) {
      book = books_id_to_english(i);
    }
    swapped_mapping [book] = i;
  }
  return swapped_mapping;
}
