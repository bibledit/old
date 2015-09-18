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


#include <manage/write.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <database/books.h>
#include <locale/translate.h>


string manage_write_url ()
{
  return "manage/write";
}


bool manage_write_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::manager ());
}


string manage_write (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  string page;

  page = Assets_Page::header (translate ("Read/write"), webserver_request);

  Assets_View view = Assets_View ();

  string user = request->query["user"];
  view.set_variable ("user", user);
  
  string bible = request->query["bible"];
  view.set_variable ("bible", bible);

  int book = convert_to_int (request->query["book"]);

  bool access = request->database_users ()->hasAccess2Bible (user, bible);

  // Toggle write access to Bible.
  if (request->query.count ("toggle")) {
    if (access) {
      bool readonly = request->database_users ()->hasReadOnlyAccess2Book (user, bible, book);
      request->database_users ()->setReadOnlyAccess2Book (user, bible, book, !readonly);
    }
  }

  // Read or write access to display.
  vector <string> tbody;
  vector <int> books = request->database_bibles ()->getBooks (bible);
  
  for (size_t i = 0; i < books.size (); i++) {
    int book = books[i];
    int row = (i % 4);
    if (row == 0) tbody.push_back ("<tr>");
    if (row > 0) tbody.push_back ("<td> | </td>");
    tbody.push_back ("<td>");
    tbody.push_back (Database_Books::getEnglishFromId (book));
    tbody.push_back ("</td>");
    tbody.push_back ("<td>");
    tbody.push_back ("<a href=\"?user=" + user + "&bible=" + bible + "&book=" + convert_to_string (book) + "&toggle=\">");
    if (request->database_users ()->hasReadOnlyAccess2Book (user, bible, book)) {
      tbody.push_back (translate ("no"));
    } else {
      tbody.push_back (translate ("yes"));
    }
    tbody.push_back ("</a>");
    tbody.push_back ("</td>");
    if (row == 3) tbody.push_back ("</tr>");
  }
  if (books.empty ()) {
    tbody.push_back ("<tr>");
    tbody.push_back ("<td>");
    tbody.push_back (translate ("No books"));
    tbody.push_back ("</td>");
    tbody.push_back ("<td>");
  }
  
  view.set_variable ("tbody", filter_string_implode (tbody, "\n"));
  
  page += view.render ("manage", "write");

  page += Assets_Page::footer ();

  return page;
}
