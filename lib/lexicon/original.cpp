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


#include <lexicon/original.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/url.h>
#include <webserver/request.h>
#include <database/morphhb.h>
#include <database/morphgnt.h>
#include <lexicon/logic.h>


string lexicon_original_url ()
{
  return "lexicon/original";
}


bool lexicon_original_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string lexicon_original (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  int book = convert_to_int (request->query["book"]);
  int chapter = convert_to_int (request->query["chapter"]);
  int verse = convert_to_int (request->query["verse"]);
  Passage passage ("", book, chapter, convert_to_string (verse));

  string page;
  string cls;

  Database_MorphHb database_morphhb;
  Database_MorphGnt database_morphgnt;

  // If the $book refers to Hebrew, take the data from there.
  vector <Database_MorphHb_Item> morphhb_items = database_morphhb.get (book, chapter, verse);
  for (size_t i = 0; i < morphhb_items.size (); i++) {
    if (!page.empty ()) page.append (" ");
    page.append ("<a href=\"" + passage.to_text ().substr (1) + "_" + convert_to_string (i) + "\">" + morphhb_items[i].word + "</a>");
  }
  if (!morphhb_items.empty ()) cls = "hebrew";
  
  // If the $book refers to Greek, take the data from there.
  vector <Database_MorphGnt_Item> morphgnt_items = database_morphgnt.get (book, chapter, verse);
  for (size_t i = 0; i < morphgnt_items.size (); i++) {
    if (!page.empty ()) page.append (" ");
    page.append ("<a href=\"" + passage.to_text ().substr (1) + "_" + convert_to_string (i) + "\">" + morphgnt_items[i].word + "</a>");
  }
  if (!morphgnt_items.empty ()) cls = "greek";
  
  // Set correct class, so it uses the correct font.
  page.insert (0, "<div class=\"" + cls + "\">");
  page.append ("</div>");
  
  return page;
}
