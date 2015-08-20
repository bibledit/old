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
#include <database/morphhb.h>
#include <database/morphgnt.h>
#include <database/strong.h>
#include <libxml/xmlreader.h>


#define BETH_STRONG 10000
#define CONJUNCTION_STRONG 10001
#define DEFINITE_ARTICLE_STRONG 10002
#define INDEFINITE_ARTICLE_STRONG 10003
#define KAF_STRONG 10004
#define LAMED_STRONG 10005
#define MEM_STRONG 10006
#define SHIN_STRONG 10007


// Clean up the Strong's number.
string lexicon_logic_strong_number_cleanup (string strong)
{
  // Remove the leading zero from a Hebrew Strong's number.
  strong = filter_string_str_replace ("H0", "H", strong);
  
  return strong;
}


// Convert the $item to a Strong's number.
vector <string> lexicon_logic_convert_item_to_strong (string item)
{
  vector <string> strongs;
  
  // If the $item is a Strong's number, take that.
  if (!item.empty ()) {
    string s = item.substr (0, 1);
    if ((s == "H") || (s == "G")) strongs.push_back (item);
  }
  
  // No Strong's number found:
  // Assume that the $item contains a passage and an offset to a lemma within that passage.
  if (strongs.empty ()) {
    vector <string> bits = filter_string_explode (item, '_');
    if (bits.size () == 2) {
      
      Passage passage = Passage::from_text (bits[0]);
      int book = passage.book;
      int chapter = passage.chapter;
      int verse = convert_to_int (passage.verse);
      
      size_t offset = convert_to_int (bits[1]);

      Database_MorphHb database_morphhb;
      vector <Database_MorphHb_Item> morphhb_items = database_morphhb.get (book, chapter, verse);
      if (morphhb_items.size () > offset) {
        // The parsing for the word at the offset.
        string parsing = morphhb_items[offset].parsing;
        vector <string> bits = filter_string_explode (parsing, '/');
        for (auto & bit : bits) {
          int strong = convert_to_int (bit);
          if (strong == 0) {
            if (bit == "b") {
              // The Hebrew letter beth ב֖.
              strong = BETH_STRONG;
            }
            else if (bit == "c") {
              // Conjunction וְ.
              strong = CONJUNCTION_STRONG;
            }
            if (bit == "d") {
              // Definite article הַ.
              strong = DEFINITE_ARTICLE_STRONG;
            }
            else if (bit == "i") {
              // The indefinite article הַ.
              strong = INDEFINITE_ARTICLE_STRONG;
            }
            else if (bit == "k") {
              // The Hebrew letter kaf כַּ.
              strong = KAF_STRONG;
            }
            else if (bit == "l") {
              // The Hebrew letter lamed לָ.
              strong = LAMED_STRONG;
            }
            else if (bit == "m") {
              // The Hebrew letter mem מִ.
              strong = MEM_STRONG;
            }
            else if (bit == "s") {
              // The Hebrew letter shin שֶׁ.
              strong = SHIN_STRONG;
            }
          }
          if (strong) {
            strongs.push_back ("H" + convert_to_string (strong));
          }
        }
      }

      Database_MorphGnt database_morphgnt;
      vector <Database_MorphGnt_Item> morphgnt_items = database_morphgnt.get (book, chapter, verse);
      if (morphgnt_items.size () > offset) {
        // The lemma.
        string lemma = morphgnt_items[offset].lemma;
        // Convert the lemma to a Strong's number.
        Database_Strong database_strong;
        vector <string> results = database_strong.strong (lemma);
        strongs.insert (strongs.end (), results.begin(), results.end());
      }
    }
  }

  return strongs;
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
  string definition = database_strong.definition (lexicon_logic_strong_number_cleanup (strong));
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
              rendering.append ("Strong's " + strong.substr (1) + " ");
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
            rendering.append ("Strong's ");
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

  // If no rendering has been found yet, try the user-defined Strong's definitions.
  if (rendering.empty ()) {
    rendering = lexicon_logic_define_user_strong (strong);
  }
  
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


// Define user-defined Strong's numbers.
string lexicon_logic_define_user_strong (string strong)
{
  string definition;
  if (!strong.empty ()) {
    if (strong.substr (0, 1) == "H") {
      strong.erase (0, 1);
      int number = convert_to_int (strong);
      if (number == BETH_STRONG) {
        definition = "particle preposition ב: in, at, by, with, among";
      }
      else if (number == CONJUNCTION_STRONG) {
        definition = "particle conjunction ו: and, so, then, when, now, or, but, that";
      }
      else if (number == DEFINITE_ARTICLE_STRONG) {
        definition = "particle definite article ה: the";
      }
      else if (number == INDEFINITE_ARTICLE_STRONG) {
        definition = "particle indefinite article ה: a";
      }
      else if (number == KAF_STRONG) {
        definition = "particle preposition כ: like, as, at, according to, after, when, if";
      }
      else if (number == LAMED_STRONG) {
        definition = "particle preposition ל: to, for, towards, belonging to, in regard to, according to, in";
      }
      else if (number == MEM_STRONG) {
        definition = "particle preposition מ: from, out of, by, by reason of, at, because of, more than";
      }
      else if (number == SHIN_STRONG) {
        definition = "particle relative ש: that";
      }
    }
  }
  return definition;
}
