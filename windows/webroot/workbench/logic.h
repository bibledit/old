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


#ifndef INCLUDED_WORKSPACE_LOGIC_H
#define INCLUDED_WORKSPACE_LOGIC_H


#include <config/libraries.h>


vector <string> workbench_get_default_names ();
map <int, string> workbench_get_default_urls (int id);
map <int, string> workbench_get_default_widths (int id);
map <int, string> workbench_get_default_heights (int id);
void workbench_create_defaults (void * webserver_request);
string workbench_get_active_name (void * webserver_request);
string workbench_process_units (string length);
void workbench_set_values (void * webserver_request, int selector, const map <int, string> & values);
void workbench_set_urls (void * webserver_request, const map <int, string> & values);
void workbench_set_widths (void * webserver_request, const map <int, string> & values);
void workbench_set_heights (void * webserver_request, const map <int, string> & values);
void workbench_set_entire_width (void * webserver_request, string value);
map <int, string> workbench_get_values (void * webserver_request, int selector, bool use);
map <int, string> workbench_get_urls (void * webserver_request, bool use);
map <int, string> workbench_get_widths (void * webserver_request);
map <int, string> workbench_get_heights (void * webserver_request);
string workbench_get_entire_width (void * webserver_request);
vector <string> workbench_get_names (void * webserver_request);
void workbench_delete (void * webserver_request, string workbench);
void workbench_reorder (void * webserver_request, const vector <string> & workbenches);
void workbench_copy (void * webserver_request, string source, string destination);
void workbench_cache_for_cloud (void * webserver_request, bool urls, bool widths, bool heights);
string workbench_get_default_name ();
void workbench_send (void * webserver_request, string desktop, string user);


#endif
