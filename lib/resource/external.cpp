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
string resource_external_get_statenbijbel_gbs (int book, int chapter, int verse) // Todo
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


string resource_external_get_king_james_version_gbs (int book, int chapter, int verse)
{
  // This script displays the King James Bible as published by the Dutch GBS.
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
string resource_external_mapping (string name) // Todo
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
