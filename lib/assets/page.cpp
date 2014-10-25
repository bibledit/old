/*
Copyright (©) 2003-2014 Teus Benschop.

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


#include <cstdlib>
#include <iostream>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/url.h>
#include <config/globals.h>


using namespace std;


Assets_Page::Assets_Page (const char * file)
{
  if (file) {};
}


Assets_Page::~Assets_Page ()
{
}


string Assets_Page::header (string title, void * webserver_request, string searchQuery) // Todo writing.
{
  Assets_Header header = Assets_Header (title, webserver_request);
  header.setSearchQuery (searchQuery);
  string page = header.run ();
  return page;
}


string Assets_Page::success (string message)
{
  if (message.length()) {};
  /* C++Port
    $view = new Assets_View (__FILE__);
    $view->view->message = $message;
    $view->render ("success.php");
  */
  return "";
}


string Assets_Page::error (string message)
{
  if (message.length()) {};
  /* C++Port
    $view = new Assets_View (__FILE__);
    $view->view->message = $message;
    $view->render ("error.php");
  */
  return "";
}


string Assets_Page::message (string message)
{
  if (message.length()) {};
  /* C++Port
    $view = new Assets_View (__FILE__);
    $view->view->message = $message;
    $view->render ("message.php");
  */
  return "";
}


string Assets_Page::footer ()
{
  string page;
  Assets_View view = Assets_View (__FILE__);
  page += view.render ("footer.html");
  view = Assets_View (__FILE__);
  page += view.render ("xhtml_finish.html");
  return page;
}


