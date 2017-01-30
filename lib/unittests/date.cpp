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


#include <unittests/date.h>
#include <unittests/utilities.h>
#include <filter/date.h>
#include <filter/string.h>


void test_date ()
{
  trace_unit_tests (__func__);
  {
    // Test the date and time related functions.
    int month = filter_date_numerical_month (filter_date_seconds_since_epoch ());
    if ((month < 1) || (month > 12)) evaluate (__LINE__, __func__, "current month", convert_to_string (month));
    int year = filter_date_numerical_year (filter_date_seconds_since_epoch ());
    if ((year < 2014) || (year > 2050)) evaluate (__LINE__, __func__, "current year", convert_to_string (year));
    struct timeval tv;
    gettimeofday (&tv, NULL);
    int reference_second = tv.tv_sec;
    int actual_second = filter_date_seconds_since_epoch ();
    if (abs (actual_second - reference_second) > 1) evaluate (__LINE__, __func__, reference_second, actual_second);
    int usecs = filter_date_numerical_microseconds ();
    if ((usecs < 0) || (usecs > 1000000)) evaluate (__LINE__, __func__, "0-1000000", convert_to_string (usecs));
  }
}
