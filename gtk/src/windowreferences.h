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


#ifndef INCLUDED_WINDOW_REFERENCES_H
#define INCLUDED_WINDOW_REFERENCES_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "reference.h"
#include "floatingwindow.h"
#include "htmlwriter2.h"
#include <webkit/webkit.h>


class WindowReferences : public FloatingWindow
{
public:
  WindowReferences(GtkWidget * parent_layout, GtkAccelGroup *accelerator_group, bool startup, bool reference_management_enabled);
  virtual ~WindowReferences();
  void set (vector <Reference>& refs, const ustring& project_in, vector <ustring> * comments_in);
  vector <Reference> get ();
  GtkWidget * signal_button;
  Reference reference;
  void save(const ustring& filename);
  void goto_next ();
  void goto_previous ();
  void copy ();
  void set_fonts ();
protected:
  GtkWidget *scrolledwindow;
  GtkWidget *webview;
private:
  void open();
  void load (const ustring & filename);
  void clear();
  static gboolean on_navigation_policy_decision_requested (WebKitWebView *web_view, WebKitWebFrame *frame, WebKitNetworkRequest *request, WebKitWebNavigationAction *navigation_action, WebKitWebPolicyDecision *policy_decision, gpointer user_data);
  void navigation_policy_decision_requested (WebKitNetworkRequest *request, WebKitWebNavigationAction *navigation_action, WebKitWebPolicyDecision *policy_decision);
  void load_webview (const gchar *url);
  ustring active_url;
  map <ustring, unsigned int> scrolling_position;
  void html_write_references (HtmlWriter2& htmlwriter);
  void html_write_action_bar (HtmlWriter2& htmlwriter, bool topbar);
  void html_write_action_page (HtmlWriter2& htmlwriter);
  vector <Reference> references;
  vector <ustring> comments;
  ustring project;
  ustring language;
  unsigned int lower_boundary;
  unsigned int upper_boundary;
  int active_entry;
  ustring references_database_filename ();
  void load ();
  void save ();
  void dismiss (bool cursor, bool all);
  ustring hide_string (unsigned int index);
  ustring hide_string (Reference& reference, ustring& comment);
  void goto_next_previous_internal (bool next);
  bool references_management_on;
};


#endif
