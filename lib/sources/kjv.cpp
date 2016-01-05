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


#include <sources/kjv.h>
#include <database/logs.h>
#include <database/kjv.h>
#include <database/sqlite.h>
#include <filter/string.h>
#include <filter/url.h>
#include <pugixml/pugixml.hpp>


using namespace pugi;


void sources_kjv_store (int book, int chapter, int verse, string lemma, string english)
{
  Database_Kjv database_kjv;
  vector <string> lemmas = filter_string_explode (lemma, ' ');
  bool output_done = false;
  for (auto & strong : lemmas) {
    if (strong.find ("strong") == string::npos) continue;
    if (output_done) {
      // Two separate two identical words.
      database_kjv.store (book, chapter, verse, "", " / ");
    }
    strong = filter_string_str_replace ("strong:", "", strong);
    database_kjv.store (book, chapter, verse, strong, english);
    output_done = true;
  }
  if (lemmas.empty ()) {
    database_kjv.store (book, chapter, verse, "", english);
  }
}


// Parses the XML data from kjv.xml.
void sources_kjv_parse ()
{
  Database_Logs::log ("Parsing data from Crosswire's KJV XML file");
  Database_Kjv database_kjv;
  database_kjv.create ();

  int book = 0;
  int chapter = 0;
  int verse = 0;
  bool within_verse = false;
  string lemma;
  string english;

  xml_document document;
  document.load_file ("sources/kjv.xml", parse_ws_pcdata);
  for (xml_node osis : document.children ()) {
    for (xml_node osisText : osis.children ()) {
      for (xml_node divbook : osisText.children ()) {
        if (strcmp (divbook.name (), "div") == 0) {
          book++;
          Database_Logs::log ("Book " + convert_to_string (book));
          chapter = 0;
          verse = 0;
          for (xml_node chapter_element : divbook.children ()) {
            if (strcmp (chapter_element.name (), "chapter") == 0) {
              chapter++;
              verse = 0;
              within_verse = false;
              for (xml_node element : chapter_element.children ()) {
                string element_name = element.name ();
                if (element_name == "verse") {
                  string sID = element.attribute ("sID").value ();
                  if (!sID.empty ()) {
                    verse++;
                    within_verse = true;
                  }
                  string eID = element.attribute ("eID").value ();
                  if (!eID.empty ()) {
                    within_verse = false;
                  }
                } else if (element_name == "w") {
                  string lemma = element.attribute ("lemma").value ();
                  lemma = filter_string_trim (lemma);
                  xml_node textnode = element.first_child ();
                  string english = textnode.text ().get ();
                  if (within_verse) {
                    sources_kjv_store (book, chapter, verse, lemma, english);
                  }
                } else if (element_name.empty ()) {
                  if (within_verse) {
                    string english = element.value ();
                    sources_kjv_store (book, chapter, verse, "", english);
                  }
                } else if (element_name == "note") {
                  xml_node textnode = element.first_child ();
                  string english = textnode.text ().get ();
                  english.insert (0, " [");
                  english.append ("]");
                  sources_kjv_store (book, chapter, verse, "", english);
                } else if (element_name == "milestone") {
                } else if (element_name == "transChange") {
                  xml_node textnode = element.first_child ();
                  string english = textnode.text ().get ();
                  english.insert (0, "<span style=\"font-style:italic;\">");
                  english.append ("</span>");
                  sources_kjv_store (book, chapter, verse, "", english);
                } else {
                  if (within_verse) {
                    cout << "|" << element_name << "|"; // Todo
                    xml_node textnode = element.first_child ();
                    string english = textnode.text ().get ();
                    cout << english; // Todo
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  database_kjv.optimize ();
  Database_Logs::log ("Finished parsing data from the KJV XML file");
}
