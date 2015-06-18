/*
** Copyright (©) 2003-2013 Teus Benschop.
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
#include "directories.h"
#include "gwrappers.h"
#include "keyboard.h"
#include <glib/gi18n.h>

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
  go_back = false;
  go_forward = false;

  Shortcuts shortcuts(0);

  gtkbuilder = gtk_builder_new ();
  gtk_builder_add_from_file (gtkbuilder, gw_build_filename (Directories->get_package_data(), "gtkbuilder.gotoreferencedialog.xml").c_str(), NULL);

  dialog = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "dialog"));
  g_signal_connect((gpointer) dialog, "key_press_event", G_CALLBACK(on_key_press_event), gpointer(this));

  notebook = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "notebook"));

  // Enter current reference into the free entry, and select it, so user can use that.
  entry_free = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "entry_free"));
  gtk_widget_grab_focus(entry_free);
  ustring ref = books_id_to_name(language, currentbook) + " " + convert_to_string(currentchapter) + ":" + currentverse;
  gtk_entry_set_text(GTK_ENTRY(entry_free), ref.c_str());
  gtk_editable_select_region(GTK_EDITABLE(entry_free), 0, -1);

  // Completion for the reference entry.
  completion_setup(entry_free, cpGoto);

  // Signals entry.
  g_signal_connect((gpointer) entry_free, "activate", G_CALLBACK(on_entry_free_activate), gpointer(this));
  
  // Aided entry widgets.
  treeview_book = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "treeview_book"));
  treeview_chapter = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "treeview_chapter"));
  treeview_verse = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "treeview_verse"));
  
  // Connect to the signals of the treeviews.
  g_signal_connect_after((gpointer) treeview_book, "move_cursor", G_CALLBACK(on_treeview_book_move_cursor), gpointer(this));
  g_signal_connect_after((gpointer) treeview_book, "row_activated", G_CALLBACK(on_treeview_book_row_activated), gpointer(this));
  g_signal_connect_after((gpointer) treeview_chapter, "move_cursor", G_CALLBACK(on_treeview_chapter_move_cursor), gpointer(this));
  g_signal_connect_after((gpointer) treeview_chapter, "row_activated", G_CALLBACK(on_treeview_chapter_row_activated), gpointer(this));
  g_signal_connect_after((gpointer) treeview_verse, "move_cursor", G_CALLBACK(on_treeview_verse_move_cursor), gpointer(this));
  g_signal_connect_after((gpointer) treeview_verse, "row_activated", G_CALLBACK(on_treeview_verse_row_activated), gpointer(this));

  // Create stores and fill the book store.
  store_book = gtk_list_store_new(1, G_TYPE_STRING);
  store_chapter = gtk_list_store_new(1, G_TYPE_STRING);
  store_verse = gtk_list_store_new(1, G_TYPE_STRING);
  vector < unsigned int >ibooks = project_get_books(settings->genconfig.project_get());
  vector < ustring > ubooks;
  for (unsigned int i = 0; i < ibooks.size(); i++)
    ubooks.push_back(books_id_to_name(language, ibooks[i]));
  listview_set_strings(treeview_book, store_book, ubooks);

  // Set the stores as models for the treeviews.
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview_book), GTK_TREE_MODEL(store_book));
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview_chapter), GTK_TREE_MODEL(store_chapter));
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview_verse), GTK_TREE_MODEL(store_verse));

  // Unreference the stores once, so they get destroyed with the treeview.
  g_object_unref(store_book);
  g_object_unref(store_chapter);
  g_object_unref(store_verse);

  // Text cell renderer.
  GtkCellRenderer *renderer;
  renderer = gtk_cell_renderer_text_new();

  // Columns.
  GtkTreeViewColumn *column_book;
  column_book = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview_book), column_book);
  GtkTreeViewColumn *column_chapter;
  column_chapter = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview_chapter), column_chapter);
  GtkTreeViewColumn *column_verse;
  column_verse = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview_verse), column_verse);

  // Set up the selection for each of the tree views.
  GtkTreeSelection *select_book;
  select_book = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_book));
  gtk_tree_selection_set_mode(select_book, GTK_SELECTION_SINGLE);
  GtkTreeSelection *select_chapter;
  select_chapter = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_chapter));
  gtk_tree_selection_set_mode(select_chapter, GTK_SELECTION_SINGLE);
  GtkTreeSelection *select_verse;
  select_verse = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_verse));
  gtk_tree_selection_set_mode(select_verse, GTK_SELECTION_SINGLE);

  // History back.
  radiobutton_back = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_back"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton_back), true);

  // History forward.
  radiobutton_forward = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_forward"));
 
  // Link history button.
  GSList *history_button_group = NULL;
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_back), history_button_group);
  history_button_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_back));
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_forward), history_button_group);
  history_button_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_forward));

  // Dialog action area.
  InDialogHelp * indialoghelp = new InDialogHelp(dialog, gtkbuilder, &shortcuts, NULL);
  cancelbutton = indialoghelp->cancelbutton;
  okbutton = indialoghelp->okbutton;
  gtk_widget_grab_default(okbutton);
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));
}


GotoReferenceDialog::~GotoReferenceDialog()
{
  gtk_widget_destroy(dialog);
}


int GotoReferenceDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialog));
}


void GotoReferenceDialog::on_jump()
/*
  This interprets the text the user has typed in the entry as a valid reference.
  If needed it will use information of the current reference to complete the info.
  For example, when the user types "1", it is interpreted as verse one of the current
  chapter of the current book. If he types "21 10" it is interpreted as the current
  book, chapter 21 verse 10. And so forth.
  If enabled it consults the verses memory database for finding out where to go
  in case only a book or only book and chapter is given.
*/
{
  int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
  if (page == 0) {
    if (reference_discover(oldbook, oldchapter, oldverse, gtk_entry_get_text(GTK_ENTRY(entry_free)), reference.book, reference.chapter, reference.verse, true)) {
      completion_finish(entry_free, cpGoto);
      newreference = true;
    } else {
      show_bad_reference();
    }
  } else if (page == 1) {
    reference.book = books_name_to_id(language, listview_get_active_string(treeview_book));
    reference.chapter = convert_to_int(listview_get_active_string(treeview_chapter));
    reference.verse = listview_get_active_string(treeview_verse);
    newreference = true;
  } else {
   go_back = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_back));
   go_forward = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_forward));
  }
}


void GotoReferenceDialog::show_bad_reference()
{
  ustring message = "No such reference: ";
  message.append(gtk_entry_get_text(GTK_ENTRY(entry_free)));
  gtkw_dialog_error(dialog, message);
}


void GotoReferenceDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_jump();
}


gboolean GotoReferenceDialog::on_key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  if (keyboard_control_state (event)) {
    if (event->keyval == GDK_g) {
      ((GotoReferenceDialog *) user_data)->on_control_g();
    }
  }
  return FALSE;
}


void GotoReferenceDialog::on_control_g()
{
  // Go to the next page.
  int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
  page++;
  if (page > 2)
    page = 0;
  gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page);
  // On the aided page, set book, chapters, verse.
  if (page == 1) {
    // Activate the current book.
    listview_focus_string(treeview_book, books_id_to_name(language, oldbook));
    // Fill chapters and verses too.
    on_move_book();
    listview_focus_string(treeview_chapter, convert_to_string(oldchapter));
    on_move_chapter();
    listview_focus_string(treeview_verse, oldverse);
    gtk_widget_grab_focus(treeview_book);
  }
}


void GotoReferenceDialog::on_entry_free_activate(GtkEntry * entry, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_activate();
}


void GotoReferenceDialog::on_activate()
{
  on_jump();
  gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
}


gboolean GotoReferenceDialog::on_treeview_book_move_cursor(GtkTreeView * treeview, GtkMovementStep step, gint count, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_move_book();
  return false;
}


void GotoReferenceDialog::on_treeview_book_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_activate_book();
}


gboolean GotoReferenceDialog::on_treeview_chapter_move_cursor(GtkTreeView * treeview, GtkMovementStep step, gint count, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_move_chapter();
  return FALSE;
}


void GotoReferenceDialog::on_treeview_chapter_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_activate_chapter();
}


gboolean GotoReferenceDialog::on_treeview_verse_move_cursor(GtkTreeView * treeview, GtkMovementStep step, gint count, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_move_verse();
  return FALSE;
}


void GotoReferenceDialog::on_treeview_verse_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
  ((GotoReferenceDialog *) user_data)->on_activate_verse();
}


void GotoReferenceDialog::on_move_book()
{
  extern Settings *settings;
  // As the book changed, update the number of chapters.
  unsigned int bookid = books_name_to_id(language, listview_get_active_string(treeview_book));
  // Something weird in Gtk: If the listview was not in view, it gives no results.
  // Deal with that here.
  if (bookid == 0)
    return;
  vector < unsigned int >chapters;
  chapters = project_get_chapters(settings->genconfig.project_get(), bookid);
  listview_set_strings(treeview_chapter, store_chapter, chapters);
  if (oldbook == bookid)
    listview_focus_string(treeview_chapter, convert_to_string(oldchapter));
  // Update the number of verses too.
  on_move_chapter();
}


void GotoReferenceDialog::on_activate_book()
{
  // Ok, book selection was made.
  // Update number of chapters, and then focus on the chapters selector.
  on_move_book();
  gtk_widget_grab_focus(treeview_chapter);
}


void GotoReferenceDialog::on_move_chapter()
{
  extern Settings *settings;
  // Chapter number changed, therefore update the verse numbers.
  ustring chapter = listview_get_active_string(treeview_chapter);
  // Something weird in Gtk: If the listview was not in view, it gives no results.
  // Deal with that here.
  if (chapter.empty())
    return;
  // Get the verses as they are in the chapter.
  unsigned int bookid = books_name_to_id(language, listview_get_active_string(treeview_book));
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
  listview_set_strings(treeview_verse, store_verse, verses2);
  if (oldbook == bookid)
    if (oldchapter == convert_to_int(chapter))
      listview_focus_string(treeview_verse, oldverse);
}


void GotoReferenceDialog::on_activate_chapter()
{
  // Ok, chapter selection was made.
  // Update number of verses, and then focus on the verses selector.
  on_move_chapter();
  gtk_widget_grab_focus(treeview_verse);
}


void GotoReferenceDialog::on_move_verse()
{
}


void GotoReferenceDialog::on_activate_verse()
{
  on_activate();
}


