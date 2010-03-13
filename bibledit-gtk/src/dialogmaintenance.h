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
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#ifndef INCLUDED_DIALOG_MAINTENANCE_H
#define INCLUDED_DIALOG_MAINTENANCE_H


#include "libraries.h"
#include <gtk/gtk.h>
#include "ustring.h"
extern "C" {
#include <gtkhtml/gtkhtml.h>
}
#include "htmlwriter2.h"


class MaintenanceDialog
{
public:
  MaintenanceDialog (int dummy);
  ~MaintenanceDialog ();
  int run ();
protected:
  GtkWidget *dialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *scrolledwindow;
  GtkWidget *htmlview;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static gboolean on_html_link_clicked(GtkHTML *html, const gchar * url, gpointer user_data);
  void html_link_clicked(const gchar * url);
  ustring active_url;
  map <ustring, unsigned int> scrolling_position;
  static void on_html_submit (GtkHTML *html, const gchar *method, const gchar *url, const gchar *encoding, gpointer user_data);
  void html_submit (const gchar *method, const gchar *url, const gchar *encoding);
  void html_add_home (HtmlWriter2& htmlwriter);
};


#endif
