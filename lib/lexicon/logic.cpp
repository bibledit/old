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
  string hover_text;
  Database_Strong database_strong;
  string definition = database_strong.get (strong);
  definition = lexicon_logic_create_xml_document (definition);
  xmlTextReaderPtr reader = xmlReaderForDoc (BAD_CAST definition.c_str(), "", "UTF-8", 0);
  while ((xmlTextReaderRead(reader) == 1)) {
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
              hover_text.append ((char *) xml);
              xmlFree (xml);
            }
          }
        }
        break;
      }
    }
  }
  xmlFreeTextReader (reader);
  if (hover_text.empty ()) hover_text = definition;
  filter_string_replace_between (hover_text, "<", ">", "");
  hover_text = filter_string_str_replace ("\"", "", hover_text);
  hover_text = filter_string_trim (hover_text);
  return hover_text;
}


string lexicon_logic_create_xml_document (string xml)
{
  xml.insert (0, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><bibledit>");
  xml.append ("</bibledit>");
  return xml;
}


string lexicon_logic_render_definition (string strong)
{
  string rendering;
  bool a_opened = false;
  Database_Strong database_strong;
  string definition = database_strong.get (lexicon_logic_strong_number_cleanup (strong));
  definition = lexicon_logic_create_xml_document (definition);
  xmlTextReaderPtr reader = xmlReaderForDoc (BAD_CAST definition.c_str(), "", "UTF-8", 0);
  while ((xmlTextReaderRead(reader) == 1)) {
    int depth = xmlTextReaderDepth (reader);
    switch (xmlTextReaderNodeType (reader)) {
      case XML_READER_TYPE_ELEMENT:
      {
        xmlChar * name = xmlTextReaderName (reader);
        if (name) {
          string element = (char *) name;
          xmlFree (name);
          if (element == "w") {
            if (depth == 1) {
              rendering.append ("<br>");
              rendering.append ("Word: ");
            }
            xmlChar * pos = xmlTextReaderGetAttribute (reader, BAD_CAST "pos");
            if (pos) {
              // Part of speech
              string s = lexicon_logic_render_part_of_speech ((char *) pos);
              rendering.append (s);
              xmlFree (pos);
            }
            xmlChar * pron = xmlTextReaderGetAttribute (reader, BAD_CAST "pron");
            if (pron) {
              // Pronunciation.
              rendering.append (" ");
              rendering.append ((char *) pron);
              rendering.append (" ");
              xmlFree (pron);
            }
            xmlChar * xlit = xmlTextReaderGetAttribute (reader, BAD_CAST "xlit");
            if (xlit) {
              // Transliteration.
              rendering.append (" ");
              rendering.append ((char *) xlit);
              rendering.append (" ");
              xmlFree (xlit);
            }
            xmlChar * src = xmlTextReaderGetAttribute (reader, BAD_CAST "src");
            if (src) {
              // Transliteration.
              rendering.append ("<a href=\"");
              rendering.append ((char *) src);
              rendering.append ("\">");
              a_opened = true;
              xmlFree (src);
            }
          }
          if ((element == "source") || (element == "strongs_derivation")) {
            rendering.append ("<br>");
            rendering.append ("Source: ");
          }
          if ((element == "meaning") || (element == "strongs_def")) {
            rendering.append ("<br>");
            rendering.append ("Meaning: ");
          }
          if ((element == "usage") || (element == "kjv_def")) {
            rendering.append ("<br>");
            rendering.append ("Usage in King James Bible: ");
          }
          if (element == "def") {
            rendering.append ("<em>");
          }
          if (element == "strongs") {
            rendering.append ("<br>");
            rendering.append ("Word: ");
          }
          if (element == "greek") {
            xmlChar * unicode = xmlTextReaderGetAttribute (reader, BAD_CAST "unicode");
            if (unicode) {
              // Greek in Unicode.
              rendering.append (" ");
              rendering.append ((char *) unicode);
              rendering.append (" ");
              xmlFree (unicode);
            }
            xmlChar * translit = xmlTextReaderGetAttribute (reader, BAD_CAST "translit");
            if (translit) {
              // Greek in translit.
              rendering.append (" ");
              rendering.append ((char *) translit);
              rendering.append (" ");
              xmlFree (translit);
            }
          }
          if (element == "pronunciation") {
            xmlChar * strongs = xmlTextReaderGetAttribute (reader, BAD_CAST "strongs");
            if (strongs) {
              // Greek in strongs.
              rendering.append (" ");
              rendering.append ((char *) strongs);
              rendering.append (" ");
              xmlFree (strongs);
            }
          }
          if (element == "see") {
            // The information inside this element is superfluous,
            // because it is already elsewhere in the defintion.
            // Therefore it can be skipped.
          }
          if (element == "strongsref") {
            string prefix;
            xmlChar * language = xmlTextReaderGetAttribute (reader, BAD_CAST "language");
            if (language) {
              prefix = (char *) language;
              prefix = prefix.substr (0, 1);
              xmlFree (language);
            }
            xmlChar * strongs = xmlTextReaderGetAttribute (reader, BAD_CAST "strongs");
            if (strongs) {
              rendering.append ("<a href=\"");
              rendering.append (prefix);
              rendering.append ((char *) strongs);
              rendering.append ("\">");
              rendering.append ((char *) strongs);
              rendering.append ("</a>");
              xmlFree (strongs);
            }
          }
        }
        break;
      }
      case XML_READER_TYPE_TEXT:
      {
        xmlChar *value = xmlTextReaderValue(reader);
        if (value) {
          rendering.append ((char *) value);
          xmlFree (value);
        }
        break;
      }
      case XML_READER_TYPE_END_ELEMENT:
      {
        xmlChar * name = xmlTextReaderName (reader);
        if (name) {
          string element = (char *) name;
          xmlFree (name);
          if (a_opened) {
            rendering.append ("</a>");
            a_opened = false;
          }
          if (element == "def") {
            rendering.append ("</em>");
          }
        }
        break;
      }
    }
  }
  xmlFreeTextReader (reader);
  rendering = filter_string_trim (rendering);
  return rendering;
}


string lexicon_logic_render_part_of_speech_pop_front (vector <string> & parts)
{
  string part;
  if (!parts.empty ()) {
    part = parts[0];
    parts.erase (parts.begin ());
  }
  return part;
}


// Render the part of speech.
string lexicon_logic_render_part_of_speech (string value)
{
  if (value == unicode_string_casefold (value)) {
    // Deal with Strong's parsings.
    vector <string> parts = filter_string_explode (value, ' ');
    value.clear ();
    for (auto part : parts) {
      value.append (" ");
      if (part == "a") {
        value.append ("adjective");
      } else if (part == "a-f") {
        value.append ("adjective feminine");
      } else if (part == "a-m") {
        value.append ("adjective masculine");
      } else if (part == "adv") {
        value.append ("adverb");
      } else if (part == "conj") {
        value.append ("conjunction");
      } else if (part == "d") {
        value.append ("demonstrative");
      } else if (part == "dp") {
        value.append ("demonstrative particle");
      } else if (part == "i") {
        value.append ("interrogative");
      } else if (part == "inj") {
        value.append ("interjection");
      } else if (part == "n") {
        value.append ("noun");
      } else if (part == "n-f") {
        value.append ("noun feminine");
      } else if (part == "n-m") {
        value.append ("noun masculine");
      } else if (part == "n-pr-m") {
        value.append ("noun proper masculine");
      } else if (part == "n-pr") {
        value.append ("noun proper");
      } else if (part == "n-pr-f") {
        value.append ("noun proper feminine");
      } else if (part == "n-pr-loc") {
        value.append ("noun proper location");
      } else if (part == "a-gent") {
        value.append ("adjective gent");
      } else if (part == "np") {
        value.append ("negative particle");
      } else if (part == "p") {
        value.append ("pronoun");
      } else if (part == "prep") {
        value.append ("preposition");
      } else if (part == "pron") {
        value.append ("pron");
      } else if (part == "prt") {
        value.append ("particle");
      } else if (part == "r") {
        value.append ("relative");
      } else if (part == "v") {
        value.append ("verb");
      } else if (part == "x") {
        value.append ("unparsed");
      } else {
        value.append (part);
      }
    }
    
  } else {
    // Deal with the BDB parsings.
    vector <string> parts = filter_string_explode (value, '-');
    value.clear ();
    string word = lexicon_logic_render_part_of_speech_pop_front (parts);
    value.append (" ");
    // BDB.
    if (word == "A") {
      value.append ("adjective");
    } else if (word == "C") {
      value.append ("conjunction");
    } else if (word == "D") {
      value.append ("adverb");
    } else if (word == "N") {
      value.append ("noun");
    } else if (word == "P") {
      value.append ("pronoun");
    } else if (word == "R") {
      value.append ("preposition");
    } else if (word == "S") {
      value.append ("suffix");
    } else if (word == "T") {
      value.append ("particle");
    } else if (word == "V") {
      value.append ("verb");
    }
  }

  value.append (" ");
  
  return value;
}


string lexicon_logic_render_part_of_speech_stem (string abbrev)
{
  return abbrev;
}


string lexicon_logic_render_part_of_speech_person (string abbrev)
{
  return abbrev;
}


string lexicon_logic_render_part_of_speech_gender (string abbrev)
{
  return abbrev;
}


string lexicon_logic_render_part_of_speech_number (string abbrev)
{
  return abbrev;
}


string lexicon_logic_render_part_of_speech_state (string abbrev)
{
  return abbrev;
}
