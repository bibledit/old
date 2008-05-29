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


#ifndef INCLUDED_BIBLEDIT_HELP_H
#define INCLUDED_BIBLEDIT_HELP_H


#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <gtk/gtk.h>
extern "C" {
  #include <gtkhtml/gtkhtml.h>
}
#include <curl/curl.h>


using namespace std;


GtkWidget *helpwindow;
GtkWidget *scrolledwindow;
GtkWidget *htmlview;
CURL *curl;

struct MemoryStruct {
  char *memory;
  size_t size;
};


int main (int argc, char *argv[]);
static gboolean on_html_url_requested (GtkHTML *html, const gchar *url, GtkHTMLStream *handle, gpointer user_data);
static gboolean on_html_link_clicked (GtkHTML *html, const gchar * url, gpointer user_data);


#endif
