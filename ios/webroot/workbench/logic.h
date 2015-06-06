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


#ifndef INCLUDED_WORKSPACE_LOGIC_H
#define INCLUDED_WORKSPACE_LOGIC_H


#include <config/libraries.h>


map <int, string> workbenchDefaultURLs (int id);
map <int, string> workbenchDefaultWidths (int id);
map <int, string> workbenchDefaultHeights (int id);
string workbenchGetActiveWorkbench (void * webserver_request);
string workbenchProcessUnits (string length);
void workbenchSetValues (void * webserver_request, int selector, const map <int, string> & values);
void workbenchSetURLs (void * webserver_request, const map <int, string> & values);
void workbenchSetWidths (void * webserver_request, const map <int, string> & values);
void workbenchSetHeights (void * webserver_request, const map <int, string> & values);
map <int, string> workbenchGetValues (void * webserver_request, int selector, bool use);
map <int, string> workbenchGetURLs (void * webserver_request, bool use);
map <int, string> workbenchGetWidths (void * webserver_request);
map <int, string> workbenchGetHeights (void * webserver_request);
vector <string> workbenchGetWorkbenches (void * webserver_request);
void workbenchDeleteWorkbench (void * webserver_request, string workbench);
void workbenchOrderWorkbenches (void * webserver_request, const vector <string> & workbenches);


#endif
