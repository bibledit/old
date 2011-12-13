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


#ifndef INCLUDED_WEBKIT_BROWSER_H
#define INCLUDED_WEBKIT_BROWSER_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "reference.h"
#include "types.h"
#include <webkit/webkit.h>


class WebkitBrowser
{
public:
  WebkitBrowser(GtkWidget * parent_vbox);
  ~WebkitBrowser();
  void focus();
  bool focused();
  void copy();
  void set_home_page (const ustring& url);
  void go_to(const ustring& url);
  time_t last_focused_time;
private:
  // Widgets.
  GtkWidget *vbox;
  GtkWidget *toolbar;
  GtkWidget *toolitem_back;
  GtkWidget *button_back;
  GtkWidget *image_back;
  GtkWidget *toolitem_forward;
  GtkWidget *button_forward;
  GtkWidget *image_forward;
  GtkWidget *toolitem_refresh;
  GtkWidget *button_refresh;
  GtkWidget *image_refresh;
  GtkWidget *toolitem_stop;
  GtkWidget *button_stop;
  GtkWidget *image_stop;
  GtkWidget *toolitem_home;
public:
  GtkWidget *button_home;
private:
  GtkWidget *image_home;
  GtkWidget *toolitem_url;
  GtkWidget *entry_url;
  GtkWidget *toolitem_enter;
  GtkWidget *button_enter;
  GtkWidget *image_enter;
  GtkWidget *scrolledwindow;
public:
  GtkWidget *webview;
private:
  GtkWidget *notebook;
  GtkWidget *label_status;
  GtkWidget *progressbar;

  ustring homepage;
  
  // Focus.
  static void on_webview_grab_focus(GtkWidget *widget, gpointer user_data);
  void webview_grab_focus();

  // Buttons.
  static void on_button_back_clicked (GtkButton *button, gpointer user_data);
  void on_button_back ();
  static void on_button_forward_clicked (GtkButton *button, gpointer user_data);
  void on_button_forward ();
  static void on_button_refresh_clicked (GtkButton *button, gpointer user_data);
  void on_button_refresh ();
  static void on_button_stop_clicked (GtkButton *button, gpointer user_data);
  void on_button_stop ();
  static void on_button_home_clicked (GtkButton *button, gpointer user_data);
  void on_button_home ();
  static void on_entry_url_activate (GtkEntry *entry, gpointer user_data);
  static void on_button_enter_clicked (GtkButton *button, gpointer user_data);
  void on_entry_url ();
  
  // Webkit callbacks.
  static void load_commit_cb (WebKitWebView* page, WebKitWebFrame* frame, gpointer data);
  void load_commit (WebKitWebView* page, WebKitWebFrame* frame);
  static void progress_change_cb (WebKitWebView* page, gint progress, gpointer data);
  void progress_change (WebKitWebView* page, gint progress);
  static void link_hover_cb (WebKitWebView* page, const gchar* title, const gchar* link, gpointer data);
  void link_hover (WebKitWebView* page, const gchar* title, const gchar* link);
  static void load_started_cb (WebKitWebView *page, WebKitWebFrame *frame, gpointer user_data);
  void load_started (WebKitWebView *page, WebKitWebFrame *frame);
  static void load_finished_cb (WebKitWebView *page, WebKitWebFrame *frame, gpointer user_data);
  void load_finished (WebKitWebView *page, WebKitWebFrame *frame);
  
};


#endif
