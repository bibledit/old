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


vector <string> filter_string_explode (string value, char delimiter);
string filter_string_implode (vector <string>& values, string delimiter);
string filter_string_str_replace (string search, string replace, string subject);
string filter_string_convert_to_string (unsigned int i);
string filter_string_convert_to_string (int i);
string filter_string_convert_to_string (char * c);
string filter_string_convert_to_string (bool b);
int filter_string_convert_to_int (string s);
bool filter_string_convert_to_bool (string s);
bool filter_string_in_array (const string& needle, const vector <string>& haystack);
vector <string> filter_string_array_unique (vector <string> values);
vector <string> filter_string_array_diff (vector <string> from, vector <string> against);
int filter_string_date_numerical_month ();
int filter_string_date_numerical_year ();
