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

#include "libraries.h"
#include <glib.h>
#include "widgetdate.h"
#include "date_time_utils.h"
#include "shortcuts.h"

DateWidget::DateWidget(guint32 * seconds_since_epoch, bool showtime)
/*
By default this dialog shows the calendar only.
If showtime is true it shows the time also.
*/
{
  // Store and initialize variabeles.
  my_seconds_since_epoch = seconds_since_epoch;
  setting_date_time = false;

  // Shortcuts.
  Shortcuts shortcuts(0);

  hbox = gtk_hbox_new(FALSE, 10);
  gtk_widget_show(hbox);

  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox2);
  gtk_box_pack_start(GTK_BOX(hbox), vbox2, TRUE, TRUE, 0);

  label_date = gtk_label_new("Date");
  gtk_widget_show(label_date);
  gtk_box_pack_start(GTK_BOX(vbox2), label_date, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_date), 0, 0.5);

  shortcuts.label(label_date);

  calendar1 = gtk_calendar_new();
  gtk_widget_show(calendar1);
  gtk_box_pack_start(GTK_BOX(vbox2), calendar1, TRUE, TRUE, 0);
  gtk_calendar_display_options(GTK_CALENDAR(calendar1), GtkCalendarDisplayOptions(GTK_CALENDAR_SHOW_HEADING | GTK_CALENDAR_SHOW_DAY_NAMES | GTK_CALENDAR_SHOW_WEEK_NUMBERS));

  label_time = NULL;
  if (showtime) {

    vseparator1 = gtk_vseparator_new();
    gtk_widget_show(vseparator1);
    gtk_box_pack_start(GTK_BOX(hbox), vseparator1, TRUE, TRUE, 0);

    vbox1 = gtk_vbox_new(FALSE, 4);
    gtk_widget_show(vbox1);
    gtk_box_pack_start(GTK_BOX(hbox), vbox1, TRUE, TRUE, 0);

    label_time = gtk_label_new("Time");
    gtk_widget_show(label_time);
    gtk_box_pack_start(GTK_BOX(vbox1), label_time, FALSE, FALSE, 0);
    gtk_misc_set_alignment(GTK_MISC(label_time), 0, 0.5);

    table1 = gtk_table_new(3, 2, FALSE);
    gtk_widget_show(table1);
    gtk_box_pack_start(GTK_BOX(vbox1), table1, FALSE, FALSE, 0);
    gtk_table_set_row_spacings(GTK_TABLE(table1), 8);
    gtk_table_set_col_spacings(GTK_TABLE(table1), 8);

    label_hour = gtk_label_new("Hour");
    gtk_widget_show(label_hour);
    gtk_table_attach(GTK_TABLE(table1), label_hour, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(label_hour), 1, 0.5);

    shortcuts.label(label_hour);

    label_minute = gtk_label_new("Minute");
    gtk_widget_show(label_minute);
    gtk_table_attach(GTK_TABLE(table1), label_minute, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(label_minute), 1, 0.5);

    shortcuts.label(label_minute);

    label_second = gtk_label_new("Second");
    gtk_widget_show(label_second);
    gtk_table_attach(GTK_TABLE(table1), label_second, 0, 1, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(label_second), 1, 0.5);

    shortcuts.label(label_second);

    spinbutton_minute_adj = gtk_adjustment_new(0, 0, 59, 1, 10, 0);
    spinbutton_minute = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_minute_adj), 1, 0);
    gtk_widget_show(spinbutton_minute);
    gtk_table_attach(GTK_TABLE(table1), spinbutton_minute, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_minute), TRUE);

    spinbutton_second_adj = gtk_adjustment_new(0, 0, 59, 1, 10, 0);
    spinbutton_second = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_second_adj), 1, 0);
    gtk_widget_show(spinbutton_second);
    gtk_table_attach(GTK_TABLE(table1), spinbutton_second, 1, 2, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_second), TRUE);

    spinbutton_hour_adj = gtk_adjustment_new(0, 0, 23, 1, 10, 0);
    spinbutton_hour = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_hour_adj), 1, 0);
    gtk_widget_show(spinbutton_hour);
    gtk_table_attach(GTK_TABLE(table1), spinbutton_hour, 1, 2, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_hour), TRUE);

  }

  shortcuts.process();
  
  g_signal_connect ((gpointer) calendar1, "day_selected", G_CALLBACK (on_calendar_changed), gpointer(this));
  if (showtime) {
    g_signal_connect ((gpointer) spinbutton_minute, "changed", G_CALLBACK (on_spinbutton_changed), gpointer(this));
    g_signal_connect ((gpointer) spinbutton_minute, "value_changed", G_CALLBACK (on_spinbutton_value_changed), gpointer(this));
    g_signal_connect ((gpointer) spinbutton_second, "changed", G_CALLBACK (on_spinbutton_changed), gpointer(this));
    g_signal_connect ((gpointer) spinbutton_second, "value_changed", G_CALLBACK (on_spinbutton_value_changed), gpointer(this));
    g_signal_connect ((gpointer) spinbutton_hour, "changed", G_CALLBACK (on_spinbutton_changed), gpointer(this));
    g_signal_connect ((gpointer) spinbutton_hour, "value_changed", G_CALLBACK (on_spinbutton_value_changed), gpointer(this));
  }

  gtk_label_set_mnemonic_widget(GTK_LABEL(label_date), calendar1);
  if (showtime) {
    gtk_label_set_mnemonic_widget(GTK_LABEL(label_hour), spinbutton_hour);
    gtk_label_set_mnemonic_widget(GTK_LABEL(label_minute), spinbutton_minute);
    gtk_label_set_mnemonic_widget(GTK_LABEL(label_second), spinbutton_second);
  }

  // Set the date and optionally the time.
  set_date ();
}

DateWidget::~DateWidget()
{
  // Normally an object would destroy the top widget,
  // but since this widget is going to be part of another parent,
  // destruction is taken care of by that parent.
  // gtk_widget_destroy(hbox);
}

void DateWidget::set_date ()
{
  // Flags.
  setting_date_time = true;
  
  // Set the date.
  guint year, month, day;
  date_time_normal_get_year_month_day(date_time_seconds_to_julian(*my_seconds_since_epoch), year, month, day);
  gtk_calendar_select_month(GTK_CALENDAR(calendar1), month - 1, year);
  gtk_calendar_select_day(GTK_CALENDAR(calendar1), day);

  // And the time.
  if (label_time) {
    unsigned int seconds_in_day = (*my_seconds_since_epoch) % 86400;
    unsigned int hour = seconds_in_day / 3600;
    unsigned int remaining_seconds = seconds_in_day - (3600 * hour);
    unsigned int minute = remaining_seconds / 60;
    unsigned int second = remaining_seconds % 60;
    gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton_minute), minute);
    gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton_second), second);
    gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton_hour), hour);
  }
  
  // Flags down.
  setting_date_time = false;
}


void DateWidget::on_spinbutton_changed (GtkEditable *editable, gpointer user_data)
{
  ((DateWidget *) user_data)->calculate_date_time();
}


void DateWidget::on_spinbutton_value_changed (GtkSpinButton *spinbutton, gpointer user_data)
{
  ((DateWidget *) user_data)->calculate_date_time();
}


void DateWidget::on_calendar_changed (GtkCalendar *calendar, gpointer user_data)
{
  ((DateWidget *) user_data)->calculate_date_time();
}


void DateWidget::calculate_date_time()
{
  // There is a flag that indicates we are setting the widgets.
  // Setting the widgets gives a lot of signals, which disturbs the whole process.
  // Therefore when setting the widgets, these signals do nothing.
  if (setting_date_time) 
    return;
  // Calculation.
  guint year;
  guint month;
  guint day;
  gtk_calendar_get_date(GTK_CALENDAR(calendar1), &year, &month, &day);
  month++;
  *my_seconds_since_epoch = date_time_julian_to_seconds(date_time_julian_day_get_parse(year, month, day));
  if (label_time) {
    unsigned int extra_seconds = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton_second));
    extra_seconds += (60 * gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton_minute)));
    extra_seconds += (3600 * gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton_hour)));
    *my_seconds_since_epoch += extra_seconds;
  }
}
