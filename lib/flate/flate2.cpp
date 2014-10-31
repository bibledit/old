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


#include <flate/flate2.h>
#include <filter/url.h>
#include <filter/string.h>
#include <database/logs.h>


using namespace std;


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


// Renders the "tpl" template through the flate C++ template engine.
string Flate::render (string tpl)
{
  string rendering;
  try {
    if (filter_url_file_exists (tpl)) {
      rendering = filter_url_file_get_contents (tpl);
      // Limit zone iterations.
      int iterations = 0;
      // Start processing zones by locating the first one.
      size_t position = rendering.find ("<!-- #BEGINZONE");
      // Iterate through the file contents till all zones have been dealt with.
      while ((position != string::npos) && (iterations < 50)) {
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
        // Locate the ending line.
        size_t zoneendposition = rendering.find (zoneendline);
        // Process if it exists.
        if (zoneendposition != string::npos) {
          // Take the ending line out.
          rendering.erase (zoneendposition, zoneendline.length ());
          // If the zone has not been enabled, remove all its contents within.
          if (!zones [name]) {
            rendering.erase (position, zoneendposition - position);
          }
        }
        // Next zone.
        position = rendering.find ("<!-- #BEGINZONE");
      }
      // Limit variable iterations.
      iterations = 0;
      // Start processing variables by locating the first one.
      position = rendering.find ("##");
      // Iterate through the file contents till all variables have been dealt with.
      while ((position != string::npos) && (iterations < 50)) {
        iterations++;
        // Position where the variable ends.
        size_t pos = rendering.find ("##", position + 1);
        if (pos == string::npos) pos = position + 4;
        // Name for the variable zone.
        string name = rendering.substr (position + 2, pos - position - 2);
        // Take the variable out.
        rendering.erase (position, name.length () + 4);
        // Insert the replacement.
        rendering.insert (position, variables [name]);
        // Next zone.
        position = rendering.find ("##");
      }
    }
  } catch (...) {
    Database_Logs::log ("Failure to process template " + tpl);
  }
  return rendering;
}
