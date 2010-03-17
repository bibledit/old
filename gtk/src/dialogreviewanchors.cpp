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
#include "dialogreviewanchors.h"
#include "listview.h"
#include "keyboard.h"
#include "help.h"
#include <gdk/gdkkeysyms.h>
#include "shortcuts.h"
#include "books.h"


ReviewAnchorsDialog::ReviewAnchorsDialog(vector < unsigned int >*books, vector < unsigned int >*chapters, vector < unsigned int >*verses, vector < unsigned int >*linenumbers)
// Reviews anchors that would be placed in a file for the Resource Viewer.
{
  // Initialize variables.
  mybooks = books;
  mychapters = chapters;
  myverses = verses;
  mylinenumbers = linenumbers;

  // Build dialog.
  Shortcuts shortcuts(0);

  reviewanchorsdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(reviewanchorsdialog), "Review anchors");
  gtk_window_set_position(GTK_WINDOW(reviewanchorsdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(reviewanchorsdialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(reviewanchorsdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  label1 = gtk_label_new("Review the anchors that will be placed.\nTo remove some anchors,\nselect them and press the Delete key.");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label1, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

  scrolledwindow3 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow3);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), scrolledwindow3, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow3), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow3), GTK_SHADOW_IN);

  treeview_anchors = gtk_tree_view_new();
  gtk_widget_show(treeview_anchors);
  gtk_container_add(GTK_CONTAINER(scrolledwindow3), treeview_anchors);
  gtk_widget_set_size_request(treeview_anchors, -1, 350);

  hbox2 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox2);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox2, TRUE, TRUE, 0);

  button_spurious_anchors = gtk_button_new();
  gtk_widget_show(button_spurious_anchors);
  gtk_box_pack_start(GTK_BOX(hbox2), button_spurious_anchors, FALSE, FALSE, 0);
  gtk_button_set_focus_on_click(GTK_BUTTON(button_spurious_anchors), FALSE);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(button_spurious_anchors), alignment1);

  hbox3 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox3);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox3);

  image1 = gtk_image_new_from_stock("gtk-find", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox3), image1, FALSE, FALSE, 0);

  label3 = gtk_label_new_with_mnemonic("Select spurious anchors");
  gtk_widget_show(label3);
  gtk_box_pack_start(GTK_BOX(hbox3), label3, FALSE, FALSE, 0);

  shortcuts.label(label3);

  dialog_action_area1 = GTK_DIALOG(reviewanchorsdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(reviewanchorsdialog, NULL, &shortcuts, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(reviewanchorsdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(reviewanchorsdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton);
  shortcuts.stockbutton(okbutton);
  shortcuts.process();

  // Storage, renderer, column and selection.
  store = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview_anchors), GTK_TREE_MODEL(store));
  g_object_unref(store);
  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
  column0 = gtk_tree_view_column_new_with_attributes("Book", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview_anchors), column0);
  column1 = gtk_tree_view_column_new_with_attributes("Chapter", renderer, "text", 1, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview_anchors), column1);
  column2 = gtk_tree_view_column_new_with_attributes("Verse", renderer, "text", 2, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview_anchors), column2);
  column3 = gtk_tree_view_column_new_with_attributes("Line", renderer, "text", 3, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview_anchors), column3);
  select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_anchors));
  gtk_tree_selection_set_mode(select, GTK_SELECTION_MULTIPLE);

  // Fill the treeview with the items, sorted optionally
  GtkTreeIter iter;
  for (unsigned int i = 0; i < books->size(); i++) {
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, books_id_to_english(books->at(i)).c_str(), 1, chapters->at(i), 2, verses->at(i), 3, linenumbers->at(i), 4, books->at(i), -1);
  }

  g_signal_connect((gpointer) treeview_anchors, "key_press_event", G_CALLBACK(on_treeview_key_press_event), gpointer(this));
  g_signal_connect((gpointer) button_spurious_anchors, "clicked", G_CALLBACK(on_button_spurious_anchors_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(static_on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_focus(treeview_anchors);
  gtk_widget_grab_default(okbutton);
}


ReviewAnchorsDialog::~ReviewAnchorsDialog()
{
  gtk_widget_destroy(reviewanchorsdialog);
}


int ReviewAnchorsDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(reviewanchorsdialog));
}


gboolean ReviewAnchorsDialog::on_treeview_key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  return ((ReviewAnchorsDialog *) user_data)->treeview_key_press_handler(event);
}


bool ReviewAnchorsDialog::treeview_key_press_handler(GdkEventKey * event)
// Handles pressing keys in the treeview.
{
  // Take action.
  if (event->keyval == GDK_Delete || event->keyval == GDK_KP_Delete) {
    // Delete each selected row.
    vector < GtkTreeIter > iters;
    gtk_tree_selection_selected_foreach(select, ReviewAnchorsDialog::on_collect_iters, gpointer(&iters));
    for (unsigned int i = 0; i < iters.size(); i++) {
      GtkTreeIter iter = iters[i];
      gtk_list_store_remove(store, &iter);
    }
  }
  // Handle other keypresses as usual. 
  return false;
}


void ReviewAnchorsDialog::on_collect_iters(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  ((vector < GtkTreeIter > *)data)->push_back(*iter);
}


void ReviewAnchorsDialog::on_button_spurious_anchors_clicked(GtkButton * button, gpointer user_data)
{
  ((ReviewAnchorsDialog *) user_data)->on_button_spurious_anchors();
}


void ReviewAnchorsDialog::on_button_spurious_anchors()
/*
This selects spurious anchors.
It does that in two passes.
In the first pass, spurious anchors are defined as anchors for books that do 
not occur at least ten times in a row. These will be selected.
In the second pass, spurious anchors are anchors that do not follow in sequence,
looking at their book, chapter and verse. These will be selected too.
*/
{
  // Deselect everything.
  gtk_tree_selection_unselect_all(select);

  // Collect the books of all the anchors.
  vector < unsigned int >books;
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview_anchors));
  GtkTreeIter iter;
  gboolean valid;
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    gint book;
    gtk_tree_model_get(model, &iter, 4, &book, -1);
    books.push_back(book);
    valid = gtk_tree_model_iter_next(model, &iter);
  }

  // Get the genuine books.
  set < unsigned int >genuinebooks;
  {
    unsigned int previous_book = 0;
    unsigned int row_counter = 0;
    for (unsigned int i = 0; i < books.size(); i++) {
      if (books[i] != previous_book) {
        row_counter = 0;
      }
      row_counter++;
      if (row_counter >= 10) {
        genuinebooks.insert(books[i]);
      }
      previous_book = books[i];
    }
  }

  // Select spurious books.
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    gint book;
    gtk_tree_model_get(model, &iter, 4, &book, -1);
    if (genuinebooks.find(book) == genuinebooks.end()) {
      gtk_tree_selection_select_iter(select, &iter);
    }
    valid = gtk_tree_model_iter_next(model, &iter);
  }

  // Second stage: Removing spurious anchors that are out of order.

  // Variables for the sequence.
  gint previousbook = 0;
  gint previouschapter = 0;
  gint previousverse = 0;

  // Go through the treeview, and consider the non-selected items.  
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    if (!gtk_tree_selection_iter_is_selected(select, &iter)) {

      // Variables.
      bool checked = false;
      bool spurious = false;

      // Get book, chapter and verse from the treeview.
      gint book, chapter, verse;
      gtk_tree_model_get(model, &iter, 4, &book, 1, &chapter, 2, &verse, -1);

      // If the book differs from the previous one, accept that only in case
      // that both chapter and verse are 1.
      if (!checked) {
        if (book != previousbook) {
          if (chapter != 1)
            spurious = true;
          if (verse != 1)
            spurious = true;
          checked = true;
        }
      }
      // At this stage the reference has either been checked or the book is 
      // the same.

      // If the chapter differs from the previous one, accept that only in 
      // case that the verse is 1 and the chapter is one up.
      if (!checked) {
        if (chapter != previouschapter) {
          if (chapter != previouschapter + 1)
            spurious = true;
          if (verse != 1)
            spurious = true;
          checked = true;
        }
      }
      // The verse ought to be one up.
      if (!checked) {
        if (verse != previousverse + 1)
          spurious = true;
        checked = true;
      }
      // Select this anchor if it is a spurious one.
      if (spurious) {
        gtk_tree_selection_select_iter(select, &iter);
      }
      // Store reference for next iteration.
      if (!spurious) {
        previousbook = book;
        previouschapter = chapter;
        previousverse = verse;
      }
    }
    valid = gtk_tree_model_iter_next(model, &iter);
  }
}


void ReviewAnchorsDialog::static_on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ReviewAnchorsDialog *) user_data)->on_okbutton_clicked();
}


void ReviewAnchorsDialog::on_okbutton_clicked()
{
  // Clear containers.
  mybooks->clear();
  mychapters->clear();
  myverses->clear();
  mylinenumbers->clear();
  // Get the model
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview_anchors));
  // Some variables needed.
  GtkTreeIter iter;
  gboolean valid;
  // Get the first iter in the store.
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    // Walk through the list, reading each row
    gint book, chapter, verse;
    gint linenumber;
    // Make sure you terminate calls to gtk_tree_model_get() with a '-1' value.
    gtk_tree_model_get(model, &iter, 1, &chapter, 2, &verse, 3, &linenumber, 4, &book, -1);
    // Store.
    mybooks->push_back(book);
    mychapters->push_back(chapter);
    myverses->push_back(verse);
    mylinenumbers->push_back(linenumber);
    // Go to next.
    valid = gtk_tree_model_iter_next(model, &iter);
  }
}


