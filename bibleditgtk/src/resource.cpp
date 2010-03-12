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


Resource::Resource(GtkWidget * window) :
myreference (0)
{
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  browser = new WebkitBrowser(vbox);
  g_signal_connect ((gpointer) browser->button_home, "clicked", G_CALLBACK (on_button_home_clicked), gpointer (this));
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
  ustring url = url_structure;
  url = resource_url_enter_reference(url, book_renderer, book_renderer2, reference);
  browser->go_to(url);
  myreference.assign (reference);
}


void Resource::open(const ustring & filename)
{
  mytemplatefile = filename;
  url_structure = resource_url_get (resource_get_url_constructor(filename), mytemplatefile);
  book_renderer = resource_get_books(filename);
  book_renderer2 = resource_get_books2(filename);
  homepage = resource_url_get(resource_get_home_page(filename), mytemplatefile);
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


void Resource::on_button_home_clicked (GtkButton *button, gpointer user_data)
{
  ((Resource *) user_data)->on_button_home();
}


void Resource::on_button_home ()
{
  ustring url = resource_url_enter_reference(homepage, book_renderer, book_renderer2, myreference);
  browser->go_to(url);
}


