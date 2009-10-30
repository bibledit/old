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


#ifndef INCLUDED_WINDOW_CHECK_USFM_H
#define INCLUDED_WINDOW_CHECK_USFM_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "reference.h"
#include "floatingwindow.h"


class WindowCheckUSFM : public FloatingWindow
{
public:
  WindowCheckUSFM(GtkWidget * parent_layout, GtkAccelGroup *accelerator_group, bool startup);
  virtual ~WindowCheckUSFM();
  void set_parameters (GtkTextBuffer * buffer, const ustring& project_in, unsigned int book_in, unsigned int chapter_in);
private:
  GtkWidget *vbox;
  GtkWidget *checkbutton_verses_at_start;
  GtkWidget *vbox_filter;
  GtkWidget *label_filter;
  GtkWidget *combobox_filter;
  GtkWidget *button_filter;
  GtkWidget *alignment4;
  GtkWidget *hbox6;
  GtkWidget *image4;
  GtkWidget *label10;
  GtkWidget *button_discover_markup;
  GtkWidget *alignment3;
  GtkWidget *hbox5;
  GtkWidget *image3;
  GtkWidget *label8;
  GtkWidget *hbox_information;
  GtkWidget *image5;
  GtkWidget *label_information_ok;
  GtkWidget *label_information_text;
  static void on_button_discover_markup_clicked (GtkButton *button, gpointer user_data);
  void on_button_discover_markup ();
  static void on_button_filter_clicked (GtkButton *button, gpointer user_data);
  void on_button_filter ();
  guint editors_changed_event_id;
  static bool on_editors_changed_timeout(gpointer user_data);
  void on_editors_changed();
  GtkTextBuffer * textbuffer;
  ustring project;
  ustring versification;
  unsigned int book;
  unsigned int chapter;
  vector <ustring> get_verses(vector <ustring> * non_line_starters);

};

#endif
