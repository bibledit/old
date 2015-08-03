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


#include <bible/import_bibleworks.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/bibleworks.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <access/bible.h>
#include <tasks/logic.h>


string bible_import_bibleworks_url ()
{
  return "bible/import_bibleworks";
}


bool bible_import_bibleworks_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::manager ());
}


string bible_import_bibleworks (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string page;
  
  page = Assets_Page::header (translate ("Import BibleWorks"), webserver_request, "");
  
  Assets_View view = Assets_View ();
  
  string success_message;
  string error_message;
  
  // The name of the Bible.
  string bible = access_bible_clamp (request, request->query["bible"]);
  view.set_variable ("bible", filter_string_sanitize_html (bible));
  
  // Whether the user has write access to this Bible.
  bool write_access = access_bible_write (request, bible);
  if (write_access) view.enable_zone ("write_access");

  // Data submission.
  if (request->post.count("submit")) {
    // Whether to keep the grammatical tags.
    bool tags = request->post.count ("tags");
    // The BibleWorks text.
    string data = request->post ["data"];
    data = filter_string_trim (data);
    if (data != "") {
      if (unicode_string_is_valid (data)) {
        // Convert the BibleWorks text to USFM.
        string usfm = Filter_Bibleworks::import (data, tags);
        // Import the USFM.
        string datafile = filter_url_tempfile ();
        filter_url_file_put_contents (datafile, usfm);
        success_message = translate("Import has started. See Journal for progress.");
        tasks_logic_queue (IMPORTUSFM, { datafile, bible });
      } else {
        error_message = translate("Please supply valid Unicode UTF-8 text.");
      }
    } else {
      success_message = translate("Nothing was imported.");
    }
  }

  view.set_variable ("success_message", success_message);
  view.set_variable ("error_message", error_message);
  
  page += view.render ("bible", "import_bibleworks");
  
  page += Assets_Page::footer ();
  
  return page;
}

