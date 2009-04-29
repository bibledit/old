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


Resource::Resource(GtkWidget * window)
{
  // Save and initialize varables.
  resource_type = rtEnd;

  // Build GUI.
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  browser = new WebkitBrowser(vbox);
}


Resource::~Resource()
{
  delete browser;
  gtk_widget_destroy(vbox);
}


void Resource::focus()
{
  browser->focus();
}


bool Resource::focused()
{
  return browser->focused();
}


void Resource::copy()
{
  browser->copy();
}


void Resource::go_to(const Reference & reference)
{
  ustring url;
  switch (resource_type) {
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


void Resource::open(const ustring & filename)
{
  mytemplatefile = filename;
  resource_type = resource_get_type(filename);
  url_structure = resource_url_modifier(resource_get_url_constructor(filename), resource_type, filename);
  index_file_structure = resource_get_index_file_constructor(filename);
  book_renderer = resource_get_books(filename);
  anchor_renderer = resource_get_anchors(filename);
  ustring homepage = resource_url_modifier(resource_get_home_page(filename), resource_type, filename);
  browser->set_home_page (homepage);
  browser->go_to(homepage);
  focus();
}


ustring Resource::template_get()
{
  return mytemplatefile;
}


time_t Resource::last_focused_time()
{
  return browser->last_focused_time;
}


