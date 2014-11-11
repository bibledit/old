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


#ifndef INCLUDED_FILTER_STRING_H
#define INCLUDED_FILTER_STRING_H


#include <config/libraries.h>


vector <string> filter_string_explode (string value, char delimiter);
string filter_string_implode (vector <string>& values, string delimiter);
string filter_string_str_replace (string search, string replace, string subject);
string convert_to_string (unsigned int i);
string convert_to_string (int i);
string convert_to_string (char * c);
string convert_to_string (const char * c);
string convert_to_string (bool b);
string convert_to_string (string s);
int convert_to_int (string s);
int convert_to_int (float f);
long long convert_to_long_long (string s);
bool convert_to_bool (string s);
bool filter_string_in_array (const string& needle, const vector <string>& haystack);
vector <string> filter_string_array_unique (vector <string> values);
vector <string> filter_string_array_diff (vector <string> from, vector <string> against);
int filter_string_date_numerical_second (int seconds);
int filter_string_date_numerical_minute (int seconds);
int filter_string_date_numerical_hour (int seconds);
int filter_string_date_numerical_month ();
int filter_string_date_numerical_year ();
int filter_string_date_numerical_microseconds ();
int filter_string_date_seconds_since_epoch ();
string filter_string_trim (string s);
string filter_string_fill (string s, int width, char fill);
bool filter_string_is_numeric (string s);
void var_dump (map <string, string> var);
string filter_string_sanitize_html (string html);


#endif
