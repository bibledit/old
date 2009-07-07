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


#include "libraries.h"
#include <glib.h>
#include "windowcheckkeyterms.h"
#include "help.h"
#include "window.h"
#include "keyterms.h"
#include "tiny_utilities.h"
#include "utilities.h"
#include <gdk/gdkkeysyms.h>
#include "combobox.h"
#include "settings.h"
#include "projectutils.h"
#include "categorize.h"
#include "mapping.h"
#include "bible.h"
#include "books.h"


WindowCheckKeyterms::WindowCheckKeyterms(GtkAccelGroup * accelerator_group, bool startup, GtkWidget * parent_box):
WindowBase(widCheckKeyterms, "Check keyterms", startup, 0, parent_box), myreference(0)
// Window for checking keyterms.
{
  // Save / initialize variables.
  previous_reference_id = 0;

  // Build gui.
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  gtk_container_add(GTK_CONTAINER(window_vbox), vbox);

  // Produce the signal to be given on a new reference.
  signal = gtk_button_new();
  gtk_box_pack_start(GTK_BOX(vbox), signal, FALSE, FALSE, 0);

  hbox_collection = gtk_hbox_new (FALSE, 5);
  gtk_widget_show (hbox_collection);
  gtk_box_pack_start (GTK_BOX (vbox), hbox_collection, FALSE, FALSE, 0);

  label_collection = gtk_label_new_with_mnemonic ("_Collection");
  gtk_widget_show (label_collection);
  gtk_box_pack_start (GTK_BOX (hbox_collection), label_collection, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_collection), 0, 0.5);

  combobox_collection = gtk_combo_box_new_text ();
  gtk_widget_show (combobox_collection);
  gtk_box_pack_start (GTK_BOX (hbox_collection), combobox_collection, TRUE, TRUE, 0);

  label_list = gtk_label_new_with_mnemonic ("_List");
  gtk_widget_show (label_list);
  gtk_box_pack_start (GTK_BOX (vbox), label_list, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_list), 0, 0.5);

  scrolledwindow_terms = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_terms);
  gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow_terms, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_terms), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_terms), GTK_SHADOW_IN);

  htmlview_terms = gtk_html_new();
  gtk_widget_show(htmlview_terms);
  gtk_container_add(GTK_CONTAINER(scrolledwindow_terms), htmlview_terms);
  gtk_html_allow_selection(GTK_HTML(htmlview_terms), true);

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

  g_signal_connect((gpointer) htmlview_terms, "link-clicked", G_CALLBACK(on_html_link_clicked), gpointer(this));
  g_signal_connect((gpointer) combobox_collection, "changed", G_CALLBACK(on_combobox_keyterm_collection_changed), gpointer(this));
// Todo  g_signal_connect((gpointer) textview_check_text, "button_press_event", G_CALLBACK(on_textview_keyterm_text_button_press_event), gpointer(this));
// Todo  g_signal_connect((gpointer) textview_check_text, "button_release_event", G_CALLBACK(on_textview_keyterm_text_button_release_event), gpointer(this));
// Todo  g_signal_connect((gpointer) textview_check_text, "key_press_event", G_CALLBACK(on_textview_keyterm_text_key_press_event), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label_collection), combobox_collection);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label_list), htmlview_terms);

  // Load the categories.
  vector <ustring> categories = keyterms_get_categories();
  combobox_set_strings(combobox_collection, categories);
  if (!categories.empty()) {
    combobox_set_index(combobox_collection, 0);
  }

  // Zero the text tag for marking renderings.
  approved_rendering_tag = NULL;
  disapproved_rendering_tag = NULL;

  // Load the keyterms.
  on_combobox_keyterm_collection ();
  
  // Main focused widget.
  last_focused_widget = combobox_collection;
  gtk_widget_grab_focus (last_focused_widget);
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


void WindowCheckKeyterms::copy_clipboard() // Todo
{
  /*
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
  */
}


void WindowCheckKeyterms::on_combobox_keyterm_collection_changed(GtkComboBox * combobox, gpointer user_data)
{
  ((WindowCheckKeyterms *) user_data)->on_combobox_keyterm_collection();
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


void WindowCheckKeyterms::on_combobox_keyterm_collection()
{
  html_link_clicked("");
}


// Todo  myid = selected_id();


void WindowCheckKeyterms::load_renderings()
{
  extern Settings *settings;
  myproject = settings->genconfig.project_get();
  ProjectConfiguration *projectconfig = settings->projectconfig(myproject);
  myversification = projectconfig->versification_get();
  ustring keyterm;
  keyterms_get_term(myid, keyterm);
  vector < ustring > renderings;
  vector < bool > wholewords;
  vector < bool > casesensitives;
  ustring category;
  {
    ustring dummy1;
    vector < Reference > dummy2;
    keyterms_get_data(myid, category, dummy1, dummy2);
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
  keyterms_get_term(myid, keyterm);
  ustring category;
  {
    ustring dummy1;
    vector < Reference > dummy2;
    keyterms_get_data(myid, category, dummy1, dummy2);
  }
  keyterms_store_renderings(myproject, keyterm, category, renderings, wholewords, casesensitives);
  load_renderings();
  show_text();
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
  keyterms_get_term(myid, keyterm);
  GtkTreeIter iter;
  gtk_tree_store_append(treestore_renderings, &iter, NULL);
  bool casesensitive = rendering != rendering.casefold();
  gtk_tree_store_set(treestore_renderings, &iter, 0, wholeword, 1, casesensitive, 2, rendering.c_str(), 3, 1, -1);
  save_renderings();
}


void WindowCheckKeyterms::show_text() // Todo
// Loads the text into the textview.
{
  /*
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

  // Create the text tag for marking renderings, if it's not yet there.
  if (!approved_rendering_tag) {
    approved_rendering_tag = gtk_text_buffer_create_tag(buffer, NULL, "background", "khaki", NULL);
  }
  if (!disapproved_rendering_tag) {
    disapproved_rendering_tag = gtk_text_buffer_create_tag(buffer, NULL, "background", "red", NULL);
  }

  // Get all the references.
  ustring dummy1;
  myreferences.clear();
  mytextstarts.clear();
  mytextends.clear();
  keyterms_get_data(myid, dummy1, dummy1, myreferences);

  // Remap references.
  {
    vector <Reference> references_temp;
    for (unsigned int i = 0; i < myreferences.size(); i++) {
      Mapping mapping(myversification, myreferences[i].book);
      vector <int> chapters;
      vector <int> verses;
      mapping.original_to_me(myreferences[i].chapter, myreferences[i].verse, chapters, verses);
      for (unsigned int i2 = 0; i2 < chapters.size(); i2++) {
        Reference reference(myreferences[i].book, chapters[i2], convert_to_string(verses[i2]));
        references_temp.push_back(reference);
      }
    }
    myreferences = references_temp;
  }

  // Retrieve the renderings.
  vector <ustring> renderings;
  vector <bool> wholewords;
  vector <bool> casesensitives;
  get_renderings(renderings, wholewords, casesensitives);

  // Display text of the references.
  for (unsigned int i = 0; i < myreferences.size(); i++) {

    ustring text;

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
    // This is for later, that we can find which reference the cursor location
    // belongs to.
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

  // Go through all text we have to mark the renderings.
  for (unsigned int i = 0; i < mytextstarts.size(); i++) {

    // Get iterators of these offsets.
    gtk_text_buffer_get_iter_at_offset(buffer, &startiter, mytextstarts[i]);
    gtk_text_buffer_get_iter_at_offset(buffer, &enditer, mytextends[i]);

    // Get the text of the verse.
    ustring text = gtk_text_buffer_get_text(buffer, &startiter, &enditer, false);

    // If at least one of the renderings is found, then the text will be approved.
    // Visibly mark the correct locations.
    vector <size_t> startpositions;
    vector <size_t> lengths;
    if (find_renderings (text, renderings, wholewords, casesensitives, &startpositions, &lengths)) {
      for (unsigned int i2 = 0; i2 < startpositions.size(); i2++) {
        GtkTextIter approvedstart = startiter;
        GtkTextIter approvedend;
        gtk_text_iter_forward_chars(&approvedstart, startpositions[i2]);
        approvedend = approvedstart;
        gtk_text_iter_forward_chars(&approvedend, lengths[i2]);
        gtk_text_buffer_apply_tag(buffer, approved_rendering_tag, &approvedstart, &approvedend);
      }
    } else {
      gtk_text_buffer_apply_tag(buffer, disapproved_rendering_tag, &startiter, &enditer);
    }
  }
  */
}


bool WindowCheckKeyterms::find_renderings (const ustring& text, const vector <ustring>& renderings, const vector <bool>& wholewords, const vector <bool>& casesensitives, vector <size_t> * startpositions, vector <size_t> * lengths)
// Finds renderings in the text.
// text: Text to be looked into.
// renderings: Renderings to look for.
// wholewords / casesensitives: Attributes of the renderings.
// startpositions: If non-NULL, will be filled with the positions that each rendering starts at.
// lengths: If non-NULL, will be filled with the lengths of the renderings found.
// Returns whether one or more renderings were found in the verse.
{
  if (startpositions)
    startpositions->clear();
  if (lengths)
    lengths->clear();

  GtkTextBuffer * textbuffer = gtk_text_buffer_new (NULL);
  gtk_text_buffer_set_text (textbuffer, text.c_str(), -1);
  GtkTextIter startiter;
  gtk_text_buffer_get_start_iter(textbuffer, &startiter);

  bool found = false;

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
      bool temporally_approved = true;
      GtkTextIter approvedstart = startiter;
      GtkTextIter approvedend;
      gtk_text_iter_forward_chars(&approvedstart, position);
      approvedend = approvedstart;
      gtk_text_iter_forward_chars(&approvedend, rendering.length());
      if (wholeword) {
        if (!gtk_text_iter_starts_word(&approvedstart))
          temporally_approved = false;
        if (!gtk_text_iter_ends_word(&approvedend))
          temporally_approved = false;
      }
      if (temporally_approved) {
        found = true;
        if (startpositions)
          startpositions->push_back (position);
        if (lengths)
          lengths->push_back (rendering.length());
      }
      position = mytext.find(myrendering, ++position);
    }
  }

  g_object_unref (textbuffer);  
  
  return found;
}


gboolean WindowCheckKeyterms::on_textview_keyterm_text_button_press(GdkEventButton * event) // Todo
{
  /*
  // Get the textbuffer.
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_check_text));

  // Store selected text.
  {
    GtkTextIter start, end;
    ustring selection_text;
    if (gtk_text_buffer_get_selection_bounds (buffer, &start, &end)) {
      gchar * text = gtk_text_buffer_get_text (buffer, &start, &end, false);
      selection_text = text;
      g_free (text);
    }
    keyterm_text_selection.push_back (selection_text);
  }

  // Double-clicking adds the word to the renderings.
  if (event->type == GDK_2BUTTON_PRESS) {

    // Get the word that was selected before the clicking started.
    ustring selection_text;
    if (keyterm_text_selection.size() >= 3) {
      selection_text = keyterm_text_selection[keyterm_text_selection.size() - 3];
    }

    // Get the full word if no text was selected before.
    if (selection_text.empty ()) {
      GtkTextIter enditer;
      GtkTextIter startiter;
      gtk_text_buffer_get_iter_at_mark(buffer, &enditer, gtk_text_buffer_get_insert(buffer));
      if (!gtk_text_iter_ends_word(&enditer))
        gtk_text_iter_forward_word_end(&enditer);
      startiter = enditer;
      gtk_text_iter_backward_word_start(&startiter);
      selection_text = gtk_text_buffer_get_text(buffer, &startiter, &enditer, false);
    }
    add_to_renderings(selection_text, true);
  }
  */
  return false;
}


gboolean WindowCheckKeyterms::on_textview_keyterm_text_button_release(GdkEventButton * event) // Todo out?
{
  return false;
}


gboolean WindowCheckKeyterms::on_textview_keyterm_text_key_press(GdkEventKey * event) // Todo out?
{
  return false;
}


unsigned int WindowCheckKeyterms::selected_id()
{
  unsigned int id = 0;
  return id;
}


ustring WindowCheckKeyterms::enter_new_rendering_here()
{
  return "<Enter new rendering here>";
}


void WindowCheckKeyterms::get_renderings(vector <ustring> &renderings, vector <bool> &wholewords, vector <bool> &casesensitives)
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


ustring WindowCheckKeyterms::collection ()
{
  return combobox_get_active_string(combobox_collection);
}


gboolean WindowCheckKeyterms::on_html_link_clicked(GtkHTML * html, const gchar * url, gpointer user_data)
{
  ((WindowCheckKeyterms *) user_data)->html_link_clicked(url);
  return true;
}


void WindowCheckKeyterms::html_link_clicked (const gchar * url)
// Callback for clicking a link.
{
  // Store scrolling position for the now active url.
  GtkAdjustment * adjustment = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolledwindow_terms));
  scrolling_position[active_url] = gtk_adjustment_get_value (adjustment);

  // New url.
  active_url = url;

  // Start writing a html page.
  HtmlWriter2 htmlwriter ("");
  bool display_another_page = false;

  //extern Settings *settings;
  //myproject = settings->genconfig.project_get();

  if (active_url.find ("keyterm ") == 0) {
    // Display a keyterm and all its data.
    ustring url = active_url;
    url.erase (0, 8);
    unsigned int keyword_id = convert_to_int (url);
    html_write_extras (htmlwriter, keyword_id);
    display_another_page = true;
  }

  else if (active_url.find ("goto ") == 0) {
    // Signal the editor to go to a reference.
    ustring url = active_url;
    url.erase (0, 5);
    myreference.assign (get_reference (url));
    new_reference_showing = &myreference;
    gtk_button_clicked(GTK_BUTTON(signal));
  }
  
  else {
    // Give the starting page with all keyterms of the active selection.
    vector <ustring> terms;
    vector <unsigned int> ids;
    keyterms_get_terms("", collection(), terms, ids);
    for (unsigned int i = 0; i < terms.size(); i++) {
      htmlwriter.paragraph_open();
      htmlwriter.hyperlink_add ("keyterm " + convert_to_string (ids[i]), terms[i]);
      htmlwriter.paragraph_close();
    }
    display_another_page = true;
  }
  
  htmlwriter.finish();
  if (display_another_page) {
    // Load the page.
    GtkHTMLStream *stream = gtk_html_begin(GTK_HTML(htmlview_terms));
    gtk_html_write(GTK_HTML(htmlview_terms), stream, htmlwriter.html.c_str(), -1);
    gtk_html_end(GTK_HTML(htmlview_terms), stream, GTK_HTML_STREAM_OK);
    // Scroll to the position that possibly was stored while this url was last active.
    gtk_adjustment_set_value (adjustment, scrolling_position[active_url]);
  }
}


void WindowCheckKeyterms::html_write_extras (HtmlWriter2& htmlwriter, unsigned int keyword_id) // Todo
{
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add ("", "Index");
  htmlwriter.paragraph_close ();
  ustring dummy;
  ustring information;
  vector <Reference> references;
  keyterms_get_data(keyword_id, dummy, information, references);
  htmlwriter.paragraph_open ();
  size_t pos = information.find (keyterms_reference_start_markup ());
  while (pos != string::npos) {
    htmlwriter.text_add (information.substr (0, pos));
    information.erase (0, pos + keyterms_reference_start_markup ().length());
    pos = information.find (keyterms_reference_end_markup ());
    if (pos != string::npos) {
      // Add a clickable reference.
      htmlwriter.paragraph_close ();
      ustring reference_text = information.substr (0, pos);
      Reference reference = get_reference (reference_text);
      htmlwriter.hyperlink_add ("goto " + reference_text, reference_text);
      information.erase (0, pos + keyterms_reference_end_markup ().length());
      // Todo add the reference's text. Todo
      extern Settings * settings;
      ustring verse = project_retrieve_verse(settings->genconfig.project_get(), reference.book, reference.chapter, reference.verse);
      if (verse.empty()) {
        verse.append("<empty>");
      } else {
        CategorizeLine cl(verse);
        cl.remove_verse_number(reference.verse);
        verse = cl.verse;
      }
      htmlwriter.text_add (" ");
      htmlwriter.text_add (verse);
      // Proceed to next.
      htmlwriter.paragraph_open ();
      pos = information.find (keyterms_reference_start_markup ());
    }    
  }
  htmlwriter.text_add (information);
  htmlwriter.paragraph_close ();
}


Reference WindowCheckKeyterms::get_reference (const ustring& text)
// Generates a reference out of the text.
{
  Reference ref (0);
  ustring book, chapter;
  decode_reference(text, book, chapter, ref.verse);
  ref.book = books_english_to_id (book);
  ref.chapter = convert_to_int (chapter);
  return ref;
}





/*

Todo various

The elaborate page has all comments, all renderings, everthing one can think of.
References should be displayed one line below the other, and the text following it.
There should be some indication on the page of whether the keyterms have been approved.
Each verse can be approved individually, even if there's no rendering that matches the verse.
The whole keyterm can be approved as well.
There should be a database called "keyterms.sql" per project, that stores the whole state of each keyterm, 
including renderings.

*/
