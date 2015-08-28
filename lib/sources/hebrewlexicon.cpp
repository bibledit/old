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


#include <sources/hebrewlexicon.h>
#include <libxml/xmlreader.h>
#include <database/logs.h>
#include <database/hebrewlexicon.h>
#include <filter/string.h>


void sources_hebrewlexicon_parse () // Todo
{
  Database_Logs::log ("Start parsing Open Scriptures's Hebrew Lexicon");
  Database_HebrewLexicon database_hebrewlexicon;
  database_hebrewlexicon.create ();

  {
    Database_Logs::log ("HebrewStrong.xml");
    xmlTextReaderPtr reader = xmlNewTextReaderFilename ("sources/hebrewlexicon/HebrewStrong.xml");
    
    string id;
    string definition;
    
    while ((xmlTextReaderRead(reader) == 1)) {
      switch (xmlTextReaderNodeType (reader)) {
        case XML_READER_TYPE_ELEMENT:
        {
          string element = (char *) xmlTextReaderName (reader);
          if (element == "entry") {
            id = (char *) xmlTextReaderGetAttribute (reader, BAD_CAST "id");
            definition = (char *) xmlTextReaderReadInnerXml (reader);
          }
          break;
        }
        case XML_READER_TYPE_TEXT:
        {
          //xmlChar *text = xmlTextReaderValue(reader);
          break;
        }
        case XML_READER_TYPE_END_ELEMENT:
        {
          string element = (char *) xmlTextReaderName(reader);
          if (element == "entry") {
            string xmlns = " xmlns=\"http://openscriptures.github.com/morphhb/namespace\"";
            definition = filter_string_str_replace (xmlns, "", definition);
            definition = convert_xml_character_entities_to_characters (definition);
            definition = filter_string_str_replace ("'", "''", definition);
            definition = filter_string_trim (definition);
            database_hebrewlexicon.setstrong (id, definition);
          }
          break;
        }
      }
    }
  }
  
  Database_Logs::log ("Finished parsing Open Scriptures's Hebrew Lexicon");
}

