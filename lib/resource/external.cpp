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


#include <resource/external.h>
#include <filter/string.h>
#include <filter/url.h>
#include <webserver/request.h>
#include "json/json.h"
#include "assets/view.h"


int gbs_digitaal_json_callback (void *userdata, int type, const char *data, uint32_t length)
{
  if (length) {};
  vector <string> * history = (vector <string> *) userdata;
  switch (type) {
    case JSON_OBJECT_BEGIN:
    case JSON_ARRAY_BEGIN:
    case JSON_OBJECT_END:
    case JSON_ARRAY_END:
      break;
    case JSON_KEY:
    case JSON_STRING:
    case JSON_INT:
    case JSON_FLOAT:
    {
      history->push_back (data);
      break;
    }
    case JSON_NULL:
    case JSON_TRUE:
    case JSON_FALSE:
      break;
  }
  return 0;
}


string gbsdigitaal (string url, int verse)
{
  string text;

  string json = filter_url_http_get (url, text);

  vector <string> history;
  
  json_parser parser;
  if (json_parser_init (&parser, NULL, gbs_digitaal_json_callback, &history)) {
    text.append ("Could not initialize");
  }
  if (json_parser_string (&parser, json.c_str(), json.length(), NULL)) {
    text.append ("Could not parse the data");
  }
  json_parser_free (&parser);

  // The history sequence to get a verse is this:
  // uid -> 0 -> number -> <verse number> -> text -> "verse text"
  for (unsigned int i = 0; i < history.size (); i++) {
    if (history[i] == "text") {
      if (i >= 5) {
        if (history [i - 4] == "uid") {
          if (history [i - 3] == "0") {
            if (history [i - 2] == "number") {
              if (history [i - 1] == to_string (verse)) {
                if (history.size () > i + 1) {
                  text = history [i + 1];
                }
              }
            }
          }
        }
      }
    }
  }

  // Take out breaks.
  text = filter_string_str_replace ("<br />", "", text);
  // Remove the note callers.
  filter_string_replace_between (text, "<sup", "</sup>", "");
  // Remove the pilcrow sign / paragraph sign if it's there.
  text = filter_string_str_replace ("¶", "", text);
  // Add new line.
  text += "\n";
  
  // Done.
  return text;
}



// This script fetches the Statenbijbel from the Dutch GBS.
string resource_external_get_statenbijbel_gbs (int book, int chapter, int verse)
{
  // Books OT: http://gbsdigitaal.nl/Data/Statenvertaling/1.json
  // Books NT: http://gbsdigitaal.nl/Data/Statenvertaling/2.json
  // Genesis intro http://gbsdigitaal.nl/Data/Statenvertaling/1/1/1.json
  // Genesis 1 http://gbsdigitaal.nl/Data/Statenvertaling/1/1/2.json
  // Matthew 1 http://gbsdigitaal.nl/Data/Statenvertaling/2/40/2.json
  
  // Old Testament = 1 and New Testament = 2.
  int testament = book >= 40 ? 2 : 1;
  
  // Chapter 1 of the text is chapter 2 in the URL.
  string urlchapter = to_string (chapter + 1);
  
  string url = "http://gbsdigitaal.nl/Data/Statenvertaling/" + to_string (testament) + "/" + to_string (book) + "/" + urlchapter + ".json";
  
  return gbsdigitaal (url, verse);
}


// This script displays the King James Bible published by the Dutch GBS.
string resource_external_get_king_james_version_gbs (int book, int chapter, int verse)
{
  // Books OT: http://gbsdigitaal.nl/Data/AuthorizedVersion/1.json
  // Books NT: http://gbsdigitaal.nl/Data/AuthorizedVersion/2.json
  // Genesis intro http://gbsdigitaal.nl/Data/AuthorizedVersion/1/1/1.json
  // Genesis 1 http://gbsdigitaal.nl/Data/AuthorizedVersion/1/1/2.json
  // Matthew 1 http://gbsdigitaal.nl/Data/AuthorizedVersion/2/40/2.json
  
  // Old (1) or New (2) Testament.
  int testament = book >= 40 ? 2 : 1;
  
  // Chapter 1 of the text is chapter 2 in the URL.
  string urlchapter = to_string (chapter + 1);
  
  string url = "http://gbsdigitaal.nl/Data/AuthorizedVersion/" + to_string (testament) + "/" + to_string (book) + "/" + urlchapter + ".json";
  
  return gbsdigitaal (url, verse);
}


// This displays the interlinear Bible from biblehub.com.
string resource_external_get_biblehub_interlinear (int book, int chapter, int verse)
{
  // Sample URL:
  // http://biblehub.com/interlinear/genesis/1-1.htm
 
  // This array maps the the book identifiers from Bibledit to the book names as used by the website.
  map <int, string> bookConverter = {
    make_pair (1, "genesis"),
    make_pair (2, "exodus"),
    make_pair (3, "leviticus"),
    make_pair (4, "numbers"),
    make_pair (5, "deuteronomy"),
    make_pair (6, "joshua"),
    make_pair (7, "judges"),
    make_pair (8, "ruth"),
    make_pair (9, "1_samuel"),
    make_pair (10, "2_samuel"),
    make_pair (11, "1_kings"),
    make_pair (12, "2_kings"),
    make_pair (13, "1_chronicles"),
    make_pair (14, "2_chronicles"),
    make_pair (15, "ezra"),
    make_pair (16, "nehemiah"),
    make_pair (17, "esther"),
    make_pair (18, "job"),
    make_pair (19, "psalms"),
    make_pair (20, "proverbs"),
    make_pair (21, "ecclesiastes"),
    make_pair (22, "songs"),
    make_pair (23, "isaiah"),
    make_pair (24, "jeremiah"),
    make_pair (25, "lamentations"),
    make_pair (26, "ezekiel"),
    make_pair (27, "daniel"),
    make_pair (28, "hosea"),
    make_pair (29, "joel"),
    make_pair (30, "amos"),
    make_pair (31, "obadiah"),
    make_pair (32, "jonah"),
    make_pair (33, "micah"),
    make_pair (34, "nahum"),
    make_pair (35, "habakkuk"),
    make_pair (36, "zephaniah"),
    make_pair (37, "haggai"),
    make_pair (38, "zechariah"),
    make_pair (39, "malachi"),
    make_pair (40, "matthew"),
    make_pair (41, "mark"),
    make_pair (42, "luke"),
    make_pair (43, "john"),
    make_pair (44, "acts"),
    make_pair (45, "romans"),
    make_pair (46, "1_corinthians"),
    make_pair (47, "2_corinthians"),
    make_pair (48, "galatians"),
    make_pair (49, "ephesians"),
    make_pair (50, "philippians"),
    make_pair (51, "colossians"),
    make_pair (52, "1_thessalonians"),
    make_pair (53, "2_thessalonians"),
    make_pair (54, "1_timothy"),
    make_pair (55, "2_timothy"),
    make_pair (56, "titus"),
    make_pair (57, "philemon"),
    make_pair (58, "hebrews"),
    make_pair (59, "james"),
    make_pair (60, "1_peter"),
    make_pair (61, "2_peter"),
    make_pair (62, "1_john"),
    make_pair (63, "2_john"),
    make_pair (64, "3_john"),
    make_pair (65, "jude"),
    make_pair (66, "revelation")
  };
  
  string bookname = bookConverter [book];
  
  string url = "http://biblehub.com/interlinear/" + bookname + "/" + to_string (chapter) + "-" + to_string (verse) + ".htm";
  
  // Get the html from the server, and tidy it up.
  string html;
  html = filter_url_http_get (url, html);
  string tidy = /* Todo Tidy_Html::run */ (html);
  vector <string> tidied = filter_string_explode (tidy, '\n');
  
  vector <string> filtered_lines;
  
  int relevant_line = 0;
  for (auto & line : tidied) {
    if (line.find ("<div") != string::npos) {
      relevant_line = 0;
    }
    if (relevant_line) {
      relevant_line++;
    }
    if (relevant_line > 3) {
      if (line.find ("iframe") == string::npos) {
        filtered_lines.push_back (line);
      }
    }
    if (line.find ("\"vheading\"") != string::npos) {
      relevant_line++;
    }
  }
  
  html = filter_string_implode (filtered_lines, "\n");
  
  html = filter_string_str_replace ("title=", Assets_View::target_conditional_blank () + " title=", html);
  
  html = filter_string_str_replace ("/abbrev.htm", "http://biblehub.com/abbrev.htm", html);
  html = filter_string_str_replace ("/hebrew/", "http://biblehub.com/hebrew/", html);
  html = filter_string_str_replace ("/hebrewparse.htm", "http://biblehub.com/hebrewparse.htm", html);
  html = filter_string_str_replace ("/greek/", "http://biblehub.com/greek/", html);
  html = filter_string_str_replace ("/grammar/", "http://biblehub.com/grammar/", html);
  
  html = filter_string_str_replace ("height=\"145\"", "", html);
  
  html = filter_string_str_replace ("&nbsp;&nbsp;", "&nbsp;", html);
  
  // Stylesheet for using web fonts,
  // because installing fonts on some tablets is very hard.
  string stylesheet =
  "<style>\n"
  ".tablefloat {\n"
  "  float: left;\n"
  "}\n"
  "@font-face {\n"
  "  font-family: \"Cardo\";\n"
  "src: url(/fonts/Cardo-Regular.ttf);\n"
  "}\n"
  ".greek {\n"
  "  font-family: Cardo;\n"
  "  font-size: large;\n"
  "}\n"
  ".tablefloatheb\n"
  "{\n"
  "  float : right;\n"
  "}\n"
  "@font-face {\n"
  "  font-family: \"Ezra SIL\";\n"
  "src: url(/fonts/SILEOT.ttf);\n"
  "}\n"
  ".hebrew {\n"
  "  font-family: \"Ezra SIL\";\n"
  "  font-size: x-large;\n"
  "}\n"
  "span[class*='ref'] {\n"
  "display: none;\n"
  "}\n"
  "</style>\n";
  string output = stylesheet;
  // The following line prevents the Hebrew and Greek from floating around
  // the name of the Resource, which would disturb the order of the words in Hebrew.
  output += "<p></p>\n";
  output += html;
  
  return output;
}


typedef struct
{
  const char *name;
  const char *versification;
  const char *mapping;
  string (* func) (int, int, int);
} resource_record;


resource_record resource_table [] =
{
  { "Statenbijbel GBS", "Dutch Traditional", "Dutch Traditional", & resource_external_get_statenbijbel_gbs },
  { "King James Version GBS", "English", "English", & resource_external_get_king_james_version_gbs },
  { "Biblehub Interlinear", "English", "English", & resource_external_get_biblehub_interlinear },
};


// The number of available external resource scripts.
unsigned int resource_external_count ()
{
  return sizeof (resource_table) / sizeof (*resource_table);
}


// Gets the names of the available external resources.
vector <string> resource_external_names ()
{
  vector <string> names;
  for (unsigned int i = 0; i < resource_external_count (); i++) {
    names.push_back (resource_table [i].name);
  }
  return names;
}


// Returns the versification for the resource.
string resource_external_versification (string name)
{
  string versification = "English";
  for (unsigned int i = 0; i < resource_external_count (); i++) {
    string resource = resource_table [i].name;
    if (name == resource) {
      versification = resource_table [i].versification;
    }
  }
  return versification;
}


// Returns the versification for the resource.
string resource_external_mapping (string name)
{
  string versification = "English";
  for (unsigned int i = 0; i < resource_external_count (); i++) {
    string resource = resource_table [i].name;
    if (name == resource) {
      versification = resource_table [i].mapping;
    }
  }
  return versification;
}


// Fetches the html data for the resource / book / chapter / verse.
string resource_external_get (string name, int book, int chapter, int verse)
{
  string (* func) (int, int, int) = NULL;

  for (unsigned int i = 0; i < resource_external_count (); i++) {
    string resource = resource_table [i].name;
    if (name == resource) {
      func = resource_table [i].func;
    }
  }
  
  if (func == NULL) {
    return "";
  }

  string result = func (book, chapter, verse);
  
  return result;
}
