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

#include "usfm2osis.h"
#include "gwrappers.h"
#include <glib.h>
#include "utilities.h"
#include "tiny_utilities.h"
#include "date_time_utils.h"
#include "usfmtools.h"
#include "constants.h"
#include "books.h"
#include "color.h"
#include "directories.h"
#include "gtkwrappers.h"
#include "pdfviewer.h"
#include "settings.h"
#include "referenceutils.h"
#include "projectutils.h"
#include "styles.h"

/*

OSIS information is available at http://www.bibletechnologies.net/.

Todd Tillinghast of Snowfall Software (todd@snowfallsoftware.com) created a
series of python scripts which do the best job of converting between usfm
and osis. The utilty can be downloaded from
http://www.virtualstorehouse.org/downloads.html.

Another resource are the Sword Project utilities available at
https://crosswire.org/ftpmirror/pub/sword/utils/.

Additionally, Dale P. at daledirect at gmail.com has created a perl script
which does converts from usfm to osis. 

The GoBibleCreator 3.0 (written in C# and adapted to Java)
converts usfm to its interpretation of OSIS for Java apps which run on MIDP
1 & 2 enabled cell phones. 

The object below transforms USFM code into OSIS code.
The validity of the resulting osis file can be checked:
xmllint --noout --schema osisCore.2.1.1.xsd osis.xml 

At times it may be necessary to use the sword api that is in svn:
cd
svn co https://crosswire.org/svn/sword/trunk swordtest
cd swordtest
./autogen.sh
./usrinst.sh
make

*/

Usfm2Osis::Usfm2Osis(const ustring& file)
{
  // Initialize variables.
  osisfile = file;
  book_bibledit_id = 0;
  division_open = false;
  paragraph_open = false;
  chapter_number = 0;
  note_is_open = false;
  character_style_open = false;
  
  // Create the XML writer.
  xmlbuffer = xmlBufferCreate();
  xmlwriter = xmlNewTextWriterMemory(xmlbuffer, 0);

  // Setup document.  
  xmlTextWriterStartDocument(xmlwriter, NULL, "UTF-8", NULL);
  xmlTextWriterSetIndent(xmlwriter, 1);
}

Usfm2Osis::~Usfm2Osis()
{
  // Close document and write it to disk.
  xmlTextWriterEndDocument(xmlwriter);
  xmlTextWriterFlush(xmlwriter);
  g_file_set_contents(osisfile.c_str(), (const gchar *)xmlbuffer->content, -1, NULL);

  // Free memory.
  if (xmlwriter)
    xmlFreeTextWriter(xmlwriter);
  if (xmlbuffer)
    xmlBufferFree(xmlbuffer);
}

void Usfm2Osis::set_stylesheet (const ustring& name)
/*
Sets the stylesheet to use.
Note that normally a stylesheet is not needed, 
because the USFM markers are interpreted according to the USFM standard.
However, if any unknown USFM marker is encountered, the stylesheet 
is consulted to find out more about it.
*/
{
  stylesheet = name;
}


void Usfm2Osis::header (const ustring& name, const ustring& description)
// Writes the header.
{
  xmlTextWriterStartElement(xmlwriter, BAD_CAST "osis");
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "xmlns", "http://www.bibletechnologies.net/2003/OSIS/namespace");
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "xsi:schemaLocation", "http://www.bibletechnologies.net/2003/OSIS/namespace http://www.bibletechnolgologies.net/OSIS/osisCore.2.1.xsd");

    xmlTextWriterStartElement(xmlwriter, BAD_CAST "osisText");
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "osisIDWork", "%s", name.c_str());
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "osisRefWork", "Bible");
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "xml:lang", "en");
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "canonical", "true");

      // The "header" element is a direct child of element "osisText".
      xmlTextWriterStartElement(xmlwriter, BAD_CAST "header");

        xmlTextWriterStartElement(xmlwriter, BAD_CAST "work");
        xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "osisWork", "%s", name.c_str());

          xmlTextWriterStartElement(xmlwriter, BAD_CAST "title");
          xmlTextWriterWriteFormatString(xmlwriter, "%s", description.c_str());
          xmlTextWriterEndElement(xmlwriter);

          xmlTextWriterStartElement(xmlwriter, BAD_CAST "type");
          xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "type", "%s", "OSIS");
          xmlTextWriterWriteFormatString(xmlwriter, "%s", "Bible");
          xmlTextWriterEndElement(xmlwriter);

          xmlTextWriterStartElement(xmlwriter, BAD_CAST "identifier");
          xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "type", "OSIS");
          xmlTextWriterWriteFormatString(xmlwriter, "Bible.%s", name.c_str());
          xmlTextWriterEndElement(xmlwriter);

          xmlTextWriterStartElement(xmlwriter, BAD_CAST "rights");
          xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "type", "x-copyright");
          guint year, month, day;
          date_time_normal_get_year_month_day (date_time_julian_day_get_current (), year, month, day);
          xmlTextWriterWriteFormatString(xmlwriter, "The document is free. It is copyrighted (C) %d by %s. Permission is granted to copy, distribute and/or modify the documents under the terms of the GNU Free Documentation License, Version 1.2 or any later version published by the Free Software Foundation; with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.", year, g_get_real_name ());
          xmlTextWriterEndElement(xmlwriter);

          xmlTextWriterStartElement(xmlwriter, BAD_CAST "refSystem");
          xmlTextWriterWriteFormatString(xmlwriter, "Bible.%s", name.c_str());
          xmlTextWriterEndElement(xmlwriter);

        // Close element work.
        xmlTextWriterEndElement(xmlwriter);

      // Close element header.
      xmlTextWriterEndElement(xmlwriter);
}


void Usfm2Osis::open_book(unsigned int id)
{
  // Close any open book.
  if (book_bibledit_id) {
    close_book();
  }

  // Save book.
  book_bibledit_id = id;
  book_osis_id = books_id_to_osis(book_bibledit_id);
  
  // Open a book division
  xmlTextWriterStartElement(xmlwriter, BAD_CAST "div");
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "type", "book");
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "osisID", "%s", book_osis_id.c_str());
}


void Usfm2Osis::close_book()
{
  if (book_bibledit_id) {
    xmlTextWriterEndElement(xmlwriter);
    book_bibledit_id = 0;
  }
}


void Usfm2Osis::load_book (vector <ustring>& data)
// Accept the USFM code of a full book and does the transformation.
{
  // Load code.
  ustring usfm_code;  
  for (unsigned int i = 0; i < data.size(); i++) {
    if (i > 0) {
      usfm_code.append (" ");
    }
    usfm_code.append (data[i]);
  }
  replace_text (usfm_code, "\n", " ");
  
  // Do headers and descriptions.
  transform_headers_and_descriptions(usfm_code);
  
  // Deal with the code for each division.
  transform_per_osis_division(usfm_code);  
}


void Usfm2Osis::transform_headers_and_descriptions(ustring& usfmcode)
// Does the first stage of the transformation.
// It finds descriptions, and (running) headers for the book.
{
  // Variable that holds the usfm code that has not been handled in this routine.
  ustring unhandled_usfm_code;
  
  // Go through all USFM code.
  ustring usfm_code;
  ustring marker_text;
  size_t marker_position;
  size_t marker_length;
  bool marker_is_opener;
  bool marker_found;
  while (usfm_code_available (usfm_code, usfmcode, 1000)) {
    marker_found = usfm_search_marker(usfm_code, marker_text, marker_position, marker_length, marker_is_opener);
    if (marker_found && (marker_position == 0)) {
      // A marker is right at the start of the line.
      if (false) {
      }
      // id 
      // description[@type='usfm' and @subType='x-id']
      else if (marker_text == "id") {
        transform_usfm_description (usfm_code, marker_text, marker_length);
      } 
      // ide
      // description[@type='usfm' and @subType='x-ide']
      else if (marker_text == "ide") {
        transform_usfm_description (usfm_code, marker_text, marker_length);
      } 
      // toc
      // div[@type="tableOfContents"] 
      // There are several toc markers in USFM. This converter transforms it into a "x-toc" description.
      else if (marker_text == "toc") {
        transform_usfm_description (usfm_code, marker_text, marker_length);
      } 
      else if (marker_text == "toc1") {
        transform_usfm_description (usfm_code, marker_text, marker_length);
      } 
      else if (marker_text == "toc2") {
        transform_usfm_description (usfm_code, marker_text, marker_length);
      } 
      else if (marker_text == "toc3") {
        transform_usfm_description (usfm_code, marker_text, marker_length);
      }
      // h 
      // div[@type='book']/title[@short='value of h marker']
      else if (marker_text == "h") {
        transform_h_title (usfm_code, marker_length, false, NULL);
      } 
      // h1 
      // div[@type='book']/title[@short='value of h marker']
      else if (marker_text == "h1") {
        transform_h_title (usfm_code, marker_length, false, NULL);
      } 
      // h2 
      // title[@type="runningHead" and placement="inside" and @short='value of h2 marker'] 
      else if (marker_text == "h2") {
        transform_h_title (usfm_code, marker_length, true, "inside");
      } 
      // h3 
      // title[@type="runningHead" and placement="outside" and @short='value of h3 marker'] 
      else if (marker_text == "h3") {
        transform_h_title (usfm_code, marker_length, true, "outside");
      } 
      else {
        // A marker is at the start, but it has not been handled,
        // so the marker is removed from the input stream, 
        // and added to the unhandled USFM code.
        ustring text;
        text.append (usfm_code.substr (0, marker_length));
        unhandled_usfm_code.append (text);
        usfm_code.erase (0, marker_length);
      }
    } else {
      // There's no marker at the start, but something else.
      // Whatever's there, remove it from the input stream, 
      // and add it to the unhandled USFM code for later processing.
      ustring text = get_erase_code_till_next_marker (usfm_code, 0, 0, false);
      unhandled_usfm_code.append (text);
    }
  }
  // Keep the unhandled USFM code for further processing.
  usfmcode = unhandled_usfm_code;
}

void Usfm2Osis::transform_block(ustring& usfm_code)
// Does the transformation of a block of USFM code.
// An attempt is made to follow the OSIS manual, Appendix F, USFM to OSIS Mapping.
{
  // Bail out if there's nothing.
  if (usfm_code.empty())
    return;

  // Go through the available code.
  ustring marker_text;
  size_t marker_position;
  size_t marker_length;
  bool marker_is_opener;
  bool marker_found;

  while (!usfm_code.empty()) {
    marker_found = usfm_search_marker(usfm_code, marker_text, marker_position, marker_length, marker_is_opener);
    if (marker_found && (marker_position == 0)) {

      if (false) {
      }
      
      // Marker id should not occur here, because it has been handled earlier.
      // Marker ide should not occur here, because it has been handled earlier.

      // Marker sts goes into osis as usfm code.
      else if (marker_text == "sts") {
        transform_usfm_description (usfm_code, marker_text, marker_length);
      }

      // rem
      // description[@type="usfm" and subType="x-rem"]
      else if (marker_text == "rem") {
        transform_usfm_description (usfm_code, marker_text, marker_length);
      } 
      
      // Markers h( 123) should not occur here, because these have been handled earlier.
      // Markers toc(123) should not occur here, because these have been handled earlier.

      // imt
      // div[@type="introduction" and @canonical="false"]/title[@type="main"] 
      else if (marker_text == "imt") {
        transform_division ("introduction", false);
        transform_general_title (usfm_code, marker_length, "main", 0);
      } 

      // imt1 
      // div[@type="introduction" and @canonical="false"]/title[@type="main"]/title[@level="1"] 
      else if (marker_text == "imt1") {
        transform_division ("introduction", false);
        transform_general_title (usfm_code, marker_length, "main", 1);
      } 

      // imt2 
      // div[@type="introduction" and @canonical="false"]/title[@type="main"]/title[@level="2"] 
      else if (marker_text == "imt2") {
        transform_division ("introduction", false);
        transform_general_title (usfm_code, marker_length, "main", 2);
      }

      // imt3 
      // div[@type="introduction" and @canonical="false"]/title[@type="main"]/title[@level="3"] 
      else if (marker_text == "imt3") {
        transform_division ("introduction", false);
        transform_general_title (usfm_code, marker_length, "main", 3);
      }

      // imt4 
      // div[@type="introduction" and @canonical="false"]/title[@type="main"]/title[@level="4"] 
      else if (marker_text == "imt4") {
        transform_division ("introduction", false);
        transform_general_title (usfm_code, marker_length, "main", 4);
      }

      // is
      // div[@type="introduction" and @canonical="false"]/div[@type="section"]/title 
      else if (marker_text == "is") {
        transform_division ("section", false);
        transform_general_title (usfm_code, marker_length, "section", 0);
      } 

      // is1 
      // div[@type="introduction" and @canonical="false"]/div[@type="section"]/title 
      else if (marker_text == "is1") {
        transform_division ("section", false);
        transform_general_title (usfm_code, marker_length, "section", 1);
      } 

      // is2
      // div[@type="introduction" and @canonical="false"]/div[@type="section"]/div[@type="subSection"]/title 
      else if (marker_text == "is2") {
        transform_division ("section", false);
        transform_general_title (usfm_code, marker_length, "section", 2);
      } 

      else if (marker_text == "is3") {
        transform_division ("section", false);
        transform_general_title (usfm_code, marker_length, "section", 3);
      } 

      else if (marker_text == "is4") {
        transform_division ("section", false);
        transform_general_title (usfm_code, marker_length, "section", 4);
      } 

      // ip 
      // div[@type="introduction" and @canonical="false"]//p 
      else if (marker_text == "ip") {
        transform_paragraph_start (usfm_code, marker_length);
      } 

      // ipi 
      // div[@type="introduction" and @canonical="false"]//list/item/p
      else if (marker_text == "ipi") {
        transform_paragraph_start (usfm_code, marker_length);
      } 

      // im See notes below on \m and \b 
      else if (marker_text == "im") {
        transform_paragraph_start (usfm_code, marker_length);
      } 

      // imi 
      // Continue the div[@type="introduction" and 
      // @canonical="false"]//list/item/p if interrupted 
      // otherwise div[@type="introduction" and 
      // @canonical="false"]//list/item 
      else if (marker_text == "imi") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // ipq
      // div[@type="introduction" and @canonical="false"]//q/p
      else if (marker_text == "ipq") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // imq 
      // div[@type="introduction" and @canonical="false"]//q 
      else if (marker_text == "imq") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // ipr 
      // div[@type="introduction" and @canonical="false"]//reference 
      else if (marker_text == "ipr") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // iq
      // div[@type="introduction" and @canonical="false"]//lg/l[@level="1"]
      else if (marker_text == "iq") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // iq1
      // div[@type="introduction" and @canonical="false"]//lg/l[@level="1"] 
      else if (marker_text == "iq1") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // iq2
      // div[@type="introduction" and @canonical="false"]//lg/l[@level="2"] 
      else if (marker_text == "iq2") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // iq3 
      // div[@type="introduction" and @canonical="false"]//lg/l[@level="3"] 
      else if (marker_text == "iq3") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      else if (marker_text == "iq4") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // ib See notes below on \m and \b 
      // (This is a format oriented marker --whitespace, but needs to be preserved round-trip) 
      else if (marker_text == "ib") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      else if (marker_text == "ili") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // iot 
      // div[@type="introduction" and @canonical="false"]//div[@type="outline"]/title 
      else if (marker_text == "iot") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      else if (marker_text == "io") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // io1 
      // div[@type="introduction" and @canonical="false"]//div[@type="outline"]/list/item 
      else if (marker_text == "io1") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // io2 
      // div[@type="introduction" and @canonical="false"]//div[@type="outline"]/list/item/list/item 
      else if (marker_text == "io2") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // io3 
      // div[@type="introduction" and @canonical="false"]//div[@type="outline"]/list/item/list/item/list/item 
      else if (marker_text == "io3") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // io4 
      // div[@type="introduction" and @canonical="false"]//div[@type="outline"]/list/item/list/item/list/item/list/item 
      else if (marker_text == "io4") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // ior 
      // div[@type="introduction" and @canonical="false"]//div[@type="outline"]/list/item/reference 
      else if (marker_text == "ior") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // iex 
      // div[@type="bridge"] 
      else if (marker_text == "iex") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      else if (marker_text == "iqt") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // imte 
      // div[@type="introduction" and @canonical="false"]/title[@type="main"] 
      else if (marker_text == "imte") {
        transform_division ("introduction", false);
        transform_general_title (usfm_code, marker_length, "main", 0);
      }

      // ie 
      else if (marker_text == "ie") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // mt
      // div[@type="book"]/title[@type="main"]
      else if (marker_text == "mt") {
        transform_general_title (usfm_code, marker_length, "main", 0);
      } 
      
      // mt1 
      // div[@type="book"]/title[@type="main"]/title[@level="1"] 
      else if (marker_text == "mt1") {
        transform_general_title (usfm_code, marker_length, "main", 1);
      } 

      // mt2 
      // div[@type="book"]/title[@type="main"]/title[@level="2"] 
      else if (marker_text == "mt2") {
        transform_general_title (usfm_code, marker_length, "main", 2);
      } 

      // mt3 
      // div[@type="book"]/title[@type="main"]/title[@level="3"] 
      else if (marker_text == "mt3") {
        transform_general_title (usfm_code, marker_length, "main", 3);
      } 
      
      // mt4 
      // div[@type="book"]/title[@type="main"]/title[@level="4"] 
      else if (marker_text == "mt4") {
        transform_general_title (usfm_code, marker_length, "main", 4);
      } 
      
      // mte 
      // div[@type="book"]/title[@type="main"]
      else if (marker_text == "mte") {
        transform_general_title (usfm_code, marker_length, "main", 0);
      } 
      
      // mte1 
      // div[@type="book"]/title[@type="main"]/title[@level="1"]
      else if (marker_text == "mte1") {
        transform_general_title (usfm_code, marker_length, "main", 1);
      } 

      // mte2
      // div[@type="book"]/title[@type="main"]/title[@level="2"]
      else if (marker_text == "mte2") {
        transform_general_title (usfm_code, marker_length, "main", 2);
      } 
      
      else if (marker_text == "mte3") {
        transform_general_title (usfm_code, marker_length, "main", 3);
      } 
      
      else if (marker_text == "mte4") {
        transform_general_title (usfm_code, marker_length, "main", 4);
      } 

      // ms
      // div[@type="majorSection"]/title
      else if (marker_text == "ms") {
        transform_division ("majorSection", true);
        transform_general_title (usfm_code, marker_length, "main", 0);
      } 

      // ms1 
      // div[@type="majorSection"]/title
      else if (marker_text == "ms1") {
        transform_division ("majorSection", true);
        transform_general_title (usfm_code, marker_length, "main", 1);
      } 
      
      // ms2 
      // div[@type="majorSection"]/div[@type="majorSection"]/title
      else if (marker_text == "ms2") {
        transform_division ("majorSection", true);
        transform_general_title (usfm_code, marker_length, "main", 2);
      } 
      
      else if (marker_text == "ms3") {
        transform_division ("majorSection", true);
        transform_general_title (usfm_code, marker_length, "main", 3);
      } 

      else if (marker_text == "ms4") {
        transform_division ("majorSection", true);
        transform_general_title (usfm_code, marker_length, "main", 4);
      } 

      // mr 
      // div[@type="majorSection"]/title[@type="scope"]/reference 
      else if (marker_text == "mr") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // s
      // div[@type="section"]/title
      else if (marker_text == "s") {
        transform_division ("section", true);
        transform_general_title (usfm_code, marker_length, NULL, 0);
      } 
      
      // s1
      // div[@type="section"]/title
      else if (marker_text == "s1") {
        transform_division ("section", true);
        transform_general_title (usfm_code, marker_length, NULL, 1);
      } 
      
      // s2
      // div[@type="subSection"]/title
      else if (marker_text == "s2") {
        transform_division ("subSection", true);
        transform_general_title (usfm_code, marker_length, NULL, 2);
      } 
      
      // s3
      // div[@type="subSection"]/div[@type="subSection"]/title
      else if (marker_text == "s3") {
        transform_division ("subSection", true);
        transform_general_title (usfm_code, marker_length, NULL, 3);
      } 

      // s4 
      // div[@type="subSection"]/div[@type="subSection"]/div[@type="subSection"]/title
      else if (marker_text == "s4") {
        transform_division ("subSection", true);
        transform_general_title (usfm_code, marker_length, NULL, 4);
      } 

      else if (marker_text == "sr") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // r
      // title[@type='parallel']/reference
      else if (marker_text == "r") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      else if (marker_text == "r") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // d
      // title[@type="psalm"] 
      else if (marker_text == "d") {
        transform_general_title (usfm_code, marker_length, "psalm", 0);
      }

      // sp 
      // speech/speaker 
      else if (marker_text == "sp") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // c 
      // Write the chapter number as a milestone.
      else if (marker_text == "c") {
        ensure_chapter_closed ();
        transform_chapter_number (usfm_code, marker_length);
      }

      // ca
      // If it is desired to have two "n" attribute values then 
      // encode two chapter elements - otherwise simply put 
      // one or more identifier in the osisID and up to one value 
      // for "n".
      else if (marker_text == "ca") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // cl
      // title type="chapterLabel"
      else if (marker_text == "cl") {
        transform_general_title (usfm_code, marker_length, "chapterLabel", 0);
      }

      // cp
      // If it is desired to have two "n" attribute values then 
      // encode two chapter elements - otherwise simply put 
      // one or more identifier in the osisID and up to one value 
      // for "n". 
      else if (marker_text == "cp") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // cd
      // div[@type="book"]/div[@type="introduction" and 
      // preceeding-sibling::chapter] -- e.g. make this an intro 
      // div which immediately follows a chapter milestone 
      else if (marker_text == "cd") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // v 
      // verse\@osisID and verse\@n 
      else if (marker_text == "v") {
        ensure_verse_closed ();
        transform_verse_number (usfm_code, marker_length);
      }

      // va 
      // If it is desired to have two "n" attribute values then 
      // encode two verse elements - otherwise simply put one 
      // or more identifier in the osisID and up to one value for 
      // "n". 
      else if (marker_text == "va") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // vp 
      // If it is desired to have two "n" attribute values then 
      // encode two verse elements - otherwise simply put one 
      // or more identifier in the osisID and up to one value for 
      // "n". 
      else if (marker_text == "vp") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // p 
      // p 
      else if (marker_text == "p") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // m
      // See notes below on \m and \b 
      else if (marker_text == "m") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // pmo 
      // q[@type="embedded"]/salute
      else if (marker_text == "pmo") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // pm 
      // q[@type="embedded"]/p 
      else if (marker_text == "pm") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // pmc 
      // q[@type="embedded"]/closer
      else if (marker_text == "pmc") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // pmr 
      // q[@type="embedded"]/p[@type="x-refrain"]
      else if (marker_text == "pmr") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // pi 
      // list/item/p 
      else if (marker_text == "pi") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // pi1 
      // list/item/p 
      else if (marker_text == "pi1") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // pi2 
      // list/item/list/item/p 
      else if (marker_text == "pi2") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // pi3 
      // list/item/list/item/list/item/p
      else if (marker_text == "pi3") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      else if (marker_text == "pi4") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // mi 
      // See notes below on \m and \b
      else if (marker_text == "mi") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // nb 
      else if (marker_text == "nb") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // cls
      // closer
      else if (marker_text == "cls") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // li 
      // list/item 
      else if (marker_text == "li") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // li1 
      // list/item 
      else if (marker_text == "li1") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // li2 
      // list/item/list/item 
      else if (marker_text == "li2") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // li3 
      // list/item/list/item/list/item 
      else if (marker_text == "li3") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // li4 
      // list/item/list/item/list/item/list/item 
      else if (marker_text == "li4") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // pc 
      // inscription 
      else if (marker_text == "pc") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // pr
      // p[@type="x-refrain"]
      else if (marker_text == "pr") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // ph 
      // list/item 
      else if (marker_text == "ph") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // ph1 
      // list/item 
      else if (marker_text == "ph1") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // ph2 
      // list/item/list/item 
      else if (marker_text == "ph2") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // ph3 
      // list/item/list/item/list/item 
      else if (marker_text == "ph3") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      else if (marker_text == "ph4") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // b
      // See notes below on \m and \b 
      else if (marker_text == "b") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // q 
      // lg/l[@level="1"]
      else if (marker_text == "q") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // q1
      // lg/l[@level="1"]
      else if (marker_text == "q1") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // q2
      // lg/l[@level="2"] 
      else if (marker_text == "q2") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // q3
      // lg/l[@level="3"]
      else if (marker_text == "q3") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      else if (marker_text == "q4") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // qr
      // l[@type="refrain"] and l[@type="attribution"] (choose qr based on context) 
      else if (marker_text == "qr") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // qc 
      // l[@type="doxology"]
      else if (marker_text == "qc") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // qs
      // lg/l[@type="selah"]
      else if (marker_text == "qs") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // qa
      // lg/l[@type="acrostic"]
      else if (marker_text == "qa") {
        transform_paragraph_start (usfm_code, marker_length);
      }
      
      // qac
      // hl[@type="acrostic"]
      else if (marker_text == "qac") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      // qm
      // q[@type="embedded"]/lg/l[@level="1"] (or leave off the level)
      else if (marker_text == "qm") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      
      // qm1
      // q[@type="embedded"]/lg/l[@level="1"] 
      else if (marker_text == "qm1") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // qm2
      // q[@type="embedded"]/lg/l[@level="2"]
      else if (marker_text == "qm2") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // qm3 q[@type="embedded"]/lg/l[@level="3"]
      else if (marker_text == "qm3") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      else if (marker_text == "qm4") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // tr 
      // table/row 
      else if (marker_text == "tr") {
        transform_paragraph_start (usfm_code, marker_length);
      }

      // tr1 
      // treat same as tr 

      // tr2 
      // treat same as tr 

      else if (marker_text == "th") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // th1
      // row[@role="label"]/cell
      else if (marker_text == "th1") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      // th2
      // row[@role="label"]/cell
      else if (marker_text == "th2") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      // th3
      // row[@role="label"]/cell
      else if (marker_text == "th3") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      // th4
      // row[@role="label"]/cell
      else if (marker_text == "th4") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      else if (marker_text == "th5") {
        transform_remove_marker (usfm_code, marker_length);
      }

      else if (marker_text == "th6") {
        transform_remove_marker (usfm_code, marker_length);
      }

      else if (marker_text == "thr") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // thr1
      // row[@role="label"]/cell[@align='right']
      else if (marker_text == "thr1") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      // thr2
      // row[@role="label"]/cell[@align='right']
      else if (marker_text == "thr2") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // thr3
      // row[@role="label"]/cell[@align='right']
      else if (marker_text == "thr3") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      // thr4
      // row[@role="label"]/cell[@align='right']
      else if (marker_text == "thr4") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      else if (marker_text == "thr5") {
        transform_remove_marker (usfm_code, marker_length);
      }

      else if (marker_text == "thr6") {
        transform_remove_marker (usfm_code, marker_length);
      }

      else if (marker_text == "tc") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // tc1
      // row/cell
      else if (marker_text == "tc1") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      // tc2
      // row/cell
      else if (marker_text == "tc2") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      // tc3
      // row/cell
      else if (marker_text == "tc3") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      // tc4
      // row/cell
      else if (marker_text == "tc4") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      else if (marker_text == "tc5") {
        transform_remove_marker (usfm_code, marker_length);
      }

      else if (marker_text == "tc6") {
        transform_remove_marker (usfm_code, marker_length);
      }

      else if (marker_text == "tcr") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // tcr1
      // row/cell[@align='right']
      else if (marker_text == "tcr1") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      // tcr2
      // row/cell[@align='right']
      else if (marker_text == "tcr2") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      // tcr3
      // row/cell[@align='right']
      else if (marker_text == "tcr3") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      // tcr4
      // row/cell[@align='right']
      else if (marker_text == "tcr4") {
        transform_remove_marker (usfm_code, marker_length);
      }
      
      else if (marker_text == "tcr5") {
        transform_remove_marker (usfm_code, marker_length);
      }

      else if (marker_text == "tcr6") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // f
      // note 
      else if (marker_text == "f") {
        transform_note (usfm_code, marker_length, marker_is_opener, false, false);
      }

      // fe
      // note[@placement="end"] 
      else if (marker_text == "fe") {
        transform_note (usfm_code, marker_length, marker_is_opener, true, false);
      }

      // fr 
      // note/reference[@type="source"] 
      else if (marker_text == "fr") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "reference", "type", "source");
      }

      // fk 
      // catchWord 
      else if (marker_text == "fk") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "catchWord");
      }

      // fq 
      // note/q 
      else if (marker_text == "fq") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "q");
      }

      // fqa 
      // note/rdg 
      else if (marker_text == "fqa") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "rdg");
      }

      else if (marker_text == "fl") {
        // We would have put it in a character style "x-label", 
        // but then the resulting OSIS file fails to validate against the schema.
        // So we just remove the character style.
        transform_character_style (usfm_code, marker_length, marker_is_opener, NULL);
      }

      else if (marker_text == "fp") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // fv 
      // seg[@type="verseNumber"]
      else if (marker_text == "fv") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "seg", "type", "verseNumber");
      }

      // ft 
      // text within the note element, may serve to indicate the 
      // end of text of another format marker 
      else if (marker_text == "ft") {
        transform_remove_marker (usfm_code, marker_length);
        ensure_character_style_closed ();
      }

      // fdc 
      // note[not(@type='crossReference')]/seg[@edition="dc"] 
      else if (marker_text == "fdc") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "seg", "edition", "dc");
      }

      // fm 
      // This marker should never be found in field texts. It is 
      // for internal use only in publishing centers. 
      else if (marker_text == "fm") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // x note[@type="crossReference"] 
      else if (marker_text == "x") {
        transform_note (usfm_code, marker_length, marker_is_opener, false, true);
      }

      // xo 
      else if (marker_text == "xo") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // xk 
      // catchWord 
      else if (marker_text == "xk") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "catchWord");
      }

      // xq 
      // q 
      else if (marker_text == "xq") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "q");
      }

      // xt 
      // text within the note element, may serve to indicate the 
      // end of text of another format marker 
      else if (marker_text == "xt") {
        transform_remove_marker (usfm_code, marker_length);
        ensure_character_style_closed ();
      }

      else if (marker_text == "xot") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "seg", "edition", "x-ot");
      }

      else if (marker_text == "xnt") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "seg", "edition", "x-nt");
      }

      // xdc 
      // note[@type='crossReference']/seg[@edition="dc"] 
      else if (marker_text == "xdc") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "seg", "edition", "dc");
      }

      // add 
      // transChange[@type="added"]
      else if (marker_text == "xdc") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "transChange", "type", "added");
      }

      // bk
      // reference[@type="x-bookName"]
      else if (marker_text == "bk") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "reference", "type", "x-bookName");
      }

      // dc 
      // transChange[@type="added" and @edition="dc"] 
      else if (marker_text == "dc") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "reference", "type", "added", "edition", "dc");
      }

      // k 
      // seg[@type="keyword"] 
      else if (marker_text == "k") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "seg", "type", "keyword");
      }

      // lit 
      // lg[@type='doxology']/l[@type='refrain'] 
      else if (marker_text == "lit") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "lg", "type", "doxology");
      }

      // nd 
      // divineName 
      else if (marker_text == "nd") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "divineName");
      }

      // ord 
      // hi[@type="super"] 
      else if (marker_text == "nd") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "hi", "type", "super");
      }

      // pn 
      // name
      else if (marker_text == "pn") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "name");
      }

      // qt
      // seg[@type="otPassage"]
      else if (marker_text == "qt") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "seg", "type", "otPassage");
      }

      // sig 
      // signed
      else if (marker_text == "sig") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "signed");
      }
      
      // sls
      // foreign[@type="x-secondaryLanguage"] 
      else if (marker_text == "sls") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "foreign", "type", "x-secondaryLanguage");
      }

      // tl
      // foreign 
      else if (marker_text == "tl") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "foreign");
      }

      else if (marker_text == "wj") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "q", "who", "Jesus");
      }
      
      // em 
      // hi[@type="emphasis"] 
      else if (marker_text == "em") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "hi", "type", "emphasis");
      }

      // bd
      // hi[@type="bold"]
      else if (marker_text == "bd") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "hi", "type", "bold");
      }

      // it
      // hi[@type="italic"] 
      else if (marker_text == "it") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "hi", "type", "italic");
      }

      // bdit
      // hi[@type="bold"]/hi[@type="italic"]
      else if (marker_text == "bdit") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "hi", "type", "bold", "type", "italic");
      }

      // no 
      // hi[@type="normal"] 
      else if (marker_text == "bdit") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "hi", "type", "normal");
      }

      // sc 
      // hi[@type="small-caps"]
      else if (marker_text == "sc") {
        transform_character_style (usfm_code, marker_length, marker_is_opener, "hi", "type", "small-caps");
      }
      
      // marker="~"

      // marker="//"

      // pb 
      // milestone[@type="pb"] 
      else if (marker_text == "pb") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // fig 
      // figure (map attributes to the "|" separated values) 
      else if (marker_text == "fig") {
        transform_figure (usfm_code, marker_text, marker_length);
      }

      // ndx 
      // index[@name="subject",@level1="..."] 
      else if (marker_text == "ndx") {
        transform_remove_marker (usfm_code, marker_length);
      }

      else if (marker_text == "pro") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // w 
      // index[@level1="..."]... 
      else if (marker_text == "w") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // wh 
      // index[@name="hebrew",@level1="..."]... 
      else if (marker_text == "wh") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // wg 
      // index[@name="greek",@level1="..."]... 
      else if (marker_text == "wg") {
        transform_remove_marker (usfm_code, marker_length);
      }

      // periph

      // The following markers are no longer part of the USFM standard.
            
      // conc
      // div[@type="concordance"]
            
      // cov
      // div[@type="coverPage"]

      // glo 
      // div[@type="glossary"] 

      // idx 
      // div[@type="index"]

      // intro
      // div[@type="introduction"] 

      // map 
      // div[@type="map"]

      // phi 
      // treat same as ph 

      // pref
      // div[@type="preface"] 
      
      // ps
      // p 
      
      // psi 
      // treat same as pi
     
      // pub 
      // div[@type="publicationData"]
      
      // pubinfo
      // Encode in using the Doublin Core elements in work in the header. 
      // If for presentation purposes this may be encoded within div[@type="publicationData"] 
      
      // restore
      // description[@type="usfm" and subType="x-restore"]
      else if (marker_text == "restore") {
        transform_usfm_description (usfm_code, marker_text, marker_length);
      } 
      
      // spin 
      // div[@type="spine"] 
      
      else {
        transform_fallback(usfm_code);
      }
      
    } else {
      transform_fallback(usfm_code);
    }
  }

  // Be sure that the division is closed.
  ensure_division_closed ();

  // Appendix F.1 Notes on \b, \m, and \mi 

  // If \b immediately follows a line group, list, or table and is not immediatelly followed by \s, \s#, \p, \ms, \ms#, 
  // or \ps, then the \b is simply indicating that the line group, list, or table is closing, OTHERWISE an explicit 
  // line break must be encoded 

  // \m immediately following a line group, list, or table closes the line group, list, or table and continues the text 
  // that follows in the already open container including a div element OTHERWISE an explicit line break must 
  // be inserted and the text that follows in the already open container including div (if a \m immediately follows 
  // a \b then an explicit line break is inserted) 

  // \mi immediately following a line group, list, or table closes the line group, list, or table and continues the text 
  // that follows in the parent of the line group, list, or table if an ancestor is q[@type="embeded] OTHERWISE 
  // a q[@type='embedded'] is inserted into the parent of of the line group, list, or table, OTHERWISE if a 
  // q[type='embedded'] is an ancestor insert an explicit line break. If not q[@type='embedded'] is an ancestor 
  // then open one and continue the text that follows with no line break. 

  // if the current container or one of its ancestors is a q[type='embeded'] then insert a explicit line break and 
  // continue within the current container OTHERWISE open a q[@type='embeded'] and continue within it 

  // \ib, \im, and \imi behave the same as \b, \m, and \mi but are within a div[@type='introduction'] 

  // In addation and overriding the above, if a line break ( 
  // ) element would be encoded as the first node (element node or text node) within a div, p, or q element so as 
  // to represent a \m or \mi concept, the line break element is unnecessary and should be ommited. 

  // In all of the above rules regarding \b, \m, \mi, \imi, and \im a note is ignored when determining immediately 
  // following, immediately preceding, and first relationships. 
}


void Usfm2Osis::transform_fallback(ustring& usfm_code)
// This transformation is called when no other transformation could be done.
{
  // Bail out if there's no usfm code available.
  if (usfm_code.empty())
    return;

  // Ensure paragraph state.
  ensure_paragraph_opened();

  // Get the text till the next marker.
  ustring marker;
  size_t marker_position;
  size_t marker_length;
  bool is_opener;
  bool marker_found = usfm_search_marker(usfm_code, marker, marker_position, marker_length, is_opener);
  ustring text;
  if (marker_found) {
    text = usfm_code.substr(0, marker_position);
    usfm_code.erase(0, marker_position);
    // Handle the case that the marker is at the start. 
    // Should never occur, but it is put here for robustness.
    if (marker_position == 0) {
      text = usfm_code.substr(0, 1);
      usfm_code.erase(0, 1);
    }
  } else {
    text = usfm_code;
    usfm_code.clear();
  }

  // Write that text. 
  xmlTextWriterWriteFormatString(xmlwriter, "%s", text.c_str());
}


void Usfm2Osis::transform_usfm_description (ustring& usfm_code, const ustring& marker_text, size_t marker_length)
// According to the Osis manual, certain USFM codes translate into "description" elements in Osis.
{
  ustring text = get_erase_code_till_next_marker (usfm_code, 0, marker_length, true);
  ustring subtype = "x-" + marker_text;
  /*
  The OSIS file fails to validate if a "description" element is written.
  So we just get rid of it, without transforming it.  
  xmlTextWriterStartElement(xmlwriter, BAD_CAST "description");
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "type", "usfm");
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "subType", subtype.c_str());
  xmlTextWriterWriteFormatString(xmlwriter, "%s", text.c_str());
  xmlTextWriterEndElement(xmlwriter);
  */
}


void Usfm2Osis::transform_h_title (ustring& usfm_code, size_t marker_length, bool runningheader, const gchar * placement)
// This transforms the \h into the Osis title element.
{
  ustring title = get_erase_code_till_next_marker (usfm_code, 0, marker_length, true);
  xmlTextWriterStartElement(xmlwriter, BAD_CAST "title");
  if (runningheader) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "type", "runningHead");
  }
  if (placement) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "placement", "%s", placement);
  }
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "short", "%s", title.c_str());
  xmlTextWriterEndElement(xmlwriter);
}


void Usfm2Osis::transform_per_osis_division(ustring& usfmcode)
// Chops the code up into OSIS divisions and initiates the transformation.
{
  // Division text
  ustring division_usfm_code;
  
  // Go through all USFM code.
  ustring usfm_code;
  ustring marker_text;
  size_t marker_position;
  size_t marker_length;
  bool marker_is_opener;
  bool marker_found;
  while (usfm_code_available (usfm_code, usfmcode, 1000)) {
    marker_found = usfm_search_marker(usfm_code, marker_text, marker_position, marker_length, marker_is_opener);
    if (marker_found && (marker_position == 0)) {
      // A marker is right at the start of the line.
      if (marker_is_opener && usfm_is_osis_division (marker_text)) {
        transform_block(division_usfm_code);
      }
      // Remove the marker from the input stream, 
      // and add it to the USFM code of this division.
      ustring text = usfm_code.substr (0, marker_length);
      division_usfm_code.append (text);
      usfm_code.erase (0, marker_length);
    } else {
      // There's no marker at the start, but something else.
      // Remove it from the input stream, and add it to the division. 
      ustring text = get_erase_code_till_next_marker (usfm_code, 0, 0, false);
      division_usfm_code.append (text);
    }
  }

  // Transform possible remaining block of USFM code.
  transform_block(division_usfm_code);
  
  // Be sure that the chapter is closed.
  ensure_chapter_closed ();
}


bool Usfm2Osis::usfm_is_osis_division (const ustring& marker)
// Returns true if the marker would result in an osis division.
{
  extern Styles * styles;
  USFMStandard * usfmstandard = styles->usfmstandard();  
  UsfmFunctionType function = usfmstandard->marker_function (marker);
  switch (function) {
    case uftUnknown:        return false;
    case uftFile:           return true;
    case uftEncoding:       return false;
    case uftStatus:         return false;
    case uftComment:        return false;
    case uftRunningHeader:  return false;
    case uftLongToc:        return false;
    case uftShortToc:       return false;
    case uftBookAbbr:       return false;
    case uftTitle:          return true;
    case uftSection:        return true;
    case uftParagraph:      return false;
    case uftList:           return false;
    case uftText:           return false;
    case uftChapter:        return false;
    case uftChapterLabel:   return false;
    case uftChapterMarker:  return false;
    case uftVerse:          return false;
    case uftVerseMarker:    return false;
    case uftNoBreak:        return false;
    case uftRow:            return false;
    case uftCell:           return false;
    case uftFootnote:       return false;
    case uftEndnote:        return false;
    case uftNoteMark:       return false;
    case uftCrossreference: return false;
    case uftKeyword:        return false;
    case uftSpace:          return false;
    case uftBreak:          return false;
    case uftPage:           return false;
    case uftFigure:         return false;
    case uftIndex:          return false;
    case uftPronunciation:  return false;
    case uftWordlist:       return false;
    case uftGreek:          return false;
    case uftHebrew:         return false;
    case uftPeripheral:     return true;
  }
  return false;
}


void Usfm2Osis::transform_division (const gchar * type, bool canonical)
// Opens a new division.
{
  // Ensure any previous division or paragraph are closed.
  ensure_paragraph_closed ();

  // Open a division.
  ensure_division_opened ();
  if (type) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "type", "%s", type);
  }
  ustring canonical_text = "false";
  if (canonical) {
    canonical_text = "true";
  }
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "canonical", "%s", canonical_text.c_str());
}


void Usfm2Osis::transform_general_title (ustring& usfm_code, size_t marker_length, const gchar * type, unsigned int level)
// This transforms a general title.
{
  ensure_paragraph_closed();
  ensure_division_opened();
  xmlTextWriterStartElement(xmlwriter, BAD_CAST "title");
  if (type) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "type", "%s", type);
  }
  if (level) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "level", "%d", level);
  }
  ustring title = get_erase_code_till_next_marker (usfm_code, 0, marker_length, true);
  xmlTextWriterWriteFormatString(xmlwriter, "%s", title.c_str());
  xmlTextWriterEndElement(xmlwriter);
}


void Usfm2Osis::transform_paragraph_start (ustring& usfm_code, size_t marker_length)
// Transforms code that starts a paragraph.
{
  // Close any previous paragraph.
  ensure_paragraph_closed ();
  
  // Open a paragraph.
  ensure_paragraph_opened();

  // Remove the marker from the input code.
  usfm_code.erase (0, marker_length);
}


void Usfm2Osis::ensure_division_opened ()
{
  // If the division is not yet open, open it.
  if (!division_open) {
    xmlTextWriterStartElement(xmlwriter, BAD_CAST "div");
    division_open = true;
  }  
}


void Usfm2Osis::ensure_division_closed ()
{
  // First ensure that the paragraph is closed.
  ensure_paragraph_closed();
  // If the division is open, close it.
  if (division_open) {
    xmlTextWriterEndElement(xmlwriter);
    division_open = false;
  }
}


void Usfm2Osis::ensure_paragraph_opened ()
{
  // First ensure that a division is open.
  ensure_division_opened();
  // If the paragraph is not open, open one.
  if (!paragraph_open) {
    xmlTextWriterStartElement(xmlwriter, BAD_CAST "p");
    paragraph_open = true;
  }
}


void Usfm2Osis::ensure_paragraph_closed ()
{
  // If the paragraph is open, close it.
  if (paragraph_open) {
    ensure_character_style_closed();
    ensure_note_closed();
    xmlTextWriterEndElement(xmlwriter);
    paragraph_open = false;
  }
}


void Usfm2Osis::transform_remove_marker (ustring& usfm_code, size_t marker_length)
// This just removes the marker.
{
  // Remove the marker from the input code.
  usfm_code.erase (0, marker_length);
}


void Usfm2Osis::ensure_chapter_closed ()
// Ensures that the chapter milestone is closed.
{
  ensure_verse_closed();
  if (!chapter_osis_id.empty()) {
    // Close chapter milestone, e.g.: <chapter eID="Titus.2"/>
    xmlTextWriterStartElement(xmlwriter, BAD_CAST "chapter");
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "eID", "%s", chapter_osis_id.c_str());
    xmlTextWriterEndElement(xmlwriter);
    chapter_osis_id.clear();
  }
}


void Usfm2Osis::transform_chapter_number (ustring& usfm_code, size_t marker_length)
{
  // Extract the chapter number from the usfm code.
  usfm_code.erase (0, marker_length);
  ustring chapter_number = get_erase_code_till_next_marker (usfm_code, 0, 0, true);
  // Store the OSIS id.
  chapter_osis_id = book_osis_id + "." + chapter_number;
  // Write milestone, e.g.: <chapter sID="Titus.2" osisID="Titus.2" n="2"/> 
  xmlTextWriterStartElement(xmlwriter, BAD_CAST "chapter");
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "sID", "%s", chapter_osis_id.c_str());
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "osisID", "%s", chapter_osis_id.c_str());
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "n", "%s", chapter_number.c_str());
  xmlTextWriterEndElement(xmlwriter);
}


void Usfm2Osis::ensure_verse_closed ()
// Ensures that the verse milestone is closed.
{
  if (!verse_osis_id.empty()) {
    // Close verse milestone, e.g.: <verse eID="Titus.2.1" />
    xmlTextWriterStartElement(xmlwriter, BAD_CAST "verse");
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "eID", "%s", verse_osis_id.c_str());
    xmlTextWriterEndElement(xmlwriter);
    verse_osis_id.clear();
  }
}


void Usfm2Osis::transform_verse_number (ustring& usfm_code, size_t marker_length)
{
  // Extract the verse number from the usfm code.
  usfm_code.erase (0, marker_length);
  ustring verse_number = usfm_get_verse_number (usfm_code, true, true);
  // Store the OSIS id.
  verse_osis_id = chapter_osis_id + "." + verse_number;
  // Write milestone, e.g.: <verse sID="Titus.2.1" osisID="Titus.2.1"/>
  xmlTextWriterStartElement(xmlwriter, BAD_CAST "verse");
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "sID", "%s", verse_osis_id.c_str());
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "osisID", "%s", verse_osis_id.c_str());
  xmlTextWriterEndElement(xmlwriter);
}


void Usfm2Osis::transform_note (ustring& usfm_code, size_t marker_length, bool is_opener, bool endnote, bool xref)
/*
This procedure only does the start of the note, and the end. All markers in between
are handled by the main procedure.

Sample transformation for a whole note:
USFM: \f + \fr 3:20 \ft \fk Ê‑va\fk* nghĩa là \fq sự sống.\fq\f*
OSIS: <note osisRef="Gen.3.20" osisID="Gen.3.20!footnote.2" n="2"><catchWord>Ê-va</catchWord> nghĩa là <q>sự sống.</q></note>

*/
{
  // Remove the marker from the input stream.
  usfm_code.erase (0, marker_length);

  // Ensure any running note gets closed.
  ensure_note_closed();
  
  // A pararaph should be open where the note can be inserted into.
  ensure_paragraph_opened();
  
  // If the marker ends the note, bail out. The note has been closed already.
  if (!is_opener) {
    return;
  }
  
  // Open the note element, and set some attributes.
  xmlTextWriterStartElement(xmlwriter, BAD_CAST "note");
  if (endnote) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "placement", "end");
  }
  if (xref) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "type", "crossReference");
  } else {
    // From USFM one cannot extract information about the type of the note, 
    // yet we need to pick a type, so let's call it a "study" note.
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "type", "study");
  }

  // Write the osisRef.
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "osisRef", "%s", verse_osis_id.c_str());

  // Extract the caller from the input stream.
  ustring caller = "+";
  if (!usfm_code.empty()) {
    caller = usfm_code.substr (0, 1);
    usfm_code.erase (0, 1);
  }

  // Remove whitespace following the caller.
  if (!usfm_code.empty()) {
    if (usfm_code.substr (0, 1) == " ") {
      usfm_code.erase (0, 1);
    }
  }    
  
  // Derive the caller as to be put in the OSIS note.
  if (caller == "+")
    caller.clear();
  if (caller == "-");
    caller.clear();
  
  // Write the osisID.
  ustring note_osis_id = verse_osis_id + "!";
  if (endnote) {
    note_osis_id.append ("endnote");
  }
  else if (xref) {
    note_osis_id.append ("crossReference");
  }
  else {
    note_osis_id.append ("footnote");
  }
  if (!caller.empty()) {
    note_osis_id.append ("." + caller);
  }
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "osisID", "%s", note_osis_id.c_str());

  // Write the n attribute, if it's there.
  if (!caller.empty()) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "n", "%s", caller.c_str());
  }    

  // Set a flag that the note is open.
  note_is_open = true;
}


void Usfm2Osis::ensure_note_closed ()
// Ensure that a note is closed.
{
  if (note_is_open) {
    ensure_character_style_closed ();
    xmlTextWriterEndElement(xmlwriter);
    note_is_open = false;
  }
}


void Usfm2Osis::transform_character_style (ustring& usfm_code, size_t marker_length, bool is_opener, const gchar * element)
{
  transform_character_style (usfm_code, marker_length, is_opener, element, NULL, NULL);
}


void Usfm2Osis::transform_character_style (ustring& usfm_code, size_t marker_length, bool is_opener, const gchar * element, const gchar * attribute_name, const gchar * attribute_value)
{
  transform_character_style (usfm_code, marker_length, is_opener, element, attribute_name, attribute_value, NULL, NULL);
}


void Usfm2Osis::transform_character_style (ustring& usfm_code, size_t marker_length, bool is_opener, const gchar * element, const gchar * attribute1_name, const gchar * attribute1_value, const gchar * attribute2_name, const gchar * attribute2_value)
{
  // Remove the marker from the input stream.
  usfm_code.erase (0, marker_length);

  // Ensure that a paragraph is opened.
  ensure_paragraph_opened();

  // If there's any character style, always ensure that the previous one closes.
  ensure_character_style_closed ();
  
  // If it happened to be a closing marker, we've done all what's needed: bail out.
  if (!is_opener)
    return;

  // If there's no element, bail out.
  if (element == NULL)
    return;

  // Open the element for the character style.
  xmlTextWriterStartElement(xmlwriter, BAD_CAST element);

  // Optionally write the first attribute.
  if (attribute1_name) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST attribute1_name, "%s", attribute1_value);
  }

  // Optionally write the second attribute.
  if (attribute2_name) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST attribute2_name, "%s", attribute2_value);
  }

  // Set flag that a character style is open.
  character_style_open = true;
}


void Usfm2Osis::ensure_character_style_closed()
{
  if (character_style_open) {
    xmlTextWriterEndElement(xmlwriter);
    character_style_open = false;
  }
}

  
void Usfm2Osis::transform_figure (ustring& usfm_code, const ustring& marker_text, size_t marker_length)
{
  // Get the actual bit that describes the picture; erase it from the line.
  // The picture comes in the form of, e.g. "|biblesociety.gif|col||||"
  ustring desc;
  ustring file;
  ustring size;
  ustring loc;
  ustring copy;
  ustring cap;
  ustring ref;
  usfm_dissect_figure (usfm_code, marker_text, marker_length, desc, file, size, loc, copy, cap, ref);

  // Transform to osis.
  xmlTextWriterStartElement(xmlwriter, BAD_CAST "figure");
  if (!desc.empty()) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "desc", "%s", desc.c_str());
  }
  if (!file.empty()) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "file", "%s", file.c_str());
  }
  if (!size.empty()) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "size", "%s", size.c_str());
  }
  if (!loc.empty()) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "loc", "%s", loc.c_str());
  }
  if (!copy.empty()) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "copy", "%s", copy.c_str());
  }
  if (!cap.empty()) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "cap", "%s", cap.c_str());
  }
  if (!ref.empty()) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "ref", "%s", ref.c_str());
  }
  xmlTextWriterEndElement(xmlwriter);
}


