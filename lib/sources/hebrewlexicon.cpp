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


void sources_hebrewlexicon_parse () // Todo
{
  Database_Logs::log ("Start parsing Open Scriptures's Hebrew Lexicon");
  Database_HebrewLexicon database_hebrewlexicon;
  database_hebrewlexicon.create ();
  
  
  
  Database_Logs::log ("Finished parsing Open Scriptures's Hebrew Lexicon");
}

/*
int main (int argc, char **argv)
{
  unlink ("hebrewstrong.sqlite");
  sqlite3 *db;
  sqlite3_open ("hebrewstrong.sqlite", &db);
  sqlite3_exec (db, "PRAGMA synchronous = OFF;", NULL, NULL, NULL);
  sqlite3_exec (db, "CREATE TABLE IF NOT EXISTS hebrewstrong (id text, definition text);", NULL, NULL, NULL);
  
  xmlTextReaderPtr reader = xmlNewTextReaderFilename ("HebrewStrong.xml");

  string id;
  string definition;
  
  set <string> values;
  
  while ((xmlTextReaderRead(reader) == 1)) {
    int depth = xmlTextReaderDepth (reader);
    switch (xmlTextReaderNodeType (reader)) {
      case XML_READER_TYPE_ELEMENT:
      {
        string element = (char *) xmlTextReaderName (reader);
        if (element == "entry") {
          id = (char *) xmlTextReaderGetAttribute (reader, BAD_CAST "id");
          cout << id << endl;
          definition = (char *) xmlTextReaderReadInnerXml (reader);
        }
        if (element == "w") {
          xmlChar * pos = xmlTextReaderGetAttribute (reader, BAD_CAST "pos");
          if (pos) {
            string value = (char *) pos;
            values.insert (value);
          }
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

          definition = str_replace (xmlns, "", definition);
          definition = convert_xml_character_entities_to_characters (definition);
          definition = str_replace ("'", "''", definition);
          definition = trim (definition);
          
          string sql = "INSERT INTO hebrewstrong VALUES ('" + id + "', '" + definition + "');";
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

  for (auto value : values) cout << value << endl;
  
  return 0;
}
*/

