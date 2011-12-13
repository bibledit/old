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

#include "mappings.h"
#include "utilities.h"
#include "directories.h"
#include "gwrappers.h"
#include <libxml/xmlreader.h>
#include "bookdata.h"
#include "books.h"
#include "tiny_utilities.h"

MappingStore::MappingStore(const ustring & system_in, const ustring & filename)
{
  system = system_in;

  if (g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    gchar *contents;
    g_file_get_contents(filename.c_str(), &contents, NULL, NULL);
    xmlParserInputBufferPtr inputbuffer;
    inputbuffer = xmlParserInputBufferCreateMem(contents, strlen(contents), XML_CHAR_ENCODING_NONE);
    xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
    if (reader) {
      char *opening_element = NULL;
      unsigned int myid;
      unsigned int mychapter;
      unsigned int myverse;
      unsigned int myoriginalid;
      unsigned int myoriginalchapter;
      unsigned int myoriginalverse;

      while ((xmlTextReaderRead(reader) == 1)) {
        switch (xmlTextReaderNodeType(reader)) {
        case XML_READER_TYPE_ELEMENT:
          {
            opening_element = (char *)xmlTextReaderName(reader);
            if (!strcmp(opening_element, "map")) {
              myid = 0;
              mychapter = 0;
              myverse = 0;
              myoriginalid = 0;
              myoriginalchapter = 0;
              myoriginalverse = 0;
            }
            break;
          }
        case XML_READER_TYPE_TEXT:
          {
            char *text = (char *)xmlTextReaderValue(reader);
            if (text) {
              if (!strcmp(opening_element, "book")) {
                myid = books_english_to_id(text);
              }
              if (!strcmp(opening_element, "chapter")) {
                mychapter = convert_to_int(text);
              }
              if (!strcmp(opening_element, "verse")) {
                myverse = convert_to_int(text);
              }
              if (!strcmp(opening_element, "original-book")) {
                myoriginalid = books_english_to_id(text);
              }
              if (!strcmp(opening_element, "original-chapter")) {
                myoriginalchapter = convert_to_int(text);
              }
              if (!strcmp(opening_element, "original-verse")) {
                myoriginalverse = convert_to_int(text);
              }
              free(text);
            }
            break;
          }
        case XML_READER_TYPE_END_ELEMENT:
          {
            char *closing_element = (char *)xmlTextReaderName(reader);
            if (!strcmp(closing_element, "map")) {
              if (myid == 0)
                break;
              if (mychapter == 0)
                break;
              if (myoriginalid == 0)
                break;
              if (myoriginalchapter == 0)
                break;
              // Verse == 0 occurs, so is allowed.
              id.push_back(myid);
              chapter.push_back(mychapter);
              verse.push_back(myverse);
              original_id.push_back(myoriginalid);
              original_chapter.push_back(myoriginalchapter);
              original_verse.push_back(myoriginalverse);
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

Mappings::Mappings(int dummy)
{
}

vector < ustring > Mappings::systems_get()
// Gets all available mapping systems and store them in the object.
{
  if (available_systems.empty()) {
    // Get the system from the templates that come with Bibledit.
    ReadFiles rf1(directories_get_package_data(), "mapping", ".xml");
    for (unsigned int i = 0; i < rf1.files.size(); i++) {
      available_filenames.push_back(gw_build_filename(directories_get_package_data(), rf1.files[i]));
      available_systems.push_back(filename_get_system(rf1.files[i]));
    }
    // Get the system from the templates provided by the user.
    ReadFiles rf2(directories_get_templates_user(), "mapping", ".xml");
    for (unsigned int i = 0; i < rf2.files.size(); i++) {
      available_filenames.push_back(gw_build_filename(directories_get_templates_user(), rf2.files[i]));
      available_systems.push_back(filename_get_system(rf2.files[i]));
    }
    // Sort everything on name.
    quick_sort(available_systems, available_filenames, 0, available_systems.size());
  }
  return available_systems;
}

ustring Mappings::filename_get_system(ustring filename)
// Taking a filename, it produces the mapping name from it.
{
  filename.erase(0, 8);
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

unsigned int Mappings::system_pointer_get(ustring system)
/*
Get a pointer to the MappingStore for "system".
If "system" does not exist, it takes "English".
If "system" is not yet loaded, it first loads it, then gives the pointer.
*/
{
  // Load the localizations.
  if (available_systems.empty()) {
    systems_get();
  }
  // Is the requested system available?
  bool available = false;
  for (unsigned int i = 0; i < available_systems.size(); i++) {
    if (system == available_systems[i]) {
      available = true;
      break;
    }
  }
  // If the system is not available, take "English".
  if (!available)
    system = "English";
  // Get a pointer to the requested system, if it is already loaded.
  for (unsigned int i = 0; i < loaded_systems.size(); i++) {
    if (system == loaded_systems[i].system)
      return i;
  }
  // Load the requested system from the filename, and return a pointer to it.
  ustring filename;
  for (unsigned int i = 0; i < available_systems.size(); i++) {
    if (system == available_systems[i]) {
      filename = available_filenames[i];
      break;
    }
  }
  MappingStore mappingstore(system, filename);
  loaded_systems.push_back(mappingstore);
  return loaded_systems.size() - 1;
}

void Mappings::me2original(const ustring & system, int mybook, int mychapter, int myverse, vector < int >&originalchapter, vector < int >&originalverse)
// Takes mybook, mychapter and myverse in the mapping system, and produces 
// the chapters:verses in the original versification system that correspond
// to it.
{
  originalchapter.clear();
  originalverse.clear();
  unsigned int pointer = system_pointer_get(system);
  for (unsigned int i = 0; i < loaded_systems[pointer].id.size(); i++) {
    if ((unsigned int)mybook == loaded_systems[pointer].id[i]) {
      if ((unsigned int)mychapter == loaded_systems[pointer].chapter[i]) {
        if ((unsigned int)myverse == loaded_systems[pointer].verse[i]) {
          originalchapter.push_back(loaded_systems[pointer].original_chapter[i]);
          originalverse.push_back(loaded_systems[pointer].original_verse[i]);
        }
      }
    }
  }
}

void Mappings::original2me(const ustring & system, int originalbook, int originalchapter, int originalverse, vector < int >&mychapter, vector < int >&myverse)
// Takes originalbook, originalchapter and originalverse, and produces 
// the chapters:verses in the versification system given, that correspond to it.
{
  mychapter.clear();
  myverse.clear();
  unsigned int pointer = system_pointer_get(system);
  for (unsigned int i = 0; i < loaded_systems[pointer].id.size(); i++) {
    if ((unsigned int)originalbook == loaded_systems[pointer].id[i]) {
      if ((unsigned int)originalchapter == loaded_systems[pointer].original_chapter[i]) {
        if ((unsigned int)originalverse == loaded_systems[pointer].original_verse[i]) {
          mychapter.push_back(loaded_systems[pointer].chapter[i]);
          myverse.push_back(loaded_systems[pointer].verse[i]);
        }
      }
    }
  }
}
