/*
Copyright (©) 2003-2016 Teus Benschop.

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
#include <menu/logic.h>
#include <client/logic.h>


const char * journal_index_url ()
{
  return "journal/index";
}


bool journal_index_acl (void * webserver_request)
{
  // In Client mode, anyone can view the journal.
#ifdef CLIENT_PREPARED
  return true;
#endif
  // The role of Consultant or higher can view the journal.
  if (Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ())) {
    return true;
  }
  // No access.
  return false;
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
  // Split entry into lines.
  vector <string> lines = filter_string_explode (entry, '\n');
  if (!lines.empty ()) entry = lines [0];
  // Sanitize HTML.
  entry = filter_string_sanitize_html (entry);
  // Convert \n to <br>
  // Todo entry = filter_string_str_replace ("\n", "<br>", entry);
  // Convert the seconds into a human readable time.
  string timestamp = filter_string_fill (convert_to_string (filter_date_numerical_hour (seconds)), 2, '0');
  timestamp.append (":");
  timestamp.append (filter_string_fill (convert_to_string (filter_date_numerical_minute (seconds)), 2, '0'));
  timestamp.append (":");
  timestamp.append (filter_string_fill (convert_to_string (filter_date_numerical_second (seconds)), 2, '0'));
  // Done.
  return "<p>" + timestamp + " | " + entry + "</p>\n";
}


// Deal with AJAX call for a possible new journal entry.
string journal_index_ajax (Webserver_Request * request, string filename)
{
  int userLevel = request->session_logic()->currentLevel ();
  // Sample filetime: "141708283400041520".
  // It is the number of seconds past the Unix epoch, plus the microseconds within the current second.
  string result = Database_Logs::getNext (filename);
  if (!result.empty()) {
    int entryLevel = convert_to_int (result);
    // Cloud: Pay attention to only rendering journal entries of sufficient user level.
    // Client: Render any journal entry.
    if ((entryLevel <= userLevel) || client_logic_client_enabled ()) {
      result = render_journal_entry (result);
    } else {
      result.clear ();
    }
    result.insert (0, filename + "\n");
  }
  return result;
}


string journal_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  int userLevel = request->session_logic()->currentLevel ();

  string filename = request->query ["filename"];
  if (!filename.empty ()) {
    return journal_index_ajax (request, filename);
  }

  Assets_Header header = Assets_Header (translate ("Journal"), webserver_request);
  header.addBreadCrumb (menu_logic_tools_menu (), menu_logic_tools_text ());
  string page = header.run ();


  Assets_View view;


  if (request->query.count ("clear")) {
    Database_Logs::clear ();
    // If the logbook has been cleared on a mobile device, and the screen shuts off,
    // and then the user activates the screen on the mobile device,
    // the logbook will then again be cleared, because that was the last opened URL.
    // Redirecting the browser to a clean URL fixes this behaviour.
    redirect_browser (request, journal_index_url ());
    return "";
  }

  
  string lastfilename;
  vector <string> entries = Database_Logs::get (lastfilename);


  string lines;
  for (auto entry : entries) {
    int entryLevel = convert_to_int (entry);
    // Cloud: Pay attention to only rendering journal entries of sufficient user level.
    // Client: Render any journal entry.
#ifndef CLIENT_PREPARED
    if (entryLevel > userLevel) continue;
#endif
    lines.append (render_journal_entry (entry));
  }
  view.set_variable ("lines", lines);

  
  // Pass the filename of the most recent entry to javascript
  // for use by the AJAX calls for getting subsequent journal entries.
  // It should be passed as a String object in JavaScript.
  // Because when it were passed as an Int, JavaScript would round the value off.
  // And rounding it off often led to double journal entries.
  string script = "var filename = \"" + lastfilename + "\";";
  view.set_variable ("script", script);


  page += view.render ("journal", "index");

  page += Assets_Page::footer ();

  return page;
}

