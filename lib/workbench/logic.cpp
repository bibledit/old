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
#include <config/logic.h>


map <int, string> workbenchDefaultURLs (int id)
{
  map <int, string> urls;
  switch (id) {
    case 1:
      urls [0] = edit_index_url ();
      urls [5] = resource_index_url ();
      break;
    case 2:
      urls [0] = edit_index_url ();
      urls [1] = notes_index_url ();
      break;
    case 3:
      urls [0] = resource_index_url ();
      urls [1] = edit_index_url ();
      urls [2] = notes_index_url ();
      break;
    case 4:
      urls [0] = edit_index_url ();
      urls [1] = consistency_index_url ();
      break;
    case 5:
      urls [0] = resource_index_url ();
      urls [1] = editone_index_url ();
      urls [5] = editverse_index_url ();
      break;
    default:
      urls [0] = edit_index_url ();
      urls [1] = resource_index_url ();
      urls [2] = notes_index_url ();
      urls [3] = search_index_url ();
      break;
  }
  return urls;
}


map <int, string> workbenchDefaultWidths (int id)
{
  map <int, string> widths;
  switch (id) {
    case 1:
      widths [0] = "100%";
      widths [5] = "100%";
      break;
    case 2:
      widths [0] = "75%";
      widths [1] = "20%";
      break;
    case 3:
      widths [0] = "32%";
      widths [1] = "42%";
      widths [2] = "22%";
      break;
    case 4:
      widths [0] = "45%";
      widths [1] = "50%";
      break;
    case 5:
      widths [0] = "52%";
      widths [1] = "42%";
      widths [5] = "66%";
      break;
    default:
      widths [0] = "24%";
      widths [1] = "24%";
      widths [2] = "24%";
      widths [3] = "24%";
      break;
  }
  return widths;
}


map <int, string> workbenchDefaultHeights (int id)
{
  map <int, string> heights;
  switch (id) {
    case 1:
      heights [0] = "25%";
      heights [1] = "65%";
      break;
    case 2:
    case 3:
    case 4:
      heights [0] = "80%";
      break;
    case 5:
      heights [0] = "92%";
      heights [1] = "24%";
      break;
    default:
      heights [0] = "90%";
      break;
  }
  return heights;
}


string workbenchGetActiveWorkbench (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  string workbench = request->database_config_user()->getActiveWorkbench ();
  if (workbench.empty ()) {
    workbench = "default";
  }
  return workbench;
}


// This function processes the units for a $length value.
string workbenchProcessUnits (string length)
{
  // If no unit is found, add %.
  if (length == convert_to_string (convert_to_int (length))) {
    return length + "%";
  }
  // If % is found, clean the value up.
  if (length.find ("%") != string::npos) {
    length = convert_to_string (convert_to_int (length)) + "%";
    return length;
  }
  // If px is found, clean the value up.
  if (length.find ("px") != string::npos) {
    length = convert_to_string (convert_to_int (length)) + "px";
    return length;
  }
  // Pass the length through.
  return length;
}


#define URLS 1
#define WIDTHS 2
#define HEIGHTS 3


void workbenchSetValues (void * webserver_request, int selector, const map <int, string> & values)
{
  // Store values locally, and for a client, store them also for sending to the server.
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  string workbench = workbenchGetActiveWorkbench (request);
  string rawvalue;
  if (selector == URLS) rawvalue = request->database_config_user()->getWorkbenchURLs ();
  if (selector == WIDTHS) rawvalue = request->database_config_user()->getWorkbenchWidths ();
  if (selector == HEIGHTS) rawvalue = request->database_config_user()->getWorkbenchHeights ();
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
    workbenchCacheForCloud (request, true, false, false);
  }
  if (selector == WIDTHS) {
    request->database_config_user()->setWorkbenchWidths (rawvalue);
    workbenchCacheForCloud (request, false, true, false);
  }
  if (selector == HEIGHTS) {
    request->database_config_user()->setWorkbenchHeights (rawvalue);
    workbenchCacheForCloud (request, false, false, true);
  }
}


void workbenchSetURLs (void * webserver_request, const map <int, string> & values)
{
  workbenchSetValues (webserver_request, URLS, values);
}


void workbenchSetWidths (void * webserver_request, const map <int, string> & values)
{
  workbenchSetValues (webserver_request, WIDTHS, values);
}


void workbenchSetHeights (void * webserver_request, const map <int, string> & values)
{
  workbenchSetValues (webserver_request, HEIGHTS, values);
}


map <int, string> workbenchGetValues (void * webserver_request, int selector, bool use)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  map <int, string> values;
  
  string workbench = workbenchGetActiveWorkbench (request);
  
  string rawvalue;
  if (selector == URLS) rawvalue = request->database_config_user()->getWorkbenchURLs ();
  if (selector == WIDTHS) rawvalue = request->database_config_user()->getWorkbenchWidths ();
  if (selector == HEIGHTS) rawvalue = request->database_config_user()->getWorkbenchHeights ();
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
    if (selector == URLS) values = workbenchDefaultURLs (0);
    if (selector == WIDTHS) values = workbenchDefaultWidths (0);
    if (selector == HEIGHTS) values = workbenchDefaultHeights (0);
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
      element.second = workbenchProcessUnits (element.second);
    }

    if (selector == HEIGHTS) {
      // Fix the units.
      element.second = workbenchProcessUnits (element.second);
    }

  }

  return values;
}


map <int, string> workbenchGetURLs (void * webserver_request, bool use)
{
  return workbenchGetValues (webserver_request, URLS, use);
}


map <int, string> workbenchGetWidths (void * webserver_request)
{
  return workbenchGetValues (webserver_request, WIDTHS, false);
}


map <int, string> workbenchGetHeights (void * webserver_request)
{
  return workbenchGetValues (webserver_request, HEIGHTS, false);
}


// Returns the names of the available workbenches.
vector <string> workbenchGetWorkbenches (void * webserver_request)
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
  if (workbenches.empty ()) workbenches.push_back (workbenchGetActiveWorkbench (request));
  return workbenches;
}


void workbenchDeleteWorkbench (void * webserver_request, string workbench)
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
  workbenchCacheForCloud (request, true, true, true);
}


// This orders the workbenches.
// It takes the order as in array $workbenches.
void workbenchOrderWorkbenches (void * webserver_request, const vector <string> & workbenches)
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
      }
    }
  }

  // Save everything.
  rawvalue = filter_string_implode (newlines, "\n");
  request->database_config_user()->setWorkbenchURLs (rawvalue);

  // Schedule for sending to Cloud.
  workbenchCacheForCloud (request, true, false, false);
}


// Store updated workbench settings for sending to the cloud.
void workbenchCacheForCloud (void * webserver_request, bool urls, bool widths, bool heights)
{
  if (config_logic_client_prepared ()) {
    // For a client, store the setting for sending to the server.
    Webserver_Request * request = (Webserver_Request *) webserver_request;
    if (urls)
      request->database_config_user()->addUpdatedSetting (Sync_Logic::settings_send_workbench_urls);
    if (widths)
      request->database_config_user()->addUpdatedSetting (Sync_Logic::settings_send_workbench_widths);
    if (heights)
      request->database_config_user()->addUpdatedSetting (Sync_Logic::settings_send_workbench_heights);
  }
}