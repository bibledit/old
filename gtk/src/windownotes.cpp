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


#include "libraries.h"
#include <glib.h>
#include "windownotes.h"
#include "help.h"
#include "floatingwindow.h"
#include "gui_features.h"
#include "notes_utils.h"
#include "books.h"
#include "combobox.h"
#include "tiny_utilities.h"
#include "date_time_utils.h"
#include <sqlite3.h>
#include "sqlite_reader.h"
#include "gwrappers.h"
#include "htmlcolor.h"
#include "settings.h"
#include "utilities.h"
#include "bible.h"
#include "gtkwrappers.h"
#include "dialogprojectnote.h"
#include "dialogyesnoalways.h"


WindowNotes::WindowNotes(GtkWidget * parent_layout, GtkAccelGroup *accelerator_group, bool startup):
FloatingWindow(parent_layout, widNotes, "Project notes", startup)
// Project notes window.
{
  // Initialize variables.
  note_editor = NULL;
  redisplay_source_id = 0;
  displayprojectnotes = NULL;
  gui_source_id = 0;
  edited_note_id = 0;
  
  GuiFeatures guifeatures(0);

  notebook1 = gtk_notebook_new();
  gtk_widget_show(notebook1);
  gtk_container_add(GTK_CONTAINER(vbox_client), notebook1);
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook1), FALSE);

  scrolledwindow_notes = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow_notes);
  gtk_container_add(GTK_CONTAINER(notebook1), scrolledwindow_notes);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow_notes), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  htmlview_notes = gtk_html_new();
  gtk_widget_show(htmlview_notes);
  gtk_container_add(GTK_CONTAINER(scrolledwindow_notes), htmlview_notes);
  gtk_html_allow_selection(GTK_HTML(htmlview_notes), true);

  connect_focus_signals (htmlview_notes);

  hbox14 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox14);
  gtk_container_add(GTK_CONTAINER(notebook1), hbox14);

  vbox_note_editor = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox_note_editor);
  gtk_box_pack_start(GTK_BOX(hbox14), vbox_note_editor, TRUE, TRUE, 0);

  toolbar_note_editor = gtk_toolbar_new();
  gtk_widget_show(toolbar_note_editor);
  gtk_box_pack_start(GTK_BOX(vbox_note_editor), toolbar_note_editor, FALSE, FALSE, 0);
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar_note_editor), GTK_TOOLBAR_BOTH);

  toolitem_note_edit_font_size = (GtkWidget *) gtk_tool_item_new();
  gtk_widget_show(toolitem_note_edit_font_size);
  gtk_container_add(GTK_CONTAINER(toolbar_note_editor), toolitem_note_edit_font_size);

  combobox_note_edit_font_size = gtk_combo_box_new_text();
  gtk_widget_show(combobox_note_edit_font_size);
  gtk_container_add(GTK_CONTAINER(toolitem_note_edit_font_size), combobox_note_edit_font_size);
  gtk_combo_box_set_focus_on_click(GTK_COMBO_BOX(combobox_note_edit_font_size), FALSE);

  toolitem_note_edit_paragraph_style = (GtkWidget *) gtk_tool_item_new();
  gtk_widget_show(toolitem_note_edit_paragraph_style);
  gtk_container_add(GTK_CONTAINER(toolbar_note_editor), toolitem_note_edit_paragraph_style);

  combobox_note_edit_paragraph_style = gtk_combo_box_new_text();
  gtk_widget_show(combobox_note_edit_paragraph_style);
  gtk_container_add(GTK_CONTAINER(toolitem_note_edit_paragraph_style), combobox_note_edit_paragraph_style);
  gtk_combo_box_set_focus_on_click(GTK_COMBO_BOX(combobox_note_edit_paragraph_style), FALSE);

  toolitem_note_edit_bold = (GtkWidget *) gtk_tool_item_new();
  gtk_widget_show(toolitem_note_edit_bold);
  gtk_container_add(GTK_CONTAINER(toolbar_note_editor), toolitem_note_edit_bold);

  togglebutton_note_edit_bold = gtk_toggle_button_new();
  gtk_widget_show(togglebutton_note_edit_bold);
  gtk_container_add(GTK_CONTAINER(toolitem_note_edit_bold), togglebutton_note_edit_bold);
  gtk_button_set_focus_on_click(GTK_BUTTON(togglebutton_note_edit_bold), FALSE);

  image29121 = gtk_image_new_from_stock("gtk-bold", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image29121);
  gtk_container_add(GTK_CONTAINER(togglebutton_note_edit_bold), image29121);

  toolitem_note_edit_italics = (GtkWidget *) gtk_tool_item_new();
  gtk_widget_show(toolitem_note_edit_italics);
  gtk_container_add(GTK_CONTAINER(toolbar_note_editor), toolitem_note_edit_italics);

  togglebutton_note_edit_italics = gtk_toggle_button_new();
  gtk_widget_show(togglebutton_note_edit_italics);
  gtk_container_add(GTK_CONTAINER(toolitem_note_edit_italics), togglebutton_note_edit_italics);
  gtk_button_set_focus_on_click(GTK_BUTTON(togglebutton_note_edit_italics), FALSE);

  image29122 = gtk_image_new_from_stock("gtk-italic", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image29122);
  gtk_container_add(GTK_CONTAINER(togglebutton_note_edit_italics), image29122);

  toolitem_note_edit_underline = (GtkWidget *) gtk_tool_item_new();
  gtk_widget_show(toolitem_note_edit_underline);
  gtk_container_add(GTK_CONTAINER(toolbar_note_editor), toolitem_note_edit_underline);

  togglebutton_note_edit_underline = gtk_toggle_button_new();
  gtk_widget_show(togglebutton_note_edit_underline);
  gtk_container_add(GTK_CONTAINER(toolitem_note_edit_underline), togglebutton_note_edit_underline);
  gtk_button_set_focus_on_click(GTK_BUTTON(togglebutton_note_edit_underline), FALSE);

  image29114 = gtk_image_new_from_stock("gtk-underline", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image29114);
  gtk_container_add(GTK_CONTAINER(togglebutton_note_edit_underline), image29114);

  toolitem_note_edit_strike_through = (GtkWidget *) gtk_tool_item_new();
  gtk_widget_show(toolitem_note_edit_strike_through);
  gtk_container_add(GTK_CONTAINER(toolbar_note_editor), toolitem_note_edit_strike_through);

  togglebutton_note_edit_strike_through = gtk_toggle_button_new();
  gtk_widget_show(togglebutton_note_edit_strike_through);
  gtk_container_add(GTK_CONTAINER(toolitem_note_edit_strike_through), togglebutton_note_edit_strike_through);
  gtk_button_set_focus_on_click(GTK_BUTTON(togglebutton_note_edit_strike_through), FALSE);

  image29123 = gtk_image_new_from_stock("gtk-strikethrough", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image29123);
  gtk_container_add(GTK_CONTAINER(togglebutton_note_edit_strike_through), image29123);

  toolitem_note_edit_left_justify = (GtkWidget *) gtk_tool_item_new();
  gtk_widget_show(toolitem_note_edit_left_justify);
  gtk_container_add(GTK_CONTAINER(toolbar_note_editor), toolitem_note_edit_left_justify);

  togglebutton_note_edit_left_justify = gtk_toggle_button_new();
  gtk_widget_show(togglebutton_note_edit_left_justify);
  gtk_container_add(GTK_CONTAINER(toolitem_note_edit_left_justify), togglebutton_note_edit_left_justify);
  gtk_button_set_focus_on_click(GTK_BUTTON(togglebutton_note_edit_left_justify), FALSE);

  image29124 = gtk_image_new_from_stock("gtk-justify-left", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image29124);
  gtk_container_add(GTK_CONTAINER(togglebutton_note_edit_left_justify), image29124);

  toolitem_note_edit_center_justify = (GtkWidget *) gtk_tool_item_new();
  gtk_widget_show(toolitem_note_edit_center_justify);
  gtk_container_add(GTK_CONTAINER(toolbar_note_editor), toolitem_note_edit_center_justify);

  togglebutton_note_edit_center_justify = gtk_toggle_button_new();
  gtk_widget_show(togglebutton_note_edit_center_justify);
  gtk_container_add(GTK_CONTAINER(toolitem_note_edit_center_justify), togglebutton_note_edit_center_justify);
  gtk_button_set_focus_on_click(GTK_BUTTON(togglebutton_note_edit_center_justify), FALSE);

  image29125 = gtk_image_new_from_stock("gtk-justify-center", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image29125);
  gtk_container_add(GTK_CONTAINER(togglebutton_note_edit_center_justify), image29125);

  toolitem_note_edit_right_justify = (GtkWidget *) gtk_tool_item_new();
  gtk_widget_show(toolitem_note_edit_right_justify);
  gtk_container_add(GTK_CONTAINER(toolbar_note_editor), toolitem_note_edit_right_justify);

  togglebutton_note_edit_right_justify = gtk_toggle_button_new();
  gtk_widget_show(togglebutton_note_edit_right_justify);
  gtk_container_add(GTK_CONTAINER(toolitem_note_edit_right_justify), togglebutton_note_edit_right_justify);
  gtk_button_set_focus_on_click(GTK_BUTTON(togglebutton_note_edit_right_justify), FALSE);

  image29126 = gtk_image_new_from_stock("gtk-justify-right", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image29126);
  gtk_container_add(GTK_CONTAINER(togglebutton_note_edit_right_justify), image29126);

  toolitem_note_edit_decrease_indent = (GtkWidget *) gtk_tool_item_new();
  gtk_widget_show(toolitem_note_edit_decrease_indent);
  gtk_container_add(GTK_CONTAINER(toolbar_note_editor), toolitem_note_edit_decrease_indent);

  button_note_edit_decrease_indent = gtk_button_new();
  gtk_widget_show(button_note_edit_decrease_indent);
  gtk_container_add(GTK_CONTAINER(toolitem_note_edit_decrease_indent), button_note_edit_decrease_indent);
  gtk_button_set_focus_on_click(GTK_BUTTON(button_note_edit_decrease_indent), FALSE);

  image29127 = gtk_image_new_from_stock("gtk-unindent", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image29127);
  gtk_container_add(GTK_CONTAINER(button_note_edit_decrease_indent), image29127);

  toolitem_note_edit_increase_indent = (GtkWidget *) gtk_tool_item_new();
  gtk_widget_show(toolitem_note_edit_increase_indent);
  gtk_container_add(GTK_CONTAINER(toolbar_note_editor), toolitem_note_edit_increase_indent);

  button_note_edit_increase_indent = gtk_button_new();
  gtk_widget_show(button_note_edit_increase_indent);
  gtk_container_add(GTK_CONTAINER(toolitem_note_edit_increase_indent), button_note_edit_increase_indent);
  gtk_button_set_focus_on_click(GTK_BUTTON(button_note_edit_increase_indent), FALSE);

  image29128 = gtk_image_new_from_stock("gtk-indent", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image29128);
  gtk_container_add(GTK_CONTAINER(button_note_edit_increase_indent), image29128);

  toolitem_note_edit_color = (GtkWidget *) gtk_tool_item_new();
  gtk_widget_show(toolitem_note_edit_color);
  gtk_container_add(GTK_CONTAINER(toolbar_note_editor), toolitem_note_edit_color);

  colorbutton_note_edit = gtk_color_button_new();
  gtk_widget_show(colorbutton_note_edit);
  gtk_container_add(GTK_CONTAINER(toolitem_note_edit_color), colorbutton_note_edit);
  gtk_button_set_focus_on_click(GTK_BUTTON(colorbutton_note_edit), FALSE);

  project_notes_editable = guifeatures.project_notes_management();

  scrolledwindow_note_editor = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow_note_editor);
  gtk_box_pack_start(GTK_BOX(vbox_note_editor), scrolledwindow_note_editor, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow_note_editor), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  htmlview_note_editor = gtk_html_new();
  gtk_widget_show(htmlview_note_editor);
  gtk_container_add(GTK_CONTAINER(scrolledwindow_note_editor), htmlview_note_editor);
  gtk_html_allow_selection(GTK_HTML(htmlview_note_editor), true);

  connect_focus_signals (htmlview_note_editor);

  vbox_controls = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox_controls);
  gtk_box_pack_start(GTK_BOX(hbox14), vbox_controls, FALSE, FALSE, 0);

  hbox_ok_cancel = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox_ok_cancel);
  gtk_box_pack_start(GTK_BOX(vbox_controls), hbox_ok_cancel, FALSE, FALSE, 0);

  button_note_cancel = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(button_note_cancel);
  gtk_box_pack_start(GTK_BOX(hbox_ok_cancel), button_note_cancel, FALSE, FALSE, 0);

  button_note_ok = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(button_note_ok);
  gtk_box_pack_start(GTK_BOX(hbox_ok_cancel), button_note_ok, FALSE, FALSE, 0);

  button_more = gtk_button_new();
  gtk_widget_show(button_more);
  gtk_box_pack_start(GTK_BOX(hbox_ok_cancel), button_more, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(button_more), alignment1);

  hbox15 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox15);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox15);

  image1 = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox15), image1, FALSE, FALSE, 0);

  label1 = gtk_label_new_with_mnemonic("_More");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(hbox15), label1, FALSE, FALSE, 0);

  label_note_category = gtk_label_new_with_mnemonic("C_ategory");
  gtk_widget_show(label_note_category);
  gtk_box_pack_start(GTK_BOX(vbox_controls), label_note_category, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_note_category), 0, 0.5);

  combobox_note_category = gtk_combo_box_new_text();
  gtk_widget_show(combobox_note_category);
  gtk_box_pack_start(GTK_BOX(vbox_controls), combobox_note_category, FALSE, FALSE, 0);

  label_note_references = gtk_label_new_with_mnemonic("_References");
  gtk_widget_show(label_note_references);
  gtk_box_pack_start(GTK_BOX(vbox_controls), label_note_references, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_note_references), 0, 0.5);

  textview_note_references = gtk_text_view_new();
  gtk_widget_show(textview_note_references);
  gtk_box_pack_start(GTK_BOX(vbox_controls), textview_note_references, TRUE, TRUE, 0);
  gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(textview_note_references), FALSE);

  connect_focus_signals (textview_note_references);
  
  g_signal_connect((gpointer) htmlview_notes, "link-clicked", G_CALLBACK(on_html_link_clicked), gpointer(this));
  g_signal_connect((gpointer) button_note_ok, "clicked", G_CALLBACK(on_button_ok_clicked), gpointer(this));
  g_signal_connect((gpointer) button_note_cancel, "clicked", G_CALLBACK(on_button_cancel_clicked), gpointer(this));
  g_signal_connect((gpointer) button_more, "clicked", G_CALLBACK(on_button_more_clicked), gpointer(this));

  // Project notes editor signals.
  g_signal_connect((gpointer) combobox_note_edit_font_size, "changed", G_CALLBACK(on_combobox_note_edit_font_size_changed), gpointer(this));
  g_signal_connect((gpointer) htmlview_note_editor, "insertion_font_style_changed", G_CALLBACK(on_note_editor_insertion_font_style_changed), gpointer(this));
  g_signal_connect((gpointer) combobox_note_edit_paragraph_style, "changed", G_CALLBACK(on_combobox_note_edit_paragraph_style_changed), gpointer(this));
  g_signal_connect((gpointer) htmlview_note_editor, "current_paragraph_style_changed", G_CALLBACK(on_note_editor_current_paragraph_style_changed), gpointer(this));
  g_signal_connect((gpointer) togglebutton_note_edit_bold, "toggled", G_CALLBACK(on_togglebutton_note_edit_bold_toggled), gpointer(this));
  g_signal_connect((gpointer) togglebutton_note_edit_italics, "toggled", G_CALLBACK(on_togglebutton_note_edit_italics_toggled), gpointer(this));
  g_signal_connect((gpointer) togglebutton_note_edit_underline, "toggled", G_CALLBACK(on_togglebutton_note_edit_underline_toggled), gpointer(this));
  g_signal_connect((gpointer) togglebutton_note_edit_strike_through, "toggled", G_CALLBACK(on_togglebutton_note_edit_strike_through_toggled), gpointer(this));
  g_signal_connect((gpointer) togglebutton_note_edit_left_justify, "toggled", G_CALLBACK(on_togglebutton_note_edit_left_justify_toggled), gpointer(this));
  g_signal_connect((gpointer) togglebutton_note_edit_center_justify, "toggled", G_CALLBACK(on_togglebutton_note_edit_center_justify_toggled), gpointer(this));
  g_signal_connect((gpointer) togglebutton_note_edit_right_justify, "toggled", G_CALLBACK(on_togglebutton_note_edit_right_justify_toggled), gpointer(this));
  g_signal_connect((gpointer) htmlview_note_editor, "current_paragraph_alignment_changed", G_CALLBACK(on_current_paragraph_alignment_changed), gpointer(this));
  g_signal_connect((gpointer) button_note_edit_decrease_indent, "clicked", G_CALLBACK(on_button_note_edit_decrease_indent_clicked), gpointer(this));
  g_signal_connect((gpointer) button_note_edit_increase_indent, "clicked", G_CALLBACK(on_button_note_edit_increase_indent_clicked), gpointer(this));
  g_signal_connect((gpointer) htmlview_note_editor, "current_paragraph_indentation_changed", G_CALLBACK(on_current_paragraph_indentation_changed), gpointer(this));
  g_signal_connect((gpointer) colorbutton_note_edit, "color_set", G_CALLBACK(on_colorbutton_note_edit_color_set), gpointer(this));
  g_signal_connect((gpointer) htmlview_note_editor, "insertion_color_changed", G_CALLBACK(on_insertion_color_changed), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label_note_references), textview_note_references);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label_note_category), combobox_note_category);

  // GUI timer.
  gui_source_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 100, GSourceFunc(on_gui_timeout), gpointer(this), NULL);

  // Signal button.
  references_available_signal_button = gtk_button_new();

  // Main focused widget.
  last_focused_widget = htmlview_notes;
  gtk_widget_grab_focus (last_focused_widget);
  
  // Set fonts.
  set_fonts ();
}


WindowNotes::~WindowNotes()
{
  // Stop possible thread that is displaying notes.
  stop_displaying_more_notes();
  // Destroy a couple of timeouts.
  gw_destroy_source(redisplay_source_id);
  gw_destroy_source(gui_source_id);
  // Destroy signal button.
  gtk_widget_destroy(references_available_signal_button);
}


void WindowNotes::go_to(Reference & reference)
{

}


void WindowNotes::new_note()
// Create a new note.
{
  // If we are currently editing a note, do nothing.
  if (note_being_edited())
    return;

  // Get the unique id for the new note.
  int id = notes_database_get_unique_id();
  // Create the new note.
  notes_fill_edit_screen(id, true);
}


void WindowNotes::notes_fill_edit_screen(int id, bool newnote)
/*
 When a new note is made, or an existing one edited, this function
 sets up the edit screen.
 */
{
  // Create the NoteEditor object.
  if (note_editor)
    delete note_editor;
  note_editor = new NoteEditor(0);
  // Save variables.
  note_editor->id = id;
  note_editor->newnote = newnote;

  // Initialize pointers to the text buffers.
  note_editor->textbuffer_references = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_note_references));

  // Get our language.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  ustring language = projectconfig->language_get();

  // Fill comboboxes for the toolbar.
  combobox_set_strings(combobox_note_edit_font_size, note_editor_font_size_names_list());
  combobox_set_string(combobox_note_edit_font_size, note_editor_font_size_enum_to_name(GTK_HTML_FONT_STYLE_DEFAULT));
  combobox_set_strings(combobox_note_edit_paragraph_style, note_editor_paragraph_style_names_list());
  combobox_set_string(combobox_note_edit_paragraph_style, note_editor_paragraph_style_enum_to_name(GTK_HTML_PARAGRAPH_STYLE_NORMAL));

  // Fetch the data for the note from the note file.
  ustring note;
  // Stored in object: ustring project;
  ustring references;
  ustring category;
  int date_created;
  ustring user_created;
  int date_modified;
  // Stored in object: ustring logbook;
  notes_read_one_from_file (id, note, project, references, category, date_created, user_created, date_modified, logbook);
  
  // Deal with the references of the notes.  
  gtk_text_buffer_set_text(note_editor->textbuffer_references, "", -1);
  if (newnote) {
    // New note: get the current reference from the editor.
    references = books_id_to_english(settings->genconfig.book_get()) + " " + settings->genconfig.chapter_get() + ":" + settings->genconfig.verse_get();
    gtk_text_buffer_insert_at_cursor(note_editor->textbuffer_references, references.c_str(), -1);
    gtk_text_buffer_insert_at_cursor(note_editor->textbuffer_references, "\n", -1);
  } else {
    // Existing note: use the reference(s) from the note file.
    // Read the reference(s) and show them.
    Parse parse(references, false);
    for (unsigned int i = 0; i < parse.words.size(); i++) {
      Reference reference(0);
      reference_discover(0, 0, "", parse.words[i], reference.book, reference.chapter, reference.verse);
      ustring ref = reference.human_readable(language);
      gtk_text_buffer_insert_at_cursor(note_editor->textbuffer_references, ref.c_str(), -1);
      gtk_text_buffer_insert_at_cursor(note_editor->textbuffer_references, "\n", -1);
    }
  }
  
  // Display the note.
  GtkHTMLStream *stream = gtk_html_begin(GTK_HTML(htmlview_note_editor));
  gtk_html_write(GTK_HTML(htmlview_note_editor), stream, note.c_str(), -1);
  gtk_html_end(GTK_HTML(htmlview_note_editor), stream, GTK_HTML_STREAM_OK);
  gtk_html_set_editable(GTK_HTML(htmlview_note_editor), project_notes_editable);
  
  note_editor->store_original_data(note);
  gtk_text_buffer_set_modified(note_editor->textbuffer_references, false);
  
  // Put all available categories into the category combo.
  {
    ReadText rt(notes_categories_filename());
    combobox_set_strings(combobox_note_category, rt.lines);
    if (rt.lines.size() > 0)
      combobox_set_string(combobox_note_category, rt.lines[0]);
  }

  // Set the category.
  if (!newnote) {
    combobox_set_string(combobox_note_category, category);
  }

  // Handle the project(s).
  {
    if (newnote) {
      project = settings->genconfig.project_get();
    }
    projects.clear();
    if (project != "All")
      projects.push_back(project);
    if (settings->genconfig.project_get() != project)
      projects.push_back(settings->genconfig.project_get());
    projects.push_back("All");
  }

  // Fetch and store the creation date for display and later use.
  {
    if (newnote) {
      note_editor->date_created = date_time_julian_day_get_current();
    } else {
      note_editor->date_created = date_created;
    }
    created_on = "Created on " + date_time_julian_human_readable(note_editor->date_created, true);
  }

  // The modification date.
  {
    if (newnote) {
      note_editor->date_modified = date_time_julian_day_get_current();
    } else {
      note_editor->date_modified = date_modified;
    }
    edited_on = "Edited on " + date_time_julian_human_readable(note_editor->date_modified, true);
  }

  // The user who created the note.
  {
    if (newnote) {
      note_editor->created_by = g_get_real_name();
    } else {
      note_editor->created_by = user_created;
    }
    created_by = "Created by " + note_editor->created_by;
  }

  // Read the logbook.
  {
    if (newnote)
      logbook.clear();
  }

  // Store category and project.
  note_editor->previous_category = combobox_get_active_string(combobox_note_category);
  note_editor->previous_project = project;

  // Set GUI elements.
  current_paragraph_indentation_changed(gtk_html_get_paragraph_indentation(GTK_HTML(htmlview_note_editor)));

  // Switch screen to displaying the editor.
  gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook1), 1);
  // Focus the widget the user is most likely going to type in.
  gtk_widget_grab_focus(htmlview_note_editor);
}


void WindowNotes::on_combobox_note_edit_font_size_changed(GtkComboBox * combobox, gpointer user_data)
{
  ((WindowNotes *) user_data)->combobox_note_edit_font_size_changed();
}


void WindowNotes::combobox_note_edit_font_size_changed()
{
  GtkHTMLFontStyle style = note_editor_font_size_name_to_enum(combobox_get_active_string(combobox_note_edit_font_size));
  gtk_html_set_font_style(GTK_HTML(htmlview_note_editor), GtkHTMLFontStyle(GTK_HTML_FONT_STYLE_SIZE_MASK & ~GTK_HTML_FONT_STYLE_SIZE_MASK), style);
}


void WindowNotes::on_note_editor_insertion_font_style_changed(GtkHTML * html, GtkHTMLFontStyle style, gpointer user_data)
{
  ((WindowNotes *) user_data)->note_editor_insertion_font_style_changed(style);
}


void WindowNotes::note_editor_insertion_font_style_changed(GtkHTMLFontStyle style)
{
  combobox_set_string(combobox_note_edit_font_size, note_editor_font_size_enum_to_name(GtkHTMLFontStyle(style & GTK_HTML_FONT_STYLE_SIZE_MASK)));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(togglebutton_note_edit_italics), style & GTK_HTML_FONT_STYLE_ITALIC);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(togglebutton_note_edit_bold), style & GTK_HTML_FONT_STYLE_BOLD);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(togglebutton_note_edit_underline), style & GTK_HTML_FONT_STYLE_UNDERLINE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(togglebutton_note_edit_strike_through), style & GTK_HTML_FONT_STYLE_STRIKEOUT);
}


void WindowNotes::on_combobox_note_edit_paragraph_style_changed(GtkComboBox * combobox, gpointer user_data)
{
  ((WindowNotes *) user_data)->combobox_note_edit_paragraph_style_changed();
}


void WindowNotes::combobox_note_edit_paragraph_style_changed()
{
  GtkHTMLParagraphStyle style = note_editor_paragraph_style_name_to_enum(combobox_get_active_string(combobox_note_edit_paragraph_style));
  gtk_html_set_paragraph_style(GTK_HTML(htmlview_note_editor), style);
}


void WindowNotes::on_note_editor_current_paragraph_style_changed(GtkHTML * html, GtkHTMLParagraphStyle style, gpointer user_data)
{
  ((WindowNotes *) user_data)->note_editor_current_paragraph_style_changed(style);
}


void WindowNotes::note_editor_current_paragraph_style_changed(GtkHTMLParagraphStyle style)
{
  combobox_set_string(combobox_note_edit_paragraph_style, note_editor_paragraph_style_enum_to_name(style));
}


void WindowNotes::on_togglebutton_note_edit_bold_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((WindowNotes *) user_data)->togglebutton_note_edit_bold_toggled();
}


void WindowNotes::togglebutton_note_edit_bold_toggled()
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton_note_edit_bold))) {
    gtk_html_set_font_style(GTK_HTML(htmlview_note_editor), (GtkHTMLFontStyle) ~ 0, GTK_HTML_FONT_STYLE_BOLD);
  } else {
    gtk_html_set_font_style(GTK_HTML(htmlview_note_editor), (GtkHTMLFontStyle) ~ GTK_HTML_FONT_STYLE_BOLD, (GtkHTMLFontStyle) 0);
  }
}


void WindowNotes::on_togglebutton_note_edit_italics_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((WindowNotes *) user_data)->togglebutton_note_edit_italics_toggled();
}


void WindowNotes::togglebutton_note_edit_italics_toggled()
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton_note_edit_italics))) {
    gtk_html_set_font_style(GTK_HTML(htmlview_note_editor), (GtkHTMLFontStyle) ~ 0, GTK_HTML_FONT_STYLE_ITALIC);
  } else {
    gtk_html_set_font_style(GTK_HTML(htmlview_note_editor), (GtkHTMLFontStyle) ~ GTK_HTML_FONT_STYLE_ITALIC, (GtkHTMLFontStyle) 0);
  }
}


void WindowNotes::on_togglebutton_note_edit_underline_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((WindowNotes *) user_data)->togglebutton_note_edit_underline_toggled();
}


void WindowNotes::togglebutton_note_edit_underline_toggled()
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton_note_edit_underline))) {
    gtk_html_set_font_style(GTK_HTML(htmlview_note_editor), (GtkHTMLFontStyle) ~ 0, GTK_HTML_FONT_STYLE_UNDERLINE);
  } else {
    gtk_html_set_font_style(GTK_HTML(htmlview_note_editor), (GtkHTMLFontStyle) ~ GTK_HTML_FONT_STYLE_UNDERLINE, (GtkHTMLFontStyle) 0);
  }
}


void WindowNotes::on_togglebutton_note_edit_strike_through_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((WindowNotes *) user_data)->togglebutton_note_edit_strike_through_toggled();
}


void WindowNotes::togglebutton_note_edit_strike_through_toggled()
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton_note_edit_strike_through))) {
    gtk_html_set_font_style(GTK_HTML(htmlview_note_editor), (GtkHTMLFontStyle) ~ 0, GTK_HTML_FONT_STYLE_STRIKEOUT);
  } else {
    gtk_html_set_font_style(GTK_HTML(htmlview_note_editor), (GtkHTMLFontStyle) ~ GTK_HTML_FONT_STYLE_STRIKEOUT, (GtkHTMLFontStyle) 0);
  }
}


void WindowNotes::on_togglebutton_note_edit_left_justify_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((WindowNotes *) user_data)->togglebutton_note_edit_left_justify_toggled();
}


void WindowNotes::togglebutton_note_edit_left_justify_toggled()
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton_note_edit_left_justify))) {
    gtk_html_set_paragraph_alignment(GTK_HTML(htmlview_note_editor), GTK_HTML_PARAGRAPH_ALIGNMENT_LEFT);
  }
}


void WindowNotes::on_togglebutton_note_edit_center_justify_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((WindowNotes *) user_data)->togglebutton_note_edit_center_justify_toggled();
}


void WindowNotes::togglebutton_note_edit_center_justify_toggled()
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton_note_edit_center_justify))) {
    gtk_html_set_paragraph_alignment(GTK_HTML(htmlview_note_editor), GTK_HTML_PARAGRAPH_ALIGNMENT_CENTER);
  }
}


void WindowNotes::on_togglebutton_note_edit_right_justify_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((WindowNotes *) user_data)->togglebutton_note_edit_right_justify_toggled();
}


void WindowNotes::togglebutton_note_edit_right_justify_toggled()
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton_note_edit_right_justify))) {
    gtk_html_set_paragraph_alignment(GTK_HTML(htmlview_note_editor), GTK_HTML_PARAGRAPH_ALIGNMENT_RIGHT);
  }
}


void WindowNotes::on_current_paragraph_alignment_changed(GtkHTML * html, GtkHTMLParagraphAlignment new_alignment, gpointer user_data)
{
  ((WindowNotes *) user_data)->current_paragraph_alignment_changed(new_alignment);
}


void WindowNotes::current_paragraph_alignment_changed(GtkHTMLParagraphAlignment new_alignment)
{
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(togglebutton_note_edit_left_justify), new_alignment == GTK_HTML_PARAGRAPH_ALIGNMENT_LEFT);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(togglebutton_note_edit_center_justify), new_alignment == GTK_HTML_PARAGRAPH_ALIGNMENT_CENTER);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(togglebutton_note_edit_right_justify), new_alignment == GTK_HTML_PARAGRAPH_ALIGNMENT_RIGHT);
}


void WindowNotes::on_button_note_edit_decrease_indent_clicked(GtkButton * button, gpointer user_data)
{
  ((WindowNotes *) user_data)->decrease_indent();
}


void WindowNotes::decrease_indent()
{
  if (note_being_edited()) {
    gtk_html_indent_pop_level(GTK_HTML(htmlview_note_editor));
  }
}


void WindowNotes::on_button_note_edit_increase_indent_clicked(GtkButton * button, gpointer user_data)
{
  ((WindowNotes *) user_data)->increase_indent();
}


void WindowNotes::increase_indent()
{
  if (note_being_edited()) {
    gtk_html_indent_push_level(GTK_HTML(htmlview_note_editor), HTML_LIST_TYPE_BLOCKQUOTE);
  }
}


void WindowNotes::on_current_paragraph_indentation_changed(GtkHTML * html, guint new_indentation, gpointer user_data)
{
  ((WindowNotes *) user_data)->current_paragraph_indentation_changed(new_indentation);
}


void WindowNotes::current_paragraph_indentation_changed(guint new_indentation)
{
  gtk_widget_set_sensitive(toolitem_note_edit_decrease_indent, new_indentation > 0);
}


void WindowNotes::on_colorbutton_note_edit_color_set(GtkColorButton * colorbutton, gpointer user_data)
{
  ((WindowNotes *) user_data)->colorbutton_note_edit_color_set(colorbutton);
}


void WindowNotes::colorbutton_note_edit_color_set(GtkColorButton * colorbutton)
{
  GdkColor gdk_color;
  gtk_color_button_get_color(colorbutton, &gdk_color);
  HTMLColor *html_color = html_color_new_from_gdk_color(&gdk_color);
  gtk_html_set_color(GTK_HTML(htmlview_note_editor), html_color);
  html_color_unref(html_color);
}


void WindowNotes::on_insertion_color_changed(GtkHTML * html, GdkColor * color, gpointer user_data)
{
  ((WindowNotes *) user_data)->insertion_color_changed(color);
}


void WindowNotes::insertion_color_changed(GdkColor * color)
{
  gtk_color_button_set_color(GTK_COLOR_BUTTON(colorbutton_note_edit), color);
}


void WindowNotes::redisplay(bool immediately)
{
  // Do not display notes while a note is being edited.
  if (note_being_edited())
    return;
  // Stop any previous notes display.
  stop_displaying_more_notes();
  // Display the notes with a little delay. This improves navigation speed.
  // Or if immediate display is requested, do that.
  gw_destroy_source(redisplay_source_id);
  if (immediately)
    redisplay_timeout();
  else
    redisplay_source_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 500, GSourceFunc(on_redisplay_timeout), gpointer(this), NULL);
}


bool WindowNotes::on_redisplay_timeout(gpointer data)
{
  ((WindowNotes *) data)->redisplay_timeout();
  return false;
}


void WindowNotes::redisplay_timeout()
{
  // Get actual verse from the settings.
  extern Settings *settings;
  ustring reference = books_id_to_english(settings->genconfig.book_get()) + " " + settings->genconfig.chapter_get() + ":" + settings->genconfig.verse_get();
  // Create displaying object.
  displayprojectnotes = new DisplayProjectNotes(reference, htmlview_notes, NULL, edited_note_id);
}


void WindowNotes::stop_displaying_more_notes()
// Stop the process of displaying notes.
{
  if (displayprojectnotes) {
    displayprojectnotes->stop();
  }
  while (displayprojectnotes) {
    // Events must go on to keep the program responsive.
    while (gtk_events_pending())
      gtk_main_iteration();
    g_usleep(100000);
    // While waiting run the on_gui routine, because this deletes the object if it is ready.
    on_gui();
  }
}


bool WindowNotes::on_gui_timeout(gpointer data)
{
  ((WindowNotes *) data)->on_gui();
  return true;
}


void WindowNotes::on_gui()
// Tasks related to the GUI.
{
  // Handle the gui part of displaying project notes.
  // These notes are displayed in a thread, and it is quite a hassle to make Gtk
  // thread-safe, therefore rather than going through this hassle, we just 
  // let the widgets be updated in the main thread.
  if (displayprojectnotes) {
    if (displayprojectnotes->ready) {
      displayprojectnotes->ready = false;
      displayprojectnotes->show_buffer();
      displayprojectnotes->position_cursor();
      delete displayprojectnotes;
      displayprojectnotes = NULL;
    }
  }
}


void WindowNotes::display(vector < unsigned int >&ids)
{
  stop_displaying_more_notes();
  displayprojectnotes = new DisplayProjectNotes("", htmlview_notes, &ids, edited_note_id);
}


void WindowNotes::on_button_cancel_clicked(GtkButton * button, gpointer user_data)
{
  ((WindowNotes *) user_data)->on_notes_button_cancel();
}


void WindowNotes::on_notes_button_cancel()
{
  // Do standard functions for both ok and cancel.
  on_notes_button_ok_cancel();
}


void WindowNotes::on_button_ok_clicked(GtkButton * button, gpointer user_data)
{
  ((WindowNotes *) user_data)->on_notes_button_ok();
}


void WindowNotes::on_notes_button_ok()
{
  // Validate and normalize the references.
  // Bad ones are removed and a message will be given.
  // If no valid references remain, stop the whole transaction and give a message.
  ustring osis_references;
  // Get and validate all references from the textview.
  {
    // Store references.
    vector <Reference> references;
    // Store possible messages here for later display.
    vector <ustring> messages;
    // Get all references from the editor.
    notes_get_references_from_editor(note_editor->textbuffer_references, references, messages);
    // Store the references in OSIS format too.
    for (unsigned int i = 0; i < references.size(); i++) {
      ustring osis_book = books_id_to_osis(references[i].book);
      ustring osis_reference = osis_book + "." + convert_to_string(references[i].chapter) + "." + references[i].verse;
      if (!osis_references.empty())
        osis_references.append(" ");
      osis_references.append(osis_reference);
    }
    // See whether there are messages to display.
    if (messages.size() > 0) {
      ustring message;
      for (unsigned int i = 0; i < messages.size(); i++) {
        message.append(messages[i]);
        message.append("\n");
      }
      gtkw_dialog_error(vbox_client, message);
    }
  }
  // See whether any references are left. If not give a message and bail out.
  if (osis_references.empty()) {
    gtkw_dialog_error(vbox_client, "No valid references. Note was not stored");
    return;
  }

  // Store the data in the file.

  // ID (integer),
  // References (text), we take variable "encoded_references"
  // Project (text) - done already.    
  // Category (text)
  ustring category = combobox_get_active_string(combobox_note_category);
  // Note (text)
  gtk_html_save(GTK_HTML(htmlview_note_editor), (GtkHTMLSaveReceiverFn) note_save_receiver, gpointer(note_editor));
  ustring note = note_editor->clean_edited_data();
  // Date created. Variabele note_info_date_created
  // Date modified.
  int date_modified;
  date_modified = date_time_julian_day_get_current();
  // Username. Use: note_info_user_created
  // Logbook (text)
  {
    // Trim off extra newlines at the end.
    logbook = trim(logbook);
    // Now add new data to the logbook.
    ustring date_user_text = date_time_julian_human_readable(date_modified, true);
    date_user_text.append(", ");
    date_user_text.append(g_get_real_name());
    date_user_text.append(" ");
    if (note_editor->newnote) {
      if (!logbook.empty())
        logbook.append("\n");
      logbook.append(date_user_text);
      logbook.append("created a new note, category \"");
      logbook.append(category);
      logbook.append("\", project \"");
      logbook.append(project);
      logbook.append("\".");
    } else {
      vector < ustring > actions;
      if (gtk_text_buffer_get_modified(note_editor->textbuffer_references)) {
        actions.push_back("modified the references");
      }
      if (note_editor->data_was_edited()) {
        actions.push_back("modified the note");
      }
      if (category != note_editor->previous_category) {
        actions.push_back("changed the category to \"" + category + "\"");
      }
      if (project != note_editor->previous_project) {
        actions.push_back("changed the project to \"" + project + "\"");
      }
      if (actions.size() > 0) {
        if (!logbook.empty())
          logbook.append("\n");
        logbook.append(date_user_text);
        for (unsigned int i = 0; i < actions.size(); i++) {
          if (i > 0)
            logbook.append(", ");
          logbook.append(actions[i]);
        }
        logbook.append(".");
      }
    }
    // Apostrophies need to be doubled before storing them.
    logbook = double_apostrophy(logbook);
  }
  // Store the data to file.
  notes_store_one_in_file(note_editor->id, note, project, osis_references, category, note_editor->date_created, note_editor->created_by, date_modified, logbook);

  // Store the note's id, so that this note can be displayed even if it normally would not have displayed.
  // The reason is that if a user edits a note, then clicks OK to store it, then it would disappear,
  // then the user would wonder where that note went, whether he cancelled it by mistake. 
  // For that reason the id of this note is stored, so that when displaying notes, this id should display as well.
  edited_note_id = note_editor->id;

  // Do standard functions for both ok and cancel.
  on_notes_button_ok_cancel();
}


void WindowNotes::on_notes_button_ok_cancel()
// Functions common to both the ok and cancel buttons.
{
  // Show the normal notes display again.
  gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook1), 0);

  // Clear some widgets.
  combobox_clear_strings(combobox_note_category);
  gtk_text_buffer_set_text(note_editor->textbuffer_references, "", -1);

  // Clear the html editor.
  gtk_html_set_editable(GTK_HTML(htmlview_note_editor), false);
  gtk_html_load_empty(GTK_HTML(htmlview_note_editor));

  // Destroy NoteEditor object.
  if (note_editor)
    delete note_editor;
  note_editor = NULL;

  // Just to be sure, redisplay the notes.
  redisplay();
}


gboolean WindowNotes::note_save_receiver(const HTMLEngine * engine, const char *data, unsigned int len, void *user_data)
// Called by the gtkhtml project note editor when saving its data
{
  ((NoteEditor *) user_data)->receive_data_from_html_editor(data, len);
  return true;
}


void WindowNotes::insert_standard_text(unsigned int selector)
// Sets the system to insert standard text into the note.
{
  // Bail out if no note is being edited.
  if (!note_being_edited())
    return;

  // Find out which standard text to insert, and where to insert it, and how.
  extern Settings *settings;
  ustring standardtext;
  bool addspace = false;
  bool gtkhtml = false;
  GtkWidget *textview = htmlview_note_editor;
  if (selector == 0) {
    standardtext = settings->genconfig.edit_note_standard_text_one_get();
    addspace = true;
    textview = htmlview_note_editor;
    gtkhtml = true;
  } else if (selector == 1) {
    standardtext = settings->genconfig.edit_note_standard_text_two_get();
    addspace = true;
    textview = htmlview_note_editor;
    gtkhtml = true;
  } else if (selector == 2) {
    standardtext = settings->genconfig.edit_note_standard_text_three_get();
    addspace = true;
    textview = htmlview_note_editor;
    gtkhtml = true;
  } else if (selector == 3) {
    standardtext = settings->genconfig.edit_note_standard_text_four_get();
    addspace = true;
    textview = htmlview_note_editor;
    gtkhtml = true;
  } else if (selector == 4) {
    standardtext = books_id_to_english(settings->genconfig.book_get()) + " " + settings->genconfig.chapter_get() + ":" + settings->genconfig.verse_get();
    addspace = false;
    textview = textview_note_references;
    gtkhtml = false;
  }
  // Add space.
  if (addspace)
    standardtext.append(" ");

  // If text was selected, erase that text.
  if (!gtkhtml) {
    GtkTextIter iter1, iter2;
    if (gtk_text_buffer_get_selection_bounds(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), &iter1, &iter2)) {
      gtk_text_buffer_delete(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), &iter1, &iter2);
    }
  }
  // If buffer does not end with a new line, insert one.
  if (!gtkhtml) {
    GtkTextIter enditer;
    gtk_text_buffer_get_end_iter(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), &enditer);
    if (!gtk_text_iter_starts_line(&enditer)) {
      gtk_text_buffer_insert(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), &enditer, "\n", -1);
    }
  }
  // Message.
  ustring message;
  // Add the standard text.
  message.append(standardtext);
  // Insert message at the end of the note.
  if (gtkhtml) {
    gtk_html_insert_html(GTK_HTML(htmlview_note_editor), message.c_str());
  } else {
    GtkTextIter enditer;
    gtk_text_buffer_get_end_iter(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), &enditer);
    gtk_text_buffer_insert(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), &enditer, message.c_str(), -1);
  }
}


void WindowNotes::get_references_from_note(vector < Reference > &references, vector < ustring > &messages)
{
  if (note_editor)
    notes_get_references_from_editor(note_editor->textbuffer_references, references, messages);
}


void WindowNotes::get_references_from_id(gint id)
// Get the references from the note id
{
  // Clear references.
  available_references.clear();
  // Fetch the references for the note from the file.
  ustring note;
  ustring project;
  ustring references;
  ustring category;
  int date_created;
  ustring user_created;
  int date_modified;
  ustring logbook;
  notes_read_one_from_file (id, note, project, references, category, date_created, user_created, date_modified, logbook);
  // Read the reference(s).
  Parse parse(references, false);
  for (unsigned int i = 0; i < parse.words.size(); i++) {
    Reference ref(0);
    reference_discover(0, 0, "", parse.words[i], ref.book, ref.chapter, ref.verse);
    available_references.push_back(ref);
  }
  // Fire a signal indicating that references are available
  gtk_button_clicked(GTK_BUTTON(references_available_signal_button));
}


gboolean WindowNotes::on_html_link_clicked(GtkHTML * html, const gchar * url, gpointer user_data)
{
  ((WindowNotes *) user_data)->html_link_clicked(html, url);
  return true;
}


void WindowNotes::html_link_clicked(GtkHTML * html, const gchar * url)
// Callback for clicking a link in the project notes.
{
  ustring myurl(url);
  bool del = myurl.substr(0, 1) == "d";
  bool refs = myurl.substr(0, 1) == "r";
  myurl = number_in_string(myurl);
  unsigned int id = convert_to_int(myurl);
  if (del) {
    // Delete the note.
    vector < gint > ids;
    ids.push_back(id);
    delete_ids(ids);
  } else if (refs) {
    // Get the reference(s) from the note.
    get_references_from_id(id);
  } else {
    // Edit note.
    notes_fill_edit_screen(id, false);
  }
}


void WindowNotes::delete_ids(const vector < gint > &ids)
// Deletes notes whose id is given in "ids".
{
  if (ids.empty())
    return;

  ustring message = "Are you sure you want to delete ";
  if (ids.size() == 1)
    message.append("this note");
  else
    message.append("these notes");
  message.append("?");
  if (!yes_no_always_dialog (message, ynadtDeleteNote, true, false))
    return;
  for (unsigned int i = 0; i < ids.size(); i++) {
    notes_delete_one(ids[i]);
  }

  // Immediate redisplay. This allows the user to easily delete a row of notes.
  redisplay(true);
}


void WindowNotes::cut()
{
  // Cut to clipboard if editing.
  if (note_being_edited()) {
    if (last_focused_widget == htmlview_note_editor)
      gtk_html_cut(GTK_HTML(htmlview_note_editor));
    if (last_focused_widget == textview_note_references) {
      GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
      gtk_text_buffer_cut_clipboard(note_editor->textbuffer_references, clipboard, true);
    }
  }
}


void WindowNotes::copy()
{
  // Copy to clipboard.
  if (note_being_edited()) {
    if (last_focused_widget == htmlview_note_editor)
      gtk_html_copy(GTK_HTML(htmlview_note_editor));
    if (last_focused_widget == textview_note_references) {
      GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
      gtk_text_buffer_copy_clipboard(note_editor->textbuffer_references, clipboard);
    }
  } else {
    gtk_html_copy(GTK_HTML(htmlview_notes));
  }
}


void WindowNotes::paste()
{
  // Paste from clipboard if editing.
  if (note_being_edited()) {
    if (last_focused_widget == htmlview_note_editor)
      gtk_html_paste(GTK_HTML(htmlview_note_editor), false);
    if (last_focused_widget == textview_note_references) {
      GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
      gtk_text_buffer_paste_clipboard(note_editor->textbuffer_references, clipboard, NULL, true);
    }
  }
}


void WindowNotes::undo()
{
  // Undo if editing.
  if (note_being_edited()) {
    gtk_html_undo(GTK_HTML(htmlview_note_editor));
  }
}


void WindowNotes::redo()
{
  if (note_being_edited()) {
    gtk_html_redo(GTK_HTML(htmlview_note_editor));
  }
}


bool WindowNotes::note_being_edited()
// Returns whether a note is now being edited.
{
  return (gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook1)) > 0);
}


void WindowNotes::on_button_more_clicked(GtkButton * button, gpointer user_data)
{
  ((WindowNotes *) user_data)->on_button_more();
}


void WindowNotes::on_button_more()
{
  ProjectNoteDialog dialog(vbox_client, projects, project, created_on, created_by, edited_on, logbook);
  if (dialog.run() == GTK_RESPONSE_OK) {
    project = dialog.project;
  }
}


void WindowNotes::set_fonts()
{
  extern Settings *settings;
  if (!settings->genconfig.text_editor_font_default_get()) {
    PangoFontDescription *desired_font_description = pango_font_description_from_string(settings->genconfig.text_editor_font_name_get().c_str());
    const char * desired_font_family = pango_font_description_get_family (desired_font_description);

    PangoContext * pango_context = NULL;
    PangoFontDescription *font_desc = NULL;
    
    pango_context = gtk_widget_get_pango_context (htmlview_notes);
    font_desc = pango_context_get_font_description (pango_context);
    pango_font_description_set_family (font_desc, desired_font_family);

    pango_context = gtk_widget_get_pango_context (htmlview_note_editor);
    font_desc = pango_context_get_font_description (pango_context);
    pango_font_description_set_family (font_desc, desired_font_family);

    pango_font_description_free(desired_font_description);
  }
}



