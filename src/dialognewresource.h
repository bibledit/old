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

#ifndef INCLUDED_DIALOGNEWRESOURCE_H
#define INCLUDED_DIALOGNEWRESOURCE_H

#include <gtk/gtk.h>
#include "ustring.h"
#include "shortcuts.h"
#include "resource_utils.h"

class NewResourceDialog
{
public:
  NewResourceDialog(const ustring& templatefile);
  ~NewResourceDialog();
private:
  // URL constructor.
  GtkWidget *image_url_constructor_ok;
  GtkWidget *label_url_constructor_ok;
  GtkWidget *label_url_constructor;
  GtkWidget *entry_url_constructor;
  static void on_entry_url_constructor_changed(GtkEditable *editable, gpointer user_data);
  void on_entry_url_constructor();
  bool url_constructor_gui();
  ustring url_constructor;

  // Index file constructor.
  GtkWidget *image_index_constructor_ok;
  GtkWidget *label_index_constructor_ok;
  GtkWidget *label_index_constructor;
  GtkWidget *entry_index_constructor;
  static void on_entry_index_constructor_changed(GtkEditable *editable, gpointer user_data);
  void on_entry_index_constructor();
  bool index_constructor_gui();
  ustring index_constructor;

  // Anchor names.
  GtkWidget *image_anchors_ok;
  GtkWidget *label_anchors_ok;
  GtkWidget *label_anchors_short;
  GtkWidget *button_anchors;
  GtkWidget *label_anchors_long;
  static void on_anchors_button_clicked(GtkButton *button, gpointer user_data);
  void on_anchors_button();
  bool anchors_gui();
  map <unsigned int, ustring> anchors;

  // Write anchors.
  GtkWidget *image_write_anchors_ok;
  GtkWidget *label_write_anchors_ok;
  GtkWidget *checkbutton_write_anchors;
  GtkWidget *button_write_anchors;
  GtkWidget *label_write_anchors;
  static void on_checkbutton_write_anchors_toggled(GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_write_anchors();
  static void on_button_write_anchors_clicked(GtkButton *button, gpointer user_data);
  void on_button_write_anchors();
  bool write_anchors_gui();
  bool write_anchors_ok;

  // Lower home page.
  GtkWidget *image_lower_home_page_ok;
  GtkWidget *label_lower_home_page_ok;
  GtkWidget *label_lower_home_page;
  GtkWidget *entry_lower_home_page;
  static void on_entry_lower_home_page_changed(GtkEditable *editable, gpointer user_data);
  void on_entry_lower_home_page();
  bool lower_home_page_gui();
  ustring lower_home_page;

  // Lower URL filter.
  GtkWidget *image_lower_url_filter_ok;
  GtkWidget *label_lower_url_filter_ok;
  GtkWidget *label_lower_url_filter;
  GtkWidget *entry_lower_url_filter;
  static void on_entry_lower_url_filter_changed(GtkEditable *editable, gpointer user_data);
  void on_entry_lower_url_filter();
  bool lower_url_filter_gui();
  ustring lower_url_filter;
};

#endif

// Todo goes out once all code has been moved to the assistant.
