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


#ifndef INCLUDED_FILTER_URL_H
#define INCLUDED_FILTER_URL_H


#include <config/libraries.h>
#include <webserver/request.h>


void filter_url_redirect (string url, Webserver_Request * request);
string filter_url_dirname (string url);
string filter_url_basename (string url);
string filter_url_create_path (string part1 = "", string part2 = "", string part3 = "", string part4 = "", string part5 = "", string part6 = "");
string filter_url_create_root_path (string part1 = "", string part2 = "", string part3 = "", string part4 = "", string part5 = "");
string filter_url_get_extension (string url);
bool filter_url_file_exists (string url);
void filter_url_mkdir (string directory);
string filter_url_file_get_contents (string filename);
void filter_url_file_put_contents (string filename, string contents);
void filter_url_file_put_contents_append (string filename, string contents);
int filter_url_filesize (string filename);
vector <string> filter_url_scandir (string folder);
int filter_url_filemtime (string filename);


#endif
