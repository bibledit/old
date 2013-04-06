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


#ifndef INCLUDED_DATE_TIME_UTILS_H
#define INCLUDED_DATE_TIME_UTILS_H


#include "libraries.h"
#include <gtk/gtk.h>


int date_time_seconds_get_current ();
int date_time_julian_day_get_current ();
ustring date_time_julian_human_readable (int julian_day, bool weekday);
ustring date_time_julian_get_month_and_year (int julian_day);
ustring date_time_seconds_human_readable (int seconds, bool weekday);
int date_time_julian_day_get_parse (const ustring& date);
int date_time_julian_day_get_parse (guint year, guint month, guint day);
void date_time_normal_get_year_month_day (guint32 julian_day, guint& year, guint& month, guint& day);
void date_time_normal_get_hour_minute_second (guint32 seconds, int& hour, int& minute, int& second);
int date_time_seconds_to_julian (int seconds);
int date_time_julian_to_seconds (int day);


#endif
