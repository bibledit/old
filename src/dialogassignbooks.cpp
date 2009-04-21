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
#include "dialogassignbooks.h"
#include "dialogtextview.h"
#include "books.h"
#include "dialoglistview.h"
#include "help.h"

#define UNKNOWNBOOK "<Unknown>"

AssignBooksDialog::AssignBooksDialog(vector < ustring > &unknown_filenames)
{
  myunknown_filenames = &unknown_filenames;

  assignbooksdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(assignbooksdialog), "Assign names to books");
  gtk_window_set_position(GTK_WINDOW(assignbooksdialog), GTK_WIN_POS_CENTER_ON_PARENT);

  dialog_vbox1 = GTK_DIALOG(assignbooksdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  label12 = gtk_label_new("Bibledit was unable to determine the names for one or more books.\nAssign names to them.");
  gtk_widget_show(label12);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label12, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label12), 0, 0.5);

  for (unsigned int i = 0; i < unknown_filenames.size(); i++) {

    hseparator1 = gtk_hseparator_new();
    gtk_widget_show(hseparator1);
    gtk_box_pack_start(GTK_BOX(dialog_vbox1), hseparator1, TRUE, TRUE, 0);

    hbox6 = gtk_hbox_new(FALSE, 6);
    gtk_widget_show(hbox6);
    gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox6, TRUE, TRUE, 0);

    button_view = gtk_button_new();
    gtk_widget_show(button_view);
    gtk_box_pack_start(GTK_BOX(hbox6), button_view, FALSE, FALSE, 0);

    g_signal_connect((gpointer) button_view, "clicked", G_CALLBACK(on_button_view_clicked), gpointer(this));

    view_buttons.push_back(button_view);

    alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_widget_show(alignment1);
    gtk_container_add(GTK_CONTAINER(button_view), alignment1);

    hbox7 = gtk_hbox_new(FALSE, 2);
    gtk_widget_show(hbox7);
    gtk_container_add(GTK_CONTAINER(alignment1), hbox7);

    image1 = gtk_image_new_from_stock("gtk-zoom-fit", GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(image1);
    gtk_box_pack_start(GTK_BOX(hbox7), image1, FALSE, FALSE, 0);

    label17 = gtk_label_new_with_mnemonic("_View");
    gtk_widget_show(label17);
    gtk_box_pack_start(GTK_BOX(hbox7), label17, FALSE, FALSE, 0);

    button_assign = gtk_button_new();
    gtk_widget_show(button_assign);
    gtk_box_pack_start(GTK_BOX(hbox6), button_assign, FALSE, FALSE, 0);

    g_signal_connect((gpointer) button_assign, "clicked", G_CALLBACK(on_button_assign_clicked), gpointer(this));

    assign_buttons.push_back(button_assign);

    alignment2 = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_widget_show(alignment2);
    gtk_container_add(GTK_CONTAINER(button_assign), alignment2);

    hbox8 = gtk_hbox_new(FALSE, 2);
    gtk_widget_show(hbox8);
    gtk_container_add(GTK_CONTAINER(alignment2), hbox8);

    image2 = gtk_image_new_from_stock("gtk-spell-check", GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(image2);
    gtk_box_pack_start(GTK_BOX(hbox8), image2, FALSE, FALSE, 0);

    label18 = gtk_label_new_with_mnemonic("_Assign");
    gtk_widget_show(label18);
    gtk_box_pack_start(GTK_BOX(hbox8), label18, FALSE, FALSE, 0);

    label13 = gtk_label_new("Filename:");
    gtk_widget_show(label13);
    gtk_box_pack_start(GTK_BOX(hbox6), label13, FALSE, FALSE, 0);
    gtk_misc_set_alignment(GTK_MISC(label13), 0, 0.5);

    label_filename = gtk_label_new(unknown_filenames[i].c_str());
    gtk_widget_show(label_filename);
    gtk_box_pack_start(GTK_BOX(hbox6), label_filename, FALSE, FALSE, 0);

    label15 = gtk_label_new("Bookname:");
    gtk_widget_show(label15);
    gtk_box_pack_start(GTK_BOX(hbox6), label15, FALSE, FALSE, 0);

    label_bookname = gtk_label_new(UNKNOWNBOOK);
    gtk_widget_show(label_bookname);
    gtk_box_pack_start(GTK_BOX(hbox6), label_bookname, FALSE, FALSE, 0);

    bookname_labels.push_back(label_bookname);

  }

  dialog_action_area1 = GTK_DIALOG(assignbooksdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(assignbooksdialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(assignbooksdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(assignbooksdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_button_ok_clicked), gpointer(this));

  gtk_widget_grab_focus(okbutton);
  gtk_widget_grab_default(okbutton);
}

AssignBooksDialog::~AssignBooksDialog()
{
  gtk_widget_destroy(assignbooksdialog);
}

int AssignBooksDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(assignbooksdialog));
}

void AssignBooksDialog::on_button_view_clicked(GtkButton * button, gpointer user_data)
{
  ((AssignBooksDialog *) user_data)->on_view(button);
}

void AssignBooksDialog::on_view(GtkButton * button)
{
  for (unsigned int i = 0; i < view_buttons.size(); i++) {
    if (GTK_WIDGET(button) == view_buttons[i]) {
      gchar *contents;
      g_file_get_contents(myunknown_filenames->at(i).c_str(), &contents, NULL, NULL);
      TextViewDialog dialog("Unknown book", "", false, contents);
      dialog.run();
    }
  }
}

void AssignBooksDialog::on_button_assign_clicked(GtkButton * button, gpointer user_data)
{
  ((AssignBooksDialog *) user_data)->on_assign(button);
}

void AssignBooksDialog::on_assign(GtkButton * button)
{
  for (unsigned int i = 0; i < assign_buttons.size(); i++) {
    if (GTK_WIDGET(button) == assign_buttons[i]) {
      vector < ustring > books;
      {
        vector < unsigned int >ids = books_type_to_ids(btUnknown);
        for (unsigned int id = 0; id < ids.size(); id++) {
          books.push_back(books_id_to_english(ids[id]));
        }
      }
      {
        vector < unsigned int >ids;
        for (unsigned int i2 = 0; i2 < books.size(); i2++)
          ids.push_back(books_english_to_id(books[i2]));
        books_standard_order(ids);
        books.clear();
        for (unsigned int i2 = 0; i2 < ids.size(); i2++)
          books.push_back(books_id_to_english(ids[i2]));
      }
      books.insert(books.begin(), UNKNOWNBOOK);
      ustring filename = myunknown_filenames->at(i);
      ustring bookname = UNKNOWNBOOK;
      for (unsigned int i2 = 0; i2 < assigned_files.size(); i2++) {
        if (filename == assigned_files[i2])
          bookname = assigned_books[i2];
      }
      ListviewDialog dialog("Assign a name to this book", books, bookname, false, NULL);
      if (dialog.run() == GTK_RESPONSE_OK) {
        if (dialog.focus != UNKNOWNBOOK) {
          assigned_files.push_back(filename);
          assigned_books.push_back(dialog.focus);
          gtk_label_set_text(GTK_LABEL(bookname_labels[i]), dialog.focus.c_str());
        }
      }
    }
  }
}

void AssignBooksDialog::on_button_ok_clicked(GtkButton * button, gpointer user_data)
{
  ((AssignBooksDialog *) user_data)->on_ok();
}

void AssignBooksDialog::on_ok()
{
  for (unsigned int i = 0; i < assigned_books.size(); i++)
    if (assigned_books[i] == UNKNOWNBOOK)
      assigned_books[i].clear();
}

bool AssignBooksDialog::assigned(const ustring & bookname)
{
  return (bookname != UNKNOWNBOOK);
}
