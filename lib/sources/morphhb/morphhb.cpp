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


#include "morphhb.h"
#include <libxml/xmlreader.h>
#include <sqlite3.h>


vector <string> explode (string value, char delimiter)
{
  vector <string> result;
  istringstream iss (value);
  for (string token; getline (iss, token, delimiter); )
  {
    result.push_back (move (token));
  }
  return result;
}


string file_get_contents (string filename)
{
  ifstream ifs (filename.c_str(), ios::in | ios::binary | ios::ate);
  streamoff filesize = ifs.tellg();
  ifs.seekg (0, ios::beg);
  vector <char> bytes ((int)filesize);
  ifs.read (&bytes[0], (int)filesize);
  return string (&bytes[0], (int)filesize);
}


int convert_to_int (string s)
{
  int i = atoi (s.c_str());
  return i;
}


string convert_to_string (string s)
{
  return s;
}


string convert_to_string (int i)
{
  ostringstream r;
  r << i;
  return r.str();
}


string str_replace (string search, string replace, string subject)
{
  size_t offposition = subject.find (search);
  while (offposition != string::npos) {
    subject.replace (offposition, search.length (), replace);
    offposition = subject.find (search, offposition + replace.length ());
  }
  return subject;
}


int main (int argc, char **argv)
{
  unlink ("morphhb.sqlite");
  sqlite3 *db;
  sqlite3_open ("morphhb.sqlite", &db);
  sqlite3_exec (db, "PRAGMA synchronous = OFF;", NULL, NULL, NULL);
  sqlite3_exec (db, "PRAGMA temp_store = MEMORY;", NULL, NULL, NULL);
  sqlite3_exec (db, "PRAGMA journal_mode = OFF;", NULL, NULL, NULL);
  sqlite3_exec (db, "CREATE TABLE morphhb (book integer, chapter integer, verse integer, word text, lemma text);", NULL, NULL, NULL);

  vector <string> books = {
    "Gen",
    "Exod",
    "Lev",
    "Num",
    "Deut",
    "Josh",
    "Judg",
    "Ruth",
    "1Sam",
    "2Sam",
    "1Kgs",
    "2Kgs",
    "1Chr",
    "2Chr",
    "Ezra",
    "Neh",
    "Esth",
    "Job",
    "Ps",
    "Prov",
    "Eccl",
    "Song",
    "Isa",
    "Jer",
    "Lam",
    "Ezek",
    "Dan",
    "Hos",
    "Joel",
    "Amos",
    "Obad",
    "Jonah",
    "Mic",
    "Nah",
    "Hab",
    "Zeph",
    "Hag",
    "Zech",
    "Mal"
  };
  
  for (unsigned int bk = 0; bk < books.size (); bk++) {
    
    string file = books[bk] + ".xml";
    cout << file << endl;
    string contents = file_get_contents (file);
    
    xmlTextReaderPtr reader = xmlNewTextReaderFilename (file.c_str());
    
    string book = convert_to_string (bk + 1);
    string chapter;
    string verse;
    string lemma;
    bool in_note = false;
    
    while ((xmlTextReaderRead(reader) == 1)) {
      switch (xmlTextReaderNodeType (reader)) {
        case XML_READER_TYPE_ELEMENT:
        {
          string element = (char *) xmlTextReaderName (reader);
          if (element == "verse") {
            string osisID = (char *) xmlTextReaderGetAttribute (reader, BAD_CAST "osisID");
            vector <string> bits = explode (osisID, '.');
            chapter = bits[1];
            verse = bits[2];
          }
          if (element == "w") {
            lemma = (char *) xmlTextReaderGetAttribute (reader, BAD_CAST "lemma");
          }
          if (element == "note") {
            in_note = true;
          }
          break;
        }
        case XML_READER_TYPE_TEXT:
        {
          if (chapter.empty ()) continue;
          if (verse.empty ()) continue;
          if (in_note) continue;
          string word = (char *) xmlTextReaderValue (reader);
          word = str_replace ("/", "", word);
          string sql = "INSERT INTO morphhb VALUES (" + book + ", " + chapter + ", " + verse + ", '" + word + "', '" + lemma + "');";
          char *error = NULL;
          int rc = sqlite3_exec (db, sql.c_str(), NULL, NULL, &error);
          if (rc != SQLITE_OK) {
            cout << sql << endl;
            cout << error << endl;
            return 0;
          }
          lemma.clear ();
          break;
        }
        case XML_READER_TYPE_END_ELEMENT:
        {
          string element = (char *) xmlTextReaderName (reader);
          if (element == "note") {
            in_note = false;
          }
          break;
        }
      }
    }
  }

  sqlite3_close (db);

  return 0;
}
