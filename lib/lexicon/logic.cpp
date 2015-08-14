/*
 Copyright (©) 2003-2015 Teus Benschop.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include <lexicon/logic.h>
#include <filter/url.h>
#include <filter/string.h>
#include <database/strong.h>
#include <libxml/xmlreader.h>


// Clean up the Strong's number.
string lexicon_logic_strong_number_cleanup (string strong)
{
  // Remove the leading zero from a Hebrew Strong's number.
  strong = filter_string_str_replace ("H0", "H", strong);
  
  return strong;
}


// Gets the text to show when the mouse cursor hovers above a Strong's definition.
string lexicon_logic_strong_hover_text (string strong)
{
  Database_Strong database_strong;
  string definition = database_strong.get (strong);
  bool done = false;
  definition = lexicon_logic_wrap_xml (definition);
  xmlTextReaderPtr reader = xmlReaderForDoc (BAD_CAST definition.c_str(), "", "UTF-8", 0);
  while ((xmlTextReaderRead(reader) == 1)) {
    if (done) continue;
    switch (xmlTextReaderNodeType (reader)) {
      case XML_READER_TYPE_ELEMENT:
      {
        xmlChar * name = xmlTextReaderName (reader);
        if (name) {
          string element = (char *) name;
          xmlFree (name);
          if ((element == "meaning") || (element == "strongs_def")) {
            xmlChar * xml = xmlTextReaderReadInnerXml (reader);
            if (xml) {
              definition = (char *) xml;
              xmlFree (xml);
            }
            done = true;
          }
        }
        break;
      }
    }
  }
  xmlFreeTextReader (reader);
  filter_string_replace_between (definition, "<", ">", "");
  definition = filter_string_str_replace ("\"", "", definition);
  definition = filter_string_trim (definition);
  return definition;
}


string lexicon_logic_wrap_xml (string xml)
{
  xml.insert (0, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><bibledit>");
  xml.append ("</bibledit>");
  return xml;
}