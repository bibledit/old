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

Resource::Resource(GtkWidget * window) {
  // Save and initialize varables.
  resource_type = rtEnd;
  browser2 = NULL;

  // Build GUI.
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  GtkWidget *hbox;
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox);
  gtk_box_pack_start(GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

  progressbar = gtk_progress_bar_new();
  gtk_widget_show(progressbar);
  gtk_box_pack_start(GTK_BOX (hbox), progressbar, true, true, 0);
  gtk_progress_bar_set_ellipsize(GTK_PROGRESS_BAR (progressbar), PANGO_ELLIPSIZE_MIDDLE);
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR (progressbar), 1);

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
  if (browser2)
    delete browser2;
  gtk_widget_destroy(vbox);
}

void Resource::focus() {
  browser->focus();
}

bool Resource::focused() {
  bool focus = false;
  if (browser->focused())
    focus = true;
  if (browser2)
    if (browser2->focused())
      focus = true;
  return focus;
}

void Resource::copy() {
  browser->copy();
  if (browser2)
    browser2->copy();
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
  homepage2 = resource_url_modifier(resource_get_lower_home_page(filename), resource_type, filename);
  url_filter = resource_get_lower_url_filter(filename);
  if (resource_type == rtURLForEachVerseAboveURLFilterBelowWithDifferentAnchors) {
    if (browser2 == NULL) {
      browser2 = new GtkHtml3Browser (vbox);
      browser->set_second_browser(url_filter, browser2);
    }
  }
  homebutton_clicked();
  focus();
}

ustring Resource::template_get() {
  return mytemplatefile;
}

void Resource::on_homebutton_clicked(GtkButton *button, gpointer user_data) {
  ((Resource *) user_data)->homebutton_clicked();
}

void Resource::homebutton_clicked() {
  browser->go_to(homepage);
  if (browser2)
    browser2->go_to(homepage2);
}

time_t Resource::last_focused_time() {
  return browser->last_focused_time;
}

