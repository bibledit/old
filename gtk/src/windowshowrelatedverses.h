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


#ifndef INCLUDED_WINDOW_SHOW_RELATED_VERSES_H
#define INCLUDED_WINDOW_SHOW_RELATED_VERSES_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "reference.h"
#include "floatingwindow.h"
extern "C" {
#include <gtkhtml/gtkhtml.h>
}
#include "htmlwriter2.h"


enum RelatedItemType {ritNone, ritKeytermId, ritStrongNumber, ritParallels};


class WindowShowRelatedVerses : public FloatingWindow
{
public:
  WindowShowRelatedVerses(GtkWidget * parent_layout, GtkAccelGroup *accelerator_group, bool startup);
  virtual ~WindowShowRelatedVerses();
  void go_to(const ustring& project, const Reference& reference);
  GtkWidget *button_item;
  RelatedItemType item_type;
  ustring item_id;
protected:
  GtkWidget *scrolledwindow;
  GtkWidget *htmlview;
private:
  Reference myreference;
  ustring myproject;
  static gboolean on_html_link_clicked(GtkHTML *html, const gchar * url, gpointer user_data);
  void html_link_clicked(const gchar * url);
  ustring active_url;
  map <ustring, unsigned int> scrolling_position;
  static void thread_start(gpointer data);
  void thread_main(gpointer data);
  bool thread_runs;
  guint event_id;
  static bool on_timeout(gpointer user_data);
  bool timeout();
};


#endif
