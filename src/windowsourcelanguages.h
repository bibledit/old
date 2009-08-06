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


#ifndef INCLUDED_WINDOW_SOURCE_LANGUAGES_H
#define INCLUDED_WINDOW_SOURCE_LANGUAGES_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "reference.h"
#include "window.h"
extern "C" {
#include <gtkhtml/gtkhtml.h>
}
#include "htmlwriter2.h"


class WindowSourceLanguages : public WindowBase
{
public:
  WindowSourceLanguages(GtkAccelGroup *accelerator_group, bool startup, GtkWidget * parent_box);
  virtual ~WindowSourceLanguages();
  void go_to (const Reference& reference_in, const ustring& project_in);
  GtkWidget * signal_button;
  Reference reference;
protected:
  GtkWidget *scrolledwindow;
  GtkWidget *htmlview;
private:
  static gboolean on_html_link_clicked(GtkHTML *html, const gchar * url, gpointer user_data);
  void html_link_clicked(const gchar * url);
  ustring active_url;
  map <ustring, unsigned int> scrolling_position;
  deque <ustring> url_stack;
  void html_write_home_page (HtmlWriter2& htmlwriter);
  void html_write_action_bar (HtmlWriter2& htmlwriter, bool topbar);
  void html_write_action_page (HtmlWriter2& htmlwriter);
  ustring project;
  ustring language;
};


#endif
