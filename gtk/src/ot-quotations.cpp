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


#include "ot-quotations.h"
#include "utilities.h"
#include "directories.h"
#include "gwrappers.h"
#include <libxml/xmlreader.h>
#include "books.h"
#include "settings.h"
#include "tiny_utilities.h"
#include "bible.h"
#include <glib/gi18n.h>

OTQuotation::OTQuotation(int dummy)
:reference(0)
{
  lxx = false;
}


OTQuotations::OTQuotations(int dummy)
{
}


void OTQuotations::read()
{
  // Get contents of the data file. Bail out if not there.
  ustring xmlfilename = gw_build_filename(Directories->get_package_data(), "ot-quotations-in-nt.xml");
  if (!g_file_test(xmlfilename.c_str(), G_FILE_TEST_IS_REGULAR))
    return;
  gchar *contents;
  g_file_get_contents(xmlfilename.c_str(), &contents, NULL, NULL);

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
  inputbuffer = xmlParserInputBufferCreateMem(contents, strlen(contents), XML_CHAR_ENCODING_NONE);
  xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
  if (reader) {
    char *opening_element = NULL;
    OTQuotation quotation(0);
    while ((xmlTextReaderRead(reader) == 1)) {
      switch (xmlTextReaderNodeType(reader)) {
      case XML_READER_TYPE_ELEMENT:
        {
          opening_element = (char *)xmlTextReaderName(reader);
          if (!strcmp(opening_element, "set")) {
            quotation.reference.book = 0;
            quotation.reference.chapter = 0;
            quotation.reference.verse.clear();
            quotation.referents.clear();
            quotation.lxx = false;
	    free(opening_element); opening_element = NULL; // not used next loop iteration
          }
          else if (!strcmp(opening_element, "nt") || !strcmp(opening_element, "ot")) {
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
            if (!strcmp(opening_element, "nt")) {
              quotation.reference.book = ref.book;
              quotation.reference.chapter = ref.chapter;
              quotation.reference.verse = ref.verse;
            }
            if (!strcmp(opening_element, "ot")) {
              quotation.referents.push_back(ref);
            }
			// cannot free(opening_element) because it will be used next loop iter in following switch case
          }
		  break;
        }
      case XML_READER_TYPE_TEXT:
        {
          char *text = (char *)xmlTextReaderValue(reader);
          if (opening_element && text) {
            if (!strcmp(opening_element, "lxx")) {
              quotation.lxx = convert_to_bool(text);
            }
            free(text);
			free(opening_element); opening_element = NULL;
          }
          break;
        }
      case XML_READER_TYPE_END_ELEMENT:
        {
          char *closing_element = (char *)xmlTextReaderName(reader);
          if (!strcmp(closing_element, "set")) {
            quotations_nt_order.push_back(quotation);
          }
		  free(closing_element);
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


void OTQuotations::get(Reference & reference, vector < Reference > &references, vector < ustring > &comments)
/*
Retrieves an Old Testament quotation of a New Testament reference.
This function does a bit more too. If an OT reference is passed, it also looks 
up the place in the NT where this is quoted.
reference: The input reference.
references: The output reference: contains the related references.
*/
{
  // Read data if we've nothing yet.
  if (quotations_nt_order.empty())
    read();

  // Store the original reference.
  references.push_back(reference);
  comments.push_back(_("Current one"));

  // Remap the references.
  extern Settings *settings;
  ustring project = settings->genconfig.project_get();
  ProjectConfiguration *projectconfig = settings->projectconfig(project, false);
  Mapping mapping(projectconfig->versification_get(), reference.book);
  for (unsigned int i = 0; i < quotations_nt_order.size(); i++) {
    mapping.book_change(quotations_nt_order[i].reference.book);
    mapping.original_to_me(quotations_nt_order[i].reference);
    for (unsigned int i2 = 0; i2 < quotations_nt_order[i].referents.size(); i2++) {
      mapping.book_change(quotations_nt_order[i].referents[i2].book);
      mapping.original_to_me(quotations_nt_order[i].referents[i2]);
    }
  }

  // Go through the quotations looking for matching ones.
  bool lxx = false;
  for (unsigned int i = 0; i < quotations_nt_order.size(); i++) {
    // If this is a NT reference, look for the corresponding OT quotations.
    if (reference.equals(quotations_nt_order[i].reference)) {
      for (unsigned int i2 = 0; i2 < quotations_nt_order[i].referents.size(); i2++) {
        references.push_back(quotations_nt_order[i].referents[i2]);
        comments.push_back(comment(_("Quotation"), lxx));
      }
    }
    // If this is an OT reference, look for possible other ones in the OT, and the NT place that quotes it.
    for (unsigned int i2 = 0; i2 < quotations_nt_order[i].referents.size(); i2++) {
      if (reference.equals(quotations_nt_order[i].referents[i2])) {
        references.push_back(quotations_nt_order[i].reference);
        comments.push_back(_("Quoted here"));
        for (unsigned int i3 = 0; i3 < quotations_nt_order[i].referents.size(); i3++) {
          if (i3 != i2) {
            references.push_back(quotations_nt_order[i].referents[i3]);
            comments.push_back(comment(_("Parallel passage"), lxx));
          }
        }
      }
    }
  }

  // If there is only one reference found, that will be the original one.
  // That means that no parallel or corresponding references were found.
  // Erase that single one in such cases.
  if (references.size() == 1) {
    references.clear();
    comments.clear();
  }
}


ustring OTQuotations::comment(const gchar * text, bool lxx)
{
  ustring s(text);
  if (lxx)
    s.append(_(" from Septuagint"));
  return s;
}


void OTQuotations::produce_in_ot_order()
// This produces the quotations in the Old Testament order.
{
  // Produce a sorted list of all references that point to something in the Old Testament.
  vector <Reference> old_testament_refs;
  {
    for (unsigned int i = 0; i < quotations_nt_order.size(); i++) {
      for (unsigned int i2 = 0; i2 < quotations_nt_order[i].referents.size(); i2++) {
        old_testament_refs.push_back (quotations_nt_order[i].referents[i2]);
      }
    }
    sort_references (old_testament_refs);
  }
  
  // Go through the OT places, and attach their NT referents.
  Reference previous_reference (0);
  for (unsigned int i = 0; i < old_testament_refs.size(); i++) {
    // Skip double references.
    if (previous_reference.equals (old_testament_refs[i])) {
      continue;
    }
    previous_reference.assign (old_testament_refs[i]);
    // Assemble new quotation.
    OTQuotation quotation (0);
    quotation.reference.assign (old_testament_refs[i]);
    // Harvest the NT referents that belong to that OT quotation.
    for (unsigned int i2 = 0; i2 < quotations_nt_order.size(); i2++) {
      for (unsigned int i3 = 0; i3 < quotations_nt_order[i2].referents.size(); i3++) {
        if (old_testament_refs[i].equals (quotations_nt_order[i2].referents[i3])) {
          quotation.referents.push_back (quotations_nt_order[i2].reference);
        }
      }
    }
    quotations_ot_order.push_back (quotation);
  }
}
