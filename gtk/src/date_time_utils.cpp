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


#include "libraries.h"
#include "utilities.h"
#include <glib.h>
#include "date_time_utils.h"
#include "tiny_utilities.h"


int date_time_seconds_get_current()
// Get the seconds since the Epoch, modified with the timezone information.
// Special code is used that works not only on Linux but also on BSD.
{
  // Get seconds elapsed since Epoch: 00:00:00 1 January 1970 UTC.
  GTimeVal gtimeval;
  g_get_current_time(&gtimeval);

  // Get both local and UCT time data.  
  time_t t_t = time(0);
  tm *tm = localtime(&t_t);
  int local_year = tm->tm_year;
  int local_day = tm->tm_yday;
  int local_hour = tm->tm_hour;
  int local_minute = tm->tm_min;
  tm = gmtime(&t_t);
  int gm_year = tm->tm_year;
  int gm_day = tm->tm_yday;
  int gm_hour = tm->tm_hour;
  int gm_minute = tm->tm_min;

  // Get the local minutes, since 1900.
  // Handle leap years.
  int local_time = local_year * 365;
  if (g_date_is_leap_year(local_year + 1900))
    local_time++;
  local_time += local_day;
  local_time *= 24;
  local_time += local_hour;
  local_time *= 60;
  local_time += local_minute;

  // Get the minutes according to UCT, which is Greenwich Mean Time.
  // Handle leap years.
  int gm_time = gm_year * 365;
  if (g_date_is_leap_year(gm_year + 1900))
    gm_time++;
  gm_time += gm_day;
  gm_time *= 24;
  gm_time += gm_hour;
  gm_time *= 60;
  gm_time += gm_minute;

  // Get the difference, in seconds;
  int difference = gm_time - local_time;
  difference *= 60;

  // Modify the seconds with this difference.
  gtimeval.tv_sec += (0 - difference);

  // Result. 
  return gtimeval.tv_sec;
}


int date_time_julian_day_get_current()
// Returns the julian day for today.
{
  // New GDate using the current date.
  GDate *date = g_date_new();
  g_date_set_time_t (date, date_time_seconds_get_current ());
  // Get the Julian day out of it.
  int julian_day = g_date_get_julian(date);
  // Free memory.
  g_date_free(date);
  // Return Julian day.
  return julian_day;
}


ustring date_time_julian_human_readable(int julian_day, bool weekday)
// Returns a Julian day in human readable form.
{
  ustring returnvalue;
  GDate *date;
  date = g_date_new_julian(julian_day);
  gchar buf[1024];
  if (weekday)
    g_date_strftime(buf, 1000, "%A, %e %B %Y", date);
  else
    g_date_strftime(buf, 1000, "%e %B %Y", date);
  returnvalue = buf;
  g_date_free(date);
  return returnvalue;
}


ustring date_time_julian_get_month_and_year(int julian_day)
// Returns the month of a Julian day in human readable form.
{
  ustring returnvalue;
  GDate *date;
  date = g_date_new_julian(julian_day);
  gchar buf[1024];
  g_date_strftime(buf, 1000, "%B %Y", date);
  returnvalue = buf;
  g_date_free(date);
  return returnvalue;
}


ustring date_time_seconds_human_readable(int seconds, bool weekday)
// Returns the seconds in human readable format.
{
  // The day, monty and year.
  int julian_day = date_time_seconds_to_julian(seconds);
  ustring returnvalue = date_time_julian_human_readable(julian_day, weekday);
  int hour, minute, second;
  date_time_normal_get_hour_minute_second(seconds, hour, minute, second);
  ustring uhour, uminute, usecond;
  uhour = convert_to_string(hour);
  if (uhour.length() == 1)
    uhour.insert(0, "0");
  uminute = convert_to_string(minute);
  if (uminute.length() == 1)
    uminute.insert(0, "0");
  usecond = convert_to_string(second);
  if (usecond.length() == 1)
    usecond.insert(0, "0");
  returnvalue.append(", " + uhour + ":" + uminute + ":" + usecond);
  return returnvalue;
}


int date_time_julian_day_get_parse(const ustring & date)
// This takes a human readable date, and tries to get the Julian day out of it.
{
  // Trim superfluous characters off.
  ustring cleaned_date = trim(date);
  // If the date has a time part, cut that off.
  size_t offposition = cleaned_date.find(":");
  if (offposition != string::npos) {
    cleaned_date.erase(offposition, cleaned_date.length() - offposition);
    offposition = cleaned_date.find_last_of(" ");
    if (offposition != string::npos) {
      cleaned_date.erase(offposition, cleaned_date.length() - offposition);
    }
  }
  // New GDate object.
  GDate *gdate;
  gdate = g_date_new();
  // Parse the date.
  g_date_set_parse(gdate, cleaned_date.c_str());
  // Julian day to return.
  int julian_day;
  // Validate it. 
  if (g_date_valid(gdate)) {
    julian_day = g_date_get_julian(gdate);
  } else {
    // Not valid, so take today's Julian day.
    julian_day = date_time_julian_day_get_current();
  }
  // Free memory.
  g_date_free(gdate);
  // Return result
  return julian_day;
}


int date_time_julian_day_get_parse(guint year, guint month, guint day)
{
  // New GDate object.
  GDate *gdate;
  gdate = g_date_new();
  // Parse the date.
  g_date_set_day(gdate, day);
  g_date_set_month(gdate, GDateMonth(month));
  g_date_set_year(gdate, year);
  // Julian day to return.
  int julian_day;
  // Validate it. 
  if (g_date_valid(gdate)) {
    julian_day = g_date_get_julian(gdate);
  } else {
    // Not valid, so take today's Julian day.
    julian_day = date_time_julian_day_get_current();
  }
  // Free memory.
  g_date_free(gdate);
  // Return result
  return julian_day;
}


void date_time_normal_get_year_month_day(guint32 julian_day, guint & year, guint & month, guint & day)
{
  // New GDate object.
  GDate *gdate;
  gdate = g_date_new();
  // Set Julian day.
  g_date_set_julian(gdate, julian_day);
  // Return results.
  year = g_date_get_year(gdate);
  month = g_date_get_month(gdate);
  day = g_date_get_day(gdate);
  // Free memory.
  g_date_free(gdate);
}


void date_time_normal_get_hour_minute_second(guint32 seconds, int &hour, int &minute, int &second)
{
  unsigned int seconds_in_day = seconds % 86400;
  hour = seconds_in_day / 3600;
  unsigned int remaining_seconds = seconds_in_day - (3600 * hour);
  minute = remaining_seconds / 60;
  second = remaining_seconds % 60;
}


int date_time_seconds_to_julian(int seconds)
// Converts seconds since Epoch to a Julian day.
{
  GTime time = seconds;
  GDate *date;
  date = g_date_new();
  g_date_set_time_t (date, time);
  int julian_day = g_date_get_julian(date);
  g_date_free(date);
  return julian_day;
}


int date_time_julian_to_seconds(int day)
// Converts a Julian day to seconds since Epoch.
{
  // Our date, from the Julian days.
  GDate *date;
  date = g_date_new_julian(day);
  // Date of the Epoch.
  GDate *epoch;
  epoch = g_date_new_dmy(1, G_DATE_JANUARY, 1970);
  // Days in between.
  int days = g_date_days_between(epoch, date);
  // Free memory.
  g_date_free(date);
  g_date_free(epoch);
  // Seconds since Epoch.  
  int seconds = days * 86400;
  return seconds;
}

