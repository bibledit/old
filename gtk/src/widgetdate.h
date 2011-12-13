/*
** Copyright (©) 2003-2012 Teus Benschop.
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


#ifndef INCLUDED_WIDGET_DATE_H
#define INCLUDED_WIDGET_DATE_H


#include <gtk/gtk.h>


class DateWidget
{
public:
  DateWidget (guint32 * seconds_since_epoch, bool showtime = false);
  ~DateWidget ();
  GtkWidget *hbox;
  void set_date ();
private:
  GtkWidget *vbox2;
  GtkWidget *label_date;
  GtkWidget *calendar1;
  GtkWidget *vseparator1;
  GtkWidget *vbox1;
  GtkWidget *label_time;
  GtkWidget *table1;
  GtkWidget *label_hour;
  GtkWidget *label_minute;
  GtkWidget *label_second;
  GtkObject *spinbutton_minute_adj;
  GtkWidget *spinbutton_minute;
  GtkObject *spinbutton_second_adj;
  GtkWidget *spinbutton_second;
  GtkObject *spinbutton_hour_adj;
  GtkWidget *spinbutton_hour;
  guint * my_seconds_since_epoch;
  static void on_spinbutton_changed (GtkEditable *editable, gpointer user_data);
  static void on_spinbutton_value_changed (GtkSpinButton *spinbutton, gpointer user_data);
  static void on_calendar_changed (GtkCalendar *calendar, gpointer user_data);
  void calculate_date_time ();
  bool setting_date_time;
};


#endif
