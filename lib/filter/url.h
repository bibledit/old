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


#pragma once


#include <iostream>
#include <cstdlib>
#include <vector>
#include <webserver/request.h>


using namespace std;


void filter_url_redirect (string url, Webserver_Request * request);
string filter_url_dirname (string url);
string filter_url_basename (string url);
string filter_url_create_path (vector <string> components);
string filter_url_get_extension (string url);
bool filter_url_file_exists (string url);
string filter_url_get_file_contents (string filename);
