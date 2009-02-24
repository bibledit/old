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
xmllint --schema osisCore.2.1.1.xsd osis-from-usfm.txt 

*/

Usfm2Osis::Usfm2Osis(const ustring& file)
{
  // Initialize variables.
  osisfile = file;
  book_bibledit_id = 0;
  
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
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "osisIDWork", name.c_str());
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "osisRefWork", "Bible");
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "xml:lang", "en");
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "canonical", "true");

      // The "header" element is a direct child of element "osisText".
      xmlTextWriterStartElement(xmlwriter, BAD_CAST "header");

        xmlTextWriterStartElement(xmlwriter, BAD_CAST "work");
        xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "osisWork", name.c_str());

          xmlTextWriterStartElement(xmlwriter, BAD_CAST "title");
          xmlTextWriterWriteFormatString(xmlwriter, "%s", description.c_str());
          xmlTextWriterEndElement(xmlwriter);

          xmlTextWriterStartElement(xmlwriter, BAD_CAST "type");
          xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "type", "OSIS");
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
          xmlTextWriterWriteFormatString(xmlwriter, "The document is free. It is copyrighted (©) %d by %s. Permission is granted to copy, distribute and/or modify the documents under the terms of the GNU Free Documentation License, Version 1.2 or any later version published by the Free Software Foundation; with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.", year, g_get_real_name ());
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
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "osisID", book_osis_id.c_str());
}


void Usfm2Osis::close_book()
{
  if (book_bibledit_id) {
    xmlTextWriterEndElement(xmlwriter);
    book_bibledit_id = 0;
  }
}


void Usfm2Osis::load_book (vector <ustring>& data)
// Accept the USFM code of a full book and starts the transformation.
{
  book_usfm_code.clear();  
  for (unsigned int i = 0; i < data.size(); i++) {
    if (i > 0) {
      book_usfm_code.append (" ");
    }
    book_usfm_code.append (data[i]);
  }
  replace_text (book_usfm_code, "\n", " ");
  
  transform_headers_descriptions();
}


void Usfm2Osis::transform_headers_descriptions() // Todo
// Does the first stage of the transformation.
// It finds descriptions, and (running) headers for the book.
{
  // Variable that holds the usfm code that has not been handled in this routine.
  ustring unhandled_usfm_code;
  
  // Go through all USFM code.
  ustring marker_text;
  size_t marker_position;
  size_t marker_length;
  bool marker_is_opener;
  bool marker_found;
  while (!book_usfm_code.empty()) {
    marker_found = usfm_search_marker(book_usfm_code, marker_text, marker_position, marker_length, marker_is_opener);
    if (marker_found && (marker_position == 0)) {
      // A marker is right at the start of the line.
      if (false) {
      }
      // id 
      // description[@type='usfm' and @subType='x-id']
      else if (marker_text == "id") {
        transform_usfm_description (marker_text, marker_length);
      } 
      // ide
      // description[@type='usfm' and @subType='x-ide']
      else if (marker_text == "ide") {
        transform_usfm_description (marker_text, marker_length);
      } 
      // toc
      // div[@type="tableOfContents"] 
      // There are several toc markers in USFM. This converter transforms it into a "x-toc" description.
      else if (marker_text == "toc") {
        transform_usfm_description (marker_text, marker_length);
      } 
      else if (marker_text == "toc1") {
        transform_usfm_description (marker_text, marker_length);
      } 
      else if (marker_text == "toc2") {
        transform_usfm_description (marker_text, marker_length);
      } 
      else if (marker_text == "toc3") {
        transform_usfm_description (marker_text, marker_length);
      }
      // h 
      // div[@type='book']/title[@short='value of h marker']
      else if (marker_text == "h") {
        transform_h_title (marker_length, false, NULL);
      } 
      // h1 
      // div[@type='book']/title[@short='value of h marker']
      else if (marker_text == "h1") {
        transform_h_title (marker_length, false, NULL);
      } 
      // h2 
      // title[@type="runningHead" and placement="inside" and @short='value of h2 marker'] 
      else if (marker_text == "h2") {
        transform_h_title (marker_length, true, "inside");
      } 
      // h3 
      // title[@type="runningHead" and placement="outside" and @short='value of h3 marker'] 
      else if (marker_text == "h3") {
        transform_h_title (marker_length, true, "outside");
      } 
      else {
        // A marker is at the start, but it has not been handled,
        // so the marker is removed from the input stream, 
        // and added to the unhandled USFM code.
        ustring text;
        text.append (book_usfm_code.substr (0, marker_length));
        unhandled_usfm_code.append (text);
        book_usfm_code.erase (0, marker_length);
      }
    } else {
      // There's no marker at the start, but something else.
      // Whatever's there, remove it from the input stream, 
      // and add it to the unhandled USFM code for later processing.
      ustring text = get_erase_code_till_next_marker (book_usfm_code, 0, 0, false);
      unhandled_usfm_code.append (text);
    }
  }
  // Keep the unhandled USFM code for further processing.
  book_usfm_code = unhandled_usfm_code;
}

void Usfm2Osis::transform() // Todo
// Does the transformation.
// An attempt is made to follow the OSIS manual, Appendix F, USFM to OSIS Mapping.
// If needed improvements have been made.
{
  ustring marker_text;
  size_t marker_position;
  size_t marker_length;
  bool marker_is_opener;
  bool marker_found;

  while (!book_usfm_code.empty()) {
    marker_found = usfm_search_marker(book_usfm_code, marker_text, marker_position, marker_length, marker_is_opener);
    if (marker_found && (marker_position == 0)) {

      if (false) {
      }

      // add 
      // transChange[@type="added"]

      // b
      // See notes below on \m and \b 

      // bd
      // hi[@type="bold"]

      // bdit
      // hi[@type="bold"]/hi[@type="italic"]

      // bk
      // reference[@type="x-bookName"]

      // c 

      // ca
      // If it is desired to have two "n" attribute values then 
      // encode two chapter elements - otherwise simply put 
      // one or more identifier in the osisID and up to one value 
      // for "n".

      // cd
      // div[@type="book"]/div[@type="introduction" and 
      // preceeding-sibling::chapter] -- e.g. make this an intro 
      // div which immediately follows a chapter milestone 

      // cl
      // title type="chapterLabel"
      
      // cls
      // closer
      
      // conc
      // div[@type="concordance"]
            
      // cov
      // div[@type="coverPage"]

      // cp
      // If it is desired to have two "n" attribute values then 
      // encode two chapter elements - otherwise simply put 
      // one or more identifier in the osisID and up to one value 
      // for "n". 

      // d
      // title[@type="psalm"] 

      // dc 
      // transChange[@type="added" and @edition="dc"] 

      // em 
      // hi[@type="emphasis"] 

      // f
      // note 

      // fdc 
      // note[not(@type='crossReference')]/seg[@edition="dc"] 

      // fe
      // note[@placement="end"] 

      // fig 
      // figure (map attributes to the "|" separated values) 

      // fk 
      // catchWord 

      // fm 
      // This marker should never be found in field texts. It is 
      // for internal use only in publishing centers. 

      // fq 
      // note/q 

      // fqa 
      // note/rdg 

      // fr 
      // note/reference[@type="source"] 

      // ft 
      // text within the note element, may serve to indicate the 
      // end of text of another format marker 

      // fv 
      // seg[@type="verseNumber"]

      // glo 
      // div[@type="glossary"] 

      // h 
      // div[@type='book']/title[@short='value of h marker']
      else if (marker_text == "h") {
        // At this stage the div[@type='book'] will be open, so no further action is needed.
        transform_h_title (marker_length, false, NULL);
      } 

      // h1 
      // div[@type='book']/title[@short='value of h marker']
      else if (marker_text == "h1") {
        // At this stage the div[@type='book'] will be open, so no further action is needed.
        transform_h_title (marker_length, false, NULL);
      } 

      // h2 
      // title[@type="runningHead" and placement="inside" and
      // @short='value of h2 marker'] 
      else if (marker_text == "h2") {
        transform_h_title (marker_length, true, "inside");
      } 

      // h3 
      // title[@type="runningHead" and placement="outside"
      // and @short='value of h3 marker'] 
      else if (marker_text == "h2") {
        transform_h_title (marker_length, true, "outside");
      } 

      // ib See notes below on \m and \b 
      // (This is a format oriented marker --whitespace, but 
      // needs to be preserved round-trip) 

      // id 
      // description[@type='usfm' and @subType='x-id']
      else if (marker_text == "id") {
        transform_usfm_description (marker_text, marker_length);
      } 
      
      // ide
      // description[@type='usfm' and @subType='x-ide']
      else if (marker_text == "ide") {
        transform_usfm_description (marker_text, marker_length);
      } 

      // idx 
      // div[@type="index"]

      // ie 

      // iex 
      // div[@type="bridge"] 

      // im See notes below on \m and \b 

      // imi 
      // Continue the div[@type="introduction" and 
      // @canonical="false"]//list/item/p if interrupted 
      // otherwise div[@type="introduction" and 
      // @canonical="false"]//list/item 

      // imq 
      // div[@type="introduction" and @canonical="false"]//q 

      // imt 
      // div[@type="introduction" and 
      // @canonical="false"]/title[@type="main"] 

      // imt1 
      // div[@type="introduction" and @canonical="false"]/title[@type="main"]/title[@level="1"] 

      // imt2 
      // div[@type="introduction" and @canonical="false"]/title[@type="main"]/title[@level="2"] 

      // imt3 
      // div[@type="introduction" and @canonical="false"]/title[@type="main"]/title[@level="3"] 

      // imt4 
      // div[@type="introduction" and @canonical="false"]/title[@type="main"]/title[@level="4"] 

      // imte 
      // div[@type="introduction" and @canonical="false"]/title[@type="main"] 

      // intro
      // div[@type="introduction"] 

      // iot 
      // div[@type="introduction" and @canonical="false"]//div[@type="outline"]/title 

      // io1 
      // div[@type="introduction" and @canonical="false"]//div[@type="outline"]/list/item 

      // io2 
      // div[@type="introduction" and @canonical="false"]//div[@type="outline"]/list/item/list/item 

      // io3 
      // div[@type="introduction" and @canonical="false"]//div[@type="outline"]/list/item/list/item/list/item 

      // io4 
      // div[@type="introduction" and @canonical="false"]//div[@type="outline"]/list/item/list/item/list/item/list/item 

      // ior 
      // div[@type="introduction" and @canonical="false"]//div[@type="outline"]/list/item/reference 

      // ip 
      // div[@type="introduction" and @canonical="false"]//p 

      // ipi 
      // div[@type="introduction" and @canonical="false"]//list/item/p

      // ipq
      // div[@type="introduction" and @canonical="false"]//q/p
      
      // ipr 
      // div[@type="introduction" and @canonical="false"]//reference 
      
      // iq
      // div[@type="introduction" and @canonical="false"]//lg/l[@level="1"]
      
      // iq1
      // div[@type="introduction" and @canonical="false"]//lg/l[@level="1"] 

      // iq2
      // div[@type="introduction" and @canonical="false"]//lg/l[@level="2"] 
      
      // iq3 
      // div[@type="introduction" and @canonical="false"]//lg/l[@level="3"] 
      
      // is
      // div[@type="introduction" and @canonical="false"]/div[@type="section"]/title 
      // is1 
      // div[@type="introduction" and @canonical="false"]/div[@type="section"]/title 

      // is2
      // div[@type="introduction" and @canonical="false"]/div[@type="section"]/div[@type="subSection"]/title 

      // it
      // hi[@type="italic"] 

      // k 
      // seg[@type="keyword"] 

      // li 
      // list/item 

      // li1 
      // list/item 

      // li2 
      // list/item/list/item 

      // li3 
      // list/item/list/item/list/item 

      // li4 
      // list/item/list/item/list/item/list/item 
      
      // lit 
      // lg[@type='doxology']/l[@type='refrain'] 

      // m
      // See notes below on \m and \b 
      
      // map 
      // div[@type="map"]

      // mi 
      // See notes below on \m and \b
      
      // mr 
      // div[@type="majorSection"]/title[@type="scope"]/reference 

      // mt // Todo from here ...
      // div[@type="book"]/title[@type="main"]
      
      // mt1 
      // div[@type="book"]/title[@type="main"]/title[@level="1"] 

      // mt2 
      // div[@type="book"]/title[@type="main"]/title[@level="2"] 

      // mt3 
      // div[@type="book"]/title[@type="main"]/title[@level="3"] 
      
      // mt4 
      // div[@type="book"]/title[@type="main"]/title[@level="4"] 
      
      // mte 
      // div[@type="book"]/title[@type="main"]
      
      // mte1 
      // div[@type="book"]/title[@type="main"]/title[@level="1"]

      // mte2  // Todo ... to here
      // div[@type="book"]/title[@type="main"]/title[@level="2"]
      
      // ms
      // div[@type="majorSection"]/title

      // ms1 
      // div[@type="majorSection"]/title
      
      // ms2 
      // div[@type="majorSection"]/div[@type="majorSection"]/title
      
      // nb 

      // nd 
      // divineName 

      // ndx 
      // index[@name="subject",@level1="..."] 

      // no 
      // hi[@type="normal"] 

      // ord 
      // hi[@type="super"] 

      // p 
      // p 

      // pb 
      // milestone[@type="pb"] 

      // pc 
      // inscription 

      // ph 
      // list/item 

      // ph1 
      // list/item 

      // ph2 
      // list/item/list/item 

      // ph3 
      // list/item/list/item/list/item 

      // phi 
      // treat same as ph 

      // pi 
      // list/item/p 

      // pi1 
      // list/item/p 

      // pi2 
      // list/item/list/item/p 

      // pi3 
      // list/item/list/item/list/item/p
      
      // pmo 
      // q[@type="embedded"]/salute
      
      // pm 
      // q[@type="embedded"]/p 
      
      // pmc 
      // q[@type="embedded"]/closer
      
      // pmr 
      // q[@type="embedded"]/p[@type="x-refrain"]
      
      // pn 
      // name
      
      // pr
      // p[@type="x-refrain"]
      
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
      
      // q 
      // lg/l[@level="1"]
      
      // q1
      // lg/l[@level="1"]
      
      // q2
      // lg/l[@level="2"] 
      
      // q3
      // lg/l[@level="3"]
      
      // qa
      // lg/l[@type="acrostic"]
      
      // qac
      // hl[@type="acrostic"]
      
      // qc 
      // l[@type="doxology"]
      
      // qm
      // q[@type="embedded"]/lg/l[@level="1"] (or leave off the level)
      
      // qm1
      // q[@type="embedded"]/lg/l[@level="1"] 
      
      // qm2
      // q[@type="embedded"]/lg/l[@level="2"]
      
      // qm3 q[@type="embedded"]/lg/l[@level="3"]
      
      // qr
      // l[@type="refrain"] and l[@type="attribution"] (choose qr based on context) 
      
      // qs
      // lg/l[@type="selah"]
      
      // qt
      // seg[@type="otPassage"]
      
      // r
      // title[@type='parallel']/reference
      
      // rem
      // description[@type="usfm" and subType="x-rem"]
      else if (marker_text == "rem") {
        transform_usfm_description (marker_text, marker_length);
      } 
      
      // restore
      // description[@type="usfm" and subType="x-restore"]
      else if (marker_text == "restore") {
        transform_usfm_description (marker_text, marker_length);
      } 
      
      // s
      // div[@type="section"]/title
      
      // s1
      // div[@type="section"]/title
      
      // s2
      // div[@type="subSection"]/title
      
      // s3
      // div[@type="subSection"]/div[@type="subSection"]/title

      // s4 
      // div[@type="subSection"]/div[@type="subSection"]/div[@type="subSection"]/title
      
      // sc 
      // hi[@type="small-caps"]
      
      // sig 
      // signed
      
      // sls
      // foreign[@type="x-secondaryLanguage"] 

      // sp 
      // speech/speaker 

      // spin 
      // div[@type="spine"] 
      
      // tc1
      // row/cell
      
      // tc2
      // row/cell
      
      // tc3
      // row/cell
      
      // tc4
      // row/cell
      
      // tcr1
      // row/cell[@align='right']
      
      // tcr2
      // row/cell[@align='right']
      
      // tcr3
      // row/cell[@align='right']
      
      // tcr4
      // row/cell[@align='right']
      
      // th1
      // row[@role="label"]/cell
      
      // th2
      // row[@role="label"]/cell
      
      // th3
      // row[@role="label"]/cell
      
      // th4
      // row[@role="label"]/cell
      
      // thr1
      // row[@role="label"]/cell[@align='right']
      
      // thr2
      // row[@role="label"]/cell[@align='right']

      // thr3
      // row[@role="label"]/cell[@align='right']
      
      // thr4
      // row[@role="label"]/cell[@align='right']
      
      // tl
      // foreign 

      // toc // Todo
      // div[@type="tableOfContents"] 
      // There are several toc markers in USFM. This converter transforms it into a "x-toc" description.
      // This is against the standard, but it was simpler to program.
      // Somebody might complain though ...
      else if (marker_text == "toc") {
        transform_usfm_description (marker_text, marker_length);
      } 
      else if (marker_text == "toc1") {
        transform_usfm_description (marker_text, marker_length);
      } 
      else if (marker_text == "toc2") {
        transform_usfm_description (marker_text, marker_length);
      } 
      else if (marker_text == "toc3") {
        transform_usfm_description (marker_text, marker_length);
      } 

      // tr 
      // table/row 

      // tr1 
      // treat same as tr 

      // tr2 
      // treat same as tr 

      // v 
      // verse\@osisID and verse\@n 

      // va 
      // If it is desired to have two "n" attribute values then 
      // encode two verse elements - otherwise simply put one 
      // or more identifier in the osisID and up to one value for 
      // "n". 

      // vp 
      // If it is desired to have two "n" attribute values then 
      // encode two verse elements - otherwise simply put one 
      // or more identifier in the osisID and up to one value for 
      // "n". 

      // w 
      // index[@level1="..."]... 

      // wh 
      // index[@name="hebrew",@level1="..."]... 

      // wg 
      // index[@name="greek",@level1="..."]... 

      // x note[@type="crossReference"] 

      // xo 

      // xt 
      // text within the note element, may serve to indicate the 
      // end of text of another format marker 

      // xk 
      // catchWord 

      // xq 
      // q 

      // xdc 
      // note[@type='crossReference']/seg[@edition="dc"] 

      else {
        transform_fallback();
      }
      
    } else {
      transform_fallback();
    }
  }

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


void Usfm2Osis::transform_fallback()
// This transformation is called when no other transformation could be done.
{
  // Bail out if there's no usfm code available.
  if (book_usfm_code.empty())
    return;

  // Get the text till the next marker.
  ustring marker;
  size_t marker_position;
  size_t marker_length;
  bool is_opener;
  bool marker_found = usfm_search_marker(book_usfm_code, marker, marker_position, marker_length, is_opener);
  ustring text;
  if (marker_found) {
    text = book_usfm_code.substr(0, marker_position);
    book_usfm_code.erase(0, marker_position);
    // Handle the case that the marker is at the start. 
    // Should never occur, but it put here for robustness.
    if (marker_position == 0) {
      text = book_usfm_code.substr(0, 1);
      book_usfm_code.erase(0, 1);
    }
  } else {
    text = book_usfm_code;
    book_usfm_code.clear();
  }

  // Write that text. 
  xmlTextWriterWriteFormatString(xmlwriter, "%s", text.c_str());
}


void Usfm2Osis::transform_usfm_description (const ustring& marker_text, size_t marker_length)
// According to the Osis manual, certain USFM codes translate into "description" elements in Osis.
{
  ustring text = get_erase_code_till_next_marker (book_usfm_code, 0, marker_length, true);
  ustring subtype = "x-" + marker_text;
  xmlTextWriterStartElement(xmlwriter, BAD_CAST "description");
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "type", "usfm");
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "subType", subtype.c_str());
  xmlTextWriterWriteFormatString(xmlwriter, "%s", text.c_str());
  xmlTextWriterEndElement(xmlwriter);
}


void Usfm2Osis::transform_h_title (size_t marker_length, bool runningheader, const gchar * placement)
// This transforms the \h into the Osis title element.
{
  ustring title = get_erase_code_till_next_marker (book_usfm_code, 0, marker_length, true);
  xmlTextWriterStartElement(xmlwriter, BAD_CAST "title");
  if (runningheader) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "type", "runningHead");
  }
  if (placement) {
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "placement", placement);
  }
  xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "short", title.c_str());
  xmlTextWriterEndElement(xmlwriter);
}


bool Usfm2Osis::usfm_is_osis_division (const ustring& marker) // Todo
// Returns true if the marker would result in an osis division.
{
  return false;
}


