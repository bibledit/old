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


#include <notes/edit.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/url.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/notes.h>
#include <notes/logic.h>
#include <access/bible.h>
#include <ipc/focus.h>
#include <navigation/passage.h>
#include <notes/note.h>


string notes_edit_url ()
{
  return "notes/edit";
}


bool notes_edit_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::manager ());
}


string notes_edit (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Database_Notes database_notes = Database_Notes (webserver_request);
  Notes_Logic notes_logic = Notes_Logic (webserver_request);
  
  
  string page;
  Assets_Header header = Assets_Header (translate("Edit Note Source"), request);
  page += header.run ();
  Assets_View view = Assets_View ();
  
  
  int identifier;
  if (request->query.count ("identifier")) identifier = convert_to_int (request->query ["identifier"]);
  else identifier = convert_to_int (request->post ["identifier"]);
  if (identifier) view.set_variable ("identifier", convert_to_string (identifier));
  
  
  
  if (request->post.count ("data")) {
    // Save note.
    string noteData = request->post["data"];
    if (database_notes.identifierExists (identifier)) {
      database_notes.setContents (identifier, noteData);
      string url = filter_url_build_http_query (notes_note_url (), "id", convert_to_string (identifier));
      // View the updated note.
      redirect_browser (request, url);
      return "";
    }
  }
  
  
  if (identifier) {
    if (database_notes.identifierExists (identifier)) {
      string noteData = database_notes.getContents (identifier);
      view.set_variable ("data", noteData);
    }
  }
  
  
  page += view.render ("notes", "edit");
  page += Assets_Page::footer ();
  return page;
}
