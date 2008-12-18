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


#include "ot-quotations.h"
#include "utilities.h"
#include "directories.h"
#include "gwrappers.h"
#include <libxml/xmlreader.h>
#include "books.h"
#include "settings.h"
#include "tiny_utilities.h"


OTQuotation::OTQuotation (int dummy) :
nt (0)
{
  lxx = false;
}


OTQuotations::OTQuotations (int dummy)
{
}


void OTQuotations::get (Reference& reference, vector <Reference>& references, vector <ustring>& comments)
/*
Retrieves an Old Testament quotation of a New Testament reference.
This function does a bit more too. If an OT reference is passed, it also looks 
up the place in the NT where this is quoted.
reference: The input reference.
references: The output reference: contains the related references.
*/
{
  // Read data if we've nothing yet.
  if (quotations.empty ()) read ();

  // Store the original reference.
  references.push_back (reference);
  comments.push_back ("Current one");

  // Remap the references.
  extern Settings * settings;
  ustring project = settings->genconfig.project_get ();
  ProjectConfiguration * projectconfig = settings->projectconfig (project, false);
  Mapping mapping (projectconfig->versification_get (), reference.book);
  for (unsigned int i = 0; i < quotations.size(); i++) {
    mapping.book_change (quotations[i].nt.book);
    mapping.original_to_me (quotations[i].nt);
    for (unsigned int i2 = 0; i2 < quotations[i].ot.size(); i2++) {
      mapping.book_change (quotations[i].ot[i2].book);
      mapping.original_to_me (quotations[i].ot[i2]);
    }
  }

  // Go through the quotations looking for matching ones.
  bool lxx = false;
  for (unsigned int i = 0; i < quotations.size(); i++) {
    // If this is a NT reference, look for the corresponding OT quotations.
    if (reference.equals (quotations[i].nt)) {
      for (unsigned int i2 = 0; i2 < quotations[i].ot.size(); i2++) {
        references.push_back (quotations[i].ot[i2]);
        comments.push_back (comment ("Quotation", lxx));
      }
    }
    // If this is an OT reference, look for possible other ones in the OT, and the NT place that quotes it.
    for (unsigned int i2 = 0; i2 < quotations[i].ot.size(); i2++) {
      if (reference.equals (quotations[i].ot[i2])) {
        references.push_back (quotations[i].nt);
        comments.push_back ("Quoted here");
        for (unsigned int i3 = 0; i3 < quotations[i].ot.size(); i3++) {
          if (i3 != i2) {
            references.push_back (quotations[i].ot[i3]);
            comments.push_back (comment ("Parallel passage", lxx));
          }
        }
      }
    }
  }
  
  // If there is only one reference found, that will be the original one.
  // That means that no parallel or corresponding references were found.
  // Erase that single one in such cases.
  if (references.size() == 1) {
    references.clear ();    
    comments.clear ();
  }
}


void OTQuotations::read ()
{
  // Get contents of the data file. Bail out if not there.
  ustring xmlfilename = gw_build_filename (directories_get_package_data (), "ot-quotations-in-nt.xml");
  if (!g_file_test (xmlfilename.c_str(), G_FILE_TEST_IS_REGULAR)) return;
  gchar *contents;
  g_file_get_contents (xmlfilename.c_str(), &contents, NULL, NULL);
    
  /*
  Read the xml data. Example:
  <set>
   <nt book="Matthew" chapter="1" verse="23"/>
   <ot book="Isaiah" chapter="8" verse="8"/>
   <ot book="Isaiah" chapter="8" verse="10"/>
   <lxx>1</lxx>
  </set>
  */
  xmlParserInputBufferPtr inputbuffer;
  inputbuffer = xmlParserInputBufferCreateMem(contents, strlen (contents), XML_CHAR_ENCODING_NONE);
  xmlTextReaderPtr reader = xmlNewTextReader (inputbuffer, NULL);
  if (reader) {
    char * opening_element = NULL;
    OTQuotation quotation (0);
    while ((xmlTextReaderRead(reader) == 1)) {
      switch (xmlTextReaderNodeType (reader)) {
        case XML_READER_TYPE_ELEMENT:
        {
          opening_element = (char *) xmlTextReaderName (reader);
          if (!strcmp (opening_element, "set")) {
            quotation.nt.book = 0;
            quotation.nt.chapter = 0;
            quotation.nt.verse.clear ();
            quotation.ot.clear ();
            quotation.lxx = false;
          }
          if (!strcmp (opening_element, "nt") || !strcmp (opening_element, "ot")) {
            Reference ref (0);
            char * attribute;
            attribute = (char *) xmlTextReaderGetAttribute (reader, BAD_CAST "book");
            if (attribute) {
              ref.book = books_english_to_id (attribute);
              free (attribute);
            }
            attribute = (char *) xmlTextReaderGetAttribute (reader, BAD_CAST "chapter");
            if (attribute) {
              ref.chapter = convert_to_int (attribute);
              free (attribute);
            }
            attribute = (char *) xmlTextReaderGetAttribute (reader, BAD_CAST "verse");
            if (attribute) {
              ref.verse = attribute;
              free (attribute);
            }
            if (!strcmp (opening_element, "nt")) {
              quotation.nt.book = ref.book;
              quotation.nt.chapter = ref.chapter;
              quotation.nt.verse = ref.verse;
            }
            if (!strcmp (opening_element, "ot")) {
              quotation.ot.push_back (ref);
            }
          }
          break;
        }
        case XML_READER_TYPE_TEXT:
        {
          char * text = (char *) xmlTextReaderValue (reader);
          if (text) {
            if (!strcmp (opening_element, "lxx")) {
              quotation.lxx = convert_to_bool (text);
            }
            free(text);
          }
          break;
        }
        case XML_READER_TYPE_END_ELEMENT:
        {
          char * closing_element = (char *) xmlTextReaderName (reader);
          if (!strcmp(closing_element, "set")) {
            quotations.push_back (quotation);
          }
          break;
        }
      }
    }
  }

  // Free memory.
  if (reader) xmlFreeTextReader(reader);
  if (inputbuffer) xmlFreeParserInputBuffer (inputbuffer);
  if (contents) g_free (contents);  
}


ustring OTQuotations::comment (const gchar * text, bool lxx)
{
  ustring s (text);
  if (lxx)
    s.append (" from Septuagint");
  return s;
}
