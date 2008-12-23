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
#include "dialoggotoreference.h"
#include "utilities.h"
#include "bible.h"
#include "gtkwrappers.h"
#include "completion.h"
#include <gdk/gdkkeysyms.h>
#include "listview.h"
#include "projectutils.h"
#include "help.h"
#include "books.h"
#include "settings.h"
#include "tiny_utilities.h"

GotoReferenceDialog::GotoReferenceDialog(unsigned int currentbook, unsigned int currentchapter, const ustring & currentverse):reference(0)
{
  // Init variables
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  language = projectconfig->language_get();
  newreference = false;
  oldbook = currentbook;
  oldchapter = currentchapter;
  oldverse = currentverse;

  accel_group = gtk_accel_group_new();

  gotoreferencedialog = gtk_dialog_new();
  gtk_widget_add_accelerator(gotoreferencedialog, "activate_focus", accel_group, GDK_G, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_window_set_title(GTK_WINDOW(gotoreferencedialog), "Goto reference");
  gtk_window_set_position(GTK_WINDOW(gotoreferencedialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(gotoreferencedialog), TRUE);
  gtk_window_set_type_hint(GTK_WINDOW(gotoreferencedialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(gotoreferencedialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 2);

  label4 = gtk_label_new("Press Control-G to get the other view");
  gtk_widget_show(label4);
  gtk_box_pack_start(GTK_BOX(vbox1), label4, FALSE, FALSE, 0);

  notebook1 = gtk_notebook_new();
  gtk_widget_show(notebook1);
  gtk_box_pack_start(GTK_BOX(vbox1), notebook1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(notebook1), 4);

  hbox1 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox1);
  gtk_container_add(GTK_CONTAINER(notebook1), hbox1);

  label1 = gtk_label_new("Go to");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(hbox1), label1, FALSE, FALSE, 2);

  entry1 = gtk_entry_new();
  gtk_widget_show(entry1);
  gtk_box_pack_start(GTK_BOX(hbox1), entry1, TRUE, TRUE, 2);
  // Enter current reference and select it so user can use that.
  ustring ref = books_id_to_name(language, currentbook) + " " + convert_to_string(currentchapter) + ":" + currentverse;
  gtk_entry_set_text(GTK_ENTRY(entry1), ref.c_str());
  gtk_editable_select_region(GTK_EDITABLE(entry1), 0, -1);

  label2 = gtk_label_new_with_mnemonic("Free");
  gtk_widget_show(label2);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook1), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook1), 0), label2);

  table1 = gtk_table_new(2, 3, FALSE);
  gtk_widget_show(table1);
  gtk_container_add(GTK_CONTAINER(notebook1), table1);
  gtk_container_set_border_width(GTK_CONTAINER(table1), 4);
  gtk_table_set_row_spacings(GTK_TABLE(table1), 4);
  gtk_table_set_col_spacings(GTK_TABLE(table1), 4);

  label5 = gtk_label_new("Book");
  gtk_widget_show(label5);
  gtk_table_attach(GTK_TABLE(table1), label5, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label5), 0, 0.5);

  label6 = gtk_label_new("Chapter");
  gtk_widget_show(label6);
  gtk_table_attach(GTK_TABLE(table1), label6, 1, 2, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label6), 0, 0.5);

  label7 = gtk_label_new("Verse");
  gtk_widget_show(label7);
  gtk_table_attach(GTK_TABLE(table1), label7, 2, 3, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label7), 0, 0.5);

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_table_attach(GTK_TABLE(table1), scrolledwindow1, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_widget_set_size_request(scrolledwindow1, 160, 200);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  treeview1 = gtk_tree_view_new();
  gtk_widget_show(treeview1);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), treeview1);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview1), FALSE);

  scrolledwindow2 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow2);
  gtk_table_attach(GTK_TABLE(table1), scrolledwindow2, 1, 2, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request(scrolledwindow2, 60, 200);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  treeview2 = gtk_tree_view_new();
  gtk_widget_show(treeview2);
  gtk_container_add(GTK_CONTAINER(scrolledwindow2), treeview2);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview2), FALSE);

  scrolledwindow3 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow3);
  gtk_table_attach(GTK_TABLE(table1), scrolledwindow3, 2, 3, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request(scrolledwindow3, 60, 200);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow3), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  treeview3 = gtk_tree_view_new();
  gtk_widget_show(treeview3);
  gtk_container_add(GTK_CONTAINER(scrolledwindow3), treeview3);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview3), FALSE);

  label3 = gtk_label_new("Aided");
  gtk_widget_show(label3);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook1), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook1), 1), label3);

  dialog_action_area1 = GTK_DIALOG(gotoreferencedialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(gotoreferencedialog, NULL, NULL);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(gotoreferencedialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock("gtk-jump-to");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(gotoreferencedialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) gotoreferencedialog, "activate_focus", G_CALLBACK(on_gotoreferencedialog_activate_focus), gpointer(this));
  g_signal_connect((gpointer) entry1, "activate", G_CALLBACK(on_entry1_activate), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));
  g_signal_connect_after((gpointer) treeview1, "move_cursor", G_CALLBACK(on_treeview1_move_cursor), gpointer(this));
  g_signal_connect_after((gpointer) treeview1, "row_activated", G_CALLBACK(on_treeview1_row_activated), gpointer(this));
  g_signal_connect_after((gpointer) treeview2, "move_cursor", G_CALLBACK(on_treeview2_move_cursor), gpointer(this));
  g_signal_connect_after((gpointer) treeview2, "row_activated", G_CALLBACK(on_treeview2_row_activated), gpointer(this));
  g_signal_connect_after((gpointer) treeview3, "move_cursor", G_CALLBACK(on_treeview3_move_cursor), gpointer(this));
  g_signal_connect_after((gpointer) treeview3, "row_activated", G_CALLBACK(on_treeview3_row_activated), gpointer(this));

  gtk_widget_grab_focus(entry1);
  gtk_widget_grab_default(okbutton1);
  gtk_window_add_accel_group(GTK_WINDOW(gotoreferencedialog), accel_group);

  // Completion
  completion_setup(entry1, cpGoto);

  // Activation of dialog.
  allow_activate = true;

  // Create stores and fill the book store.
  store1 = gtk_list_store_new(1, G_TYPE_STRING);
  store2 = gtk_list_store_new(1, G_TYPE_STRING);
  store3 = gtk_list_store_new(1, G_TYPE_STRING);
  vector < unsigned int >ibooks = project_get_books(settings->genconfig.project_get());
  vector < ustring > ubooks;
  for (unsigned int i = 0; i < ibooks.size(); i++)
    ubooks.push_back(books_id_to_name(language, ibooks[i]));
  listview_set_strings(treeview1, store1, ubooks);
  // Set the stores as models for the treeviews.
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview1), GTK_TREE_MODEL(store1));
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview2), GTK_TREE_MODEL(store2));
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview3), GTK_TREE_MODEL(store3));
  // Unreference the stores once, so they get destroyed with the treeview.
  g_object_unref(store1);
  g_object_unref(store2);
  g_object_unref(store3);
  // Text cell renderer.
  GtkCellRenderer *renderer;
  renderer = gtk_cell_renderer_text_new();
  // Columns.
  column1 = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview1), column1);
  column2 = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview2), column2);
  column3 = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview3), column3);
  // Set up the selection for each of the tree views.
  select1 = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview1));
  gtk_tree_selection_set_mode(select1, GTK_SELECTION_SINGLE);
  select2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview2));
  gtk_tree_selection_set_mode(select2, GTK_SELECTION_SINGLE);
  select3 = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview3));
  gtk_tree_selection_set_mode(select3, GTK_SELECTION_SINGLE);
}

GotoReferenceDialog::~GotoReferenceDialog()
{
  gtk_widget_destroy(gotoreferencedialog);
}

int GotoReferenceDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(gotoreferencedialog));
}

void GotoReferenceDialog::on_jump()
{
  /*
     This interprets the text the user has typed in the entry as a valid reference.
     If needed it will use information of the current reference to complete the info.
     For example, when the user types "1", it is interpreted as verse one of the current
     chapter of the current book. If he types "21 10" it is interpreted as the current
     book, chapter 21 verse 10. And so forth.
   */
  newreference = true;
  int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook1));
  if (page == 0) {
    if (reference_discover(oldbook, oldchapter, oldverse, gtk_entry_get_text(GTK_ENTRY(entry1)), reference.book, reference.chapter, reference.verse)) {
      completion_finish(entry1, cpGoto);
    } else {
      newreference = false;
      show_bad_reference();
    }
  } else {
    reference.book = books_name_to_id(language, listview_get_active_string(treeview1));
    reference.chapter = convert_to_int(listview_get_active_string(treeview2));
    reference.verse = listview_get_active_string(treeview3);
  }
}

void GotoReferenceDialog::show_bad_reference()
{
  ustring message = "No such reference: ";
  message.append(gtk_entry_get_text(GTK_ENTRY(entry1)));
  gtkw_dialog_error(gotoreferencedialog, message);
}

void GotoReferenceDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_jump();
}

void GotoReferenceDialog::on_gotoreferencedialog_activate_focus(GtkWindow * window, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_control_g();
}

void GotoReferenceDialog::on_control_g()
{
  int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook1));
  gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook1), 1 - page);
  allow_activate = false;
  g_timeout_add(1, GSourceFunc(on_timeout), gpointer(this));
  // If we're on the aided page, set book, chapters, verse.
  page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook1));
  if (page == 1) {
    // Activate the current book.
    listview_focus_string(treeview1, books_id_to_name(language, oldbook));
    // Fill chapters and verses too.
    on_move1();
    listview_focus_string(treeview2, convert_to_string(oldchapter));
    on_move2();
    listview_focus_string(treeview3, oldverse);
    gtk_widget_grab_focus(treeview1);
  }
}

void GotoReferenceDialog::on_entry1_activate(GtkEntry * entry, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_activate();
}

void GotoReferenceDialog::on_activate()
{
  if (allow_activate) {
    on_jump();
    gtk_dialog_response(GTK_DIALOG(gotoreferencedialog), GTK_RESPONSE_OK);
  }
}

bool GotoReferenceDialog::on_timeout(gpointer data)
{
  ((GotoReferenceDialog *) data)->timeout();
  return false;
}

void GotoReferenceDialog::timeout()
{
  allow_activate = true;
}

gboolean GotoReferenceDialog::on_treeview1_move_cursor(GtkTreeView * treeview, GtkMovementStep step, gint count, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_move1();
  return false;
}

void GotoReferenceDialog::on_treeview1_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_activate1();
}

gboolean GotoReferenceDialog::on_treeview2_move_cursor(GtkTreeView * treeview, GtkMovementStep step, gint count, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_move2();
  return FALSE;
}

void GotoReferenceDialog::on_treeview2_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_activate2();
}

gboolean GotoReferenceDialog::on_treeview3_move_cursor(GtkTreeView * treeview, GtkMovementStep step, gint count, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_move3();
  return FALSE;
}

void GotoReferenceDialog::on_treeview3_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_activate3();
}

void GotoReferenceDialog::on_move1()
{
  extern Settings *settings;
  // As the book changed, update the number of chapters.
  unsigned int bookid = books_name_to_id(language, listview_get_active_string(treeview1));
  // Something weird in Gtk: If the listview was not in view, it gives no results.
  // Deal with that here.
  if (bookid == 0)
    return;
  vector < unsigned int >chapters;
  chapters = project_get_chapters(settings->genconfig.project_get(), bookid);
  listview_set_strings(treeview2, store2, chapters);
  if (oldbook == bookid)
    listview_focus_string(treeview2, convert_to_string(oldchapter));
  // Update the number of verses too.
  on_move2();
}

void GotoReferenceDialog::on_activate1()
{
  // Ok, book selection was made.
  // Update number of chapters, and then focus on the chapters selector.
  on_move1();
  gtk_widget_grab_focus(treeview2);
}

void GotoReferenceDialog::on_move2()
{
  extern Settings *settings;
  // Chapter number changed, therefore update the verse numbers.
  ustring chapter = listview_get_active_string(treeview2);
  // Something weird in Gtk: If the listview was not in view, it gives no results.
  // Deal with that here.
  if (chapter.empty())
    return;
  // Get the verses as they are in the chapter.
  unsigned int bookid = books_name_to_id(language, listview_get_active_string(treeview1));
  vector < ustring > verses;
  verses = project_get_verses(settings->genconfig.project_get(), bookid, convert_to_int(chapter));
  // The verses may contain sequences and ranges, but fill them up with the 
  // verses contained in those. E.g.: 10-12 also contains 11.
  vector < ustring > verses2;
  set < ustring > verses2_set;
  for (unsigned int i = 0; i < verses.size(); i++) {
    verses2.push_back(verses[i]);
    if (number_in_string(verses[i]) != verses[i]) {
      vector < unsigned int >int_verses = verse_range_sequence(verses[i]);
      for (unsigned int i2 = 0; i2 < int_verses.size(); i2++) {
        ustring verse2 = convert_to_string(int_verses[i2]);
        // No double verses will be listed.
        if (verses2_set.find(verse2) == verses2_set.end()) {
          verses2.push_back(verse2);
        }
        verses2_set.insert(verse2);
      }
    }
  }
  listview_set_strings(treeview3, store3, verses2);
  if (oldbook == bookid)
    if (oldchapter == convert_to_int(chapter))
      listview_focus_string(treeview3, oldverse);
}

void GotoReferenceDialog::on_activate2()
{
  // Ok, chapter selection was made.
  // Update number of verses, and then focus on the verses selector.
  on_move2();
  gtk_widget_grab_focus(treeview3);
}

void GotoReferenceDialog::on_move3()
{
}

void GotoReferenceDialog::on_activate3()
{
  on_activate();
}
