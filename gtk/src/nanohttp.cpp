/*
 ** Copyright (©) 2003-2013 Teus Benschop.
 **  
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


#include "nanohttp.h"
#include <libxml/nanohttp.h>
#include "gwrappers.h"
#include "tiny_utilities.h"


void nano_http_server_access ()
{
  ustring url = "http://localhost/";
  void *http_context = xmlNanoHTTPOpen (url.c_str(), NULL);
  if (!http_context) {
    gw_critical ("Could not connect to the local web server");
    return;
  }

  int return_code = xmlNanoHTTPReturnCode (http_context);
  if (return_code != 200) {
    gw_critical ("Local web server returned unusual code: " + convert_to_string (return_code));
  }

  char *buffer = (char*) calloc (1, 1000);
  int size = 0;
  int nread;
  while ((nread = xmlNanoHTTPRead(http_context, buffer + size, 999)) > 0) {
    size += nread;
    if (nread == 999) {
      buffer = (char*) realloc (buffer, size + 999);
      buffer[size + 1] = '\0';
    }
  }
  free(buffer);

  xmlNanoHTTPClose(http_context);
}


// The functions above work, but are sensitive to harsh conditions.
// E.g. when requesting http://localhost (without a final slash)
// it gave an error. It wanted http://localhost/.

