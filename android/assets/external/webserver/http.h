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


#ifndef INCLUDED_WEBSERVER_HTTP_H
#define INCLUDED_WEBSERVER_HTTP_H


#include <config/libraries.h>
#include <webserver/request.h>


bool http_parse_header (string header, Webserver_Request * request);
void http_parse_post (string content, Webserver_Request * request);
void http_assemble_response (Webserver_Request * request);
void http_serve_cache_file (Webserver_Request * request);


#endif
