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


#ifndef INCLUDED_WINDOW_STYLES_H
#define INCLUDED_WINDOW_STYLES_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "floatingwindow.h"
#include "style.h"
#include "stylesheetutils.h"


class WindowStyles : public FloatingWindow
{
public:
  WindowStyles(GtkWidget * parent_layout, GtkAccelGroup *accelerator_group, bool startup, GtkWidget *stl, GtkWidget *stl_menu, GtkWidget *stl_expand_all, GtkWidget *stl_collapse_all, GtkWidget *stl_insert, GtkWidget *stl_edit_mode, GtkWidget *stl_new, GtkWidget *stl_properties, GtkWidget *stl_delete, GtkWidget *stlsheet_switch, GtkWidget *stlsheet_new, GtkWidget *stlsheet_delete, GtkWidget *stlsheet_rename);
  virtual ~WindowStyles();
  void load(const ustring& stylesheet);
  GtkWidget *apply_signal;
  GtkWidget *open_signal;
  GtkWidget *edited_signal;
  GtkWidget *treeview;
  ustring get_focus();
  void use(const ustring& marker);
  ustring get_sheet();
  void export_sheet(const ustring& filename, StylesheetExportFormatType format);
  void on_stylesheet_import();
private:
  GtkWidget *vbox;

  // Menu widgets.
  GtkWidget *style;
  GtkWidget *style_menu;
  GtkWidget *style_expand_all;
  GtkWidget *style_collapse_all;
  GtkWidget *style_insert;
  GtkWidget *style_edit_mode;
  GtkWidget *style_new;
  GtkWidget *style_properties;
  GtkWidget *style_delete;
  GtkWidget *stylesheet_switch;
  GtkWidget *stylesheet_new;
  GtkWidget *stylesheet_remove;
  GtkWidget *stylesheet_rename;

  GtkWidget *scrolledwindow;

  // Styles treeview.
  GtkTreeStore *treestore;
  GtkTreeViewColumn *treecolumn;
  GtkTreeSelection *treeselect;

  // Data
  ustring mystylesheet;
  vector <ustring> allrecentmarkers;
  map <ustring,unsigned int> usagecounters;
  vector<bool> expanded_states;

  // Menu callbacks.
  guint styles_menu_handler_id;
  static void on_style_menu_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_style_menu();
  guint expand_all_handler_id;
  static void on_expand_all_activate(GtkMenuItem *menuitem, gpointer user_data);
  void expand_all();
  guint collapse_all_handler_id;
  static void on_collapse_all_activate(GtkMenuItem *menuitem, gpointer user_data);
  void collapse_all();
  bool styletree_expand_collapse(bool expand);
  static void on_treeview_styles_row_collapsed(GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data);
  static void on_treeview_styles_row_expanded(GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data);
  void on_treeview_styles_collapsed_expanded(GtkTreeIter *iter, GtkTreePath *path, bool expand);
  guint new_handler_id;
  static void on_style_new_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_style_new();
  guint properties_handler_id;
  static void on_style_properties_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_style_properties();
  guint delete_handler_id;
  static void on_style_delete_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_style_delete();
  void delete_categories(vector<ustring> categories);
  guint insert_handler_id;
  static void on_style_insert_activate(GtkMenuItem *menuitem, gpointer user_data);
  guint switch_stylesheet_handler_id;
  static void on_stylesheet_switch_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_stylesheet_switch();
  guint new_stylesheet_handler_id;
  static void on_stylesheet_new_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_stylesheet_new();
  guint delete_stylesheet_handler_id;
  static void on_stylesheet_delete_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_stylesheet_delete();
  guint rename_stylesheet_handler_id;
  static void on_stylesheet_rename_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_stylesheet_rename();

  // Menu popup.
  static gboolean on_popup_menu(GtkWidget *widget, gpointer user_data);
  void popup_menu(GtkWidget *widget);
  void show_styles_popup_menu(GtkWidget *my_widget, GdkEventButton *event);

  // Styletree callbacks.
  static gboolean on_key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data);
  static gboolean on_button_press_event(GtkWidget * widget, GdkEventButton * event, gpointer user_data);
  bool on_button_press(GtkWidget * widget, GdkEventButton * event);
  static void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  void row_activated();

  // Supporting functions.
  size_t get_expanded_state_offset(const ustring& row);
  bool get_expanded_state(const ustring& row);
  void stylesheet_open(const ustring& stylesheet);
  void reload();
  void insert();
  void load_recently_used_styles();
  vector<ustring> get_focused_regular_styles();
  vector<ustring> get_focused_recently_used_styles();
  vector<ustring> get_focused_categories();
  ustring get_string_before_focus();
  ustring get_focused_style();
  void focus_string(const ustring& string);
  void focus_iter(GtkTreeIter * iter, bool expand);
  void use_style(const ustring& style);
  void clear_recently_used_styles();
  void clear_recently_used_style(const ustring& style);
  void load_stylesheet();
  void get_focused_strings(vector<ustring>& focused_strings, vector<bool>& recently_used, vector<bool>& categories);
  ustring get_name(const ustring& marker);
  void retrieve_recently_used_data();
  void process_recently_used_data();
  void store_recently_used_data();

  // Delayed load.
  static bool on_load_timeout(gpointer data);
};


#endif
