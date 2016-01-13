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


#include <flate/flate.h>
#include <filter/url.h>
#include <filter/string.h>
#include <database/logs.h>
#include <locale/translate.h>


Flate::Flate ()
{
}


Flate::~Flate ()
{
}


// Sets a variable (key and value) for the html template.
void Flate::set_variable (string key, string value)
{
  variables [key] = value;
}


// Enable a zone in the html template.
void Flate::enable_zone (string zone)
{
  zones [zone] = true;
}


// Renders the html template through the flate C++ template engine.
string Flate::render (string html)
{
  string rendering;
  try {
    if (file_exists (html)) {
      rendering = filter_url_file_get_contents (html);
      process_zones (rendering);
      process_variables (rendering);
      process_translate (rendering);
    }
  } catch (...) {
    Database_Logs::log ("Failure to process template " + html);
  }
  return rendering;
}


void Flate::process_zones (string& rendering)
{
  // Limit zone iterations.
  int iterations = 0;
  // Start processing zones by locating the first one.
  size_t position = rendering.find ("<!-- #BEGINZONE");
  // Iterate through the file contents till all zones have been dealt with.
  while ((position != string::npos) && (iterations < 1000)) {
    iterations++;
    // Position where the starting zone ends.
    size_t pos = rendering.find ("-->", position);
    string zonestartline = rendering.substr (position, pos - position + 3);
    // Remove the opening tag for the current zone.
    rendering.erase (position, zonestartline.length ());
    // Name for the current zone.
    string name = zonestartline.substr (16, zonestartline.length () - 16 - 4);
    // Assemble the ending line for the current zone.
    string zoneendline = "<!-- #ENDZONE " + name + " -->";
    // Locate the ending position.
    size_t zoneendposition = rendering.find (zoneendline);
    // Process if it exists.
    if (zoneendposition != string::npos) {
      // Take the ending line out.
      rendering.erase (zoneendposition, zoneendline.length ());
      // If the zone has not been enabled, remove all its contents within.
      if (zones.count (name) == 0) {
        rendering.erase (position, zoneendposition - position);
      }
    }
    // Next zone.
    position = rendering.find ("<!-- #BEGINZONE");
  }
}


void Flate::process_variables (string& rendering)
{
  // Limit variable iterations.
  int iterations = 0;
  // Start processing variables by locating the first one.
  size_t position = rendering.find ("##");
  // Iterate through the contents till all variables have been dealt with.
  while ((position != string::npos) && (iterations < 1000)) {
    iterations++;
    bool correct = true;
    // Check that this is a correct position: It should not have hashes nearby.
    if (correct) if (position > 0) if (rendering.substr (position - 1, 1) == "#") {
      correct = false;
    }
    if (correct) if (position < rendering.size ()) if (rendering.substr (position + 2, 1) == "#") {
      correct = false;
    }
    // Position where the variable ends.
    size_t pos = rendering.find ("##", position + 1);
    if (pos == string::npos) pos = position + 4;
    // Name for the variable zone.
    string name = rendering.substr (position + 2, pos - position - 2);
    // No new line in the variable name.
    if (correct) if (name.find ("\n") != string::npos) correct = false;
    if (correct) {
      // Take the variable out.
      rendering.erase (position, name.length () + 4);
      // Insert the replacement.
      rendering.insert (position, variables [name]);
    }
    // Next zone.
    position = rendering.find ("##", position + 1);
  }
}


void Flate::process_translate (string& rendering)
{
  // Clean up the "translate" (gettext) calls.
  rendering = filter_string_str_replace ("translate (", "translate(", rendering);
  // Gettext markup.
  string gettextopen = "translate(\"";
  string gettextclose = "\")";
  // Limit gettext iterations.
  int iterations = 0;
  // Start processing variables by locating the first one.
  size_t position = rendering.find (gettextopen);
  // Iterate through the contents till all gettext calls have been dealt with.
  while ((position != string::npos) && (iterations < 1000)) {
    iterations++;
    // Remove the gettext opener.
    rendering.erase (position, gettextopen.length());
    // Position where the gettext call ends.
    size_t pos = rendering.find (gettextclose, position);
    if (pos != string::npos) {
      // Take the gettext closer out.
      rendering.erase (pos, gettextclose.length());
      // The English string.
      string english = rendering.substr (position, pos - position);
      // Take the English out.
      rendering.erase (position, pos - position);
      // Insert the localization.
      rendering.insert (position, translate (english));
    }
    // Next gettext call.
    position = rendering.find (gettextopen);
  }
}
