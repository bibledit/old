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
#include <glib.h>
#include "windowstyles.h"
#include "help.h"
#include "floatingwindow.h"
#include "tiny_utilities.h"
#include "utilities.h"
#include "combobox.h"
#include "settings.h"
#include "projectutils.h"
#include "stylesheetutils.h"
#include "keyboard.h"
#include "dialognewstylesheet.h"
#include "gtkwrappers.h"
#include "dialogstylesheet.h"
#include "dialogopenstylesheet.h"
#include "usfmtools.h"
#include "dialogentry.h"
#include "projectutils.h"
#include "gwrappers.h"
#include "styles.h"
#include <glib/gi18n.h>

WindowStyles::WindowStyles(GtkWidget * parent_layout, GtkAccelGroup *accelerator_group, bool startup, GtkWidget *stl, GtkWidget *stl_menu, GtkWidget *stl_expand_all, GtkWidget *stl_collapse_all, GtkWidget *stl_insert, GtkWidget *stl_edit_mode, GtkWidget *stl_new, GtkWidget *stl_properties, GtkWidget *stl_delete, GtkWidget *stlsheet_switch, GtkWidget *stlsheet_new, GtkWidget *stlsheet_delete, GtkWidget *stlsheet_rename):
FloatingWindow(parent_layout, widStyles, "Stylesheet", startup)
// Styles window.
{
  // Variables.
  style = stl;
  style_menu = stl_menu;
  style_expand_all = stl_expand_all;
  style_collapse_all = stl_collapse_all;
  style_insert = stl_insert;
  style_edit_mode = stl_edit_mode;
  style_new = stl_new;
  style_properties = stl_properties;
  style_delete = stl_delete;
  stylesheet_switch = stlsheet_switch;
  stylesheet_new = stlsheet_new;
  stylesheet_remove = stlsheet_delete;
  stylesheet_rename = stlsheet_rename;

  // Signalling buttons.
  apply_signal = gtk_button_new();
  open_signal = gtk_button_new();
  edited_signal = gtk_button_new();

  // GUI proper.
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  gtk_container_add(GTK_CONTAINER(vbox_client), vbox);

  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  treestore = gtk_tree_store_new(1, G_TYPE_STRING);

  treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(treestore));
  gtk_widget_show(treeview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), treeview);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), false);

  connect_focus_signals (treeview);

  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

  treecolumn = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), treecolumn);

  treeselect = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  gtk_tree_selection_set_mode(treeselect, GTK_SELECTION_MULTIPLE);

  styles_menu_handler_id = 0;
  expand_all_handler_id = 0;
  collapse_all_handler_id = 0;
  new_handler_id = 0;
  properties_handler_id = 0;
  delete_handler_id = 0;
  insert_handler_id = 0;
  switch_stylesheet_handler_id = 0;
  new_stylesheet_handler_id = 0;
  delete_stylesheet_handler_id = 0;
  rename_stylesheet_handler_id = 0;
  if (style)
    styles_menu_handler_id = g_signal_connect((gpointer) style, "activate", G_CALLBACK(on_style_menu_activate), gpointer(this));
  if (style_expand_all)
    expand_all_handler_id = g_signal_connect((gpointer) style_expand_all, "activate", G_CALLBACK(on_expand_all_activate), gpointer(this));
  if (style_collapse_all)
    collapse_all_handler_id = g_signal_connect((gpointer) style_collapse_all, "activate", G_CALLBACK(on_collapse_all_activate), gpointer(this));
  if (style_new)
    new_handler_id = g_signal_connect((gpointer) style_new, "activate", G_CALLBACK(on_style_new_activate), gpointer(this));
  if (style_properties)
    properties_handler_id = g_signal_connect((gpointer) style_properties, "activate", G_CALLBACK(on_style_properties_activate), gpointer(this));
  if (style_delete)
    delete_handler_id = g_signal_connect((gpointer) style_delete, "activate", G_CALLBACK(on_style_delete_activate), gpointer(this));
  if (style_insert)
    insert_handler_id = g_signal_connect((gpointer) style_insert, "activate", G_CALLBACK(on_style_insert_activate), gpointer(this));
  if (stylesheet_switch)
    switch_stylesheet_handler_id = g_signal_connect((gpointer) stylesheet_switch, "activate", G_CALLBACK(on_stylesheet_switch_activate), gpointer(this));
  if (stylesheet_new)
    new_stylesheet_handler_id = g_signal_connect((gpointer) stylesheet_new, "activate", G_CALLBACK(on_stylesheet_new_activate), gpointer(this));
  if (stylesheet_remove)
    delete_stylesheet_handler_id = g_signal_connect((gpointer) stylesheet_remove, "activate", G_CALLBACK(on_stylesheet_delete_activate), gpointer(this));
  if (stylesheet_rename)
    rename_stylesheet_handler_id = g_signal_connect((gpointer) stylesheet_rename, "activate", G_CALLBACK(on_stylesheet_rename_activate), gpointer(this));

  g_signal_connect((gpointer) treeview, "key_press_event", G_CALLBACK(on_key_press_event), gpointer(this));
  g_signal_connect((gpointer) treeview, "button_press_event", G_CALLBACK(on_button_press_event), gpointer(this));
  g_signal_connect((gpointer) treeview, "popup_menu", G_CALLBACK(on_popup_menu), gpointer(this));
  g_signal_connect((gpointer) treeview, "row_collapsed", G_CALLBACK(on_treeview_styles_row_collapsed), gpointer(this));
  g_signal_connect((gpointer) treeview, "row_expanded", G_CALLBACK(on_treeview_styles_row_expanded), gpointer(this));
  g_signal_connect((gpointer) treeview, "row_activated", G_CALLBACK(on_row_activated), gpointer(this));

  // Load the expanded states from genconfig. Cater for 20 states.
  // Keep them in memory for higher speed.
  extern Settings *settings;
  expanded_states = settings->genconfig.styles_category_expanded_get();
  if (expanded_states.size() < 20) {
    unsigned int add = 20 - expanded_states.size();
    for (unsigned int i = 0; i < add; i++)
      expanded_states.push_back(false);
  }
  
  // Main focused widget.
  last_focused_widget = treeview;
  gtk_widget_grab_focus (last_focused_widget);
}


WindowStyles::~WindowStyles()
{
  // Save the expanded states to the configuration.
  extern Settings *settings;
  settings->genconfig.styles_category_expanded_set(expanded_states);
  // Save the style usage data.
  store_recently_used_data();
  // Destroy signal buttons.
  gtk_widget_destroy(apply_signal);
  gtk_widget_destroy(open_signal);
  gtk_widget_destroy(edited_signal);
  // Disconnect signals manually as the widgets they are now connected to are not destroyed at this stage.
  if (styles_menu_handler_id)
    g_signal_handler_disconnect((gpointer) style, styles_menu_handler_id);
  if (expand_all_handler_id)
    g_signal_handler_disconnect((gpointer) style_expand_all, expand_all_handler_id);
  if (collapse_all_handler_id)
    g_signal_handler_disconnect((gpointer) style_collapse_all, collapse_all_handler_id);
  if (new_handler_id)
    g_signal_handler_disconnect((gpointer) style_new, new_handler_id);
  if (properties_handler_id)
    g_signal_handler_disconnect((gpointer) style_properties, properties_handler_id);
  if (delete_handler_id)
    g_signal_handler_disconnect((gpointer) style_delete, delete_handler_id);
  if (insert_handler_id)
    g_signal_handler_disconnect((gpointer) style_insert, insert_handler_id);
  if (switch_stylesheet_handler_id)
    g_signal_handler_disconnect((gpointer) stylesheet_switch, switch_stylesheet_handler_id);
  if (new_stylesheet_handler_id)
    g_signal_handler_disconnect((gpointer) stylesheet_new, new_stylesheet_handler_id);
  if (delete_stylesheet_handler_id)
    g_signal_handler_disconnect((gpointer) stylesheet_remove, delete_stylesheet_handler_id);
  if (rename_stylesheet_handler_id)
    g_signal_handler_disconnect((gpointer) stylesheet_rename, rename_stylesheet_handler_id);
}


void WindowStyles::load(const ustring & stylesheet)
{
  // Only load a new sheet if there's a change.
  if (stylesheet == mystylesheet)
    return;
  // If there was a previous stylesheet, save its usage data.
  if (!mystylesheet.empty())
    store_recently_used_data();
  // Save new sheet
  mystylesheet = stylesheet;
  // The actual loading is done with a delay, to make it easier for Gtk.
  g_timeout_add(500, GSourceFunc(on_load_timeout), gpointer(this));
  // Set the name of the sheet.
  status1 (mystylesheet);
}


bool WindowStyles::on_load_timeout(gpointer data)
{
  ((WindowStyles *) data)->reload();
  return false;
}


void WindowStyles::reload()
{
  // If no sheet, bail out.
  if (mystylesheet.empty())
    return;

  // (Re)load the styles.
  extern Styles *styles;
  Usfm *usfm = styles->usfm(mystylesheet);
  usfm->reload();

  // Load recently used markers and usage counts.
  retrieve_recently_used_data();

  // Clear store.
  gtk_tree_store_clear(treestore);

  // Load recently used styles.
  load_recently_used_styles();

  // Load stylesheet itself.
  load_stylesheet();
}


gboolean WindowStyles::on_key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  // Pressing Delete deletes the selected style(s).
  if (keyboard_delete_pressed(event)) {
    ((WindowStyles *) user_data)->on_style_delete();
  }
  // Pressing Insert creates a new style.
  if (keyboard_insert_pressed(event)) {
    ((WindowStyles *) user_data)->on_style_new();
  }
  // Collapse or expand a category.
  if (keyboard_left_arrow_pressed(event)) {
    return ((WindowStyles *) user_data)->styletree_expand_collapse(false);
  }
  if (keyboard_right_arrow_pressed(event)) {
    return ((WindowStyles *) user_data)->styletree_expand_collapse(true);
  }
  return FALSE;
}


gboolean WindowStyles::on_button_press_event(GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
  return ((WindowStyles *) user_data)->on_button_press(widget, event);
}


bool WindowStyles::on_button_press(GtkWidget * widget, GdkEventButton * event)
{
  // Double-clicking a style if edit mode is on, edits it.
  if (event->type == GDK_2BUTTON_PRESS) {
    bool edit_mode = style_edit_mode && gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(style_edit_mode));
    if (edit_mode) {
      on_style_properties();
      return true;
    }
  }
  // Popup menu handler.
  if (event->button == 3 && event->type == GDK_BUTTON_PRESS) {
    show_styles_popup_menu(treeview, event);
    return true;
  }
  return false;
}


gboolean WindowStyles::on_popup_menu(GtkWidget * widget, gpointer user_data)
{
  ((WindowStyles *) user_data)->popup_menu(widget);
  // Do not call the original handler.
  return true;
}


void WindowStyles::popup_menu(GtkWidget * widget)
// Keyboard popup menu handler.
{
  show_styles_popup_menu(treeview, NULL);
}


void WindowStyles::on_treeview_styles_row_collapsed(GtkTreeView * treeview, GtkTreeIter * iter, GtkTreePath * path, gpointer user_data)
{
  ((WindowStyles *) user_data)->on_treeview_styles_collapsed_expanded(iter, path, false);
}


void WindowStyles::on_treeview_styles_row_expanded(GtkTreeView * treeview, GtkTreeIter * iter, GtkTreePath * path, gpointer user_data)
{
  ((WindowStyles *) user_data)->on_treeview_styles_collapsed_expanded(iter, path, true);
}


void WindowStyles::on_treeview_styles_collapsed_expanded(GtkTreeIter * iter, GtkTreePath * path, bool expand)
{
  // Get the name of the row that has been affected.
  gchar *str_data;
  gtk_tree_model_get(GTK_TREE_MODEL(treestore), iter, 0, &str_data, -1);
  ustring row = str_data;
  g_free(str_data);
  // Store expanded it.
  size_t offset = get_expanded_state_offset(row);
  try {
    expanded_states[offset] = expand;
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
}


void WindowStyles::expand_all()
{
  gtk_tree_view_expand_all(GTK_TREE_VIEW(treeview));
  // Work around a bug in Gtk2.
  // When this function is called from the menu, the widget does not redraw.
  gtk_widget_queue_draw(treeview);
}


void WindowStyles::collapse_all()
{
  gtk_tree_view_collapse_all(GTK_TREE_VIEW(treeview));
}


void WindowStyles::on_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
  ((WindowStyles *) user_data)->row_activated();
}


void WindowStyles::row_activated()
{
  bool editmode = style_edit_mode && gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(style_edit_mode));
  if (editmode) {
    on_style_properties();
  } else {
    insert();
  }
}


void WindowStyles::insert()
// Insert a style in the editor.
{
  gtk_button_clicked(GTK_BUTTON(apply_signal));
}


ustring WindowStyles::get_focus()
// Return the focused style.
{
  return get_focused_style();
}


void WindowStyles::use(const ustring & marker)
// Adapt GUI to using this style.
{
  // Update recently used styles.
  use_style(marker);
  // Focus the style we selected.
  ustring complete_style = marker + " " + get_name(marker);
  focus_string(complete_style);
  // Store focus.
  extern Settings *settings;
  settings->session.selected_style = complete_style;
}


#define RECENTLY_USED _("Recently Used")


void WindowStyles::load_recently_used_styles()
// Sets the recently used styles in the treeview.
{
  // Make them complete styles, including their name.
  vector < ustring > styles;
  for (unsigned int i = 0; i < allrecentmarkers.size(); i++) {
    styles.push_back(allrecentmarkers[i] + " " + get_name(allrecentmarkers[i]));
  }

  // If no styles, bail out.
  if (styles.empty())
    return;

  // Insert the category.
  GtkTreeIter iter;
  gtk_tree_store_append(treestore, &iter, NULL);
  gtk_tree_store_set(treestore, &iter, 0, RECENTLY_USED, -1);

  // Insert the styles themselves.
  GtkTreeIter child_iter;
  for (unsigned int i = 0; i < styles.size(); i++) {
    gtk_tree_store_append(treestore, &child_iter, &iter);
    gtk_tree_store_set(treestore, &child_iter, 0, styles[i].c_str(), -1);
  }

  // Expand if needed.
  bool state = get_expanded_state(RECENTLY_USED);
  if (state) {
    GtkTreePath *path;
    path = gtk_tree_model_get_path(GTK_TREE_MODEL(treestore), &iter);
    gtk_tree_view_expand_row(GTK_TREE_VIEW(treeview), path, false);
    gtk_tree_path_free(path);
  }
  // Focus the category.
  focus_string(RECENTLY_USED);

  // Rewrite gui.
  while (gtk_events_pending())
    gtk_main_iteration();
}


vector < ustring > WindowStyles::get_focused_regular_styles()
{
  // Containers.
  vector < ustring > focused_strings;
  vector < bool > recently_used;
  vector < bool > categories;
  // Get all focused strings.
  get_focused_strings(focused_strings, recently_used, categories);
  // Focused styles.
  vector < ustring > focused_styles;
  for (unsigned int i = 0; i < focused_strings.size(); i++) {
    if (!categories[i]) {
      if (!recently_used[i]) {
        size_t pos = focused_strings[i].find(" ");
        focused_styles.push_back(focused_strings[i].substr(0, pos));
      }
    }
  }
  return focused_styles;
}


vector < ustring > WindowStyles::get_focused_recently_used_styles()
{
  // Containers.
  vector < ustring > focused_strings;
  vector < bool > recently_used;
  vector < bool > categories;
  // Get all focused strings.
  get_focused_strings(focused_strings, recently_used, categories);
  // Focused styles.
  vector < ustring > focused_styles;
  for (unsigned int i = 0; i < focused_strings.size(); i++) {
    if (!categories[i]) {
      if (recently_used[i]) {
        size_t pos = focused_strings[i].find(" ");
        focused_styles.push_back(focused_strings[i].substr(0, pos));
      }
    }
  }
  return focused_styles;
}


vector < ustring > WindowStyles::get_focused_categories()
{
  // Containers.
  vector < ustring > focused_strings;
  vector < bool > recently_used;
  vector < bool > categories;
  // Get all focused strings.
  get_focused_strings(focused_strings, recently_used, categories);
  // Focused styles.
  vector < ustring > focused_categories;
  for (unsigned int i = 0; i < focused_strings.size(); i++) {
    if (categories[i]) {
      focused_categories.push_back(focused_strings[i]);
    }
  }
  return focused_categories;
}


ustring WindowStyles::get_sheet()
// Returns the object's stylesheet.
{
  return mystylesheet;
}


void WindowStyles::on_style_menu_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((WindowStyles *) user_data)->on_style_menu();
}


void WindowStyles::on_style_menu()
{
  // Some variables to be used to (dis/en)able menu items.
  vector < ustring > selected_recently_used_styles = get_focused_recently_used_styles();
  vector < ustring > selected_regular_styles = get_focused_regular_styles();
  vector < ustring > selected_categories = get_focused_categories();
  bool editmode = style_edit_mode && gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(style_edit_mode));
  // Set sensitivity of menu items.
  if (style_insert)
    gtk_widget_set_sensitive(style_insert, ((selected_recently_used_styles.size() + selected_regular_styles.size()) == 1));
  if (style_expand_all)
    gtk_widget_set_sensitive(style_expand_all, !mystylesheet.empty());
  if (style_collapse_all)
    gtk_widget_set_sensitive(style_collapse_all, !mystylesheet.empty());
  if (style_new)
    gtk_widget_set_sensitive(style_new, editmode && !mystylesheet.empty());
  if (style_properties)
    gtk_widget_set_sensitive(style_properties, editmode && ((selected_recently_used_styles.size() + selected_regular_styles.size()) == 1));
  if (style_delete)
    gtk_widget_set_sensitive(style_delete, editmode && ((selected_recently_used_styles.size() + selected_regular_styles.size() + selected_categories.size()) > 0));
  if (stylesheet_remove)
    gtk_widget_set_sensitive(stylesheet_remove, !mystylesheet.empty());
  if (stylesheet_rename)
    gtk_widget_set_sensitive(stylesheet_rename, !mystylesheet.empty());
}


void WindowStyles::on_expand_all_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((WindowStyles *) user_data)->expand_all();
}


void WindowStyles::on_collapse_all_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((WindowStyles *) user_data)->collapse_all();
}


void WindowStyles::show_styles_popup_menu(GtkWidget * my_widget, GdkEventButton * event)
{
  int button, event_time;
  if (event) {
    button = event->button;
    event_time = event->time;
  } else {
    button = 0;
    event_time = gtk_get_current_event_time();
  }
  on_style_menu();
  gtk_menu_popup(GTK_MENU(style_menu), NULL, NULL, NULL, NULL, button, event_time);
}


void WindowStyles::on_style_new_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((WindowStyles *) user_data)->on_style_new();
}


void WindowStyles::on_style_new()
{
  // Do nothing if this widget does not display.
  if (!style_new)
    return;
  // Do nothing if edit mode is off or no stylesheet is open.
  if (!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(style_edit_mode)))
    return;
  if (mystylesheet.empty())
    return;
  NewStylesheetDialog dialog(mystylesheet);
  if (dialog.run() != GTK_RESPONSE_OK)
    return;
  // Some people will insert the backslash too: remove it.
  ustring input = trim(dialog.name);
  {
    size_t position = input.find("\\");
    if (position != string::npos)
      input.erase(position, 1);
  }
  // If nothing left, bail out.
  if (input.empty())
    return;
  // Check that the marker does not already exist.
  vector < ustring > markers = stylesheet_get_markers(mystylesheet, NULL);
  for (unsigned int i = 0; i < markers.size(); i++) {
    if (input == markers[i]) {
      gtkw_dialog_error(NULL, _("This marker already exists in the stylesheet")); {
      return;
      }
    }
  }
  // Create style.
  stylesheet_new_style(mystylesheet, input);
  // Reopen stylesheet.
  reload();
  Style style(mystylesheet, input, false);
  ustring focus = input + " " + style.name;
  focus_string(focus);
}


void WindowStyles::on_style_properties_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((WindowStyles *) user_data)->on_style_properties();
}


void WindowStyles::on_style_properties()
{
  // If inactive, bail out.
  if (!style_properties)
    return;
  // Get the first focused style.
  ustring style = get_focused_style();
  // Do nothing if none.
  if (style.empty())
    return;
  // Edit the style.
  StylesheetDialog dialog(mystylesheet, style);
  if (dialog.run() == GTK_RESPONSE_OK) {
    // Reopen sheet and focus the style.
    reload();
    focus_string(style + " " + dialog.name);
    gtk_button_clicked(GTK_BUTTON(edited_signal));
  }
}


void WindowStyles::on_style_delete_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((WindowStyles *) user_data)->on_style_delete();
}


void WindowStyles::on_style_delete()
{
  // If inactive, bail out.
  if (!style_delete)
    return;
  // Do nothing if edit mode is off or no styles have been selected.
  if (!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(style_edit_mode)))
    return;
  // Get focused items.
  vector < ustring > focused_recently_used_styles;
  vector < ustring > focused_regular_styles;
  vector < ustring > focused_categories;
  focused_recently_used_styles = get_focused_recently_used_styles();
  focused_regular_styles = get_focused_regular_styles();
  focused_categories = get_focused_categories();
  // How many items have been selected?
  int selected_count;
  selected_count = focused_recently_used_styles.size() + focused_regular_styles.size() + focused_categories.size();
  if (selected_count == 0)
    return;
  // Find the text of the item just before the first focused item.
  ustring string_before_focus = get_string_before_focus();
  // Delete the styles.
  for (unsigned int i = 0; i < focused_recently_used_styles.size(); i++) {
    clear_recently_used_style(focused_recently_used_styles[i]);
  }
  for (unsigned int i = 0; i < focused_regular_styles.size(); i++) {
    stylesheet_delete_style(mystylesheet, focused_regular_styles[i]);
  }
  delete_categories(focused_categories);
  reload();
  // Focus on the string just before the deleted ones(s).
  focus_string(string_before_focus);
}


void WindowStyles::on_style_insert_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((WindowStyles *) user_data)->insert();
}


void WindowStyles::stylesheet_open(const ustring & stylesheet)
// Open a stylesheet, initiated by this object.
{
  load(stylesheet);
  gtk_button_clicked(GTK_BUTTON(open_signal));
}


void WindowStyles::on_stylesheet_new_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((WindowStyles *) user_data)->on_stylesheet_new();
}


void WindowStyles::on_stylesheet_new()
{
  // Show dialog. Bail out on Cancel.
  NewStylesheetDialog dialog("");
  if (dialog.run() != GTK_RESPONSE_OK)
    return;
  // New sheet.
  stylesheet_create_new(dialog.name, dialog.stylesheettype);
  // Open it.
  stylesheet_open(dialog.name);
}


void WindowStyles::on_stylesheet_delete_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((WindowStyles *) user_data)->on_stylesheet_delete();
}


void WindowStyles::on_stylesheet_delete()
{
  OpenStylesheetDialog dialog(osdtDelete, mystylesheet);
  if (dialog.run() == GTK_RESPONSE_OK) {
    if (gtkw_dialog_question(NULL, _("Do you really want to delete stylesheet ") + dialog.stylesheet + "?") == GTK_RESPONSE_YES) {
      stylesheet_delete(dialog.stylesheet);
    }
  }
}


void WindowStyles::on_stylesheet_rename_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((WindowStyles *) user_data)->on_stylesheet_rename();
}


void WindowStyles::on_stylesheet_rename()
{
  // Ask for a new name.
  EntryDialog dialog(_("Rename stylesheet"), _("Give a new name for this stylesheet"), mystylesheet);
  if (dialog.run() == GTK_RESPONSE_OK) {
    // Check whether the new name does not already exist.
    if (!stylesheet_exists(dialog.entered_value)) {
      // Rename the stylesheet.
      stylesheet_copy(mystylesheet, dialog.entered_value);
      stylesheet_delete(mystylesheet);
      // Store new name in configuration.
      stylesheet_open(dialog.entered_value);
    } else {
      // No, it already exist - give message about that.
      gtkw_dialog_error(NULL, "This name already exists.");
    }
  }
}


void WindowStyles::on_stylesheet_import()
{
  ustring filename = gtkw_file_chooser_open(NULL, _("Open stylesheet"), "");
  if (filename.empty())
    return;
  ustring styleheet = stylesheet_import(filename);
  if (!styleheet.empty()) {
    stylesheet_open(styleheet);
  }
}


void WindowStyles::export_sheet(const ustring& filename, StylesheetExportFormatType format)
{
  if (filename.empty())
    return;
  switch (format) {
    case seftBibledit: 
    {
      stylesheet_export_bibledit(mystylesheet, filename);
      break;
    }
    case seftParatext:
    {
      stylesheet_export_paratext(mystylesheet, filename);
      break;
    }
  }
}


void WindowStyles::on_stylesheet_switch_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((WindowStyles *) user_data)->on_stylesheet_switch();
}


void WindowStyles::on_stylesheet_switch()
{
  OpenStylesheetDialog dialog(osdtSwitch, mystylesheet);
  if (dialog.run() == GTK_RESPONSE_OK) {
    // Open the sheet.
    stylesheet_open(dialog.stylesheet);
  }
}


bool WindowStyles::styletree_expand_collapse(bool expand)
// Expands (or collapses) the selected item(s) in the treeview.
{
  // Indication whether we expanded or collapsed something.
  bool expanded_collapsed = false;
  // Get the model
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
  // Some variables needed.
  GtkTreeIter iter;
  gboolean valid;
  // Get the first iter in the store.
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    // Walk through the list.
    if (gtk_tree_selection_iter_is_selected(treeselect, &iter)) {
      GtkTreePath *path;
      path = gtk_tree_model_get_path(model, &iter);
      if (expand)
        gtk_tree_view_expand_row(GTK_TREE_VIEW(treeview), path, false);
      else
        gtk_tree_view_collapse_row(GTK_TREE_VIEW(treeview), path);
      gtk_tree_path_free(path);
      expanded_collapsed = true;
    }
    // Next.
    valid = gtk_tree_model_iter_next(model, &iter);
  }
  // Return whether we did something.
  return expanded_collapsed;
}


void WindowStyles::delete_categories(vector < ustring > categories)
{
  // Get a list of all the markers in our stylesheet.
  vector < ustring > markers;
  markers = stylesheet_get_markers(mystylesheet, NULL);
  // Categorize them all.
  vector < UsfmCategory > usfmcategories;
  usfm_categorize_markers(markers, usfmcategories);
  // Get the human readable form of the category for them all.
  vector < ustring > ucategories;
  for (unsigned int i = 0; i < usfmcategories.size(); i++) {
    ucategories.push_back(usfm_get_category_name(usfmcategories[i]));
  }
  // Go through all categories we wish to delete.  
  for (unsigned int i = 0; i < categories.size(); i++) {
    if (categories[i] == RECENTLY_USED) {
      // If it's the recently used category, clear it.
      clear_recently_used_styles();
    } else {
      // If it's a regular category, clear it completely.
      for (unsigned int i2 = 0; i2 < markers.size(); i2++) {
        if (categories[i] == ucategories[i2]) {
          stylesheet_delete_style(mystylesheet, markers[i2]);
        }
      }
    }
  }
}


ustring WindowStyles::get_string_before_focus()
// This returns the visible text just before the first focused item.
{
  // Variable.
  ustring returnvalue;
  // Get all focused strings. If none, do nothing.
  vector < ustring > focused_strings;
  vector < bool > dummy1, dummy2;
  get_focused_strings(focused_strings, dummy1, dummy2);
  if (!focused_strings.empty()) {
    // Get the model
    GtkTreeModel *model;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
    // Some variables needed.
    GtkTreeIter iter;
    gboolean valid;
    ustring previousvalue;
    // Get the first iter in the store.
    valid = gtk_tree_model_get_iter_first(model, &iter);
    while (valid) {
      // Walk through the list, reading each row
      gchar *str_data;
      gtk_tree_model_get(model, &iter, 0, &str_data, -1);
      // If this is the first focused string, store the previous value.
      if (strcmp(str_data, focused_strings[0].c_str()) == 0)
        returnvalue = previousvalue;
      // Set previous value for next round.
      previousvalue = str_data;
      // Free memory.
      g_free(str_data);
      // See whether the row is expanded.
      GtkTreePath *path;
      path = gtk_tree_model_get_path(model, &iter);
      bool row_expanded = gtk_tree_view_row_expanded(GTK_TREE_VIEW(treeview), path);
      gtk_tree_path_free(path);
      // If row expanded, walk through the children.
      if (row_expanded) {
        GtkTreeIter child_iter;
        bool valid_child;
        valid_child = gtk_tree_model_iter_children(model, &child_iter, &iter);
        while (valid_child) {
          gchar *str_data;
          gtk_tree_model_get(model, &child_iter, 0, &str_data, -1);
          if (strcmp(str_data, focused_strings[0].c_str()) == 0)
            returnvalue = previousvalue;
          previousvalue = str_data;
          g_free(str_data);
          valid_child = gtk_tree_model_iter_next(model, &child_iter);
        }
      }
      valid = gtk_tree_model_iter_next(model, &iter);
    }
  }
  // Return the result.
  return returnvalue;
}


ustring WindowStyles::get_focused_style()
{
  // Containers.
  vector < ustring > focused_strings;
  vector < bool > recently_used;
  vector < bool > categories;
  // Get all focused strings.
  get_focused_strings(focused_strings, recently_used, categories);
  // Focused styles.
  vector < ustring > focused_styles;
  for (unsigned int i = 0; i < focused_strings.size(); i++) {
    if (!categories[i])
      focused_styles.push_back(focused_strings[i]);
  }
  // Focused style.
  ustring focused_style;
  if (focused_styles.size() > 0) {
    focused_style = focused_styles[0];
    size_t pos = focused_style.find(" ");
    focused_style = focused_style.substr(0, pos);
  }
  return focused_style;
}


void WindowStyles::focus_string(const ustring & string)
// Focuses "string" in the treeview.
{
  // Get the model
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
  // Some variables needed.
  GtkTreeIter iter;
  gboolean valid;
  bool focused = false;
  // Get the first iter in the store.
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    // Walk through the list, reading each row
    gchar *str_data;
    // Make sure you terminate calls to gtk_tree_model_get() with a '-1' value.
    gtk_tree_model_get(model, &iter, 0, &str_data, -1);
    // If this is the string we wish to focus, select it, put the cursor on it,
    // and focus on the treeview.
    if (str_data == string) {
      // Only focus the first occurrence.
      if (!focused) {
        focus_iter(&iter, false);
      }
      focused = true;
    }
    g_free(str_data);
    // Walk through the children.
    GtkTreeIter child_iter;
    bool valid_child;
    valid_child = gtk_tree_model_iter_children(model, &child_iter, &iter);
    while (valid_child) {
      gchar *str_data;
      gtk_tree_model_get(model, &child_iter, 0, &str_data, -1);
      if (str_data == string) {
        if (!focused)
          focus_iter(&child_iter, true);
        focused = true;
      }
      g_free(str_data);
      valid_child = gtk_tree_model_iter_next(model, &child_iter);
    }
    valid = gtk_tree_model_iter_next(model, &iter);
  }
  while (gtk_events_pending())
    gtk_main_iteration();
}


void WindowStyles::focus_iter(GtkTreeIter * iter, bool expand)
{
  GtkTreeModel *model = GTK_TREE_MODEL(treestore);
  gtk_tree_selection_select_iter(treeselect, iter);
  GtkTreePath *path = gtk_tree_model_get_path(model, iter);
  if (expand) {
    gtk_tree_view_expand_to_path(GTK_TREE_VIEW(treeview), path);
  }
  gtk_tree_view_set_cursor(GTK_TREE_VIEW(treeview), path, NULL, false);
  gtk_tree_path_free(path);
  while (gtk_events_pending())
    gtk_main_iteration();
}


size_t WindowStyles::get_expanded_state_offset(const ustring & row)
{
  // MAP: changed from if..if..if  to  if..else if..else if...
  // Next improvement: make O(1) lookup.
  size_t offset = 0;            // Default: recently used.
  if (row == usfm_get_category_name(ucIdentificationInformation)) {
    offset = 1;
  }
  else if (row == usfm_get_category_name(ucIntroductionTitlesHeadings)) {
    offset = 2;
  }
  else if (row == usfm_get_category_name(ucIntroductionParagraphsPoetry)) {
    offset = 3;
  }
  else if (row == usfm_get_category_name(ucIntroductionOtherElements)) {
    offset = 4;
  }
  else if (row == usfm_get_category_name(ucTitles)) {
    offset = 5;
  }
  else if (row == usfm_get_category_name(ucHeadings)) {
    offset = 6;
  }
  else if (row == usfm_get_category_name(ucChaptersAndVerses)) {
    offset = 7;
  }
  else if (row == usfm_get_category_name(ucParagraphs)) {
    offset = 8;
  }
  else if (row == usfm_get_category_name(ucLists)) {
    offset = 9;
  }
  else if (row == usfm_get_category_name(ucPoetryElements)) {
    offset = 10;
  }
  else if (row == usfm_get_category_name(ucTableElements)) {
    offset = 11;
  }
  else if (row == usfm_get_category_name(ucFootnotes)) {
    offset = 12;
  }
  else if (row == usfm_get_category_name(ucCrossReferences)) {
    offset = 13;
  }
  else if (row == usfm_get_category_name(ucExtendedStudyNotes)) {
    offset = 14;
  }
  else if (row == usfm_get_category_name(ucSpecialText)) {
    offset = 15;
  }
  else if (row == usfm_get_category_name(ucCharacterStyles)) {
    offset = 16;
  }
  else if (row == usfm_get_category_name(ucSpacingsAndBreaks)) {
    offset = 17;
  }
  else if (row == usfm_get_category_name(ucSpecialFeatures)) {
    offset = 18;
  }
  else if (row == usfm_get_category_name(ucPeripheralMaterials)) {
    offset = 19;
  }
  else if (row == usfm_get_category_name(ucNonstandardStyles)) {
    offset = 20;
  }

  return offset;
}


void WindowStyles::use_style(const ustring & style)
// "Uses" a style, and so ranks it at the top of the recently used styles,
// and updates both the store and the configuration.
{
  // Increase the usage count of this style. 
  // Add it to list of styles used if it is not yet in.
  set < ustring > styles_in(allrecentmarkers.begin(), allrecentmarkers.end());
  if (styles_in.find(style) == styles_in.end()) {
    usagecounters[style] = 0;
    allrecentmarkers.push_back(style);
  }
  usagecounters[style]++;
  // Sort stuff.
  process_recently_used_data();
  // Display no more than a maximum number of styles.
  while (allrecentmarkers.size() > 30)
    allrecentmarkers.erase(allrecentmarkers.end());
  // Make them complete styles.
  vector < ustring > styles;
  for (unsigned int i = 0; i < allrecentmarkers.size(); i++) {
    styles.push_back(allrecentmarkers[i] + " " + get_name(allrecentmarkers[i]));
  }
  // Display information.
  GtkTreeModel *model = GTK_TREE_MODEL(treestore);
  // Some variables needed.
  GtkTreeIter iter;
  // Take out the frequently used category, if it is in.
  gtk_tree_model_get_iter_first(model, &iter);
  gchar *str_data;
  gtk_tree_model_get(model, &iter, 0, &str_data, -1);
  if (strcmp(str_data, RECENTLY_USED) == 0) {
    gtk_tree_store_remove(treestore, &iter);
  }
  g_free(str_data);
  // Add the category at the very start.
  gtk_tree_store_prepend(treestore, &iter, NULL);
  gtk_tree_store_set(treestore, &iter, 0, RECENTLY_USED, -1);
  // The iter now points to the frequently used row. Add any children here.
  GtkTreeIter child_iter;
  for (unsigned int i = 0; i < styles.size(); i++) {
    gtk_tree_store_append(treestore, &child_iter, &iter);
    gtk_tree_store_set(treestore, &child_iter, 0, styles[i].c_str(), -1);
  }
  // Expand if needed.
  bool state = get_expanded_state(RECENTLY_USED);
  if (state) {
    GtkTreePath *path;
    path = gtk_tree_model_get_path(GTK_TREE_MODEL(treestore), &iter);
    gtk_tree_view_expand_row(GTK_TREE_VIEW(treeview), path, false);
    gtk_tree_path_free(path);
  }
}


void WindowStyles::clear_recently_used_styles()
{
  vector < ustring > styles;
  vector < unsigned int >count;
  stylesheet_set_recently_used(mystylesheet, styles, count);
}


void WindowStyles::clear_recently_used_style(const ustring & style)
{
  // Remove the style.
  vector < ustring > styles;
  for (unsigned int i = 0; i < allrecentmarkers.size(); i++) {
    if (allrecentmarkers[i] != style) {
      styles.push_back(allrecentmarkers[i]);
    }
  }
  allrecentmarkers = styles;
  // Store the styles.
  store_recently_used_data();
}


void WindowStyles::load_stylesheet()
// Load a whole stylesheet.
{
  vector < ustring > markers;
  vector < ustring > names0;
  markers = stylesheet_get_markers(mystylesheet, &names0);
  map < ustring, ustring > names;
  for (unsigned int i = 0; i < markers.size(); i++) {
    names[markers[i]] = names0[i];
  }
  vector < UsfmCategory > categories;
  usfm_categorize_markers(markers, categories);
  UsfmCategory category = ucIdentificationInformation;
  bool insert_category = true;
  GtkTreeIter iter;
  for (unsigned int i = 0; i < markers.size(); i++) {
    if (categories[i] != category) {
      insert_category = true;
      category = categories[i];
    }
    ustring ucategory;
    if (insert_category) {
      ucategory = usfm_get_category_name(categories[i]);
      gtk_tree_store_append(treestore, &iter, NULL);
      gtk_tree_store_set(treestore, &iter, 0, ucategory.c_str(), -1);
    }
    GtkTreeIter child_iter;
    gtk_tree_store_append(treestore, &child_iter, &iter);
    ustring row;
    row = markers[i] + " " + names[markers[i]];
    gtk_tree_store_set(treestore, &child_iter, 0, row.c_str(), -1);
    if (insert_category) {
      bool state = get_expanded_state(ucategory);
      if (state) {
        GtkTreePath *path;
        path = gtk_tree_model_get_path(GTK_TREE_MODEL(treestore), &iter);
        gtk_tree_view_expand_row(GTK_TREE_VIEW(treeview), path, false);
        gtk_tree_path_free(path);
      }
    }
    insert_category = false;
  }
  while (gtk_events_pending())
    gtk_main_iteration();

  // Focus the last style, if there's any.
  extern Settings *settings;
  ustring last_style = settings->session.selected_style;
  if (!last_style.empty()) {
    focus_string(last_style);
  }
}


void WindowStyles::get_focused_strings(vector < ustring > &focused_strings, vector < bool > &recently_used, vector < bool > &categories)
{
  // Get the model
  GtkTreeModel *model = GTK_TREE_MODEL(treestore);
  // Get the selected paths.
  GList *sellist = NULL;
  sellist = gtk_tree_selection_get_selected_rows(treeselect, &model);
  // Walk through all selected ones.
  while (sellist) {
    // Get the path.
    GtkTreePath *path = (GtkTreePath *) sellist->data;
    // Get iterator to the style.
    GtkTreeIter iter;
    gtk_tree_model_get_iter(model, &iter, path);
    // Get the data.
    gchar *str_data;
    gtk_tree_model_get(model, &iter, 0, &str_data, -1);
    ustring data = str_data;
    // Store the style.
    focused_strings.push_back(data);
    // Free memory.    
    g_free(str_data);
    // See whether we're in the recently used styles or elsewhere.
    bool recently_used_style = false;
    GtkTreeIter parent_iter;
    if (gtk_tree_model_iter_parent(model, &parent_iter, &iter)) {
      gchar *str_data;
      gtk_tree_model_get(model, &parent_iter, 0, &str_data, -1);
      recently_used_style = (strcmp(str_data, RECENTLY_USED) == 0);
      g_free(str_data);
    }
    recently_used.push_back(recently_used_style);
    // See whether we're in a category or elsewhere.
    // Depth 1: category; depth 2: style.
    int depth = gtk_tree_path_get_depth(path);
    categories.push_back(depth == 1);
    // Free memory.
    gtk_tree_path_free(path);
    // Next round.
    sellist = sellist->next;
  }
  // Free memory for list.
  g_list_free(sellist);
}


bool WindowStyles::get_expanded_state(const ustring & row)
// Taking the string in "row", this returns the expanded state.
{
  // Look for the categorised rows, and if found return the state.
  bool state = false;
  size_t offset = get_expanded_state_offset(row);
  if (offset < expanded_states.size())
    state = expanded_states[offset];
  return state;
}


ustring WindowStyles::get_name(const ustring & marker)
// Returns the name of the marker.
{
  extern Styles *styles;
  Usfm *usfm = styles->usfm(mystylesheet);
  for (unsigned int i = 0; i < usfm->styles.size(); i++)
    if (marker == usfm->styles[i].marker)
      return usfm->styles[i].name;
  return _("Unknown");
}


void WindowStyles::retrieve_recently_used_data()
// Retrieves the recently used styles and usage counts from disk.
{
  vector < unsigned int >count;
  stylesheet_get_recently_used(mystylesheet, allrecentmarkers, count);
  for (unsigned int i = 0; i < allrecentmarkers.size(); i++) {
    usagecounters[allrecentmarkers[i]] = count[i];
  }
  process_recently_used_data();
}


void WindowStyles::process_recently_used_data()
// Do some processing on the data of the recently used styles.
{
  // Sort them on usage count. Most used first.
  vector < unsigned int >count;
  for (unsigned int i = 0; i < allrecentmarkers.size(); i++) {
    count.push_back(G_MAXINT - usagecounters[allrecentmarkers[i]]);
  }
  quick_sort(count, allrecentmarkers, 0, allrecentmarkers.size());
  // Take out the ones that are not in the stylesheet.
  set < ustring > styles2;
  extern Styles *styles;
  Usfm *usfm = styles->usfm(mystylesheet);
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    styles2.insert(usfm->styles[i].marker);
  }
  vector < ustring > existing_ones;
  for (unsigned int i = 0; i < allrecentmarkers.size(); i++) {
    if (styles2.find(allrecentmarkers[i]) != styles2.end())
      existing_ones.push_back(allrecentmarkers[i]);
  }
  allrecentmarkers = existing_ones;
}


void WindowStyles::store_recently_used_data()
// Stores the recently used styles and usage count to disk.
{
  vector < unsigned int >count;
  for (unsigned int i = 0; i < allrecentmarkers.size(); i++) {
    count.push_back(usagecounters[allrecentmarkers[i]]);
  }
  stylesheet_set_recently_used(mystylesheet, allrecentmarkers, count);
}
