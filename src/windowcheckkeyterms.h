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


#ifndef INCLUDED_WINDOW_CHECK_KEYTERMS_H
#define INCLUDED_WINDOW_CHECK_KEYTERMS_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "reference.h"
#include "window.h"
extern "C" {
#include <gtkhtml/gtkhtml.h>
}
#include "htmlwriter2.h"


class WindowCheckKeyterms : public WindowBase
{
public:
  WindowCheckKeyterms(GtkAccelGroup *accelerator_group, bool startup, GtkWidget * parent_box);
  virtual ~WindowCheckKeyterms();
  void go_to(const ustring& project, Reference& reference);
  void copy_clipboard();
  Reference * new_reference_showing;
  GtkWidget *signal;
  ustring collection ();
protected:
  GtkWidget *vbox;
private:
  Reference myreference;
  
  // Widgets.
  GtkWidget *hbox_collection;
  GtkWidget *label_collection;
  GtkWidget *combobox_collection;
  GtkWidget *label_list;
  GtkWidget *scrolledwindow_terms;
  GtkWidget *htmlview_terms;
  GtkWidget *treeview_renderings;

  // Underlying constructions.
  GtkTreeStore *treestore_renderings;
  GtkTreeSelection *treeselect_renderings;

  // Callbacks.
  static gboolean on_html_link_clicked(GtkHTML *html, const gchar * url, gpointer user_data);
  void html_link_clicked(const gchar * url);
  static void on_entry_keyterm_changed(GtkEditable *editable, gpointer user_data);
  static void on_entry_keyterm_activate(GtkEntry *entry, gpointer user_data);
  static void on_combobox_keyterm_collection_changed(GtkComboBox *combobox, gpointer user_data);
  static void keyterm_whole_word_toggled(GtkCellRendererToggle *cell, gchar *path_str, gpointer data);
  static void keyterm_case_sensitive_toggled(GtkCellRendererToggle *cell, gchar *path_str, gpointer data);
  static void cell_edited(GtkCellRendererText *cell, const gchar *path_string, const gchar *new_text, gpointer data);

  // Action routines.
  void on_combobox_keyterm_collection();
  void load_renderings();
  void save_renderings();
  void clear_renderings();
  void on_rendering_toggle(GtkCellRendererToggle *cell, gchar *path_str, bool first_toggle);
  void on_cell_edited(GtkCellRendererText *cell, const gchar *path_string, const gchar *new_text);
  void add_to_renderings(const ustring& rendering, bool wholeword);
  bool find_renderings (const ustring& text, const vector <ustring>& renderings, const vector <bool>& wholewords, const vector <bool>& casesensitives, vector <size_t> * startpositions, vector <size_t> * lengths);
  gboolean on_textview_keyterm_text_button_press(GdkEventButton *event);
  gboolean on_textview_keyterm_text_button_release(GdkEventButton *event);
  vector <ustring> keyterm_text_selection;
  gboolean on_textview_keyterm_text_key_press(GdkEventKey *event);

  // Data routines.
  ustring enter_new_rendering_here();
  void get_renderings(vector <ustring>& renderings, vector<bool>& wholewords, vector<bool>& casesensitives);
  Reference get_reference (const ustring& text);

  // Variables.
  unsigned int keyword_id;
  
  // Html work.
  ustring active_url;
  map <ustring, unsigned int> scrolling_position;
  void html_write_extras (HtmlWriter2& htmlwriter, unsigned int keyword_id);
  
};


#endif
