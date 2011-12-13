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


#include "utilities.h"
#include <glib.h>
#include "webkitbrowser.h"
#include "settings.h"
#include "tiny_utilities.h"
#include "gwrappers.h"
#include "directories.h"
#include "books.h"
#include "resource_utils.h"


WebkitBrowser::WebkitBrowser(GtkWidget * parent_vbox)
{
  // Save and initialize variables.
  vbox = parent_vbox;
  last_focused_time = 0;

  toolbar = gtk_toolbar_new ();
  gtk_widget_show (toolbar);
  gtk_box_pack_start (GTK_BOX (vbox), toolbar, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_BOTH);

  toolitem_back = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem_back);
  gtk_container_add (GTK_CONTAINER (toolbar), toolitem_back);

  button_back = gtk_button_new ();
  gtk_widget_show (button_back);
  gtk_container_add (GTK_CONTAINER (toolitem_back), button_back);

  image_back = gtk_image_new_from_stock ("gtk-go-back", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_back);
  gtk_container_add (GTK_CONTAINER (button_back), image_back);

  toolitem_forward = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem_forward);
  gtk_container_add (GTK_CONTAINER (toolbar), toolitem_forward);

  button_forward = gtk_button_new ();
  gtk_widget_show (button_forward);
  gtk_container_add (GTK_CONTAINER (toolitem_forward), button_forward);

  image_forward = gtk_image_new_from_stock ("gtk-go-forward", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_forward);
  gtk_container_add (GTK_CONTAINER (button_forward), image_forward);

  toolitem_refresh = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem_refresh);
  gtk_container_add (GTK_CONTAINER (toolbar), toolitem_refresh);

  button_refresh = gtk_button_new ();
  gtk_widget_show (button_refresh);
  gtk_container_add (GTK_CONTAINER (toolitem_refresh), button_refresh);

  image_refresh = gtk_image_new_from_stock ("gtk-refresh", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_refresh);
  gtk_container_add (GTK_CONTAINER (button_refresh), image_refresh);

  toolitem_stop = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem_stop);
  gtk_container_add (GTK_CONTAINER (toolbar), toolitem_stop);

  button_stop = gtk_button_new ();
  gtk_widget_show (button_stop);
  gtk_container_add (GTK_CONTAINER (toolitem_stop), button_stop);

  image_stop = gtk_image_new_from_stock ("gtk-stop", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_stop);
  gtk_container_add (GTK_CONTAINER (button_stop), image_stop);

  toolitem_home = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem_home);
  gtk_container_add (GTK_CONTAINER (toolbar), toolitem_home);

  button_home = gtk_button_new ();
  gtk_widget_show (button_home);
  gtk_container_add (GTK_CONTAINER (toolitem_home), button_home);
  gtk_button_set_focus_on_click (GTK_BUTTON (button_home), FALSE);

  image_home = gtk_image_new_from_stock ("gtk-home", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_home);
  gtk_container_add (GTK_CONTAINER (button_home), image_home);

  toolitem_url = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem_url);
  gtk_tool_item_set_expand (GTK_TOOL_ITEM (toolitem_url), TRUE);
  gtk_container_add (GTK_CONTAINER (toolbar), toolitem_url);

  entry_url = gtk_entry_new ();
  gtk_widget_show (entry_url);
  gtk_container_add (GTK_CONTAINER (toolitem_url), entry_url);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry_url), 9679);

  toolitem_enter = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem_enter);
  gtk_container_add (GTK_CONTAINER (toolbar), toolitem_enter);

  button_enter = gtk_button_new ();
  gtk_widget_show (button_enter);
  gtk_container_add (GTK_CONTAINER (toolitem_enter), button_enter);

  image_enter = gtk_image_new_from_stock ("gtk-ok", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_enter);
  gtk_container_add (GTK_CONTAINER (button_enter), image_enter);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

  webview = webkit_web_view_new();
  gtk_widget_show(webview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), webview);

  notebook = gtk_notebook_new ();
  gtk_widget_show (notebook);
  gtk_box_pack_start (GTK_BOX (vbox), notebook, FALSE, FALSE, 0);
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook), FALSE);
  gtk_notebook_set_show_border (GTK_NOTEBOOK (notebook), FALSE);

  label_status = gtk_label_new ("");
  gtk_widget_show (label_status);
  gtk_container_add (GTK_CONTAINER (notebook), label_status);
  gtk_label_set_ellipsize (GTK_LABEL (label_status), PANGO_ELLIPSIZE_MIDDLE);

  progressbar = gtk_progress_bar_new ();
  gtk_widget_show (progressbar);
  gtk_container_add (GTK_CONTAINER (notebook), progressbar);
  gtk_progress_bar_set_ellipsize (GTK_PROGRESS_BAR (progressbar), PANGO_ELLIPSIZE_MIDDLE);

  // Set gui as if loading has finished, so that the sensitivitiy of buttons gets right.
  load_finished (NULL, NULL);

  // Signals.
  g_signal_connect ((gpointer) button_back, "clicked", G_CALLBACK (on_button_back_clicked), gpointer (this));
  g_signal_connect ((gpointer) button_forward, "clicked", G_CALLBACK (on_button_forward_clicked), gpointer (this));
  g_signal_connect ((gpointer) button_refresh, "clicked", G_CALLBACK (on_button_refresh_clicked), gpointer (this));
  g_signal_connect ((gpointer) button_stop, "clicked", G_CALLBACK (on_button_stop_clicked), gpointer (this));
  g_signal_connect ((gpointer) button_home, "clicked", G_CALLBACK (on_button_home_clicked), gpointer (this));
  g_signal_connect ((gpointer) entry_url, "activate", G_CALLBACK (on_entry_url_activate), gpointer (this));
  g_signal_connect ((gpointer) button_enter, "clicked", G_CALLBACK (on_button_enter_clicked), gpointer (this));
  g_signal_connect(G_OBJECT(webview), "grab_focus", G_CALLBACK(on_webview_grab_focus), gpointer(this));
  g_signal_connect (G_OBJECT (webview), "load-committed", G_CALLBACK (load_commit_cb), gpointer(this));
  g_signal_connect (G_OBJECT (webview), "load-started", G_CALLBACK (load_started_cb), gpointer(this));
  g_signal_connect (G_OBJECT (webview), "load-progress-changed", G_CALLBACK (progress_change_cb), gpointer(this));
  g_signal_connect (G_OBJECT (webview), "load-finished", G_CALLBACK (load_finished_cb), gpointer(this));
  g_signal_connect (G_OBJECT (webview), "hovering-over-link", G_CALLBACK (link_hover_cb), gpointer(this));
}


WebkitBrowser::~WebkitBrowser()
{
  gtk_widget_destroy(toolbar);
  gtk_widget_destroy(scrolledwindow);
  gtk_widget_destroy(notebook);
}


void WebkitBrowser::focus()
{
  gtk_widget_grab_focus(webview);
}


bool WebkitBrowser::focused()
{
  return GTK_WIDGET_HAS_FOCUS(webview);
}


void WebkitBrowser::copy()
{
  if (focused()) {
    webkit_web_view_copy_clipboard (WEBKIT_WEB_VIEW (webview));
  }
}


void WebkitBrowser::go_to(const ustring & url)
{
  webkit_web_view_open (WEBKIT_WEB_VIEW(webview), url.c_str());
}


void WebkitBrowser::on_webview_grab_focus(GtkWidget * widget, gpointer user_data)
{
  ((WebkitBrowser *) user_data)->webview_grab_focus();
}


void WebkitBrowser::webview_grab_focus()
// This function records the most recent time that the browser received focus.
{
  last_focused_time = time(0);
}


void WebkitBrowser::on_button_back_clicked (GtkButton *button, gpointer user_data)
{
  ((WebkitBrowser *) user_data)->on_button_back();
}


void WebkitBrowser::on_button_back ()
{
  webkit_web_view_go_back (WEBKIT_WEB_VIEW(webview));
}


void WebkitBrowser::on_button_forward_clicked (GtkButton *button, gpointer user_data)
{
  ((WebkitBrowser *) user_data)->on_button_forward();
}


void WebkitBrowser::on_button_forward ()
{
  webkit_web_view_go_forward (WEBKIT_WEB_VIEW(webview));
}


void WebkitBrowser::on_button_refresh_clicked (GtkButton *button, gpointer user_data)
{
  ((WebkitBrowser *) user_data)->on_entry_url();
}


void WebkitBrowser::on_button_refresh ()
{
  webkit_web_view_reload (WEBKIT_WEB_VIEW(webview));
}


void WebkitBrowser::on_button_stop_clicked (GtkButton *button, gpointer user_data)
{
  ((WebkitBrowser *) user_data)->on_button_stop();
}


void WebkitBrowser::on_button_stop ()
{
  webkit_web_view_stop_loading (WEBKIT_WEB_VIEW(webview));
  load_finished (NULL, NULL);
}


void WebkitBrowser::on_button_home_clicked (GtkButton *button, gpointer user_data)
{
  ((WebkitBrowser *) user_data)->on_button_home();
}


void WebkitBrowser::on_button_home ()
{
  if (!homepage.empty()) {
    go_to (homepage);
  }
}


void WebkitBrowser::on_entry_url_activate (GtkEntry *entry, gpointer user_data)
{
  ((WebkitBrowser *) user_data)->on_entry_url();
}


void WebkitBrowser::on_button_enter_clicked (GtkButton *button, gpointer user_data)
{
  ((WebkitBrowser *) user_data)->on_entry_url();
}

void WebkitBrowser::on_entry_url ()
{
  const gchar* uri = gtk_entry_get_text (GTK_ENTRY (entry_url));
  g_assert (uri);
  go_to (uri);
}


void WebkitBrowser::load_commit_cb (WebKitWebView* page, WebKitWebFrame* frame, gpointer data)
{
  ((WebkitBrowser *) data)->load_commit(page, frame);
}


void WebkitBrowser::load_commit (WebKitWebView* page, WebKitWebFrame* frame)
{
  const gchar* uri = webkit_web_frame_get_uri(frame);
  if (uri)
    gtk_entry_set_text (GTK_ENTRY (entry_url), uri);
}


void WebkitBrowser::progress_change_cb (WebKitWebView* page, gint progress, gpointer data)
{
  ((WebkitBrowser *) data)->progress_change(page, progress);
}


void WebkitBrowser::progress_change (WebKitWebView* page, gint progress)
{
  double percentage = CLAMP (progress, 0, 100);
  double fraction = percentage / 100;
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), fraction);
}


void WebkitBrowser::link_hover_cb (WebKitWebView* page, const gchar* title, const gchar* link, gpointer data)
{
  ((WebkitBrowser *) data)->link_hover (page, title, link);
}

void WebkitBrowser::link_hover (WebKitWebView* page, const gchar* title, const gchar* link)
{
  gtk_label_set_text (GTK_LABEL (label_status), link);
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar), link);
}


void WebkitBrowser::set_home_page (const ustring& url)
{
  homepage = url;
}


void WebkitBrowser::load_started_cb (WebKitWebView *page, WebKitWebFrame *frame, gpointer user_data)
{
  ((WebkitBrowser *) user_data)->load_started (page, frame);
}


void WebkitBrowser::load_started (WebKitWebView *page, WebKitWebFrame *frame)
{
  gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), 1);
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), 0);
  gtk_widget_set_sensitive (button_refresh, false);
  gtk_widget_set_sensitive (button_stop, true);
  gtk_widget_set_sensitive (button_back, webkit_web_view_can_go_back (WEBKIT_WEB_VIEW (webview)));
  gtk_widget_set_sensitive (button_forward, webkit_web_view_can_go_forward (WEBKIT_WEB_VIEW (webview)));
}


void WebkitBrowser::load_finished_cb (WebKitWebView *page, WebKitWebFrame *frame, gpointer user_data)
{
  ((WebkitBrowser *) user_data)->load_finished (page, frame);
}


void WebkitBrowser::load_finished (WebKitWebView *page, WebKitWebFrame *frame)
{
  gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), 0);
  gtk_widget_set_sensitive (button_refresh, true);
  gtk_widget_set_sensitive (button_stop, false);
  gtk_widget_set_sensitive (button_back, webkit_web_view_can_go_back (WEBKIT_WEB_VIEW (webview)));
  gtk_widget_set_sensitive (button_forward, webkit_web_view_can_go_forward (WEBKIT_WEB_VIEW (webview)));
}


