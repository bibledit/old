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

#include "libraries.h"
#include <glib.h>
#include "windowcheckkeyterms.h"
#include "help.h"
#include "windows.h"
#include "keyterms.h"
#include "tiny_utilities.h"
#include "utilities.h"
#include <gdk/gdkkeysyms.h>
#include "combobox.h"
#include "settings.h"
#include "projectutils.h"
#include "categorize.h"
#include "mapping.h"

 WindowCheckKeyterms::WindowCheckKeyterms(GtkAccelGroup * accelerator_group, bool startup):
WindowBase(widCheckKeyterms, "Check keyterms", startup, 0), myreference(0)
// Window for checking keyterms.
{
  // Save / initialize variables.
  previous_reference_id = 0;

  // Build gui.
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  label23 = gtk_label_new_with_mnemonic("Ke_yterm");
  gtk_widget_show(label23);
  gtk_box_pack_start(GTK_BOX(vbox), label23, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label23), 0, 0.5);

  entry_keyterm = gtk_entry_new();
  gtk_widget_show(entry_keyterm);
  gtk_box_pack_start(GTK_BOX(vbox), entry_keyterm, FALSE, FALSE, 0);
  gtk_entry_set_invisible_char(GTK_ENTRY(entry_keyterm), 8226);

  g_signal_connect((gpointer) entry_keyterm, "visibility-notify-event", G_CALLBACK(on_visibility_notify_event), gpointer(this));

  // Produce the signal to be given on a new reference.
  signal = gtk_button_new();
  gtk_box_pack_start(GTK_BOX(vbox), signal, FALSE, FALSE, 0);

  label24 = gtk_label_new_with_mnemonic("_Collection");
  gtk_widget_show(label24);
  gtk_box_pack_start(GTK_BOX(vbox), label24, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label24), 0, 0.5);

  combobox_collection = gtk_combo_box_new_text();
  gtk_widget_show(combobox_collection);
  gtk_box_pack_start(GTK_BOX(vbox), combobox_collection, FALSE, TRUE, 0);

  g_signal_connect((gpointer) combobox_collection, "visibility-notify-event", G_CALLBACK(on_visibility_notify_event), gpointer(this));

  label25 = gtk_label_new_with_mnemonic("_List");
  gtk_widget_show(label25);
  gtk_box_pack_start(GTK_BOX(vbox), label25, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label25), 0, 0.5);

  scrolledwindow_keyterm = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow_keyterm);
  gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow_keyterm, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow_keyterm), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow_keyterm), GTK_SHADOW_IN);

  // Store and view for the keywords.
  treestore_keywords = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_INT);
  treeview_keyterm = gtk_tree_view_new_with_model(GTK_TREE_MODEL(treestore_keywords));
  gtk_widget_show(treeview_keyterm);
  gtk_container_add(GTK_CONTAINER(scrolledwindow_keyterm), treeview_keyterm);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview_keyterm), FALSE);

  g_signal_connect((gpointer) treeview_keyterm, "visibility-notify-event", G_CALLBACK(on_visibility_notify_event), gpointer(this));

  // Renderer, column and selection.
  GtkCellRenderer *renderer_keywords = gtk_cell_renderer_text_new();
  treecolumn_keywords = gtk_tree_view_column_new_with_attributes("", renderer_keywords, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview_keyterm), treecolumn_keywords);
  treeselect_keywords = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_keyterm));
  gtk_tree_selection_set_mode(treeselect_keywords, GTK_SELECTION_SINGLE);

  scrolledwindow_comments = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow_comments);
  gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow_comments, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow_comments), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow_comments), GTK_SHADOW_IN);

  textview_comments = gtk_text_view_new();
  gtk_widget_show(textview_comments);
  gtk_container_add(GTK_CONTAINER(scrolledwindow_comments), textview_comments);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textview_comments), FALSE);
  gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(textview_comments), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview_comments), GTK_WRAP_WORD);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textview_comments), FALSE);

  g_signal_connect((gpointer) textview_comments, "visibility-notify-event", G_CALLBACK(on_visibility_notify_event), gpointer(this));

  scrolledwindow_renderings = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow_renderings);
  gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow_renderings, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow_renderings), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow_renderings), GTK_SHADOW_IN);

  // Store for the renderings.
  treestore_renderings = gtk_tree_store_new(4, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_BOOLEAN);

  treeview_renderings = gtk_tree_view_new_with_model(GTK_TREE_MODEL(treestore_renderings));
  gtk_widget_show(treeview_renderings);
  gtk_container_add(GTK_CONTAINER(scrolledwindow_renderings), treeview_renderings);

  g_signal_connect((gpointer) treeview_renderings, "visibility-notify-event", G_CALLBACK(on_visibility_notify_event), gpointer(this));

  // Renderer, column and selection.
  GtkCellRenderer *renderer_renderings = gtk_cell_renderer_toggle_new();
  g_signal_connect(renderer_renderings, "toggled", G_CALLBACK(keyterm_whole_word_toggled), gpointer(this));
  GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("Whole\nword", renderer_renderings, "active", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview_renderings), column);
  renderer_renderings = gtk_cell_renderer_toggle_new();
  g_signal_connect(renderer_renderings, "toggled", G_CALLBACK(keyterm_case_sensitive_toggled), gpointer(this));
  column = gtk_tree_view_column_new_with_attributes("Case\nsensitive", renderer_renderings, "active", 1, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview_renderings), column);
  renderer_renderings = gtk_cell_renderer_text_new();
  g_signal_connect(renderer_renderings, "edited", G_CALLBACK(cell_edited), gpointer(this));
  column = gtk_tree_view_column_new_with_attributes("Rendering", renderer_renderings, "text", 2, "editable", 3, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview_renderings), column);
  treeselect_renderings = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_renderings));
  gtk_tree_selection_set_mode(treeselect_renderings, GTK_SELECTION_SINGLE);

  // Textview for the text to be checked.
  scrolledwindow_check_text = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow_check_text);
  gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow_check_text, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow_check_text), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow_check_text), GTK_SHADOW_IN);

  textview_check_text = gtk_text_view_new();
  gtk_widget_show(textview_check_text);
  gtk_container_add(GTK_CONTAINER(scrolledwindow_check_text), textview_check_text);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textview_check_text), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview_check_text), GTK_WRAP_WORD);

  g_signal_connect((gpointer) textview_check_text, "visibility-notify-event", G_CALLBACK(on_visibility_notify_event), gpointer(this));

  g_signal_connect((gpointer) entry_keyterm, "changed", G_CALLBACK(on_entry_keyterm_changed), gpointer(this));
  g_signal_connect((gpointer) entry_keyterm, "activate", G_CALLBACK(on_entry_keyterm_activate), gpointer(this));
  g_signal_connect((gpointer) combobox_collection, "changed", G_CALLBACK(on_combobox_keyterm_collection_changed), gpointer(this));
  g_signal_connect((gpointer) treeview_keyterm, "button_press_event", G_CALLBACK(on_treeview_keyterm_button_press_event), gpointer(this));
  g_signal_connect((gpointer) treeview_keyterm, "key_press_event", G_CALLBACK(on_treeview_keyterm_key_press_event), gpointer(this));
  g_signal_connect((gpointer) treeview_keyterm, "row_activated", G_CALLBACK(on_treeview_keyterm_row_activated), gpointer(this));
  g_signal_connect((gpointer) textview_check_text, "button_press_event", G_CALLBACK(on_textview_keyterm_text_button_press_event), gpointer(this));
  g_signal_connect((gpointer) textview_check_text, "button_release_event", G_CALLBACK(on_textview_keyterm_text_button_release_event), gpointer(this));
  g_signal_connect((gpointer) textview_check_text, "key_press_event", G_CALLBACK(on_textview_keyterm_text_key_press_event), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label23), entry_keyterm);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label24), combobox_collection);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label25), treeview_keyterm);

  // Load the categories.
  vector < ustring > categories = keyterms_get_categories();
  categories.push_back(all_categories());
  combobox_set_strings(combobox_collection, categories);
  combobox_set_string(combobox_collection, all_categories());

  // Zero the text tag for marking renderings.
  approved_rendering_tag = NULL;
  disapproved_rendering_tag = NULL;

  // Load the keyterms.
  on_entry_keyterm_change();
}

WindowCheckKeyterms::~WindowCheckKeyterms()
{
}

void WindowCheckKeyterms::go_to(const ustring & project, Reference & reference)
{
  // Bail out if there's no change in the reference.
  if (myreference.equals(reference))
    return;

  // Store the new reference in the object.
  myreference.assign(reference);
}

void WindowCheckKeyterms::copy_clipboard()
{
  if (GTK_WIDGET_HAS_FOCUS(textview_check_text)) {
    // Clipboard.
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_check_text));
    gtk_text_buffer_copy_clipboard(buffer, clipboard);

    // Get the selected text.
    GtkTextIter startiter, enditer;
    gtk_text_buffer_get_iter_at_mark(buffer, &startiter, gtk_text_buffer_get_insert(buffer));
    gtk_text_buffer_get_iter_at_mark(buffer, &enditer, gtk_text_buffer_get_selection_bound(buffer));
    gtk_text_iter_order(&startiter, &enditer);
    ustring word = gtk_text_buffer_get_text(buffer, &startiter, &enditer, false);
    add_to_renderings(word, false);
  }
}

void WindowCheckKeyterms::on_entry_keyterm_changed(GtkEditable * editable, gpointer user_data)
{
  ((WindowCheckKeyterms *) user_data)->on_entry_keyterm_change();
}

void WindowCheckKeyterms::on_entry_keyterm_activate(GtkEntry * entry, gpointer user_data)
{
  ((WindowCheckKeyterms *) user_data)->on_entry_keyterm_activated();
}

void WindowCheckKeyterms::on_combobox_keyterm_collection_changed(GtkComboBox * combobox, gpointer user_data)
{
  ((WindowCheckKeyterms *) user_data)->on_combobox_keyterm_collection();
}

gboolean WindowCheckKeyterms::on_treeview_keyterm_button_press_event(GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
  ((WindowCheckKeyterms *) user_data)->on_treeview_keyterm_button_press();
  return false;
}

gboolean WindowCheckKeyterms::on_treeview_keyterm_key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  ((WindowCheckKeyterms *) user_data)->on_treeview_keyterm_key_press();
  return false;
}

void WindowCheckKeyterms::on_treeview_keyterm_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
  ((WindowCheckKeyterms *) user_data)->on_treeview_keyterm_activated();
}

void WindowCheckKeyterms::keyterm_whole_word_toggled(GtkCellRendererToggle * cell, gchar * path_str, gpointer data)
{
  ((WindowCheckKeyterms *) data)->on_rendering_toggle(cell, path_str, true);
}

void WindowCheckKeyterms::keyterm_case_sensitive_toggled(GtkCellRendererToggle * cell, gchar * path_str, gpointer data)
{
  ((WindowCheckKeyterms *) data)->on_rendering_toggle(cell, path_str, false);
}

void WindowCheckKeyterms::cell_edited(GtkCellRendererText * cell, const gchar * path_string, const gchar * new_text, gpointer data)
{
  ((WindowCheckKeyterms *) data)->on_cell_edited(cell, path_string, new_text);
}

gboolean WindowCheckKeyterms::on_textview_keyterm_text_button_press_event(GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
  return ((WindowCheckKeyterms *) user_data)->on_textview_keyterm_text_button_press(event);
}

gboolean WindowCheckKeyterms::on_textview_keyterm_text_button_release_event(GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
  return ((WindowCheckKeyterms *) user_data)->on_textview_keyterm_text_button_release(event);
}

gboolean WindowCheckKeyterms::on_textview_keyterm_text_key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  return ((WindowCheckKeyterms *) user_data)->on_textview_keyterm_text_key_press(event);
}

void WindowCheckKeyterms::on_entry_keyterm_change()
{
  // Get the keyterm and collection.
  ustring keyterm = gtk_entry_get_text(GTK_ENTRY(entry_keyterm));
  ustring collection = combobox_get_active_string(combobox_collection);
  if (collection == all_categories())
    collection.clear();

  // Clear the store.
  gtk_tree_store_clear(treestore_keywords);

  // Project
  extern Settings *settings;
  myproject = settings->genconfig.project_get();

  // If the keyterm entered is viewed as a rendering, then get the keyterms that
  // have this rendering.
  deque < ustring > keyterms;
  if (keyterm.length() >= 3) {
    keyterms = keyterms_rendering_retrieve_terms(myproject, keyterm);
  }
  keyterms.push_front(keyterm);

  // Get the keyterms and devide them into four levels.
  vector < ustring > terms1, terms2, terms3, terms4;
  vector < unsigned int >parents2, parents3, parents4;
  vector < unsigned int >ids1, ids2, ids3, ids4;
  set < unsigned int >id_set;
  for (unsigned int i = 0; i < keyterms.size(); i++) {
    keyterm = keyterms[i];

    vector < ustring > terms;
    vector < unsigned int >levels;
    vector < unsigned int >parents;
    vector < unsigned int >ids;

    keyterms_get_terms(keyterm, collection, terms, levels, parents, ids);
    for (unsigned int i = 0; i < terms.size(); i++) {
      switch (levels[i]) {
      case 1:
        {
          terms1.push_back(terms[i]);
          ids1.push_back(ids[i]);
          break;
        }
      case 2:
        {
          terms2.push_back(terms[i]);
          parents2.push_back(parents[i]);
          ids2.push_back(ids[i]);
          break;
        }
      case 3:
        {
          terms3.push_back(terms[i]);
          parents3.push_back(parents[i]);
          ids3.push_back(ids[i]);
          break;
        }
      default:                 // Level 4 and higher.
        {
          terms4.push_back(terms[i]);
          parents4.push_back(parents[i]);
          ids4.push_back(ids[i]);
          break;
        }
      }
    }
  }

  // Going through the higher levels to the lower ones, if a lower level does
  // not have an id, specified as parent in a higher level, add that id and term.
  set < unsigned int >ids3set(ids3.begin(), ids3.end());
  for (unsigned int i = 0; i < parents4.size(); i++) {
    if (ids3set.find(parents4[i]) == ids3set.end()) {
      ustring term;
      unsigned int parent;
      if (keyterms_get_term(parents4[i], term, parent)) {
        terms3.push_back(term);
        ids3.push_back(parents4[i]);
        parents3.push_back(parent);
        ids3set.insert(parents4[i]);
      }
    }
  }
  set < unsigned int >ids2set(ids2.begin(), ids2.end());
  for (unsigned int i = 0; i < parents3.size(); i++) {
    if (ids2set.find(parents3[i]) == ids2set.end()) {
      ustring term;
      unsigned int parent;
      if (keyterms_get_term(parents3[i], term, parent)) {
        terms2.push_back(term);
        ids2.push_back(parents3[i]);
        parents2.push_back(parent);
        ids2set.insert(parents3[i]);
      }
    }
  }
  set < unsigned int >ids1set(ids1.begin(), ids1.end());
  for (unsigned int i = 0; i < parents2.size(); i++) {
    if (ids1set.find(parents2[i]) == ids1set.end()) {
      ustring term;
      unsigned int parent;
      if (keyterms_get_term(parents2[i], term, parent)) {
        terms1.push_back(term);
        ids1.push_back(parents2[i]);
        ids1set.insert(parents2[i]);
      }
    }
  }

  // Sort the terms.
  quick_sort(terms1, ids1, 0, terms1.size());

  // Display all the terms.  
  // First display them for level 1.
  for (unsigned int i1 = 0; i1 < terms1.size(); i1++) {
    GtkTreeIter iter1;
    gtk_tree_store_append(treestore_keywords, &iter1, NULL);
    gtk_tree_store_set(treestore_keywords, &iter1, 0, terms1[i1].c_str(), 1, ids1[i1], -1);
    GtkTreePath *path1;
    path1 = gtk_tree_model_get_path(GTK_TREE_MODEL(treestore_keywords), &iter1);
    gtk_tree_view_expand_row(GTK_TREE_VIEW(treeview_keyterm), path1, false);
    gtk_tree_path_free(path1);
    if (i1 == 0)
      gtk_tree_selection_select_iter(treeselect_keywords, &iter1);
    // Add possible terms at level 2.
    for (unsigned int i2 = 0; i2 < parents2.size(); i2++) {
      if (parents2[i2] == ids1[i1]) {
        GtkTreeIter iter2;
        gtk_tree_store_append(treestore_keywords, &iter2, &iter1);
        gtk_tree_store_set(treestore_keywords, &iter2, 0, terms2[i2].c_str(), 1, ids2[i2], -1);
        GtkTreePath *path2;
        path2 = gtk_tree_model_get_path(GTK_TREE_MODEL(treestore_keywords), &iter2);
        gtk_tree_view_expand_to_path(GTK_TREE_VIEW(treeview_keyterm), path2);
        gtk_tree_path_free(path2);
        // Add possible terms at level 3.
        for (unsigned int i3 = 0; i3 < parents3.size(); i3++) {
          if (parents3[i3] == ids2[i2]) {
            GtkTreeIter iter3;
            gtk_tree_store_append(treestore_keywords, &iter3, &iter2);
            gtk_tree_store_set(treestore_keywords, &iter3, 0, terms3[i3].c_str(), 1, ids3[i3], -1);
            GtkTreePath *path3;
            path3 = gtk_tree_model_get_path(GTK_TREE_MODEL(treestore_keywords), &iter3);
            gtk_tree_view_expand_to_path(GTK_TREE_VIEW(treeview_keyterm), path3);
            gtk_tree_path_free(path3);
            // Add possible terms at level 4.
            for (unsigned int i4 = 0; i4 < parents4.size(); i4++) {
              if (parents4[i4] == ids3[i3]) {
                GtkTreeIter iter4;
                gtk_tree_store_append(treestore_keywords, &iter4, &iter3);
                gtk_tree_store_set(treestore_keywords, &iter4, 0, terms4[i4].c_str(), 1, ids4[i4], -1);
                GtkTreePath *path4;
                path4 = gtk_tree_model_get_path(GTK_TREE_MODEL(treestore_keywords), &iter4);
                gtk_tree_view_expand_to_path(GTK_TREE_VIEW(treeview_keyterm), path4);
                gtk_tree_path_free(path4);
              }
            }
          }
        }
      }
    }
  }
}

void WindowCheckKeyterms::on_entry_keyterm_activated()
{
  gtk_widget_grab_focus(treeview_keyterm);
  show_information();
}

void WindowCheckKeyterms::on_combobox_keyterm_collection()
{
  on_entry_keyterm_change();
}

void WindowCheckKeyterms::on_treeview_keyterm_button_press()
{
  //  on_treeview_change ();
}

void WindowCheckKeyterms::on_treeview_keyterm_key_press()
{
  //  on_treeview_change ();
}

void WindowCheckKeyterms::on_treeview_keyterm_activated()
{
  show_information();
  gtk_widget_grab_focus(textview_check_text);
  myid = selected_id();
  load_renderings();
  show_text();
  check_text();
}

void WindowCheckKeyterms::on_treeview_change()
{
  g_timeout_add(1, GSourceFunc(treeview_changed_timeout), gpointer(this));
}

bool WindowCheckKeyterms::treeview_changed_timeout(gpointer data)
{
  ((WindowCheckKeyterms *) data)->show_information();
  return false;
}

void WindowCheckKeyterms::show_information()
// Show all data for the selected keyword. Most important data at the top.
{
  unsigned int id = selected_id();
  ustring category;
  ustring information;
  vector < Reference > references;
  vector < ustring > related;
  keyterms_get_data(id, category, information, references, related);
  ustring information1;
  string_append_line(information1, "Category: " + category);
  for (unsigned int i = 0; i < related.size(); i++)
    string_append_line(information1, "Related to: " + related[i]);
  if (references.size() == 0)
    string_append_line(information1, "This one has no references of its own. Select one of its descendants.");
  string_append_line(information1, information);
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_comments));
  gtk_text_buffer_set_text(buffer, information1.c_str(), -1);
}

void WindowCheckKeyterms::load_renderings()
{
  extern Settings *settings;
  myproject = settings->genconfig.project_get();
  ProjectConfiguration *projectconfig = settings->projectconfig(myproject);
  myversification = projectconfig->versification_get();
  ustring keyterm;
  unsigned int dummy;
  keyterms_get_term(myid, keyterm, dummy);
  vector < ustring > renderings;
  vector < bool > wholewords;
  vector < bool > casesensitives;
  ustring category;
  {
    ustring dummy1;
    vector < Reference > dummy2;
    vector < ustring > dummy3;
    keyterms_get_data(myid, category, dummy1, dummy2, dummy3);
  }
  keyterms_retrieve_renderings(myproject, keyterm, category, renderings, wholewords, casesensitives);
  gtk_tree_store_clear(treestore_renderings);
  GtkTreeIter iter;
  for (unsigned int i = 0; i < renderings.size(); i++) {
    gtk_tree_store_append(treestore_renderings, &iter, NULL);
    bool wholeword = wholewords[i];
    bool casesensitive = casesensitives[i];
    gtk_tree_store_set(treestore_renderings, &iter, 0, wholeword, 1, casesensitive, 2, renderings[i].c_str(), 3, 1, -1);
  }
  gtk_tree_store_append(treestore_renderings, &iter, NULL);
  gtk_tree_store_set(treestore_renderings, &iter, 0, false, 1, true, 2, enter_new_rendering_here().c_str(), 3, 1, -1);
}

void WindowCheckKeyterms::save_renderings()
{
  vector < ustring > renderings;
  vector < bool > wholewords;
  vector < bool > casesensitives;
  get_renderings(renderings, wholewords, casesensitives);
  ustring keyterm;
  unsigned int dummy;
  keyterms_get_term(myid, keyterm, dummy);
  ustring category;
  {
    ustring dummy1;
    vector < Reference > dummy2;
    vector < ustring > dummy3;
    keyterms_get_data(myid, category, dummy1, dummy2, dummy3);
  }
  keyterms_store_renderings(myproject, keyterm, category, renderings, wholewords, casesensitives);
  load_renderings();
  check_text();
}

void WindowCheckKeyterms::on_rendering_toggle(GtkCellRendererToggle * cell, gchar * path_str, bool first_toggle)
{
  unsigned int column = 1;
  if (first_toggle)
    column = 0;
  GtkTreeModel *model = (GtkTreeModel *) treestore_renderings;
  GtkTreeIter iter;
  GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
  gboolean setting;
  gtk_tree_model_get_iter(model, &iter, path);
  gtk_tree_model_get(model, &iter, column, &setting, -1);
  setting = !setting;
  gtk_tree_store_set(treestore_renderings, &iter, column, setting, -1);
  gtk_tree_path_free(path);
  save_renderings();
}

void WindowCheckKeyterms::on_cell_edited(GtkCellRendererText * cell, const gchar * path_string, const gchar * new_text)
{
  GtkTreeModel *model = (GtkTreeModel *) treestore_renderings;
  GtkTreePath *path = gtk_tree_path_new_from_string(path_string);
  GtkTreeIter iter;
  gtk_tree_model_get_iter(model, &iter, path);
  gchar *old_text;
  gtk_tree_model_get(model, &iter, 2, &old_text, -1);
  g_free(old_text);
  gtk_tree_store_set(treestore_renderings, &iter, 2, g_strdup(new_text), -1);
  gtk_tree_path_free(path);
  save_renderings();
}

void WindowCheckKeyterms::add_to_renderings(const ustring & rendering, bool wholeword)
// Adds "rendering" to renderings. If it contains any capitals, the 
// casesensitive is set too.
{
  extern Settings *settings;
  myproject = settings->genconfig.project_get();
  ustring keyterm;
  unsigned int dummy;
  keyterms_get_term(myid, keyterm, dummy);
  GtkTreeIter iter;
  gtk_tree_store_append(treestore_renderings, &iter, NULL);
  bool casesensitive = rendering != rendering.casefold();
  gtk_tree_store_set(treestore_renderings, &iter, 0, wholeword, 1, casesensitive, 2, rendering.c_str(), 3, 1, -1);
  save_renderings();
}

void WindowCheckKeyterms::show_text()
// Loads the text into the textview.
{
  // Get data about the project.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());

  // Clear buffer.
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_check_text));
  gtk_text_buffer_set_text(buffer, "", 0);
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_start_iter(buffer, &startiter);
  gtk_text_buffer_get_end_iter(buffer, &enditer);
  gtk_text_buffer_remove_all_tags(buffer, &startiter, &enditer);

  // Get all the references.
  ustring dummy1;
  vector < ustring > dummy2;
  myreferences.clear();
  mytextstarts.clear();
  mytextends.clear();
  keyterms_get_data(myid, dummy1, dummy1, myreferences, dummy2);

  // Remap references.
  {
    vector < Reference > references_temp;
    for (unsigned int i = 0; i < myreferences.size(); i++) {
      Mapping mapping(myversification, myreferences[i].book);
      vector < int >chapters;
      vector < int >verses;
      mapping.original_to_me(myreferences[i].chapter, myreferences[i].verse, chapters, verses);
      for (unsigned int i2 = 0; i2 < chapters.size(); i2++) {
        Reference reference(myreferences[i].book, chapters[i2], convert_to_string(verses[i2]));
        references_temp.push_back(reference);
      }
    }
    myreferences = references_temp;
  }

  // Display text of the references.
  ustring text;
  for (unsigned int i = 0; i < myreferences.size(); i++) {

    // Get the starting offset.
    gint start_reference_offset;
    gtk_text_buffer_get_end_iter(buffer, &startiter);
    start_reference_offset = gtk_text_iter_get_offset(&startiter);

    // Display the reference.
    ustring reference = myreferences[i].human_readable(projectconfig->language_get());
    reference.append(" ");
    gtk_text_buffer_insert(buffer, &startiter, reference.c_str(), -1);

    // Insert the verse text, store the marks.
    ustring verse = project_retrieve_verse(settings->genconfig.project_get(), myreferences[i].book, myreferences[i].chapter, myreferences[i].verse);
    if (verse.empty()) {
      verse.append("<empty>");
    } else {
      CategorizeLine cl(verse);
      cl.remove_verse_number(myreferences[i].verse);
      verse = cl.verse;
    }
    verse.append("\n");
    gtk_text_buffer_get_end_iter(buffer, &startiter);
    gint offset;
    offset = gtk_text_iter_get_offset(&startiter);
    mytextstarts.push_back(offset);
    gtk_text_buffer_insert(buffer, &startiter, verse.c_str(), -1);
    gtk_text_buffer_get_end_iter(buffer, &startiter);
    offset = gtk_text_iter_get_offset(&startiter);
    mytextends.push_back(offset);

    // Tag the whole area with an id equal to the reference offset + 1.
    // This is for later, that we can find to which reference the cursorlocation
    // belongs.
    GtkTextTag *tag;
    tag = gtk_text_buffer_create_tag(buffer, NULL, NULL);
    g_object_set_data(G_OBJECT(tag), "id", GINT_TO_POINTER(i + 1));
    gtk_text_buffer_get_iter_at_offset(buffer, &startiter, start_reference_offset);
    gtk_text_buffer_get_end_iter(buffer, &enditer);
    gtk_text_buffer_apply_tag(buffer, tag, &startiter, &enditer);
  }
  // Move the cursor to the beginning of the buffer.
  gtk_text_buffer_get_start_iter(buffer, &startiter);
  gtk_text_buffer_place_cursor(buffer, &startiter);
  // Initialize previous reference's id.
  previous_reference_id = G_MAXINT;
}

void WindowCheckKeyterms::check_text()
// Modifies the buttons, images, and tags the text, depending on whether the 
// text contains the renderings.
{
  // Retrieve the renderings.
  vector < ustring > renderings;
  vector < bool > wholewords;
  vector < bool > casesensitives;
  get_renderings(renderings, wholewords, casesensitives);

  // Get the textbuffer.
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_check_text));

  // Create the text tag for marking renderings, if it's not yet there.
  if (!approved_rendering_tag) {
    approved_rendering_tag = gtk_text_buffer_create_tag(buffer, NULL, "background", "khaki", NULL);
  }
  if (!disapproved_rendering_tag) {
    disapproved_rendering_tag = gtk_text_buffer_create_tag(buffer, NULL, "background", "red", NULL);
  }
  // Remove all occurrences of this tag.
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_start_iter(buffer, &startiter);
  gtk_text_buffer_get_end_iter(buffer, &enditer);
  gtk_text_buffer_remove_tag(buffer, approved_rendering_tag, &startiter, &enditer);
  gtk_text_buffer_remove_tag(buffer, disapproved_rendering_tag, &startiter, &enditer);

  // Go through all text we have.
  for (unsigned int i = 0; i < mytextstarts.size(); i++) {

    // Get iterators of these offsets.
    gtk_text_buffer_get_iter_at_offset(buffer, &startiter, mytextstarts[i]);
    gtk_text_buffer_get_iter_at_offset(buffer, &enditer, mytextends[i]);

    // Get the text of the verse.
    ustring text = gtk_text_buffer_get_text(buffer, &startiter, &enditer, false);

    // If at least one of the renderings is found, then the text will be approved.
    // Visibly mark the correct locations.
    bool approved = false;
    for (unsigned int i2 = 0; i2 < renderings.size(); i2++) {

      ustring rendering = renderings[i2];
      bool wholeword = wholewords[i2];
      bool casesensitive = casesensitives[i2];

      ustring mytext;
      ustring myrendering;
      if (casesensitive) {
        mytext = text;
        myrendering = rendering;
      } else {
        mytext = text.casefold();
        myrendering = rendering.casefold();
      }
      size_t position = mytext.find(myrendering);
      while (position != string::npos) {
        bool myapproved = true;
        GtkTextIter approvedstart = startiter;
        GtkTextIter approvedend;
        gtk_text_iter_forward_chars(&approvedstart, position);
        approvedend = approvedstart;
        gtk_text_iter_forward_chars(&approvedend, rendering.length());
        if (wholeword) {
          if (!gtk_text_iter_starts_word(&approvedstart))
            myapproved = false;
          if (!gtk_text_iter_ends_word(&approvedend))
            myapproved = false;
        }
        if (myapproved) {
          approved = true;
          gtk_text_buffer_apply_tag(buffer, approved_rendering_tag, &approvedstart, &approvedend);
        }
        position = mytext.find(myrendering, ++position);
      }
    }
    // Mark text as disapproved.
    if (!approved) {
      gtk_text_buffer_apply_tag(buffer, disapproved_rendering_tag, &startiter, &enditer);
    }
  }
}

gboolean WindowCheckKeyterms::on_textview_keyterm_text_button_press(GdkEventButton * event)
{
  // Double-clicking adds the word to the renderings.
  if (event->type == GDK_2BUTTON_PRESS) {
    // Get the textbuffer.
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_check_text));
    // Get the word.
    GtkTextIter enditer;
    GtkTextIter startiter;
    gtk_text_buffer_get_iter_at_mark(buffer, &enditer, gtk_text_buffer_get_insert(buffer));
    if (!gtk_text_iter_ends_word(&enditer))
      gtk_text_iter_forward_word_end(&enditer);
    startiter = enditer;
    gtk_text_iter_backward_word_start(&startiter);
    ustring word = gtk_text_buffer_get_text(buffer, &startiter, &enditer, false);
    add_to_renderings(word, true);
  }
  return false;
}

gboolean WindowCheckKeyterms::on_textview_keyterm_text_button_release(GdkEventButton * event)
{
  check_move_new_reference();
  return false;
}

gboolean WindowCheckKeyterms::on_textview_keyterm_text_key_press(GdkEventKey * event)
{
  check_move_new_reference();
  return false;
}

void WindowCheckKeyterms::check_move_new_reference()
{
  // Get the textbuffer.
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_check_text));
  // Get iterator at insertion point.
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));
  // Get tags at that iterator.
  GSList *tags = NULL, *tagp = NULL;
  tags = gtk_text_iter_get_tags(&iter);
  for (tagp = tags; tagp != NULL; tagp = tagp->next) {
    GtkTextTag *tag = (GtkTextTag *) tagp->data;
    guint id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(tag), "id"));
    if (id) {
      id--;
      if (id < myreferences.size()) {
        if (id != previous_reference_id) {
          new_reference_showing = &myreferences[id];
          gtk_button_clicked(GTK_BUTTON(signal));
          previous_reference_id = id;
        }
      }
    }
  }
  // Free tags list if tags were found.
  if (tags) {
    g_slist_free(tags);
  }
}

unsigned int WindowCheckKeyterms::selected_id()
{
  // Selected id.
  unsigned int id = 0;
  // Get the model
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview_keyterm));
  // Get the selected paths.
  GList *sellist = NULL;
  sellist = gtk_tree_selection_get_selected_rows(treeselect_keywords, &model);
  // Get selected one.
  if (sellist) {
    // Get the path.
    GtkTreePath *path = (GtkTreePath *) sellist->data;
    // Get iterator to the id.
    GtkTreeIter iter;
    gtk_tree_model_get_iter(model, &iter, path);
    // Get the data.
    gtk_tree_model_get(model, &iter, 1, &id, -1);
    // Free memory.
    gtk_tree_path_free(path);
  }
  // Free memory for list.
  g_list_free(sellist);
  // Return result.
  return id;
}

ustring WindowCheckKeyterms::all_categories()
{
  return "All collections";
}

ustring WindowCheckKeyterms::enter_new_rendering_here()
{
  return "<Enter new rendering here>";
}

void WindowCheckKeyterms::get_renderings(vector < ustring > &renderings, vector < bool > &wholewords, vector < bool > &casesensitives)
{
  GtkTreeModel *model = (GtkTreeModel *) treestore_renderings;
  GtkTreeIter iter;
  gboolean valid;
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    int wholeword, casesensitive;
    gchar *str_data;
    gtk_tree_model_get(model, &iter, 0, &wholeword, 1, &casesensitive, 2, &str_data, -1);
    ustring rendering(str_data);
    if (!rendering.empty()) {
      if (rendering != enter_new_rendering_here()) {
        renderings.push_back(rendering);
        wholewords.push_back(wholeword);
        casesensitives.push_back(casesensitive);
      }
    }
    g_free(str_data);
    valid = gtk_tree_model_iter_next(model, &iter);
  }
}
