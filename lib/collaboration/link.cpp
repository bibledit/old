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


#include <collaboration/link.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/git.h>
#include <filter/url.h>
#include <locale/translate.h>
#include <access/bible.h>
#include <database/config/bible.h>
#include <database/jobs.h>
#include <git2.h>


void collaboration_link (string object, int jobid, string direction)
{
  string url = Database_Config_Bible::getRemoteRepositoryUrl (object);
  string path = filter_git_directory (object);
  bool result = true;
  string error;

  Database_Jobs database_jobs;

  string page = collaboration_link_header ();
  Assets_View view = Assets_View ();
  view.set_variable ("object", object);
  view.set_variable ("url", url);
  if (direction == "me") view.enable_zone ("takeme");
  if (direction == "repo") view.enable_zone ("takerepo");
  page += view.render ("collaboration", "clone");
  page += Assets_Page::footer ();
  database_jobs.setStart (jobid, page);

  if (result) {
    if (object.empty ()) {
      error = gettext ("No Bible given");
      result = false;
    }
  }
  if (result) {
    if (direction != "repo" && direction != "me") {
      error = gettext ("It is unclear which data to copy to where");
      result = false;
    }
  }
  if (result) {
    result = filter_git_remote_clone (url, path, jobid, error);
  }

  

/* Todo rename detection off / default pushing method, name / email.

 
 
 // Switch rename detection off.
    // This is necessary for the consultation notes, since git has been seen to "detect" spurious renames.
    exec ("cd $escapedDir; git config diff.renamelimit 0");
    exec ("cd $escapedDir; git config diff.renames false");
    
    // Newer versions of git ask the user to set the default pushing method.
    exec ("cd $escapedDir; git config push.default matching");
    
    // On some machines the mail name and address are not set properly; therefore these are set here.
    $database_config_general = Database_Config_General::getInstance();
    $mail_name = Database_Config_General::getSiteMailName();
    if (!$mail_name) $mail_name = "Bibledit";
    exec ("cd $escapedDir; git config user.name \"$mail_name\"");
    $mail_address = Database_Config_General::getSiteMailAddress();
    if (!$mail_address) $mail_address = "bibledit-web@bibledit.org";
    exec ("cd $escapedDir; git config user.email \"$mail_address\"");
*/

  page = collaboration_link_header ();
  view = Assets_View ();
  view.set_variable ("object", object);
  view.set_variable ("url", url);
  if (direction == "me") view.enable_zone ("takeme");
  if (direction == "repo") view.enable_zone ("takerepo");
  if (result) {
    view.enable_zone ("okay");
  } else {
    view.enable_zone ("error");
    view.set_variable ("error", error);
  }
  page += view.render ("collaboration", "link");
  page += Assets_Page::footer ();
  database_jobs.setResult (jobid, page);
}


string collaboration_link_header ()
{
  return "<h1>" + gettext ("Link repository") + "</h1>\n";
}
