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
#include "dialogselectbooks.h"
#include "utilities.h"
#include "bible.h"
#include "dialogselectchapters.h"
#include "projectutils.h"
#include "books.h"
#include "scriptureportions.h"
#include "settings.h"
#include "help.h"
#include "shortcuts.h"
#include "listview.h"
#include <gdk/gdkkeysyms.h>
#include "gwrappers.h"
#include "screen.h"


SelectBooksDialog::SelectBooksDialog(bool showportions)
/*
This dialog selects books.
This function takes the book from the project that is now open, and
the language of that project.
It then loads the books.
bookset: Indicator for the caller's relevant books.
*/
{
  // Initialize variables.
  extern Settings *settings;
  myproject = settings->genconfig.project_get();
  ProjectConfiguration *projectconfig = settings->projectconfig(myproject);
  mylanguage = projectconfig->language_get();
  myselectables = project_get_books(myproject);
  myshowportions = showportions;

  // Build dialog.
  Shortcuts shortcuts(0);

  selectbooksdialog = gtk_dialog_new();
  ustring title("Select books");
  if (showportions)
    title.append(" and portions");
  gtk_window_set_title(GTK_WINDOW(selectbooksdialog), title.c_str());
  gtk_window_set_position(GTK_WINDOW(selectbooksdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(selectbooksdialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(selectbooksdialog));
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  hbox11 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox11);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox11, TRUE, TRUE, 0);

  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox2);
  gtk_box_pack_start(GTK_BOX(hbox11), vbox2, TRUE, TRUE, 0);

  labelbooks = gtk_label_new("Books");
  gtk_widget_show(labelbooks);
  gtk_box_pack_start(GTK_BOX(vbox2), labelbooks, FALSE, FALSE, 0);

  shortcuts.label(labelbooks);

  scrolledwindowbooks = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindowbooks);
  gtk_box_pack_start(GTK_BOX(vbox2), scrolledwindowbooks, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindowbooks), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindowbooks), GTK_SHADOW_IN);

  treeviewbooks = gtk_tree_view_new();
  gtk_widget_show(treeviewbooks);
  gtk_container_add(GTK_CONTAINER(scrolledwindowbooks), treeviewbooks);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeviewbooks), FALSE);

  vbox3 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox3);
  gtk_box_pack_start(GTK_BOX(hbox11), vbox3, TRUE, TRUE, 0);

  if (myshowportions) {

    labelportions = gtk_label_new("Portions");
    gtk_widget_show(labelportions);
    gtk_box_pack_start(GTK_BOX(vbox3), labelportions, FALSE, FALSE, 0);

    shortcuts.label(labelportions);

    scrolledwindowportions = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show(scrolledwindowportions);
    gtk_box_pack_start(GTK_BOX(vbox3), scrolledwindowportions, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindowportions), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindowportions), GTK_SHADOW_IN);

    treeviewportions = gtk_tree_view_new();
    gtk_widget_show(treeviewportions);
    gtk_container_add(GTK_CONTAINER(scrolledwindowportions), treeviewportions);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeviewportions), FALSE);

  }

  table1 = gtk_table_new(2, 3, FALSE);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(vbox1), table1, FALSE, FALSE, 0);

  nobutton = gtk_button_new();
  gtk_widget_show(nobutton);
  gtk_table_attach(GTK_TABLE(table1), nobutton, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(nobutton), alignment1);

  hbox4 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox4);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox4);

  image1 = gtk_image_new_from_stock("gtk-clear", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox4), image1, FALSE, FALSE, 0);

  label6 = gtk_label_new_with_mnemonic("No books");
  gtk_widget_show(label6);
  gtk_box_pack_start(GTK_BOX(hbox4), label6, FALSE, FALSE, 0);

  shortcuts.label(label6);

  otbutton = gtk_button_new();
  gtk_widget_show(otbutton);
  gtk_table_attach(GTK_TABLE(table1), otbutton, 1, 2, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment2 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment2);
  gtk_container_add(GTK_CONTAINER(otbutton), alignment2);

  hbox5 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox5);
  gtk_container_add(GTK_CONTAINER(alignment2), hbox5);

  image2 = gtk_image_new_from_stock("gtk-goto-first", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image2);
  gtk_box_pack_start(GTK_BOX(hbox5), image2, FALSE, FALSE, 0);

  label7 = gtk_label_new_with_mnemonic("Old Testament");
  gtk_widget_show(label7);
  gtk_box_pack_start(GTK_BOX(hbox5), label7, FALSE, FALSE, 0);

  shortcuts.label(label7);

  ntbutton = gtk_button_new();
  gtk_widget_show(ntbutton);
  gtk_table_attach(GTK_TABLE(table1), ntbutton, 2, 3, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment3 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment3);
  gtk_container_add(GTK_CONTAINER(ntbutton), alignment3);

  hbox6 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox6);
  gtk_container_add(GTK_CONTAINER(alignment3), hbox6);

  image3 = gtk_image_new_from_stock("gtk-goto-last", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image3);
  gtk_box_pack_start(GTK_BOX(hbox6), image3, FALSE, FALSE, 0);

  label8 = gtk_label_new_with_mnemonic("New Testament");
  gtk_widget_show(label8);
  gtk_box_pack_start(GTK_BOX(hbox6), label8, FALSE, FALSE, 0);

  shortcuts.label(label8);

  otherbutton = gtk_button_new();
  gtk_widget_show(otherbutton);
  gtk_table_attach(GTK_TABLE(table1), otherbutton, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment6 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment6);
  gtk_container_add(GTK_CONTAINER(otherbutton), alignment6);

  hbox9 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox9);
  gtk_container_add(GTK_CONTAINER(alignment6), hbox9);

  image6 = gtk_image_new_from_stock("gtk-dialog-warning", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image6);
  gtk_box_pack_start(GTK_BOX(hbox9), image6, FALSE, FALSE, 0);

  label11 = gtk_label_new_with_mnemonic("Other books");
  gtk_widget_show(label11);
  gtk_box_pack_start(GTK_BOX(hbox9), label11, FALSE, FALSE, 0);

  shortcuts.label(label11);

  allbutton = gtk_button_new();
  gtk_widget_show(allbutton);
  gtk_table_attach(GTK_TABLE(table1), allbutton, 1, 2, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment4 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment4);
  gtk_container_add(GTK_CONTAINER(allbutton), alignment4);

  hbox7 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox7);
  gtk_container_add(GTK_CONTAINER(alignment4), hbox7);

  image4 = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image4);
  gtk_box_pack_start(GTK_BOX(hbox7), image4, FALSE, FALSE, 0);

  label9 = gtk_label_new_with_mnemonic("All books");
  gtk_widget_show(label9);
  gtk_box_pack_start(GTK_BOX(hbox7), label9, FALSE, FALSE, 0);

  shortcuts.label(label9);

  currentbutton = gtk_button_new();
  gtk_widget_show(currentbutton);
  gtk_table_attach(GTK_TABLE(table1), currentbutton, 2, 3, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment5 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment5);
  gtk_container_add(GTK_CONTAINER(currentbutton), alignment5);

  hbox8 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox8);
  gtk_container_add(GTK_CONTAINER(alignment5), hbox8);

  image5 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image5);
  gtk_box_pack_start(GTK_BOX(hbox8), image5, FALSE, FALSE, 0);

  label10 = gtk_label_new_with_mnemonic("Current book");
  gtk_widget_show(label10);
  gtk_box_pack_start(GTK_BOX(hbox8), label10, FALSE, FALSE, 0);

  shortcuts.label(label10);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(selectbooksdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  const gchar *helpfile;
  if (showportions) {
    helpfile = "file/print/portion";
  } else {
    helpfile = "file/print/selection";
  }
  new InDialogHelp(selectbooksdialog, NULL, &shortcuts, helpfile);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(selectbooksdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  shortcuts.stockbutton(cancelbutton);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(selectbooksdialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  shortcuts.stockbutton(okbutton);

  shortcuts.process();

  g_signal_connect((gpointer) treeviewbooks, "key_press_event", G_CALLBACK(on_treeviewbooks_key_press_event), gpointer(this));
  if (myshowportions) {
    g_signal_connect((gpointer) treeviewportions, "row_activated", G_CALLBACK(on_treeviewportions_row_activated), gpointer(this));
  }
  g_signal_connect((gpointer) nobutton, "clicked", G_CALLBACK(on_nobutton_clicked), gpointer(this));
  g_signal_connect((gpointer) otbutton, "clicked", G_CALLBACK(on_otbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) ntbutton, "clicked", G_CALLBACK(on_ntbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) otherbutton, "clicked", G_CALLBACK(on_otherbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) allbutton, "clicked", G_CALLBACK(on_allbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) currentbutton, "clicked", G_CALLBACK(on_currentbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_focus(treeviewbooks);
  gtk_widget_grab_default(okbutton);

  gtk_label_set_mnemonic_widget(GTK_LABEL(labelbooks), treeviewbooks);
  if (myshowportions)
    gtk_label_set_mnemonic_widget(GTK_LABEL(labelportions), treeviewportions);

  // Storage, renderer, column and selection for books.
  {
    storebooks = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeviewbooks), GTK_TREE_MODEL(storebooks));
    g_object_unref(storebooks);
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    columnbooks = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeviewbooks), columnbooks);
    selectbooks = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeviewbooks));
    gtk_tree_selection_set_mode(selectbooks, GTK_SELECTION_MULTIPLE);
  }

  // Storage, renderer, column and selection for portions.
  if (myshowportions) {
    storeportions = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeviewportions), GTK_TREE_MODEL(storeportions));
    g_object_unref(storeportions);
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    columnportions = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeviewportions), columnportions);
    selectportions = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeviewportions));
    gtk_tree_selection_set_mode(selectportions, GTK_SELECTION_SINGLE);
  }
  // Load content.
  myselectables = project_get_books(myproject);
  loadbooks();
  if (myshowportions)
    loadportions();

  // Update gui.
  update_gui_event_id = 0;
  if (myshowportions) {
    update_gui_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 100, GSourceFunc(on_update_gui_timeout), gpointer(this), NULL);
  }

  new DialogAutoScaler (selectbooksdialog, G_MAXINT);
}


SelectBooksDialog::~SelectBooksDialog()
{
  gw_destroy_source(update_gui_event_id);
  gtk_widget_destroy(selectbooksdialog);
}


int SelectBooksDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(selectbooksdialog));
}


void SelectBooksDialog::language(const ustring & language)
/*
Sets the language to display the books in.
The language has been set already, but can be set here too if another language
is required.
It will reload the books.
*/
{
  mylanguage = language;
  loadbooks();
}


void SelectBooksDialog::selectables(vector < unsigned int >&selectablebooks)
// Sets the selectables books, if this is different from the books in the project.
{
  myselectables = selectablebooks;
  loadbooks();
}


void SelectBooksDialog::selection_set(const set < unsigned int >&selectedbooks)
// Sets the selected books.
{
  myselection = selectedbooks;
  loadbooks();
}


gboolean SelectBooksDialog::on_treeviewbooks_key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  return ((SelectBooksDialog *) user_data)->on_treeviewbooks_key_press(event);
}


gboolean SelectBooksDialog::on_treeviewbooks_key_press(GdkEventKey * event)
{
  // Take special action on Enter: OK button.
  if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter) {
    on_okbutton();
    gtk_dialog_response(GTK_DIALOG(selectbooksdialog), GTK_RESPONSE_OK);
    return true;
  }
  // Let default handler take action.
  return false;
}


void SelectBooksDialog::on_treeviewportions_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
  ((SelectBooksDialog *) user_data)->on_treeviewportions_row();
}


void SelectBooksDialog::on_treeviewportions_row()
{
  // Get the selected portion and the offset of it, and the corresponding book.
  ustring portion = listview_get_active_string(treeviewportions);
  int offset = listview_get_active_offset(treeviewportions);
  unsigned int book;
  {
    vector < ustring > books = listview_get_strings(treeviewbooks);
    book = books_name_to_id(mylanguage, books[offset]);
  }

  // Run the dialog to select a portion.
  SelectChaptersDialog dialog(myproject, book, portion);
  if (dialog.run() == GTK_RESPONSE_OK) {
    // Store new portion in treeview.
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeviewportions));
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selectportions, &model, &iter)) {
      gtk_list_store_set(storeportions, &iter, 0, dialog.new_selection.c_str(), -1);
    }
  }
  // Unselect everything.
  gtk_tree_selection_unselect_all(selectportions);
}


void SelectBooksDialog::on_nobutton_clicked(GtkButton * button, gpointer user_data)
{
  ((SelectBooksDialog *) user_data)->on_button_no();
}


void SelectBooksDialog::on_button_no()
{
  gtk_tree_selection_unselect_all(selectbooks);
}


void SelectBooksDialog::on_otbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((SelectBooksDialog *) user_data)->on_button_ot();
}


void SelectBooksDialog::on_button_ot()
{
  vector < unsigned int >ids = books_type_to_ids(btOldTestament);
  myselection.clear();
  for (unsigned int i = 0; i < ids.size(); i++) {
    myselection.insert(ids[i]);
  }
  loadbooks();
}


void SelectBooksDialog::on_ntbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((SelectBooksDialog *) user_data)->on_button_nt();
}


void SelectBooksDialog::on_button_nt()
{
  vector < unsigned int >ids = books_type_to_ids(btNewTestament);
  myselection.clear();
  for (unsigned int i = 0; i < ids.size(); i++) {
    myselection.insert(ids[i]);
  }
  loadbooks();
}


void SelectBooksDialog::on_otherbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((SelectBooksDialog *) user_data)->on_button_other();
}


void SelectBooksDialog::on_button_other()
{
  myselection.clear();
  vector < unsigned int >ids = books_type_to_ids(btFrontBackMatter);
  for (unsigned int i = 0; i < ids.size(); i++) {
    myselection.insert(ids[i]);
  }
  ids = books_type_to_ids(btOtherMaterial);
  for (unsigned int i = 0; i < ids.size(); i++) {
    myselection.insert(ids[i]);
  }
  loadbooks();
}


void SelectBooksDialog::on_allbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((SelectBooksDialog *) user_data)->on_button_all();
}


void SelectBooksDialog::on_button_all()
{
  gtk_tree_selection_select_all(selectbooks);
}


void SelectBooksDialog::on_currentbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((SelectBooksDialog *) user_data)->on_button_current();
}


void SelectBooksDialog::on_button_current()
{
  myselection.clear();
  extern Settings *settings;
  myselection.insert(settings->genconfig.book_get());
  loadbooks();
}


void SelectBooksDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((SelectBooksDialog *) user_data)->on_okbutton();
}


void SelectBooksDialog::on_okbutton()
{
  // Get the list of books now selected.
  selection.clear();
  vector < ustring > books;
  gtk_tree_selection_selected_foreach(selectbooks, selection_foreach_function, gpointer(&books));
  for (unsigned int i = 0; i < books.size(); i++) {
    unsigned int book = books_name_to_id(mylanguage, books[i]);
    if (book)
      selection.push_back(book);
  }
  // Also produce a set out of that list.
  selectionset.clear();
  for (unsigned int i = 0; i < selection.size(); i++) {
    selectionset.insert(selection[i]);
  }
  // If portions are showing, store the values there too.
  if (myshowportions) {
    // Get books, includes and portions.
    vector < ustring > reordered_books;
    vector < bool > reordered_includes;
    vector < ustring > reordered_portions;
    {
      vector < ustring > books = listview_get_strings(treeviewbooks);
      for (unsigned int i = 0; i < books.size(); i++) {
        unsigned int book = books_name_to_id(mylanguage, books[i]);
        reordered_books.push_back(books_id_to_english(book));
        bool include = (selectionset.find(book) != selectionset.end());
        reordered_includes.push_back(include);
      }
    }
    reordered_portions = listview_get_strings(treeviewportions);
    // Save books, includes and portions.
    extern Settings *settings;
    ProjectConfiguration *projectconfig = settings->projectconfig(myproject);
    projectconfig->reordered_books_set(reordered_books);
    projectconfig->reordered_includes_set(reordered_includes);
    projectconfig->reordered_portions_set(reordered_portions);
  }
}


void SelectBooksDialog::selection_display(set < unsigned int >&books)
{
  // Get the model
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeviewbooks));
  // Some variables needed.
  GtkTreeIter iter;
  gboolean valid;
  gint index = 0;
  bool focused = false;
  // Get the first iter in the store.
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    // Produce path.
    GtkTreePath *path = gtk_tree_path_new_from_indices(index, -1);
    // Walk through the list, reading each row
    gchar *str_data;
    // Make sure you terminate calls to gtk_tree_model_get() with a '-1' value.
    gtk_tree_model_get(model, &iter, 0, &str_data, -1);
    // See if this is a book we wish to select.
    unsigned int book_id = books_name_to_id(mylanguage, str_data);
    bool include = myselection.find(book_id) != myselection.end();
    // Put cursor on first one selected and select it.
    if (include) {
      if (path) {
        gtk_tree_selection_select_iter(selectbooks, &iter);
        if (!focused) {
          gtk_tree_view_set_cursor(GTK_TREE_VIEW(treeviewbooks), path, NULL, false);
          gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(treeviewbooks), path, NULL, true, 0.5, 0.5);
          focused = true;
        }
      }
      gtk_widget_grab_focus(treeviewbooks);
    }
    // Free some data.
    gtk_tree_path_free(path);
    g_free(str_data);
    // Next row.
    valid = gtk_tree_model_iter_next(model, &iter);
    index++;
  }
}


void SelectBooksDialog::loadbooks()
{
  // Translate and load the selectable books.
  vector < ustring > ubooks;
  for (unsigned int i = 0; i < myselectables.size(); i++) {
    ubooks.push_back(books_id_to_name(mylanguage, myselectables[i]));
  }
  listview_set_strings(treeviewbooks, storebooks, ubooks);
  // Initially select books.
  selection_display(myselection);
}


void SelectBooksDialog::selection_foreach_function(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  gchar *book;
  gtk_tree_model_get(model, iter, 0, &book, -1);
  ((vector < ustring > *)data)->push_back(book);
  g_free(book);
}


void SelectBooksDialog::loadportions()
{
  // Get the list of (included) books and their portions.
  myselectables.clear();
  myselection.clear();
  vector < ustring > portions;
  ScripturePortions scriptureportions(myproject);
  for (unsigned int i = 0; i < scriptureportions.reordered_books.size(); i++) {
    unsigned int book = books_english_to_id(scriptureportions.reordered_books[i]);
    myselectables.push_back(book);
    if (scriptureportions.reordered_includes[i])
      myselection.insert(book);
    portions.push_back(scriptureportions.reordered_portions[i]);
  }
  // Show both lists.
  loadbooks();
  listview_set_strings(treeviewportions, storeportions, portions);
}


bool SelectBooksDialog::on_update_gui_timeout(gpointer user_data)
{
  ((SelectBooksDialog *) user_data)->update_gui();
  return true;
}


void SelectBooksDialog::update_gui()
{
  // Get the upper corners of the visible area of both treeviews.
  // If one changed, then the other will be scrolled to the same position,
  // and vice versa.
  GdkRectangle gdk_rectangle;
  gtk_tree_view_get_visible_rect(GTK_TREE_VIEW(treeviewbooks), &gdk_rectangle);
  if (gdk_rectangle.y != treeviewbooks_previous_y) {
    treeviewbooks_previous_y = gdk_rectangle.y;
    gtk_tree_view_scroll_to_point(GTK_TREE_VIEW(treeviewportions), -1, gdk_rectangle.y);
  }
  gtk_tree_view_get_visible_rect(GTK_TREE_VIEW(treeviewportions), &gdk_rectangle);
  if (gdk_rectangle.y != treeviewportions_previous_y) {
    treeviewportions_previous_y = gdk_rectangle.y;
    gtk_tree_view_scroll_to_point(GTK_TREE_VIEW(treeviewbooks), -1, gdk_rectangle.y);
  }
}


