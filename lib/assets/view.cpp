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


using namespace std;


Assets_View::Assets_View (string file)
{
  // Store name of the calling C++ file for selecting the default template file.
  caller = file;
  /* C++Port
    // Modifier for the links in the headers.
    $bibledit_root_folder = Bootstrap::getInstance ()->bibledit_root_folder;
    $file_path = dirname ($calling_file_php);
    $header_path_modifier = "";
    $iterations_count = 0;
    while (($file_path != $bibledit_root_folder) && ($iterations_count < 10)) {
      $file_path = dirname ($file_path);
      $iterations_count++;
      $header_path_modifier .= "../";
    }
    $this->view->header_path_modifier = $header_path_modifier;
  */
}


Assets_View::~Assets_View ()
{
  if (view) flateFreeMem (view);
}


// Sets a variable (key and value) for the html template.
void Assets_View::set_variable (string key, string value)
{
  variables [key] = value;
}


// Enable displaying a zone in the html template.
void Assets_View::enable_zone (string zone)
{
  variables [zone] = "";
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
    cout << "Cannot find template file " + tpl << endl; // Todo create a logbook database, to log everything there.
    return "";
  }

  // Load the template in memory. 
  flateSetFile (&view, (char *) tpl.c_str());

  // Set any variables in the template.
  map <string, string>::iterator iter;
  for (iter = variables.begin (); iter != variables.end(); ++iter) {
    flateSetVar (view, (char *) iter->first.c_str(), (char *) iter->second.c_str());
  }

  /* C++Port
    $session_logic = Session_Logic::getInstance ();
    if ($session_logic->loggedIn ()) {
      $this->view->user = $session_logic->currentUser ();
      $this->view->level = $session_logic->currentLevel (true);
    }
  */

  // Get and return the page contents.
  char *buf = flatePage (view);
  string page = buf;
  free (buf);
  if (view) flateFreeMem (view); // Fix memory leak detected by valgrind.
  view = NULL; // No double free.
  return page;
}
