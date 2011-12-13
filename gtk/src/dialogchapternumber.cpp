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
#include <gtk/gtk.h>
#include "dialogchapternumber.h"
#include "settings.h"
#include "projectutils.h"
#include "versification.h"
#include "combobox.h"
#include "help.h"
#include "books.h"
#include "tiny_utilities.h"


ChapterNumberDialog::ChapterNumberDialog(int dummy)
{
  // Build gui.  
  dialogchapternumber = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialogchapternumber), "Insert Chapter");
  gtk_window_set_position(GTK_WINDOW(dialogchapternumber), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint(GTK_WINDOW(dialogchapternumber), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(dialogchapternumber)->vbox;
  gtk_widget_show(dialog_vbox1);

  label_purpose = gtk_label_new("Insert a new chapter");
  gtk_widget_show(label_purpose);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label_purpose, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_purpose), 0, 0.5);

  hbox1 = gtk_hbox_new(FALSE, 5);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new("Number");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(hbox1), label1, FALSE, FALSE, 0);

  entry1 = gtk_entry_new();
  gtk_widget_show(entry1);
  gtk_box_pack_start(GTK_BOX(hbox1), entry1, FALSE, FALSE, 0);
  gtk_entry_set_max_length(GTK_ENTRY(entry1), 4);
  gtk_entry_set_activates_default(GTK_ENTRY(entry1), TRUE);
  gtk_entry_set_width_chars(GTK_ENTRY(entry1), 4);

  label_info = gtk_label_new("");
  gtk_widget_show(label_info);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_info), 0, 0.5);

  dialog_action_area1 = GTK_DIALOG(dialogchapternumber)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(dialogchapternumber, NULL, NULL, NULL);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(dialogchapternumber), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(dialogchapternumber), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) entry1, "changed", G_CALLBACK(on_entry1_changed), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label1), entry1);

  gtk_widget_grab_focus(okbutton1);
  gtk_widget_grab_default(okbutton1);

  // GUI.
  set_gui();
}


ChapterNumberDialog::~ChapterNumberDialog()
{
  gtk_widget_destroy(dialogchapternumber);
}


int ChapterNumberDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialogchapternumber));
}


void ChapterNumberDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((ChapterNumberDialog *) user_data)->on_ok();
}


void ChapterNumberDialog::on_ok()
// Insert a chapter
{
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  unsigned int newchapter = get_chapter();
  vector < ustring > lines;
  versification_create_chapter_template(projectconfig->versification_get(), settings->genconfig.book_get(), newchapter, lines);
  CategorizeChapterVerse ccv(lines);
  project_store_chapter(settings->genconfig.project_get(), settings->genconfig.book_get(), ccv);
  settings->genconfig.chapter_set(convert_to_string(newchapter));
}


void ChapterNumberDialog::set_gui()
{
  extern Settings *settings;

  set < unsigned int >current_chapters;
  {
    vector < unsigned int >chapters = project_get_chapters(settings->genconfig.project_get(), settings->genconfig.book_get());
    for (unsigned int i = 0; i < chapters.size(); i++)
      current_chapters.insert(chapters[i]);
  }

  bool sensitive = true;
  ustring message;
  ustring ch = gtk_entry_get_text(GTK_ENTRY(entry1));
  ch = number_in_string(ch);
  if (ch.empty()) {
    sensitive = false;
    message = "Enter a chapter number";
  } else {
    if (current_chapters.find(convert_to_int(ch)) != current_chapters.end()) {
      sensitive = false;
      message = "This chapter exists in the book";
    }
  }
  gtk_widget_set_sensitive(okbutton1, sensitive);
  gtk_label_set_text(GTK_LABEL(label_info), message.c_str());
}


void ChapterNumberDialog::on_entry1_changed(GtkEditable * editable, gpointer user_data)
{
  ((ChapterNumberDialog *) user_data)->set_gui();
}


unsigned int ChapterNumberDialog::get_chapter()
{
  return convert_to_int(gtk_entry_get_text(GTK_ENTRY(entry1)));
}


