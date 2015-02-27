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
#include <webserver/request.h>


string resource_external_get_one (int book, int chapter, int verse)
{
  return "External Resource One " + to_string (book) + " " + to_string (chapter) + " " + to_string (verse);
}


string resource_external_get_two (int book, int chapter, int verse)
{
  return "External Resource Two " + to_string (book) + " " + to_string (chapter) + " " + to_string (verse);
}


typedef struct
{
  const char *name;
  string (* func) (int, int, int);
} resource_record;


resource_record resource_table [] =
{
  { "External Resource One", & resource_external_get_one },
  { "External Resource Two", & resource_external_get_two },
};


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


