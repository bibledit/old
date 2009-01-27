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

#include "ot-nt-parallels.h"
#include "utilities.h"
#include "directories.h"
#include "gwrappers.h"
#include <libxml/xmlreader.h>
#include "books.h"
#include "tiny_utilities.h"

OtNtParallelSet::OtNtParallelSet(int dummy)
{
}

OtNtParallelSection::OtNtParallelSection(int dummy)
{
}

OtNtParallels::OtNtParallels(int dummy)
{
}

void OtNtParallels::readot()
{
  read("ot-parallel-passages.xml");
}

void OtNtParallels::readnt()
{
  read("nt-parallel-passages.xml");
}

void OtNtParallels::get(Reference & reference, vector < Reference > &references, vector < ustring > &comments)
/*
Retrieves an Old Testament or New Testament parallel passage.
reference: The input reference.
references: The output reference: contains the related references.
comments: The headings the passages fall under, or another comment.
*/
{
  // Read data if we've nothing yet.
  if (sections.empty()) {
    readot();
    readnt();
  }
  // Store the original reference.
  references.push_back(reference);
  comments.push_back("Current one");

  // Go through the references looking for matching ones.
  for (unsigned int i = 0; i < sections.size(); i++) {
    for (unsigned int i2 = 0; i2 < sections[i].sets.size(); i2++) {
      bool match = false;
      for (unsigned int i3 = 0; i3 < sections[i].sets[i2].references.size(); i3++) {
        if (reference.equals(sections[i].sets[i2].references[i3]))
          match = true;
      }
      if (match) {
        for (unsigned int i3 = 0; i3 < sections[i].sets[i2].references.size(); i3++) {
          if (!reference.equals(sections[i].sets[i2].references[i3]))
            references.push_back(sections[i].sets[i2].references[i3]);
          comments.push_back(sections[i].title);
        }
      }
    }
  }

  // If there is only one reference found, that will be the original one.
  // That means that no parallel or corresponding references were found.
  // Erase that one in such cases.
  if (references.size() == 1) {
    references.clear();
    comments.clear();
  }
}

void OtNtParallels::read(const gchar * filename)
{
  // Get contents of the data file. Bail out if not there.
  ustring xmlfilename = gw_build_filename(directories_get_package_data(), filename);
  if (!g_file_test(xmlfilename.c_str(), G_FILE_TEST_IS_REGULAR))
    return;
  gchar *contents;
  g_file_get_contents(xmlfilename.c_str(), &contents, NULL, NULL);

  /*
     Read the xml data. Example:
     <section title="The Descendants of Noah's Sons">
     <set>
     <reference book="Genesis" chapter="10" verse="2-4"/>
     <reference book="1 Chronicles" chapter="1" verse="5-7"/>
     </set>
     <set>
     <reference book="Genesis" chapter="10" verse="6-8"/>
     <reference book="1 Chronicles" chapter="1" verse="8-10"/>
     </set>
     <set>
     <reference book="Genesis" chapter="10" verse="13-18"/>
     <reference book="1 Chronicles" chapter="1" verse="11-16"/>
     </set>
     </section>
   */
  xmlParserInputBufferPtr inputbuffer;
  inputbuffer = xmlParserInputBufferCreateMem(contents, strlen(contents), XML_CHAR_ENCODING_NONE);
  xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
  if (reader) {
    char *opening_element = NULL;
    OtNtParallelSection parallelsection(0);
    OtNtParallelSet parallelset(0);
    while ((xmlTextReaderRead(reader) == 1)) {
      switch (xmlTextReaderNodeType(reader)) {
      case XML_READER_TYPE_ELEMENT:
        {
          opening_element = (char *)xmlTextReaderName(reader);
          if (!strcmp(opening_element, "section")) {
            parallelsection.sets.clear();
            parallelsection.title.clear();
            char *attribute;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "title");
            if (attribute) {
              parallelsection.title = attribute;
              free(attribute);
            }
          }
          if (!strcmp(opening_element, "set")) {
            parallelset.references.clear();
          }
          if (!strcmp(opening_element, "reference")) {
            Reference ref(0);
            char *attribute;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "book");
            if (attribute) {
              ref.book = books_english_to_id(attribute);
              free(attribute);
            }
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "chapter");
            if (attribute) {
              ref.chapter = convert_to_int(attribute);
              free(attribute);
            }
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "verse");
            if (attribute) {
              ref.verse = attribute;
              free(attribute);
            }
            parallelset.references.push_back(ref);
          }
          break;
        }
      case XML_READER_TYPE_TEXT:
        {
          char *text = (char *)xmlTextReaderValue(reader);
          if (text) {
            free(text);
          }
          break;
        }
      case XML_READER_TYPE_END_ELEMENT:
        {
          char *closing_element = (char *)xmlTextReaderName(reader);
          if (!strcmp(closing_element, "set")) {
            parallelsection.sets.push_back(parallelset);
          }
          if (!strcmp(closing_element, "section")) {
            sections.push_back(parallelsection);
          }
          break;
        }
      }
    }
  }
  // Free memory.
  if (reader)
    xmlFreeTextReader(reader);
  if (inputbuffer)
    xmlFreeParserInputBuffer(inputbuffer);
  if (contents)
    g_free(contents);
}

OtNtParallelDataSet::OtNtParallelDataSet(int dummy)
{
}

OtNtParallelDataSection::OtNtParallelDataSection(int dummy)
{
}
