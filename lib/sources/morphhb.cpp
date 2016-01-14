/*
 Copyright (©) 2003-2016 Teus Benschop.
 
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


#include <sources/morphhb.h>
#include <database/morphhb.h>
#include <database/logs.h>
#include <filter/string.h>


void sources_morphhb_parse ()
{
  Database_Logs::log ("Start parsing Open Scriptures's Hebrew");
  Database_MorphHb database_morphhb;
  database_morphhb.create ();

  /* To redo this with pugixml
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
  
  for (size_t bk = 0; bk < books.size (); bk++) {
    
    string file = "sources/morphhb/" + books[bk] + ".xml";
    Database_Logs::log (file);
    
    xmlTextReaderPtr reader = xmlNewTextReaderFilename (file.c_str());
    
    int book = bk + 1;
    int chapter;
    int verse;
    string lemma;
    bool in_note = false;
    bool in_rdg = false;
    
    while ((xmlTextReaderRead(reader) == 1)) {
      switch (xmlTextReaderNodeType (reader)) {
        case XML_READER_TYPE_ELEMENT:
        {
          string element = (char *) xmlTextReaderName (reader);
          if (element == "verse") {
            string osisID = (char *) xmlTextReaderGetAttribute (reader, BAD_CAST "osisID");
            vector <string> bits = filter_string_explode (osisID, '.');
            chapter = convert_to_int (bits[1]);
            verse = convert_to_int (bits[2]);
          }
          if (element == "w") {
            lemma = (char *) xmlTextReaderGetAttribute (reader, BAD_CAST "lemma");
          }
          if (element == "note") {
            in_note = true;
          }
          if (element == "rdg") {
            in_rdg = true;
            database_morphhb.store (book, chapter, verse, "", "/");
          }
          break;
        }
        case XML_READER_TYPE_TEXT:
        {
          if (chapter == 0) continue;
          if (verse == 0) continue;
          if (!in_rdg) if (in_note) continue;
          string word = (char *) xmlTextReaderValue (reader);
          word = filter_string_str_replace ("/", "", word);
          database_morphhb.store (book, chapter, verse, lemma, word);
          lemma.clear ();
          word.clear ();
          break;
        }
        case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
        {
          if (chapter == 0) continue;
          if (verse == 0) continue;
          if (!in_rdg) if (in_note) continue;
          database_morphhb.store (book, chapter, verse, "", " ");
        }
        case XML_READER_TYPE_END_ELEMENT:
        {
          string element = (char *) xmlTextReaderName (reader);
          if (element == "note") {
            in_note = false;
          }
          if (element == "rdg") {
            in_rdg = false;
          }
          break;
        }
      }
    }
  }
   */

  database_morphhb.optimize ();
  Database_Logs::log ("Finished parsing Open Scriptures's Hebrew");
}
