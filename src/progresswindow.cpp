/*
** Copyright (©) 2003-2009 Teus Benschop.
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/

#include "progresswindow.h"
#include "gwrappers.h"
#include "directories.h"

ProgressWindow::ProgressWindow(const ustring & info, bool showcancel)
{
  progresswindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(progresswindow), "Bibledit");
  gtk_window_set_position(GTK_WINDOW(progresswindow), GTK_WIN_POS_CENTER_ALWAYS);
  gtk_window_set_default_size(GTK_WINDOW(progresswindow), 300, -1);
  gtk_window_set_default_icon_from_file(gw_build_filename(directories_get_package_data(), "bibledit.xpm").c_str(), NULL);
  gtk_window_set_type_hint(GTK_WINDOW(progresswindow), GDK_WINDOW_TYPE_HINT_DIALOG);

  // The window is set modal to turn off any responses to the parent window.
  // That means that clicking "Cancel" will get handled properly.
  gtk_window_set_modal(GTK_WINDOW(progresswindow), true);

  vbox3 = gtk_vbox_new(FALSE, 10);
  gtk_widget_show(vbox3);
  gtk_container_add(GTK_CONTAINER(progresswindow), vbox3);
  gtk_container_set_border_width(GTK_CONTAINER(vbox3), 10);

  infolabel = gtk_label_new(info.c_str());
  gtk_widget_show(infolabel);
  gtk_box_pack_start(GTK_BOX(vbox3), infolabel, FALSE, FALSE, 0);

  progressbar = gtk_progress_bar_new();
  gtk_widget_show(progressbar);
  gtk_box_pack_start(GTK_BOX(vbox3), progressbar, FALSE, FALSE, 0);

  hbox6 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox6);
  gtk_box_pack_start(GTK_BOX(vbox3), hbox6, FALSE, FALSE, 0);

  if (showcancel) {

    cancelbutton = gtk_button_new_from_stock("gtk-cancel");
    gtk_widget_show(cancelbutton);
    gtk_box_pack_start(GTK_BOX(hbox6), cancelbutton, FALSE, FALSE, 0);
    GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

    g_signal_connect((gpointer) cancelbutton, "clicked", G_CALLBACK(on_cancelbutton_clicked), gpointer(this));
    gtk_widget_grab_focus(cancelbutton);
    gtk_widget_grab_default(cancelbutton);

  }

  g_signal_connect((gpointer) progresswindow, "delete_event", G_CALLBACK(on_delete_event), gpointer(this));

  gtk_widget_show(progresswindow);
  gtk_window_present(GTK_WINDOW(progresswindow));

  cancel = false;

  gui();
}

ProgressWindow::~ProgressWindow()
{
  gtk_widget_destroy(progresswindow);
}

void ProgressWindow::on_cancelbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ProgressWindow *) user_data)->on_cancel();
}

void ProgressWindow::on_cancel()
{
  cancel = true;
  gui();
}

gboolean ProgressWindow::on_delete_event(GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
  // Window cannot be deleted.
  return true;
}

void ProgressWindow::gui()
{
  // Update the GUI.
  while (gtk_events_pending()) {
    gtk_main_iteration();
  }
}

void ProgressWindow::set_text(const ustring & text)
{
  gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar), text.c_str());
  gui();
}

void ProgressWindow::set_fraction(double fraction)
{
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar), fraction);
  gui();
}

void ProgressWindow::set_iterate(double minimum, double maximum, unsigned int max_iteration)
{
  my_minimum = minimum;
  my_maximum = maximum;
  modulus = 0;
  if (max_iteration > 300)
    modulus = max_iteration / 100;
  my_max_iterations = max_iteration;
  iteration_counter = 0;
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar), my_minimum);
  gui();
}

void ProgressWindow::iterate()
{
  iteration_counter++;
  // It appears that if there are many steps to take, and so we have many 
  // iterations in the process, the whole process is slowed down mostly by the
  // update of the graphical user interface. As this is not desirable,
  // measures have been taken in the ProgressWindow object to automatically reduce
  // the number of iterations depending on how many there are going to be
  // in the process. This can result in something going more than ten (!) 
  // times faster.
  if (modulus != 0) {
    if ((iteration_counter % modulus) != 0)
      return;
  }
  double percentage;
  percentage = double (iteration_counter) / double (my_max_iterations);
  percentage *= (my_maximum - my_minimum);
  percentage += my_minimum;
  // If percentage gets too high, reset the bar.
  if (percentage > 1)
    iteration_counter = 0;
  percentage = CLAMP(percentage, 0, 1);
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar), percentage);
  gui();
}

void ProgressWindow::pulse()
{
  gtk_progress_bar_pulse(GTK_PROGRESS_BAR(progressbar));
  gui();
}

void ProgressWindow::hide()
{
  gtk_widget_hide_all(progresswindow);
  gui();
}
