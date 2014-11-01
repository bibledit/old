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


#include <assets/view.h>
#include <cstdlib>
#include <iostream>
#include <filter/url.h>
#include <config/globals.h>
#include <database/logs.h>
#include <flate/flate2.h>


using namespace std;


Assets_View::Assets_View (string file)
{
  // Store name of the calling C++ file for selecting the default template file.
  caller = file;
}


Assets_View::~Assets_View ()
{
}


// Sets a variable (key and value) for the html template.
void Assets_View::set_variable (string key, string value)
{
  variables [key] = value;
}


// Enable displaying a zone in the html template.
void Assets_View::enable_zone (string zone)
{
  zones [zone] = true;
}


// Renders the "tpl" template through the flate template engine.
// If "tpl" is not given, it takes a default template based on the calling C++ file,
// e.g. when called by index/index.cpp, then the template file index/index.tpl is used.
// Setting the session variables in the template is postponed to the very last moment,
// since these could change during the course of the calling page.
string Assets_View::render (string tpl)
{
  // If variable tpl is not given, construct if from the calling path, 
  // and change the extension to "html".
  if (tpl.empty ()) {
    string extension = filter_url_get_extension (caller);
    tpl = caller.substr (0, caller.length () - extension.length ());
    tpl = filter_url_basename (tpl);
    tpl += "html";
  }
  
  // Variable tpl is a relative path. Make it a full one.
  tpl = filter_url_create_root_path (filter_url_dirname (caller), tpl);
  
  // The flate engine crashes if the template does not exist, so be sure it exists.  
  if (!filter_url_file_exists (tpl)) {
    Database_Logs::log ("Cannot find template file " + tpl);
    return "";
  }

  // Instantiate and fill the template engine. 
  Flate flate = Flate ();

  // Copy the variables and zones to the engine.
  map <string, string>::iterator iter1;
  for (iter1 = variables.begin (); iter1 != variables.end(); ++iter1) {
    flate.set_variable (iter1->first, iter1->second);
  }
  map <string, bool>::iterator iter2;
  for (iter2 = zones.begin (); iter2 != zones.end(); ++iter2) {
    flate.enable_zone (iter2->first);
  }

  /* C++Port
    $session_logic = Session_Logic::getInstance ();
    if ($session_logic->loggedIn ()) {
      $this->view->user = $session_logic->currentUser ();
      $this->view->level = $session_logic->currentLevel (true);
    }
  */

  // Get and return the page contents.
  string page = flate.render (tpl);
  return page;
}
