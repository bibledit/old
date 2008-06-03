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

#ifndef INCLUDED_GTK_HTML_3_BROWSER_H
#define INCLUDED_GTK_HTML_3_BROWSER_H

#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "reference.h"
#include "types.h"
extern "C" {
#include <gtkhtml/gtkhtml.h>
}
#include <curl/curl.h>

struct CurlMemoryStruct
{
  char *memory;
  size_t size;
};

class GtkHtml3Browser
{
public:
  GtkHtml3Browser(GtkWidget * vbox);
  ~GtkHtml3Browser();
  void focus();
  bool focused();
  void copy();
  void go_to(const ustring& url);
  time_t last_focused_time;
private:
  // Widgets.
  GtkWidget *my_vbox;
  GtkWidget *scrolledwindow;
  GtkWidget *htmlview;

  // Timer.
  guint event_id;
  static bool on_timeout(gpointer user_data);
  bool timeout();

  // Loader.
  static gboolean on_html_url_requested(GtkHTML *html, const gchar *url, GtkHTMLStream *handle, gpointer user_data);
  void html_url_requested(GtkHTML *html, const gchar *url, GtkHTMLStream *handle);
  static gboolean on_html_link_clicked(GtkHTML *html, const gchar * url, gpointer user_data);
  void html_link_clicked(GtkHTML *html, const gchar * url);
  ustring loaded_url;

  // Fetcher.
  static void *myrealloc(void *ptr, size_t size);
  static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);

  // Focus.
  static void on_htmlview_grab_focus(GtkWidget *widget, gpointer user_data);
  void htmlview_grab_focus();
};

#endif
