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


#include <workbench/logic.h>
#include <filter/string.h>
#include <filter/url.h>
#include <webserver/request.h>
#include <edit/index.h>
#include <editone/index.h>
#include <editverse/index.h>
#include <search/index.h>
#include <resource/index.h>
#include <notes/index.h>
#include <consistency/index.h>
#include <sync/logic.h>
#include <locale/translate.h>


vector <string> workbench_get_default_names ()
{
  // Any of the names below should not contain commas,
  // because the sorting mechanism takes the comma as a separator,
  // so if commas are in a name, the sorting no longer works.
  return {
    translate ("Editor and Resources"),
    translate ("Editor and Notes"),
    translate ("Resources and Editor and Notes"),
    translate ("Editor and Consistency tool"),
    translate ("Visual editor and USFM editor")
  };
}


map <int, string> workbench_get_default_urls (int id)
{
  map <int, string> urls;
  switch (id) {
    case 1:
      urls [0] = edit_index_url (); // Todo
      urls [5] = resource_index_url ();
      break;
    case 2:
      urls [0] = edit_index_url (); // Todo
      urls [1] = notes_index_url ();
      break;
    case 3:
      urls [0] = resource_index_url ();
      urls [1] = edit_index_url (); // Todo
      urls [2] = notes_index_url ();
      break;
    case 4:
      urls [0] = edit_index_url (); // Todo
      urls [1] = consistency_index_url ();
      break;
    case 5:
      urls [0] = resource_index_url ();
      urls [1] = editone_index_url (); // Todo
      urls [5] = editverse_index_url (); // Todo
      break;
    default:
      urls [0] = edit_index_url (); // Todo
      urls [1] = resource_index_url ();
      urls [2] = notes_index_url ();
      urls [3] = search_index_url ();
      break;
  }
  return urls;
}


map <int, string> workbench_get_default_widths (int id)
{
  map <int, string> widths;
  switch (id) {
    case 1:
      widths [0] = "1";
      widths [5] = "1";
      break;
    case 2:
      widths [0] = "3";
      widths [1] = "1";
      break;
    case 3:
      widths [0] = "3";
      widths [1] = "4";
      widths [2] = "2";
      break;
    case 4:
      widths [0] = "1";
      widths [1] = "1";
      break;
    case 5:
      widths [0] = "1";
      widths [1] = "1";
      widths [5] = "1";
      break;
    default:
      widths [0] = "1";
      widths [1] = "1";
      widths [2] = "1";
      widths [3] = "1";
      break;
  }
  return widths;
}


map <int, string> workbench_get_default_heights (int id)
{
  map <int, string> heights;
  switch (id) {
    case 1:
      heights [0] = "1";
      heights [1] = "3";
      break;
    case 2:
    case 3:
    case 4:
      heights [0] = "1";
      break;
    case 5:
      heights [0] = "9";
      heights [1] = "1";
      break;
    default:
      heights [0] = "1";
      break;
  }
  return heights;
}


void workbench_create_defaults (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  // Save current active desktop.
  string desktop = request->database_config_user()->getActiveWorkbench ();

  // Create or update the default desktops.
  vector <string> names = workbench_get_default_names ();
  for (unsigned int i = 0; i < names.size (); i++) {
    request->database_config_user()->setActiveWorkbench (names [i]);
    int bench = i + 1;
    workbench_set_urls (request, workbench_get_default_urls (bench));
    workbench_set_widths (request, workbench_get_default_widths (bench));
    workbench_set_heights (request, workbench_get_default_heights (bench));
  }

  // Restore current active desktop.
  request->database_config_user()->setActiveWorkbench (desktop);
}


string workbench_get_active_name (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  string workbench = request->database_config_user()->getActiveWorkbench ();
  if (workbench.empty ()) {
    workbench = workbench_get_default_name ();
  }
  return workbench;
}


// This function processes the units for a $length value.
string workbench_process_units (string length)
{
  // If a size factor is found, great, otherwise default to 1
  if (length == convert_to_string (convert_to_int (length))) {
    return length;
  } else {
    string length = "1";
    return length;
  }
}


#define URLS 1
#define WIDTHS 2
#define HEIGHTS 3
#define ENTIREWIDTH 4


void workbench_set_values (void * webserver_request, int selector, const map <int, string> & values)
{
  // Store values locally, and for a client, store them also for sending to the server.
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  string workbench = workbench_get_active_name (request);
  string rawvalue;
  if (selector == URLS) rawvalue = request->database_config_user()->getWorkbenchURLs ();
  if (selector == WIDTHS) rawvalue = request->database_config_user()->getWorkbenchWidths ();
  if (selector == HEIGHTS) rawvalue = request->database_config_user()->getWorkbenchHeights ();
  if (selector == ENTIREWIDTH) rawvalue = request->database_config_user()->getEntireWorkbenchWidths ();
  vector <string> currentlines = filter_string_explode (rawvalue, '\n');
  vector <string> newlines;
  for (auto & line : currentlines) {
    if (line.find (workbench + "_") != 0) {
      newlines.push_back (line);
    }
  }
  for (auto & element : values) {
    string line = workbench + "_" + convert_to_string (element.first) + "_" + element.second;
    newlines.push_back (line);
  }
  rawvalue = filter_string_implode (newlines, "\n");
  if (selector == URLS) {
    request->database_config_user()->setWorkbenchURLs (rawvalue);
    workbench_cache_for_cloud (request, true, false, false);
  }
  if (selector == WIDTHS) {
    request->database_config_user()->setWorkbenchWidths (rawvalue);
    workbench_cache_for_cloud (request, false, true, false);
  }
  if (selector == HEIGHTS) {
    request->database_config_user()->setWorkbenchHeights (rawvalue);
    workbench_cache_for_cloud (request, false, false, true);
  }
  if (selector == ENTIREWIDTH) {
    request->database_config_user()->setEntireWorkbenchWidths (rawvalue);
    workbench_cache_for_cloud (request, false, true, false);
  }
}


void workbench_set_urls (void * webserver_request, const map <int, string> & values)
{
  // Get current order of the workbenches.
  vector <string> order = workbench_get_names (webserver_request);
  // Update the values: This reorders the workbenches.
  workbench_set_values (webserver_request, URLS, values);
  // Put the workbenches in the original order.
  workbench_reorder (webserver_request, order);
}


void workbench_set_widths (void * webserver_request, const map <int, string> & values)
{
  workbench_set_values (webserver_request, WIDTHS, values);
}


void workbench_set_heights (void * webserver_request, const map <int, string> & values)
{
  workbench_set_values (webserver_request, HEIGHTS, values);
}


void workbench_set_entire_width (void * webserver_request, string value)
{
  map <int, string> values = {make_pair (0, value)};
  workbench_set_values (webserver_request, ENTIREWIDTH, values);
}


map <int, string> workbench_get_values (void * webserver_request, int selector, bool use)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  map <int, string> values;
  
  string workbench = workbench_get_active_name (request);
  
  string rawvalue;
  if (selector == URLS) rawvalue = request->database_config_user()->getWorkbenchURLs ();
  if (selector == WIDTHS) rawvalue = request->database_config_user()->getWorkbenchWidths ();
  if (selector == HEIGHTS) rawvalue = request->database_config_user()->getWorkbenchHeights ();
  if (selector == ENTIREWIDTH) rawvalue = request->database_config_user()->getEntireWorkbenchWidths ();
  vector <string> lines = filter_string_explode (rawvalue, '\n');
  for (auto & line : lines) {
    if (line.find (workbench + "_") == 0) {
      vector <string> bits = filter_string_explode (line, '_');
      if (bits.size() == 3) {
        int key = convert_to_int (bits [1]);
        string value = bits [2];
        values [key] = value;
      }
    }
  }
  
  if (values.empty ()) {
    if (selector == URLS) values = workbench_get_default_urls (0);
    if (selector == WIDTHS) values = workbench_get_default_widths (0);
    if (selector == HEIGHTS) values = workbench_get_default_heights (0);
    if (selector == ENTIREWIDTH) values.clear ();
  }

  for (auto & element : values) {
    
    if ((selector == URLS) && use) {
  
      // Add query value for suppressing the topbar as the workbench already has one.
      if (element.second != "") {
        element.second = filter_url_build_http_query (element.second, "topbar", "0");
      }
      
      // Transform the internal URLs to full ones.
      vector <string> bits = filter_string_explode (element.second, '/');
      if (bits.size() == 2) {
        element.second.insert (0, "/");
      }
      
      // Encode URL.
      element.second = filter_string_str_replace (" ", "%20", element.second);
    }

    if (selector == WIDTHS) {
      // Fix the units.
      element.second = workbench_process_units (element.second);
    }

    if (selector == HEIGHTS) {
      // Fix the units.
      element.second = workbench_process_units (element.second);
    }

  }

  return values;
}


map <int, string> workbench_get_urls (void * webserver_request, bool use)
{
  return workbench_get_values (webserver_request, URLS, use);
}


map <int, string> workbench_get_widths (void * webserver_request)
{
  return workbench_get_values (webserver_request, WIDTHS, false);
}


map <int, string> workbench_get_heights (void * webserver_request)
{
  return workbench_get_values (webserver_request, HEIGHTS, false);
}


string workbench_get_entire_width (void * webserver_request)
{
  map <int, string> values = workbench_get_values (webserver_request, ENTIREWIDTH, false);
  for (auto & element : values) return element.second;
  return "";
}


// Returns the names of the available workbenches.
vector <string> workbench_get_names (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  vector <string> workbenches;
  // The names and the order of the workbenches is taken from the URLs.
  string rawvalue = request->database_config_user()->getWorkbenchURLs ();
  vector <string> lines = filter_string_explode (rawvalue, '\n');
  for (auto & line : lines) {
    vector <string> bits = filter_string_explode (line, '_');
    if (bits.size() == 3) {
      if (find (workbenches.begin(), workbenches.end(), bits[0]) == workbenches.end()) {
        workbenches.push_back (bits[0]);
      }
    }
  }
  if (workbenches.empty ()) workbenches.push_back (workbench_get_active_name (request));
  return workbenches;
}


void workbench_delete (void * webserver_request, string workbench)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  string rawvalue;
  vector <string> currentlines;
  vector <string> newlines;
  
  rawvalue = request->database_config_user()->getWorkbenchURLs ();
  currentlines = filter_string_explode (rawvalue, '\n');
  newlines.clear ();
  for (auto & line : currentlines) {
    if (line.find (workbench + "_") != 0) newlines.push_back (line);
  }
  rawvalue = filter_string_implode (newlines, "\n");
  request->database_config_user()->setWorkbenchURLs (rawvalue);
  
  rawvalue = request->database_config_user()->getWorkbenchWidths ();
  currentlines = filter_string_explode (rawvalue, '\n');
  newlines.clear ();
  for (auto & line : currentlines) {
    if (line.find (workbench + "_") != 0) newlines.push_back (line);
  }
  rawvalue = filter_string_implode (newlines, "\n");
  request->database_config_user()->setWorkbenchWidths (rawvalue);
  
  rawvalue = request->database_config_user()->getWorkbenchHeights ();
  currentlines = filter_string_explode (rawvalue, '\n');
  newlines.clear ();
  for (auto & line : currentlines) {
    if (line.find (workbench + "_") != 0) newlines.push_back (line);
  }
  rawvalue = filter_string_implode (newlines, "\n");
  request->database_config_user()->setWorkbenchHeights (rawvalue);
  
  request->database_config_user()->setActiveWorkbench ("");
  
  // For a client, store the setting for sending to the server.
  workbench_cache_for_cloud (request, true, true, true);
}


// This orders the workbenches.
// It takes the order as in array $workbenches.
void workbench_reorder (void * webserver_request, const vector <string> & workbenches)
{
  // The order of the workbenches is taken from the URLs.
  // Widths and heights are not considered for the order.
  
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  // Retrieve the old order of the workbenches, plus their details.
  string rawvalue = request->database_config_user()->getWorkbenchURLs ();
  vector <string> oldlines = filter_string_explode (rawvalue, '\n');
  
  // Create vector with the sorted workbench definitions.
  vector <string> newlines;
  for (auto & workbench : workbenches) {
    for (auto & line : oldlines) {
      if (line.find (workbench + "_") == 0) {
        newlines.push_back (line);
        line.clear ();
      }
    }
  }
  
  // Add any extra ones.
  for (auto & line : oldlines) {
    if (!line.empty ()) {
      newlines.push_back (line);
    }
  }

  // Save everything.
  rawvalue = filter_string_implode (newlines, "\n");
  request->database_config_user()->setWorkbenchURLs (rawvalue);

  // Schedule for sending to Cloud.
  workbench_cache_for_cloud (request, true, false, false);
}


// Copy desktop $source to $destination
void workbench_copy (void * webserver_request, string source, string destination)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  // Save current active desktop.
  string active_desktop = request->database_config_user()->getActiveWorkbench ();
  
  // Copy source desktop to destination.
  request->database_config_user()->setActiveWorkbench (source);
  map <int, string> urls = workbench_get_urls (webserver_request, false);
  map <int, string> widths = workbench_get_widths (webserver_request);
  map <int, string> heights = workbench_get_heights (webserver_request);
  string entire_width = workbench_get_entire_width (webserver_request);
  request->database_config_user()->setActiveWorkbench (destination);
  workbench_set_urls (webserver_request, urls);
  workbench_set_widths (webserver_request, widths);
  workbench_set_heights (webserver_request, heights);
  workbench_set_entire_width (webserver_request, entire_width);
  
  // Restore current active desktop.
  request->database_config_user()->setActiveWorkbench (active_desktop);
}


// Store updated workbench settings for sending to the cloud.
void workbench_cache_for_cloud (void * webserver_request, bool urls, bool widths, bool heights)
{
#ifdef HAVE_CLIENT
  // For a client, store the setting for sending to the server.
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  if (urls)
    request->database_config_user()->addUpdatedSetting (Sync_Logic::settings_send_workbench_urls);
  if (widths)
    request->database_config_user()->addUpdatedSetting (Sync_Logic::settings_send_workbench_widths);
  if (heights)
    request->database_config_user()->addUpdatedSetting (Sync_Logic::settings_send_workbench_heights);
#else
  (void) webserver_request;
  (void) urls;
  (void) widths;
  (void) heights;
#endif
}


string workbench_get_default_name ()
{
  return "Default";
}


// Send the named $desktop to a $user name.
void workbench_send (void * webserver_request, string desktop, string user)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  // Save current active desktop.
  string active_desktop = request->database_config_user()->getActiveWorkbench ();
  
  // Retrieve settings for the $desktop of the current user.
  request->database_config_user()->setActiveWorkbench (desktop);
  map <int, string> urls = workbench_get_urls (webserver_request, false);
  map <int, string> widths = workbench_get_widths (webserver_request);
  map <int, string> heights = workbench_get_heights (webserver_request);
  string entire_width = workbench_get_entire_width (webserver_request);
  
  // Restore current active desktop.
  request->database_config_user()->setActiveWorkbench (active_desktop);

  // New webserver request object for the destination user.
  Webserver_Request destination_request;
  destination_request.session_logic ()->setUsername (user);
  
  // Save desktop for destination user.
  active_desktop = destination_request.database_config_user()->getActiveWorkbench ();
  destination_request.database_config_user()->setActiveWorkbench (desktop);
  
  // Copy source desktop to destination.
  workbench_set_urls (&destination_request, urls);
  workbench_set_widths (&destination_request, widths);
  workbench_set_heights (&destination_request, heights);
  workbench_set_entire_width (&destination_request, entire_width);

  // Restore desktop for the destination user.
  request->database_config_user()->setActiveWorkbench (active_desktop);
}
