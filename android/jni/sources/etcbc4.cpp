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


#include <sources/etcbc4.h>
#include <database/logs.h>
#include <database/etcbc4.h>
#include <filter/string.h>
#include <filter/url.h>
#include <libxml/xmlreader.h>


void sources_etcb4_download ()
{
  Database_Logs::log ("Start to download the raw Hebrew morphology data from the ETCBC4 database");
  Database_Etcbc4 database_etcbc4;
  database_etcbc4.create ();
  
  // The book names for downloading data.
  vector <string> books = {
    "Genesis",
    "Exodus",
    "Leviticus",
    "Numeri",
    "Deuteronomium",
    "Josua",
    "Judices",
    "Ruth",
    "Samuel_I",
    "Samuel_II",
    "Reges_I",
    "Reges_II",
    "Chronica_I",
    "Chronica_II",
    "Esra",
    "Nehemia",
    "Esther",
    "Iob",
    "Psalmi",
    "Proverbia",
    "Ecclesiastes",
    "Canticum",
    "Jesaia",
    "Jeremia",
    "Threni",
    "Ezechiel",
    "Daniel",
    "Hosea",
    "Joel",
    "Amos",
    "Obadia",
    "Jona",
    "Micha",
    "Nahum",
    "Habakuk",
    "Zephania",
    "Haggai",
    "Sacharia",
    "Maleachi"
  };

  for (unsigned int bk = 0; bk < books.size (); bk++) {

    int book = bk + 1;
    string bookname = books[bk];

    bool book_done = false;
    for (int chapter = 1; chapter <= 150; chapter++) {
      if (book_done) continue;

      for (int verse = 1; verse < 200; verse++) {
        if (book_done) continue;

        string data = database_etcbc4.raw (book, chapter, verse);
        if (!data.empty ()) continue;
        
        string url = "https://shebanq.ancient-data.org/hebrew/verse?version=4b&book=" + bookname + "&chapter=" + convert_to_string (chapter) + "&verse=" + convert_to_string (verse);

        string error;
        string response = filter_url_http_get (url, error);
        if (!error.empty ()) {
          Database_Logs::log (error);
          continue;
        }
        if (response.find ("does not exist") != string::npos) {
          if (verse == 1) book_done = true;
          break;
        }
        Database_Logs::log (bookname + " " + convert_to_string (chapter) + "." + convert_to_string (verse));
        database_etcbc4.store (book, chapter, verse, response);
        // Wait a second so as not to overload the website.
        this_thread::sleep_for (chrono::seconds (1));
      }
    }
  }

  Database_Logs::log ("Finished downloading from the ETCBC4 database");
}


string sources_etcb4_clean (string item)
{
  item = filter_string_str_replace ("/", "", item);
  item = filter_string_str_replace ("]", "", item);
  item = filter_string_str_replace ("[", "", item);
  item = filter_string_str_replace ("=", "", item);
  item = filter_string_trim (item);
  return item;
}


// Parses the raw html data as downloaded from the ETCBC4 database.
// The parser is supposed to ran only by the developers.
// No memory is freed: It leaks a lot of memory.
void sources_etcb4_parse ()
{
  Database_Logs::log ("Parsing data from the ETCBC4 database");
  Database_Etcbc4 database_etcbc4;
  database_etcbc4.create ();
  string readit = "readit";
  vector <int> books = database_etcbc4.books ();
  for (auto book : books) {
    vector <int> chapters = database_etcbc4.chapters (book);
    for (auto chapter : chapters) {
      Database_Logs::log ("Parsing book " + convert_to_string (book) + " chapter " + convert_to_string (chapter));
      vector <int> verses = database_etcbc4.verses (book, chapter);
      for (auto verse : verses) {
        string data = database_etcbc4.raw (book, chapter, verse);
        if (data.empty ()) continue;
        data = filter_string_str_replace (non_breaking_space (), "", data);
        data.insert (0, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><bibledit>");
        data.append ("</bibledit>");
        string word;
        string vocalized_lexeme;
        string consonantal_lexeme;
        string gloss;
        string pos;
        string subpos;
        string gender;
        string number;
        string person;
        string state;
        string tense;
        string stem;
        string phrase_function;
        string phrase_type;
        string phrase_relation;
        string phrase_a_relation;
        string clause_text_type;
        string clause_type;
        string clause_relation;
        xmlTextReaderPtr reader = xmlReaderForMemory (data.c_str(), data.length (), "", NULL, 0);
        while ((xmlTextReaderRead(reader) == 1)) {
          switch (xmlTextReaderNodeType (reader)) {
            case XML_READER_TYPE_ELEMENT:
            {
              string element = (char *) xmlTextReaderName (reader);
              if (element == "span") {
                string clazz = (char *) xmlTextReaderGetAttribute (reader, BAD_CAST "class");
                if (clazz == "ht") word = readit;
                if (clazz.find ("hl_hlv") != string::npos) vocalized_lexeme = readit;
                if (clazz.find ("hl_hlc") != string::npos) consonantal_lexeme = readit;
                if (clazz == "gl") gloss = readit;
                if (clazz.find ("_pos") != string::npos) pos = readit;
                if (clazz.find ("_subpos") != string::npos) subpos = readit;
                if (clazz.find ("_gender") != string::npos) gender = readit;
                if (clazz.find ("_gnumber") != string::npos) number = readit;
                if (clazz.find ("_person") != string::npos) person = readit;
                if (clazz.find ("_state") != string::npos) state = readit;
                if (clazz.find ("_tense") != string::npos) tense = readit;
                if (clazz.find ("_stem") != string::npos) stem = readit;
                if (clazz.find ("ph_fun") != string::npos) phrase_function = readit;
                if (clazz.find ("ph_typ") != string::npos) phrase_type = readit;
                if (clazz.find ("ph_rela") != string::npos) phrase_relation = readit;
                if (clazz.find ("ph_arela") != string::npos) phrase_a_relation = readit;
                if (clazz.find ("cl_txt") != string::npos) clause_text_type = readit;
                if (clazz.find ("cl_typ") != string::npos) clause_type = readit;
                if (clazz.find ("cl_rela") != string::npos) clause_relation = readit;
              }
              break;
            }
            case XML_READER_TYPE_TEXT:
            {
              string value = (char *) xmlTextReaderValue (reader);
              value = sources_etcb4_clean (value);
              if (word == readit) word = value;
              if (vocalized_lexeme == readit) vocalized_lexeme = value;
              if (consonantal_lexeme == readit) consonantal_lexeme = value;
              if (gloss == readit) gloss = value;
              if (pos == readit) pos = value;
              if (subpos == readit) subpos = value;
              if (gender == readit) gender = value;
              if (number == readit) number = value;
              if (person == readit) person = value;
              if (state == readit) state = value;
              if (tense == readit) tense = value;
              if (stem == readit) stem = value;
              if (phrase_function == readit) phrase_function = value;
              if (phrase_type == readit) phrase_type = value;
              if (phrase_relation == readit) phrase_relation = value;
              if (phrase_a_relation == readit) phrase_a_relation = value;
              if (clause_text_type == readit) clause_text_type = value;
              if (clause_type == readit) clause_type = value;
              if (clause_relation == readit) clause_relation = value;
              break;
            }
            case XML_READER_TYPE_END_ELEMENT:
            {
              string element = (char *) xmlTextReaderName (reader);
              if (element == "table") {
                database_etcbc4.store (book, chapter, verse,
                                      word, vocalized_lexeme, consonantal_lexeme, gloss, pos, subpos,
                                      gender, number, person,
                                      state, tense, stem,
                                      phrase_function, phrase_type, phrase_relation,
                                      phrase_a_relation, clause_text_type, clause_type, clause_relation);
                word.clear ();
                vocalized_lexeme.clear ();
                consonantal_lexeme.clear ();
                gloss.clear ();
                pos.clear ();
                subpos.clear ();
                gender.clear ();
                number.clear ();
                person.clear ();
                state.clear ();
                tense.clear ();
                stem.clear ();
                phrase_function.clear ();
                phrase_type.clear ();
                phrase_relation.clear ();
                phrase_a_relation.clear ();
                clause_text_type.clear ();
                clause_type.clear ();
                clause_relation.clear ();
              }
              break;
            }
          }
        }
      }
    }
  }
  
  Database_Logs::log ("Finished parsing data from the ETCBC4 database");
}
