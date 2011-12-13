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
#include "dialogeditdictionary.h"
#include "help.h"
#include "spelling.h"
#include "utilities.h"
#include "shortcuts.h"
#include "tiny_utilities.h"
#include "projectutils.h"
#include "dialoglistview.h"
#include "gtkwrappers.h"
#include "screen.h"


EditDictionaryDialog::EditDictionaryDialog(const ustring & dictionary)
{
  mydictionary = dictionary;
  Shortcuts shortcuts(0);

  textviewdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(textviewdialog), "Edit Dictionary");
  gtk_window_set_position(GTK_WINDOW(textviewdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint(GTK_WINDOW(textviewdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(textviewdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), scrolledwindow1, TRUE, TRUE, 0);
  // Never show horizontal scrollbar.
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_SHADOW_IN);

  textview1 = gtk_text_view_new();
  gtk_widget_show(textview1);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), textview1);

  table1 = gtk_table_new(3, 4, FALSE);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), table1, FALSE, FALSE, 0);
  gtk_table_set_row_spacings(GTK_TABLE(table1), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table1), 5);

  label4 = gtk_label_new("Import from");
  gtk_widget_show(label4);
  gtk_table_attach(GTK_TABLE(table1), label4, 1, 2, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  label5 = gtk_label_new("Export to");
  gtk_widget_show(label5);
  gtk_table_attach(GTK_TABLE(table1), label5, 2, 3, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  button_import_dict = gtk_button_new();
  gtk_widget_show(button_import_dict);
  gtk_table_attach(GTK_TABLE(table1), button_import_dict, 1, 2, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment3 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment3);
  gtk_container_add(GTK_CONTAINER(button_import_dict), alignment3);

  hbox4 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox4);
  gtk_container_add(GTK_CONTAINER(alignment3), hbox4);

  image3 = gtk_image_new_from_stock("gtk-harddisk", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image3);
  gtk_box_pack_start(GTK_BOX(hbox4), image3, FALSE, FALSE, 0);

  label3 = gtk_label_new_with_mnemonic("Dictionary");
  gtk_widget_show(label3);
  gtk_box_pack_start(GTK_BOX(hbox4), label3, FALSE, FALSE, 0);

  shortcuts.label(label3);

  button_import_file = gtk_button_new();
  gtk_widget_show(button_import_file);
  gtk_table_attach(GTK_TABLE(table1), button_import_file, 1, 2, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment2 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment2);
  gtk_container_add(GTK_CONTAINER(button_import_file), alignment2);

  hbox3 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox3);
  gtk_container_add(GTK_CONTAINER(alignment2), hbox3);

  image2 = gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image2);
  gtk_box_pack_start(GTK_BOX(hbox3), image2, FALSE, FALSE, 0);

  label2 = gtk_label_new_with_mnemonic("File");
  gtk_widget_show(label2);
  gtk_box_pack_start(GTK_BOX(hbox3), label2, FALSE, FALSE, 0);

  shortcuts.label(label2);

  button_sort = gtk_button_new();
  gtk_widget_show(button_sort);
  gtk_table_attach(GTK_TABLE(table1), button_sort, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(button_sort), alignment1);

  hbox2 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox2);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox2);

  image1 = gtk_image_new_from_stock("gtk-sort-ascending", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox2), image1, FALSE, FALSE, 0);

  label1 = gtk_label_new_with_mnemonic("Sort, remove duplicates");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(hbox2), label1, FALSE, FALSE, 0);

  shortcuts.label(label1);

  button_export_dict = gtk_button_new();
  gtk_widget_show(button_export_dict);
  gtk_table_attach(GTK_TABLE(table1), button_export_dict, 2, 3, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment4 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment4);
  gtk_container_add(GTK_CONTAINER(button_export_dict), alignment4);

  hbox5 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox5);
  gtk_container_add(GTK_CONTAINER(alignment4), hbox5);

  image4 = gtk_image_new_from_stock("gtk-harddisk", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image4);
  gtk_box_pack_start(GTK_BOX(hbox5), image4, FALSE, FALSE, 0);

  label6 = gtk_label_new_with_mnemonic("Dictionary");
  gtk_widget_show(label6);
  gtk_box_pack_start(GTK_BOX(hbox5), label6, FALSE, FALSE, 0);

  shortcuts.label(label6);

  button_export_file = gtk_button_new();
  gtk_widget_show(button_export_file);
  gtk_table_attach(GTK_TABLE(table1), button_export_file, 2, 3, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment5 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment5);
  gtk_container_add(GTK_CONTAINER(button_export_file), alignment5);

  hbox6 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox6);
  gtk_container_add(GTK_CONTAINER(alignment5), hbox6);

  image5 = gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image5);
  gtk_box_pack_start(GTK_BOX(hbox6), image5, FALSE, FALSE, 0);

  label7 = gtk_label_new_with_mnemonic("File");
  gtk_widget_show(label7);
  gtk_box_pack_start(GTK_BOX(hbox6), label7, FALSE, FALSE, 0);

  shortcuts.label(label7);

  button_count = gtk_button_new();
  gtk_widget_show(button_count);
  gtk_table_attach(GTK_TABLE(table1), button_count, 3, 4, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment6 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment6);
  gtk_container_add(GTK_CONTAINER(button_count), alignment6);

  hbox7 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox7);
  gtk_container_add(GTK_CONTAINER(alignment6), hbox7);

  image6 = gtk_image_new_from_stock("gtk-info", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image6);
  gtk_box_pack_start(GTK_BOX(hbox7), image6, FALSE, FALSE, 0);

  label8 = gtk_label_new_with_mnemonic("Word count");
  gtk_widget_show(label8);
  gtk_box_pack_start(GTK_BOX(hbox7), label8, FALSE, FALSE, 0);

  shortcuts.label(label8);

  dialog_action_area1 = GTK_DIALOG(textviewdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(textviewdialog, NULL, &shortcuts, "file/project/project-properties/dictionaries/edit");

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(textviewdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(textviewdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton1);
  shortcuts.stockbutton(okbutton1);
  shortcuts.process();

  g_signal_connect((gpointer) button_sort, "clicked", G_CALLBACK(on_button_sort_clicked), gpointer(this));
  g_signal_connect((gpointer) button_import_dict, "clicked", G_CALLBACK(on_button_import_dict_clicked), gpointer(this));
  g_signal_connect((gpointer) button_import_file, "clicked", G_CALLBACK(on_button_import_file_clicked), gpointer(this));
  g_signal_connect((gpointer) button_export_dict, "clicked", G_CALLBACK(on_button_export_dict_clicked), gpointer(this));
  g_signal_connect((gpointer) button_export_file, "clicked", G_CALLBACK(on_button_export_file_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));
  g_signal_connect((gpointer) button_count, "clicked", G_CALLBACK(on_button_count_clicked), gpointer(this));

  gtk_widget_grab_focus(textview1);
  gtk_widget_grab_default(okbutton1);

  // Load dictionary.  
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textview1), true);
  textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
  filename = spelling_dictionary_filename(dictionary);

  gchar *contents;
  g_file_get_contents(filename.c_str(), &contents, NULL, NULL);
  if (contents) {
    gtk_text_buffer_set_text(textbuffer, contents, -1);
    g_free(contents);
  }

  new DialogAutoScaler (textviewdialog, G_MAXINT);
}


EditDictionaryDialog::~EditDictionaryDialog()
{
  gtk_widget_destroy(textviewdialog);
}


int EditDictionaryDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(textviewdialog));
}


void EditDictionaryDialog::on_button_sort_clicked(GtkButton * button, gpointer user_data)
{
  ((EditDictionaryDialog *) user_data)->on_button_sort(NULL);
}


void EditDictionaryDialog::on_button_sort(vector < ustring > *lines_ptr)
{
  vector < ustring > lines;
  vector < ustring > content;
  set < ustring > contentset;
  if (lines_ptr)
    lines = *lines_ptr;
  else
    textbuffer_get_lines(textbuffer, lines, true);
  for (unsigned int i = 0; i < lines.size(); i++) {
    if (lines[i].empty())
      continue;
    if (contentset.find(lines[i]) != contentset.end())
      continue;
    contentset.insert(lines[i]);
    content.push_back(lines[i]);
  }
  sort(content.begin(), content.end());
  gtk_text_buffer_set_text(textbuffer, "", -1);
  for (unsigned int i = 0; i < content.size(); i++) {
    gtk_text_buffer_insert_at_cursor(textbuffer, content[i].c_str(), -1);
    gtk_text_buffer_insert_at_cursor(textbuffer, "\n", -1);
  }
  GtkTextIter iter;
  gtk_text_buffer_get_start_iter(textbuffer, &iter);
  gtk_text_buffer_place_cursor(textbuffer, &iter);
}


void EditDictionaryDialog::on_button_import_dict_clicked(GtkButton * button, gpointer user_data)
{
  ((EditDictionaryDialog *) user_data)->on_button_import_dict();
}


void EditDictionaryDialog::on_button_import_dict()
{
  vector < ustring > dictionaries = get_other_editable_dictionaries();
  ListviewDialog dialog("Select a dictionary", dictionaries, "", false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    ustring filename = spelling_dictionary_filename(dialog.focus);
    ReadText rt(filename, true, false);
    vector < ustring > lines;
    textbuffer_get_lines(textbuffer, lines, true);
    for (unsigned int i = 0; i < rt.lines.size(); i++) {
      lines.push_back(rt.lines[i]);
    }
    on_button_sort(&lines);
  }
}


void EditDictionaryDialog::on_button_import_file_clicked(GtkButton * button, gpointer user_data)
{
  ((EditDictionaryDialog *) user_data)->on_button_import_file();
}


void EditDictionaryDialog::on_button_import_file()
{
  ustring filename = gtkw_file_chooser_open(textviewdialog, "", "");
  if (filename.empty())
    return;
  ReadText rt(filename, true, false);
  vector < ustring > lines;
  textbuffer_get_lines(textbuffer, lines, true);
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    lines.push_back(rt.lines[i]);
  }
  on_button_sort(&lines);
}


void EditDictionaryDialog::on_button_export_dict_clicked(GtkButton * button, gpointer user_data)
{
  ((EditDictionaryDialog *) user_data)->on_button_export_dict();
}


void EditDictionaryDialog::on_button_export_dict()
{
  vector < ustring > dictionaries = get_other_editable_dictionaries();
  ListviewDialog dialog("Select a dictionary", dictionaries, "", false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    ustring filename = spelling_dictionary_filename(dialog.focus);
    vector < ustring > lines;
    textbuffer_get_lines(textbuffer, lines, true);
    write_lines(filename, lines);
  }
}


void EditDictionaryDialog::on_button_export_file_clicked(GtkButton * button, gpointer user_data)
{
  ((EditDictionaryDialog *) user_data)->on_button_export_file();
}


void EditDictionaryDialog::on_button_export_file()
{
  ustring filename = gtkw_file_chooser_save(textviewdialog, "", "");
  if (filename.empty())
    return;
  vector < ustring > lines;
  textbuffer_get_lines(textbuffer, lines, true);
  write_lines(filename, lines);
}


void EditDictionaryDialog::on_button_count_clicked(GtkButton * button, gpointer user_data)
{
  ((EditDictionaryDialog *) user_data)->on_button_count();
}


void EditDictionaryDialog::on_button_count()
{
  vector < ustring > lines;
  textbuffer_get_lines(textbuffer, lines, true);
  int wordcount = lines.size();
  for (unsigned int i = 0; i < lines.size(); i++) {
    if (lines[i].empty())
      wordcount--;
  }
  gtkw_dialog_info(textviewdialog, "Word count is " + convert_to_string(wordcount));
  write_lines(filename, lines);
}


void EditDictionaryDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((EditDictionaryDialog *) user_data)->on_okbutton();
}


void EditDictionaryDialog::on_okbutton()
{
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_start_iter(textbuffer, &startiter);
  gtk_text_buffer_get_end_iter(textbuffer, &enditer);
  g_file_set_contents(filename.c_str(), gtk_text_buffer_get_text(textbuffer, &startiter, &enditer, true), -1, NULL);
}


vector < ustring > EditDictionaryDialog::get_other_editable_dictionaries()
{
  vector < ustring > dictionaries;
  if (mydictionary != spelling_global_dictionary())
    dictionaries.push_back(spelling_global_dictionary());
  vector < ustring > projects = projects_get_all();
  for (unsigned int i = 0; i < projects.size(); i++) {
    if (mydictionary != spelling_project_dictionary(projects[i])) {
      dictionaries.push_back(spelling_project_dictionary(projects[i]));
    }
  }
  return dictionaries;
}


