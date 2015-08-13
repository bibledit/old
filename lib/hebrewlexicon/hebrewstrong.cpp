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


#include "hebrewstrong.h"
#include <libxml/xmlreader.h>
#include <sqlite3.h>


string str_replace (string search, string replace, string subject)
{
  size_t offposition = subject.find (search);
  while (offposition != string::npos) {
    subject.replace (offposition, search.length (), replace);
    offposition = subject.find (search, offposition + replace.length ());
  }
  return subject;
}


string convert_xml_character_entities_to_characters (string data)
{
  bool keep_going = true;
  int iterations = 0;
  size_t pos1 = -1;
  do {
    iterations++;
    pos1 = data.find ("&#x", pos1 + 1);
    if (pos1 == string::npos) {
      keep_going = false;
      continue;
    }
    size_t pos2 = data.find (";", pos1);
    if (pos2 == string::npos) {
      keep_going = false;
      continue;
    }
    string entity = data.substr (pos1 + 3, pos2 - pos1 - 3);
    data.erase (pos1, pos2 - pos1 + 1);
    int codepoint;
    stringstream ss;
    ss << hex << entity;
    ss >> codepoint;
    
    // The following is not available in GNU libstdc++.
    // wstring_convert <codecvt_utf8 <char32_t>, char32_t> conv1;
    // string u8str = conv1.to_bytes (codepoint);
    
    int cp = codepoint;
    // Adapted from: http://www.zedwood.com/article/cpp-utf8-char-to-codepoint.
    char c[5]={ 0x00,0x00,0x00,0x00,0x00 };
    if     (cp<=0x7F) { c[0] = cp;  }
    else if(cp<=0x7FF) { c[0] = (cp>>6)+192; c[1] = (cp&63)+128; }
    else if(0xd800<=cp && cp<=0xdfff) {} //invalid block of utf8
    else if(cp<=0xFFFF) { c[0] = (cp>>12)+224; c[1]= ((cp>>6)&63)+128; c[2]=(cp&63)+128; }
    else if(cp<=0x10FFFF) { c[0] = (cp>>18)+240; c[1] = ((cp>>12)&63)+128; c[2] = ((cp>>6)&63)+128; c[3]=(cp&63)+128; }
    string u8str = string (c);
    
    data.insert (pos1, u8str);
  } while (keep_going & (iterations < 100000));
  return data;
}


int main (int argc, char **argv)
{
  unlink ("hebrewstrong.sqlite");
  sqlite3 *db;
  sqlite3_open ("hebrewstrong.sqlite", &db);
  sqlite3_exec (db, "PRAGMA synchronous = OFF;", NULL, NULL, NULL);
  sqlite3_exec (db, "CREATE TABLE IF NOT EXISTS hebrewstrong (id text, word text, source text, meaning text, usage text);", NULL, NULL, NULL);
  
  xmlTextReaderPtr reader = xmlNewTextReaderFilename ("HebrewStrong.xml");

  string id;
  string word;
  string source;
  string meaning;
  string usage;
  
  while ((xmlTextReaderRead(reader) == 1)) {
    int depth = xmlTextReaderDepth (reader);
    switch (xmlTextReaderNodeType (reader)) {
      case XML_READER_TYPE_ELEMENT:
      {
        string element = (char *) xmlTextReaderName (reader);
        if (element == "entry") {
          id.clear ();
          word.clear ();
          source.clear ();
          meaning.clear ();
          usage.clear ();
          id = (char *) xmlTextReaderGetAttribute (reader, BAD_CAST "id");
          cout << id << endl;
        }
        if (element == "w") {
          if (depth == 2) {
            if (!word.empty ()) {
              cout << "Duplicate word" << endl;
              return 0;
            }
            word = (char *) xmlTextReaderReadOuterXml (reader);
          }
        }
        if (element == "source") {
          if (!source.empty ()) {
            cout << "Duplicate source" << endl;
            return 0;
          }
          source = (char *) xmlTextReaderReadOuterXml (reader);
        }
        if (element == "meaning") {
          if (!meaning.empty ()) {
            cout << "Duplicate meaning" << endl;
            return 0;
          }
          meaning = (char *) xmlTextReaderReadOuterXml (reader);
        }
        if (element == "usage") {
          if (!usage.empty ()) {
            cout << "Duplicate usage" << endl;
            return 0;
          }
          usage = (char *) xmlTextReaderReadOuterXml (reader);
        }
        break;
      }
      case XML_READER_TYPE_TEXT:
      {
        xmlChar *text = xmlTextReaderValue(reader);
        break;
      }
      case XML_READER_TYPE_END_ELEMENT:
      {
        string element = (char *) xmlTextReaderName(reader);
        if (element == "entry") {
          string xmlns = " xmlns=\"http://openscriptures.github.com/morphhb/namespace\"";

          word = str_replace (xmlns, "", word);
          word = convert_xml_character_entities_to_characters (word);
          word = str_replace ("'", "''", word);
          
          source = str_replace (xmlns, "", source);
          source  = str_replace ("'", "''", source);
          
          meaning = str_replace (xmlns, "", meaning);
          meaning = str_replace ("'", "''", meaning);
          
          usage = str_replace (xmlns, "", usage);
          usage = str_replace ("'", "''", usage);
          
          string sql = "INSERT INTO hebrewstrong VALUES ('" + id + "', '" + word + "', '" + source + "', '" + meaning + "', '" + usage + "');";
          char *error = NULL;
          int rc = sqlite3_exec (db, sql.c_str(), NULL, NULL, &error);
          if (rc != SQLITE_OK) {
            cout << sql << endl;
            cout << error << endl;
            return 0;
          }
        }
        break;
      }
    }
  }

  sqlite3_close (db);

  return 0;
}


