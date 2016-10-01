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


#include <system/timezone.h>
#include <webserver/request.h>
#include <filter/string.h>


string system_timeoffset_url ()
{
  return "system/timeoffset";
}


bool system_timeoffset_acl (void * webserver_request)
{
  (void) (webserver_request);
  return true;
}


string system_timeoffset (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  if (request->query.count ("offset")) {
    string input = request->query ["offset"];
    int timezone = convert_to_int (input);
    if (timezone < MINIMUM_TIMEZONE) {
      timezone = MINIMUM_TIMEZONE;
    }
    if (timezone > MAXIMUM_TIMEZONE) {
      timezone = MAXIMUM_TIMEZONE;
    }
#ifdef HAVE_URLSETTINGS
    Database_Config_General::setTimezone (timezone);
#endif
  }
  
  return "";
}
