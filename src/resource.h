/*
** Copyright (©) 2003-2008 Teus Benschop.
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
** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#ifndef INCLUDED_RESOURCE_H
#define INCLUDED_RESOURCE_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "reference.h"
#include <gtkmozembed.h>
#include "types.h"


class Resource
{
public:
  Resource (GtkWidget * vbox, GtkWidget * notebook_page, GtkWidget * tab_label);
  ~Resource ();
  void focus ();
  bool focused ();
  void defocus ();
  void parent_notebook_switches_to_page (GtkWidget * page);
  GtkWidget * parent_notebook_page ();
  void copy ();
  bool can_go_back ();
  bool can_go_forward ();
  void go_back ();
  void go_forward ();
  void reload ();
  bool loading;
  void stop ();
  void home ();
  void go_to (const Reference& reference);
  ustring template_get ();
  void open (const ustring& filename);
  GtkWidget * focus_signal;
private:
  // Main window.
  GtkWidget *my_vbox;

  // Top bar.
  GtkWidget *hbox;
  GtkWidget *progressbar;
  GtkWidget *animation;

  // Browser.
  GtkWidget *mozembed;

  // Callbacks.
  static gboolean on_progressbar_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  gboolean on_progressbar_button_press (GdkEventButton *event);

  // Resource loader.
  ustring mytemplatefile;
  ResourceType resource_type;
  ustring homepage;
  
  // Reference logic.
  ustring url_structure;
  ustring index_file_structure;
  map <unsigned int, ustring> book_renderer;
  map <unsigned int, ustring> anchor_renderer;

  // Embedded mozilla callbacks.
  static void on_location_changed_cb (GtkMozEmbed *embed, gpointer data);
  void on_location_changed ();
  static void on_link_message_cb (GtkMozEmbed *embed, gpointer data);
  void on_link_message ();
  static void on_progress_cb (GtkMozEmbed *embed, gint cur, gint max, gpointer data);
  void on_progress (gint cur, gint max);
  static void on_progress_all_cb (GtkMozEmbed *embed, const char *aURI, gint cur, gint max, gpointer data);
  void on_progress_all (gint cur, gint max);
  static void on_load_started_cb (GtkMozEmbed *embed, gpointer data);
  void on_load_started ();
  static void on_load_finished_cb (GtkMozEmbed *embed, gpointer data);
  void on_load_finished ();
  static void on_new_window_cb (GtkMozEmbed *embed, GtkMozEmbed **retval, guint chromemask, gpointer data);
  void on_new_window (GtkMozEmbed **retval);
  static void on_destroy_brsr_cb (GtkMozEmbed *embed, gpointer data);
  void on_destroy_brsr (GtkMozEmbed *embed);
  static gint on_open_uri_cb (GtkMozEmbed *embed, const char *uri, gpointer data);
  gint on_open_uri (const char *uri);
  static void on_js_status_changed_cb (GtkMozEmbed *embed, gpointer data);
  void on_js_status_changed ();
  static void on_visibility_cb (GtkMozEmbed *embed, gboolean visibility, gpointer data);
  void on_visibility (gboolean visibility);
  static void on_size_to_cb (GtkMozEmbed *embed, gint width, gint height, gpointer data);
  void on_size_to (gint width, gint height);
  static gint on_dom_key_down_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data);
  gint on_dom_key_down (gpointer dom_event);
  static gint on_dom_key_press_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data);
  gint on_dom_key_press (gpointer dom_event);
  static gint on_dom_key_up_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data);
  gint on_dom_key_up (gpointer dom_event);
  static gint on_dom_mouse_down_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data);
  gint on_dom_mouse_down (gpointer dom_event);
  static gint on_dom_mouse_up_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data);
  gint on_dom_mouse_up (gpointer dom_event);
  static gint on_dom_mouse_click_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data);
  gint on_dom_mouse_click (gpointer dom_event);
  static gint on_dom_mouse_dbl_click_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data);
  gint on_dom_mouse_dbl_click (gpointer dom_event);
  static gint on_dom_mouse_over_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data);
  gint on_dom_mouse_over (gpointer dom_event);
  static gint on_dom_mouse_out_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data);
  gint on_dom_mouse_out (gpointer dom_event);
  static void on_security_change_cb (GtkMozEmbed *embed, gpointer request, guint state, gpointer data);
  void on_security_change (gpointer request, guint state);
  static void on_status_change_cb (GtkMozEmbed *embed, gpointer request, gint status, gpointer message, gpointer data);
  void on_status_change (gpointer request, gint status, gpointer message);
  static gint on_dom_activate_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data);
  gint on_dom_activate (gpointer dom_event);
  static gint on_dom_focus_in_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data);
  gint on_dom_focus_in (gpointer dom_event);
  static gint on_dom_focus_out_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data);
  gint on_dom_focus_out (gpointer dom_event);

  // Focus handling.
  bool focus_set;
  GtkWidget * my_notebook_page;
  GtkWidget * my_tab_label;
  ustring title;
};


#endif
