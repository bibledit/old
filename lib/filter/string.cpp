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


#include <filter/string.h>


// A C++ equivalent for PHP's explode function.
// Split a string on a delimiter.
// Return a vector of strings.
vector <string> filter_string_explode (string value, char delimiter)
{
  vector <string> result;
  istringstream iss (value);
  for (string token; getline (iss, token, delimiter); )
  {
    result.push_back (move (token));
  }
  return result;
}


// A C++ equivalent for PHP's implode function.
// Join a vector of string, with delimiters, into a string.
// Return this string.
string filter_string_implode (vector <string>& values, string delimiter)
{
  string full;
  for (vector<string>::iterator it = values.begin (); it != values.end (); ++it)
  {
    full += (*it);
    if (it != values.end ()-1) full += delimiter;
  }
  return full;  
}


// A C++ rough equivalent for PHP's str_replace function.
string filter_string_str_replace (string search, string replace, string subject)
{
  size_t offposition = subject.find (search);
  while (offposition != string::npos) {
    subject.replace (offposition, search.length (), replace);
    offposition = subject.find (search, offposition + replace.length ());
  }
  return subject;
}


string convert_to_string (unsigned int i)
{
  ostringstream r;
  r << i;
  return r.str();
}


string convert_to_string (int i)
{
  string s = to_string (i);
  return s;
}


string convert_to_string (char * c)
{
  string s = c;
  return s;
}


string convert_to_string (const char * c)
{
  string s = c;
  return s;
}


string convert_to_string (bool b)
{
  if (b) return "1";
  return "0";
}


string convert_to_string (string s)
{
  return s;
}


string convert_to_string (float f)
{
  ostringstream r;
  r << f;
  return r.str();
}


int convert_to_int (string s)
{
  int i = atoi (s.c_str());
  return i;
}


int convert_to_int (float f)
{
  int i = (int)round(f);
  return i;
}


long long convert_to_long_long (string s)
{
  long long i = 0;
  istringstream r (s);
  r >> i;
  return i;
}


float convert_to_float (string s)
{
  float f = 0;
  istringstream r (s);
  r >> f;
  return f;
}


bool convert_to_bool (string s)
{
  bool b;
  istringstream (s) >> b;
  return b;
}


// A C++ equivalent for PHP's in_array function.
bool filter_string_in_array (const string& needle, const vector <string>& haystack)
{
  int max = haystack.size ();
  if (max == 0) return false;
  for (int i = 0; i < max; i++) {
    if (haystack [i] == needle) return true;
  }
  return false;
}


// A C++ equivalent for PHP's array_unique function.
vector <string> filter_string_array_unique (vector <string> values)
{
  vector <string> result;
  set <string> unique;
  for (unsigned int i = 0; i < values.size (); i++) {
    if (unique.find (values[i]) == unique.end ()) {
      unique.insert (values[i]);
      result.push_back ((values[i]));
    }
  }
  return result;
}


// A C++ equivalent for PHP's array_diff function.
// Returns items in "from" which are not present in "against".
vector <string> filter_string_array_diff (vector <string> from, vector <string> against)
{
  vector <string> result;
  set <string> against2 (against.begin (), against.end ());
  for (unsigned int i = 0; i < from.size (); i++) {
    if (against2.find (from[i]) == against2.end ()) {
      result.push_back ((from[i]));
    }
  }
  return result;
}


// Gets the second within the minute from the seconds since the Unix epoch.
int filter_string_date_numerical_second (int seconds)
{
  time_t tt = seconds;
  tm utc_tm = *gmtime(&tt);
  int second = utc_tm.tm_sec;
  return second;
}


// Gets the minute within the hour from the seconds since the Unix epoch.
int filter_string_date_numerical_minute (int seconds)
{
  time_t tt = seconds;
  tm utc_tm = *gmtime(&tt);
  int minute = utc_tm.tm_min;
  return minute;
}


// Gets the hour within the day from the seconds since the Unix epoch.
int filter_string_date_numerical_hour (int seconds)
{
  time_t tt = seconds;
  tm utc_tm = *gmtime(&tt);
  int hour = utc_tm.tm_hour;
  return hour;
}


// A C++ equivalent for PHP's date ("n") function.
// Numeric representation of a month: 1 through 12.
int filter_string_date_numerical_month ()
{
  auto now = chrono::system_clock::now ();
  time_t tt = chrono::system_clock::to_time_t (now);
#ifdef WIN32
  tm utc_tm;
  gmtime_s(&utc_tm, &tt);
#else
  tm utc_tm = *gmtime(&tt);
#endif
  int month = utc_tm.tm_mon + 1;
  return month;  
}


// A C++ equivalent for PHP's date ("Y") function.
// A full numeric representation of a year, 4 digits: 2014.
int filter_string_date_numerical_year ()
{
  auto now = chrono::system_clock::now ();
  time_t tt = chrono::system_clock::to_time_t (now);
#ifdef WIN32
  tm utc_tm;
  gmtime_s(&utc_tm, &tt);
#else
  tm utc_tm = *gmtime(&tt);
#endif
  // Get years since 1900, and correct to get years since birth of Christ.
  int year = utc_tm.tm_year + 1900; 
  return year;  
}


// This function gives the number of microseconds within the current second.
int filter_string_date_numerical_microseconds ()
{
  auto now = chrono::system_clock::now ();
  auto duration = now.time_since_epoch ();
  auto microseconds = chrono::duration_cast<std::chrono::microseconds>(duration).count();
  int usecs = microseconds % 1000000;
  return usecs;
}


// This function returns the seconds since the Unix epoch, which is 1 January 1970 UTC.
int filter_string_date_seconds_since_epoch ()
{
  auto now = chrono::system_clock::now ();
  auto duration = now.time_since_epoch ();
  int seconds = (int) chrono::duration_cast<std::chrono::seconds>(duration).count();
  return seconds;
}


// A C++ equivalent for PHP's trim function.
string filter_string_trim (string s)
{
  if (s.length () == 0)
    return s;
  // Strip spaces, tabs, new lines, carriage returns.
  size_t beg = s.find_first_not_of(" \t\n\r");
  size_t end = s.find_last_not_of(" \t\n\r");
  // No non-spaces  
  if (beg == string::npos)
    return "";
  return string (s, beg, end - beg + 1);
}


// A C++ equivalent for PHP's ltrim function.
string filter_string_ltrim (string s)
{
  if (s.length () == 0) return s;
  // Strip spaces, tabs, new lines, carriage returns.
  size_t pos = s.find_first_not_of(" \t\n\r");
  // No non-spaces  
  if (pos == string::npos) return "";
  return s.substr (pos);
}


// Fills a string up to "width", with the character "fill" at the left.
string filter_string_fill (string s, int width, char fill)
{
  ostringstream str;
  str << setfill (fill) << setw (width) << s;
  return str.str();
}


// Returns true/false whether s is numeric.
bool filter_string_is_numeric (string s)
{
  for (char c : s) if (!isdigit (c)) return false;
  return true;
}


void var_dump (map <string, string> var)
{
  for (map <string, string>::const_iterator iter = var.begin(); iter != var.end(); ++iter) {
    cout << "first: " << (*iter).first << ", second: " << (*iter).second << endl;
  }
  
}


// C++ equivalent for PHP function htmlspecialchars.
string filter_string_sanitize_html (string html)
{
  html = filter_string_str_replace ("&", "&amp;", html);
  html = filter_string_str_replace ("\"", "&quot;", html);
  html = filter_string_str_replace ("'", "&apos;", html);
  html = filter_string_str_replace ("<", "&lt;", html);
  html = filter_string_str_replace (">", "&gt;", html);
  return html;
}



// Returns a soft hyphen.
string get_soft_hyphen ()
{
  // The "­" below is not an empty string, but the soft hyphen U+00AD.
  return "­";
}


// Returns a no-break space (NBSP).
string get_no_break_space ()
{
  // The space below is a no-break space.
  return " ";
}


// Returns an "en space", this is a nut, half an em space.
string get_en_space ()
{
  // The space below is U+2002.
  return " ";
}
