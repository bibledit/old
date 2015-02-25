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


#include <resource/logic.h>
#include <webserver/request.h>
#include <access/bible.h>
#include <database/usfmresources.h>
#include <database/resources.h>
#include <filter/string.h>


vector <string> Resource_Logic::getNames (void * webserver_request)
{
  vector <string> names;
  
  // Take Bibles the user has read access to.
  vector <string> bibles = access_bible_bibles (webserver_request);
  names.insert (names.end(), bibles.begin (), bibles.end());
  
  // Take USFM Resources.
  Database_UsfmResources database_usfmresources = Database_UsfmResources ();
  vector <string> usfm_resources = database_usfmresources.getResources ();
  names.insert (names.end(), usfm_resources.begin(), usfm_resources.end());
  
  // Take external Resources.
  Database_Resources database_resources = Database_Resources ();
  vector <string> resources = database_resources.getNames ();
  names.insert (names.end (), resources.begin(), resources.end());
  
  names = array_unique (names);
  sort (names.begin(), names.end());
  
  return names;
}
