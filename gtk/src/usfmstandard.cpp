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

#include "usfmstandard.h"
#include <libxml/xmlreader.h>
#include "directories.h"
#include "gwrappers.h"
#include "tiny_utilities.h"


USFMStandard::USFMStandard(int dummy)
{
  load();
}


USFMStandard::~USFMStandard()
{
}


void USFMStandard::load()
{
  // Bail out if the usfm standard is not there.
  ustring xmlfilename = gw_build_filename(directories_get_package_data(), "usfm.xml");
  if (!g_file_test(xmlfilename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    gw_critical ("Can't find USFM standard file");
    return;
  }
  
  // Get filecontents. 
  gchar *contents;
  g_file_get_contents(xmlfilename.c_str(), &contents, NULL, NULL);

  // Read the data.
  xmlParserInputBufferPtr inputbuffer;
  inputbuffer = xmlParserInputBufferCreateMem(contents, strlen(contents), XML_CHAR_ENCODING_NONE);
  xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
  if (reader) {
    char *opening_element = NULL;
    while ((xmlTextReaderRead(reader) == 1)) {
      
      switch (xmlTextReaderNodeType(reader)) {
      case XML_READER_TYPE_ELEMENT:
        {
          opening_element = (char *)xmlTextReaderName(reader);
          if (!strcmp(opening_element, "entry")) {
            
            // Optionally store previous one.
            store ();
            
            // Attribute value.
            char *attribute;
      
            // The marker.      
            marker.clear();
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "marker");
            if (attribute) {
              marker = attribute;
              free(attribute);
            }

            // Whether the marker starts with a backslash. Nearly always it does.
            // E.g.: startswithbackslash="no"
            startswithbackslash_v = true;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "startswithbackslash");
            if (attribute) {
              if (!strcmp(attribute, "no")) {
                startswithbackslash_v = false;
              }
              free(attribute);
            }

            // Whether there's an endmarker, e.g. such as in \add text\add*.
            // E.g. hasendmarker="optional"
            hasendmarker_v = uhemtNo;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "hasendmarker");
            if (attribute) {
              if (!strcmp(attribute, "yes")) {
                hasendmarker_v = uhemtYes;
              }
              if (!strcmp(attribute, "optional")) {
                hasendmarker_v = uhemtOptional;
              }
              free(attribute);
            }

            // Any variants upon the marker, e.g. \mt has variant \mt1, \mt2, etc.
            // E.g. variants="1234"
            variants.clear();
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "variants");
            if (attribute) {
              variants = attribute;
              free(attribute);
            }
            
            // Whether the marker starts a line in well-formed USFM code.
            startsline_v = true;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "startsline");
            if (attribute) {
              if (!strcmp(attribute, "no")) {
                startsline_v = false;
              }
              free(attribute);
            }
            
            // The function of the marker.
            function_v = uftComment;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "function");
            if (attribute) {
              if (!strcmp(attribute, "book_abbr")) {
                function_v = uftBookAbbr;
              }
              if (!strcmp(attribute, "break")) {
                function_v = uftBreak;
              }
              if (!strcmp(attribute, "cell")) {
                function_v = uftCell;
              }
              if (!strcmp(attribute, "chapter")) {
                function_v = uftChapter;
              }
              if (!strcmp(attribute, "chapter_label")) {
                function_v = uftChapterLabel;
              }
              if (!strcmp(attribute, "chapter_marker")) {
                function_v = uftChapterMarker;
              }
              if (!strcmp(attribute, "comment")) {
                function_v = uftComment;
              }
              if (!strcmp(attribute, "crossreference")) {
                function_v = uftCrossreference;
              }
              if (!strcmp(attribute, "encoding")) {
                function_v = uftEncoding;
              }
              if (!strcmp(attribute, "endnote")) {
                function_v = uftEndnote;
              }
              if (!strcmp(attribute, "figure")) {
                function_v = uftFigure;
              }
              if (!strcmp(attribute, "file")) {
                function_v = uftFile;
              }
              if (!strcmp(attribute, "footnote")) {
                function_v = uftFootnote;
              }
              if (!strcmp(attribute, "greek")) {
                function_v = uftGreek;
              }
              if (!strcmp(attribute, "hebrew")) {
                function_v = uftHebrew;
              }
              if (!strcmp(attribute, "index")) {
                function_v = uftIndex;
              }
              if (!strcmp(attribute, "keyword")) {
                function_v = uftKeyword;
              }
              if (!strcmp(attribute, "list")) {
                function_v = uftList;
              }
              if (!strcmp(attribute, "long_toc")) {
                function_v = uftLongToc;
              }
              if (!strcmp(attribute, "no_break")) {
                function_v = uftNoBreak;
              }
              if (!strcmp(attribute, "note_mark")) {
                function_v = uftNoteMark;
              }
              if (!strcmp(attribute, "page")) {
                function_v = uftPage;
              }
              if (!strcmp(attribute, "paragraph")) {
                function_v = uftParagraph;
              }
              if (!strcmp(attribute, "peripheral")) {
                function_v = uftPeripheral;
              }
              if (!strcmp(attribute, "pronunciation")) {
                function_v = uftPronunciation;
              }
              if (!strcmp(attribute, "row")) {
                function_v = uftRow;
              }
              if (!strcmp(attribute, "running_header")) {
                function_v = uftRunningHeader;
              }
              if (!strcmp(attribute, "section")) {
                function_v = uftSection;
              }
              if (!strcmp(attribute, "short_toc")) {
                function_v = uftShortToc;
              }
              if (!strcmp(attribute, "space")) {
                function_v = uftSpace;
              }
              if (!strcmp(attribute, "status")) {
                function_v = uftStatus;
              }
              if (!strcmp(attribute, "text")) {
                function_v = uftText;
              }
              if (!strcmp(attribute, "title")) {
                function_v = uftTitle;
              }
              if (!strcmp(attribute, "verse")) {
                function_v = uftVerse;
              }
              if (!strcmp(attribute, "verse_marker")) {
                function_v = uftVerseMarker;
              }
              if (!strcmp(attribute, "wordlist")) {
                function_v = uftWordlist;
              }
              free(attribute);
            }
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
          if (!strcmp(closing_element, "entry")) {
          }
          break;
        }
      }
    }
    // Optionally store last marker's data.
    store ();
  } 
  
  // Free memory.
  if (reader)
    xmlFreeTextReader(reader);
  if (inputbuffer)
    xmlFreeParserInputBuffer(inputbuffer);
  if (contents)
    g_free(contents);
}


void USFMStandard::store ()
// Stores parameters of one marker into the various maps.
{
  if (!marker.empty()) {
    // Marker itself.
    exists[marker] = true;
    startswithbackslash[marker] = startswithbackslash_v;
    hasendmarker[marker] = hasendmarker_v;
    startsline[marker] = startsline_v;
    function[marker] = function_v; 
    // The variants, if any.
    if (!variants.empty()) {
      for (unsigned int i = 0; i < variants.length(); i++) {
        ustring variant_marker = marker + variants.substr (i, 1);
        exists[variant_marker] = true;
        startswithbackslash[variant_marker] = startswithbackslash_v;
        hasendmarker[variant_marker] = hasendmarker_v;
        startsline[variant_marker] = startsline_v;
        function[variant_marker] = function_v; 
      }
    }
  }
  marker.clear();
  variants.clear();
}


bool USFMStandard::marker_exists (const ustring& marker)
// Returns true if the marker exists in the USFM standard.
{
  return exists [marker];
}


UsfmFunctionType USFMStandard::marker_function (const ustring& marker)
// Returns the function of the marker.
{
  return function [marker];
}

