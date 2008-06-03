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
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **  
 */

#include "utilities.h"
#include <glib.h>
#include "resource.h"
#include "settings.h"
#include "tiny_utilities.h"
#include "gwrappers.h"
#include "directories.h"
#include "books.h"
#include "resource_utils.h"
extern "C" {
#include <gtkhtml/gtkhtml.h>
}

Resource::Resource(GtkWidget * vbox, GtkWidget * notebook_page, GtkWidget * tab_label) {
  // Save and initialize varables.
  my_vbox = vbox;
  my_notebook_page = notebook_page;
  my_tab_label = tab_label;
  resource_type = rtEnd;
  last_focused = false;

  // Build GUI.
  GtkWidget *hbox;
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox);
  gtk_box_pack_start(GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

  progressbar = gtk_progress_bar_new();
  gtk_widget_show(progressbar);
  gtk_box_pack_start(GTK_BOX (hbox), progressbar, true, true, 0);
  gtk_progress_bar_set_ellipsize(GTK_PROGRESS_BAR (progressbar), PANGO_ELLIPSIZE_MIDDLE);

  homebutton = gtk_button_new();
  gtk_widget_show(homebutton);
  gtk_box_pack_start(GTK_BOX (hbox), homebutton, false, false, 0);
  gtk_button_set_focus_on_click(GTK_BUTTON (homebutton), FALSE);

  GtkWidget *image;
  image = gtk_image_new_from_stock("gtk-home", GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show(image);
  gtk_container_add(GTK_CONTAINER (homebutton), image);

  browser = new GtkHtml3Browser (vbox);

  g_signal_connect ((gpointer) homebutton, "clicked", G_CALLBACK (on_homebutton_clicked), gpointer(this));
}

Resource::~Resource() {
  delete browser;
  gtk_widget_destroy(my_vbox);
}

void Resource::focus() {
  browser->focus();
}

bool Resource::focused() {
  return browser->focused();
}

void Resource::parent_notebook_switches_to_page(GtkWidget * page) {
  if (page == my_notebook_page) {
    focus();
  }
}

void Resource::copy() {
  browser->copy(); // Todo try out. also copy of other widgets in bibledit.
}

void Resource::go_to(const Reference& reference) {
  ustring url;
  switch (resource_type)
  {
    case rtForeignDataURLForEachVerse:
    case rtURLForEachVerse:
    {
      url = resource_construct_url(url_structure, book_renderer, reference);
      break;
    }
    case rtURLForEachVerseAboveURLFilterBelowWithDifferentAnchors:
    {
      url = resource_construct_url(url_structure, book_renderer, anchor_renderer, reference);
      break;
    }
    case rtEnd:
    {
      break;
    }
  }
  if (!url.empty()) {
    browser->go_to(url);
  }
}

void Resource::open(const ustring& filename) {
  mytemplatefile = filename;
  resource_type = resource_get_type(filename);
  url_structure = resource_url_modifier(resource_get_url_constructor(filename), resource_type, filename);
  index_file_structure = resource_get_index_file_constructor(filename);
  book_renderer = resource_get_books(filename);
  anchor_renderer = resource_get_anchors(filename);
  gtk_progress_bar_set_text(GTK_PROGRESS_BAR (progressbar), resource_get_title(filename).c_str());
  homepage = resource_url_modifier(resource_get_home_page(filename), resource_type, filename);
  homebutton_clicked();
}

ustring Resource::template_get() {
  return mytemplatefile;
}

GtkWidget * Resource::parent_notebook_page() {
  return my_notebook_page;
}

void Resource::on_homebutton_clicked(GtkButton *button, gpointer user_data) {
  ((Resource *) user_data)->homebutton_clicked();
}

void Resource::homebutton_clicked() {
  browser->go_to(homepage);
}

time_t Resource::last_focused_time() {
  return browser->last_focused_time;
}

/* Todo old code
 * gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR (progressbar), 0);
 */

/*
 * Frames will not be allowed, instead the Resource object uses two different browsers, the top one and the bottom one.
 * Each browser gets a filter assigned that, when a link of a kind is clicked, then it passes it back to the Resource
 * object, who wil deal with it.
 */

/*
 GtkWidget *vpaned;
 GtkWidget *vbox_top;
 GtkWidget *scrolledwindow_top;
 GtkWidget *textview_top;
 GtkWidget *vbox_bottom;
 GtkWidget *scrolledwindow_bottom;
 GtkWidget *textview_bottom;

 vpaned = gtk_vpaned_new ();
 gtk_widget_show (vpaned);
 gtk_box_pack_start (GTK_BOX (vbox1), vpaned, TRUE, TRUE, 0);
 gtk_paned_set_position (GTK_PANED (vpaned), 143);

 vbox_top = gtk_vbox_new (FALSE, 0);
 gtk_widget_show (vbox_top);
 gtk_paned_pack1 (GTK_PANED (vpaned), vbox_top, FALSE, TRUE);

 scrolledwindow_top = gtk_scrolled_window_new (NULL, NULL);
 gtk_widget_show (scrolledwindow_top);
 gtk_box_pack_start (GTK_BOX (vbox_top), scrolledwindow_top, TRUE, TRUE, 0);
 gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_top), GTK_SHADOW_IN);

 textview_top = gtk_text_view_new ();
 gtk_widget_show (textview_top);
 gtk_container_add (GTK_CONTAINER (scrolledwindow_top), textview_top);

 vbox_bottom = gtk_vbox_new (FALSE, 0);
 gtk_widget_show (vbox_bottom);
 gtk_paned_pack2 (GTK_PANED (vpaned), vbox_bottom, TRUE, TRUE);

 scrolledwindow_bottom = gtk_scrolled_window_new (NULL, NULL);
 gtk_widget_show (scrolledwindow_bottom);
 gtk_box_pack_start (GTK_BOX (vbox_bottom), scrolledwindow_bottom, TRUE, TRUE, 0);
 gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_bottom), GTK_SHADOW_IN);

 textview_bottom = gtk_text_view_new ();
 gtk_widget_show (textview_bottom);
 gtk_container_add (GTK_CONTAINER (scrolledwindow_bottom), textview_bottom);

 */


// Todo we need to have a system which progressbar of which resource to make blue, the one that has focus.
