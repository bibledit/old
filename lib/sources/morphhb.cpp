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
#include <filter/string.h>
#include <pugixml/pugixml.hpp>


using namespace pugi;


void sources_morphhb_parse ()
{
  cout << "Starting" << endl;
  Database_MorphHb database_morphhb;
  database_morphhb.create ();

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
    
    if (books[bk] != "Ruth") continue; // Todo
    
    string file = "sources/morphhb/" + books[bk] + ".xml";
    cout << file << endl;

    int book = bk + 1;

    xml_document document;
    document.load_file (file.c_str());
    xml_node osis_node = document.first_child ();
    xml_node osisText_node = osis_node.child ("osisText");
    xml_node div_book_node = osisText_node.child ("div");
    for (xml_node chapter_node : div_book_node.children()) {
      for (xml_node verse_node : chapter_node.children ()) {
        string node_name = verse_node.name ();
        if (node_name != "verse") continue;

        // Get the passage.
        string osisID = verse_node.attribute ("osisID").value ();
        vector <string> bits = filter_string_explode (osisID, '.');
        int chapter = convert_to_int (bits[1]);
        int verse = convert_to_int (bits[2]);

        bool word_stored = false;
        
        // Most of the nodes will be "w" but there's more nodes as well, see the source XML file.
        for (xml_node node : verse_node.children ()) {

          if (word_stored) database_morphhb.store (book, chapter, verse, "", " ");

          string node_name = node.name ();

          if (node_name == "w") {
            string lemma = node.attribute ("lemma").value ();
            string word = node.child_value ();
            word = filter_string_str_replace ("/", "", word);
            database_morphhb.store (book, chapter, verse, lemma, word);
          }
          
          if (node_name == "seg") {
            string word = node.child_value ();
            database_morphhb.store (book, chapter, verse, "", word);            
          }
          
          word_stored = true;
        }
        
      }
      
      bool in_note = false;
      bool in_rdg = false;
      

      
    }

  /* To redo this with pugixml


   
    while ((xmlTextReaderRead(reader) == 1)) {
      switch (xmlTextReaderNodeType (reader)) {
        case XML_READER_TYPE_ELEMENT:
        {
          string element = (char *) xmlTextReaderName (reader);
          if (element == "note") {
            in_note = true;
          }
          if (element == "rdg") {
            in_rdg = true;
            database_morphhb.store (book, chapter, verse, "", "/");
          }
          break;
        }
        }
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
   */
    
  }

  database_morphhb.optimize ();
  cout << "Completed" << endl;
}
