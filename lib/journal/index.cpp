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


#include <journal/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <webserver/request.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/date.h>
#include <filter/roles.h>
#include <database/logs.h>
#include <locale/translate.h>


const char * journal_index_url ()
{
  return "journal/index";
}


bool journal_index_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string render_journal_entry (string entry)
{
  // Remove the user's level.
  entry.erase (0, 2);
  // Extract and remove the seconds since the Unix epoch.
  time_t seconds = convert_to_int (entry);
  entry.erase (0, 11);
  // Localize the seconds.
  seconds = filter_date_local_seconds (seconds);
  // Sanitize HTML.
  entry = filter_string_sanitize_html (entry);
  // Convert \n to <br>
  entry = filter_string_str_replace ("\n", "<br>", entry);
  // Convert the seconds into a human readable time.
  string timestamp = filter_string_fill (convert_to_string (filter_date_numerical_hour (seconds)), 2, '0');
  timestamp.append (":");
  timestamp.append (filter_string_fill (convert_to_string (filter_date_numerical_minute (seconds)), 2, '0'));
  timestamp.append (":");
  timestamp.append (filter_string_fill (convert_to_string (filter_date_numerical_second (seconds)), 2, '0'));
  // Done.
  return timestamp + " | " + entry;
}


// Deal with AJAX call for a possible new journal entry.
string journal_index_ajax (Webserver_Request * request, string filename)
{
  int userLevel = request->session_logic()->currentLevel ();
  // Sample filetime: "141708283400041520".
  // It is the number of seconds past the Unix epoch, plus the microseconds within the current second.
  Database_Logs database_logs = Database_Logs ();
  string result = database_logs.getNext (filename);
  if (!result.empty()) {
    int entryLevel = convert_to_int (result);
    if (entryLevel <= userLevel) result = render_journal_entry (result);
    else result.clear ();
    result.insert (0, filename + "\n");
  }
  return result;
}


string journal_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  int userLevel = request->session_logic()->currentLevel ();

  string filename = request->query ["filename"];
  if (!filename.empty ()) return journal_index_ajax (request, filename);

  Database_Logs database_logs = Database_Logs ();

  
  Assets_Header header = Assets_Header (translate ("Journal"), webserver_request);
  string page = header.run ();


  Assets_View view = Assets_View ();
  view.set_variable ("journal", translate ("Journal"));


  if (request->query.count ("clear")) {
    database_logs.clear ();
  }

  
  string lastfilename;
  vector <string> entries = database_logs.get (0, lastfilename);


  // By default it shows no more than so many entries to avoid clogging the browser.
  // The user can elect to view all entries.
  if (request->query.count ("all") == 0) {
    size_t limit = 1000;
    if (entries.size () > limit) {
      limit = entries.size () - limit;
      for (unsigned int i = 0; i < limit; i++) {
        entries.erase (entries.begin());
      }
    }
  }


  string lines;
  for (auto entry : entries) {
    int entryLevel = convert_to_int (entry);
    if (entryLevel > userLevel) continue;
    entry = render_journal_entry (entry);
    lines.append ("<p>");
    lines.append (entry);
    lines.append ("</p>\n");
  }
  view.set_variable ("lines", lines);

  
  // Pass the filename of the most recent entry to javascript
  // for use by the AJAX calls for getting subsequent journal entries.
  string script = "var filename = " + lastfilename + ";";
  view.set_variable ("script", script);


  page += view.render ("journal", "index");

  page += Assets_Page::footer ();

  return page;
}

