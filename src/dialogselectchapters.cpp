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
#include "dialogselectchapters.h"
#include "listview.h"
#include "utilities.h"
#include "portion_utils.h"
#include "projectutils.h"
#include "constants.h"
#include "help.h"
#include "books.h"
#include "tiny_utilities.h"


SelectChaptersDialog::SelectChaptersDialog (const ustring& project, unsigned int book, const ustring& currentselection)
{
  // Save / check variables.
  myproject = project;
  mybook = book;
  myportion = currentselection;
  
  // Build dialog.
  selectchaptersdialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (selectchaptersdialog), "Select portion");
  gtk_window_set_position (GTK_WINDOW (selectchaptersdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal (GTK_WINDOW (selectchaptersdialog), TRUE);

  dialog_vbox1 = GTK_DIALOG (selectchaptersdialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (4, 3, false);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table1), 2);
  gtk_table_set_col_spacings (GTK_TABLE (table1), 2);

  buttonall = gtk_button_new ();
  gtk_widget_show (buttonall);
  gtk_table_attach (GTK_TABLE (table1), buttonall, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  alignment5 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment5);
  gtk_container_add (GTK_CONTAINER (buttonall), alignment5);

  hbox8 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox8);
  gtk_container_add (GTK_CONTAINER (alignment5), hbox8);

  image5 = gtk_image_new_from_stock ("gtk-justify-fill", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image5);
  gtk_box_pack_start (GTK_BOX (hbox8), image5, FALSE, FALSE, 0);

  label18 = gtk_label_new_with_mnemonic ("_All");
  gtk_widget_show (label18);
  gtk_box_pack_start (GTK_BOX (hbox8), label18, FALSE, FALSE, 0);

  label17 = gtk_label_new_with_mnemonic ("_To:");
  gtk_widget_show (label17);
  gtk_table_attach (GTK_TABLE (table1), label17, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label17), 1, 0.5);

  label16 = gtk_label_new_with_mnemonic ("_From:");
  gtk_widget_show (label16);
  gtk_table_attach (GTK_TABLE (table1), label16, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label16), 1, 0.5);

  label19 = gtk_label_new ("chapter");
  gtk_widget_show (label19);
  gtk_table_attach (GTK_TABLE (table1), label19, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label20 = gtk_label_new ("verse");
  gtk_widget_show (label20);
  gtk_table_attach (GTK_TABLE (table1), label20, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_table_attach (GTK_TABLE (table1), scrolledwindow1, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_SHADOW_IN);

  treeviewchapterfrom = gtk_tree_view_new ();
  gtk_widget_show (treeviewchapterfrom);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), treeviewchapterfrom);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeviewchapterfrom), FALSE);

  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow2);
  gtk_table_attach (GTK_TABLE (table1), scrolledwindow2, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_SHADOW_IN);

  treeviewversefrom = gtk_tree_view_new ();
  gtk_widget_show (treeviewversefrom);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), treeviewversefrom);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeviewversefrom), FALSE);

  scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow3);
  gtk_table_attach (GTK_TABLE (table1), scrolledwindow3, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow3), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow3), GTK_SHADOW_IN);

  treeviewchapterto = gtk_tree_view_new ();
  gtk_widget_show (treeviewchapterto);
  gtk_container_add (GTK_CONTAINER (scrolledwindow3), treeviewchapterto);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeviewchapterto), FALSE);

  scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow4);
  gtk_table_attach (GTK_TABLE (table1), scrolledwindow4, 2, 3, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_SHADOW_IN);

  treeviewverseto = gtk_tree_view_new ();
  gtk_widget_show (treeviewverseto);
  gtk_container_add (GTK_CONTAINER (scrolledwindow4), treeviewverseto);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeviewverseto), FALSE);

  dialog_action_area1 = GTK_DIALOG (selectchaptersdialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (selectchaptersdialog, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (selectchaptersdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (selectchaptersdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton, GTK_CAN_DEFAULT);

  // Store, column, selection.
  liststorechapterfrom = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeviewchapterfrom), GTK_TREE_MODEL (liststorechapterfrom));
  g_object_unref (liststorechapterfrom);
  GtkCellRenderer *rendererchapterfrom = gtk_cell_renderer_text_new ();
  columnchapterfrom = gtk_tree_view_column_new_with_attributes ("", rendererchapterfrom, "text", 0, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeviewchapterfrom), columnchapterfrom);
  selectchapterfrom = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeviewchapterfrom));
  gtk_tree_selection_set_mode (selectchapterfrom, GTK_SELECTION_SINGLE);

  liststoreversefrom = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeviewversefrom), GTK_TREE_MODEL (liststoreversefrom));
  g_object_unref (liststoreversefrom);
  GtkCellRenderer *rendererversefrom = gtk_cell_renderer_text_new ();
  columnversefrom = gtk_tree_view_column_new_with_attributes ("", rendererversefrom, "text", 0, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeviewversefrom), columnversefrom);
  selectversefrom = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeviewversefrom));
  gtk_tree_selection_set_mode (selectversefrom, GTK_SELECTION_SINGLE);

  liststorechapterto = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeviewchapterto), GTK_TREE_MODEL (liststorechapterto));
  g_object_unref (liststorechapterto);
  GtkCellRenderer *rendererchapterto = gtk_cell_renderer_text_new ();
  columnchapterto = gtk_tree_view_column_new_with_attributes ("", rendererchapterto, "text", 0, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeviewchapterto), columnchapterto);
  selectchapterto = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeviewchapterto));
  gtk_tree_selection_set_mode (selectchapterto, GTK_SELECTION_SINGLE);

  liststoreverseto = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeviewverseto), GTK_TREE_MODEL (liststoreverseto));
  g_object_unref (liststoreverseto);
  GtkCellRenderer *rendererverseto = gtk_cell_renderer_text_new ();
  columnverseto = gtk_tree_view_column_new_with_attributes ("", rendererverseto, "text", 0, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeviewverseto), columnverseto);
  selectverseto = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeviewverseto));
  gtk_tree_selection_set_mode (selectverseto, GTK_SELECTION_SINGLE);

  g_signal_connect ((gpointer) treeviewchapterfrom, "row_activated",
                    G_CALLBACK (on_treeviewchapterfrom_row_activated),
                    gpointer(this));
  g_signal_connect ((gpointer) treeviewchapterfrom, "button_release_event",
                    G_CALLBACK (on_treeviewchapterfrom_button_release_event),
                    gpointer(this));
  g_signal_connect ((gpointer) treeviewchapterfrom, "key_release_event",
                    G_CALLBACK (on_treeviewchapterfrom_key_release_event),
                    gpointer(this));
  g_signal_connect ((gpointer) treeviewversefrom, "row_activated",
                    G_CALLBACK (on_treeviewversefrom_row_activated),
                    gpointer(this));
  g_signal_connect ((gpointer) treeviewversefrom, "button_release_event",
                    G_CALLBACK (on_treeviewversefrom_button_release_event),
                    gpointer(this));
  g_signal_connect ((gpointer) treeviewversefrom, "key_release_event",
                    G_CALLBACK (on_treeviewversefrom_key_release_event),
                    gpointer(this));
  g_signal_connect ((gpointer) treeviewchapterto, "row_activated",
                    G_CALLBACK (on_treeviewchapterto_row_activated),
                    gpointer(this));
  g_signal_connect ((gpointer) treeviewchapterto, "button_release_event",
                    G_CALLBACK (on_treeviewchapterto_button_release_event),
                    gpointer(this));
  g_signal_connect ((gpointer) treeviewchapterto, "key_release_event",
                    G_CALLBACK (on_treeviewchapterto_key_release_event),
                    gpointer(this));
  g_signal_connect ((gpointer) treeviewverseto, "row_activated",
                    G_CALLBACK (on_treeviewverseto_row_activated),
                    gpointer(this));
  g_signal_connect ((gpointer) treeviewverseto, "button_release_event",
                    G_CALLBACK (on_treeviewverseto_button_release_event),
                    gpointer(this));
  g_signal_connect ((gpointer) treeviewverseto, "key_release_event",
                    G_CALLBACK (on_treeviewverseto_key_release_event),
                    gpointer(this));
  g_signal_connect ((gpointer) buttonall, "clicked",
                    G_CALLBACK (on_buttonall_clicked),
                    gpointer(this));
  g_signal_connect ((gpointer) okbutton, "clicked",
                    G_CALLBACK (on_okbutton_clicked),
                    gpointer(this));

  gtk_label_set_mnemonic_widget (GTK_LABEL (label16), treeviewchapterfrom);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label17), treeviewchapterto);

  gtk_widget_grab_focus (treeviewchapterfrom);
  gtk_widget_grab_default (okbutton);
  
  // Set comboboxes.
  portion_set (myportion);
}


SelectChaptersDialog::~SelectChaptersDialog ()
{
  gtk_widget_destroy (selectchaptersdialog);
}


int SelectChaptersDialog::run ()
{
  return gtk_dialog_run (GTK_DIALOG (selectchaptersdialog));
}


void SelectChaptersDialog::on_okbutton_clicked (GtkButton *button, gpointer user_data)
{
  ((SelectChaptersDialog *) user_data)->on_okbutton ();
}


void SelectChaptersDialog::on_okbutton ()
{
  new_selection = select_portion_get_label (myproject, mybook,
    convert_to_int (listview_get_active_string (treeviewchapterfrom)),
    listview_get_active_string (treeviewversefrom),
    convert_to_int (listview_get_active_string (treeviewchapterto)),
    listview_get_active_string (treeviewverseto));
}


void SelectChaptersDialog::on_fromchapter ()
{
  if (driving_combos)
    return;
  // Fill verses combo.
  unsigned int from = convert_to_int (listview_get_active_string (treeviewchapterfrom));
  vector<ustring> verses = project_get_verses (myproject, mybook, from);
  listview_set_strings (treeviewversefrom, liststoreversefrom, verses);
  listview_focus_string (treeviewversefrom, verses[0], false);
  // If from_chapter > to_chapter, push to_chapter up, as this is probably what the user wants.
  unsigned int to = convert_to_int (listview_get_active_string (treeviewchapterto));
  if (from > to) {
    listview_focus_string (treeviewchapterto, from, false);
  }
}


void SelectChaptersDialog::on_fromverse ()
{
  verses_from_to (true);
}


void SelectChaptersDialog::on_tochapter ()
{
  if (driving_combos)
    return;
  // Fill verses combo.
  unsigned int to = convert_to_int (listview_get_active_string (treeviewchapterto));
  vector<ustring> verses = project_get_verses (myproject, mybook, to);
  listview_set_strings (treeviewverseto, liststoreverseto, verses);
  listview_focus_string (treeviewverseto, verses[verses.size() - 1], false);
  // Modify "from" chapter, if need be.
  unsigned int from = convert_to_int (listview_get_active_string (treeviewchapterfrom));
  if (to < from) {
    listview_focus_string (treeviewchapterfrom, to, false);
  }
}


void SelectChaptersDialog::on_toverse ()
{
  verses_from_to (false);
}


void SelectChaptersDialog::on_buttonall_clicked (GtkButton *button, gpointer user_data)
{
  ((SelectChaptersDialog *) user_data)->on_all ();
}


void SelectChaptersDialog::on_all ()
{
  portion_set (CHAPTER_VERSE_SELECTION_ALL); 
}


void SelectChaptersDialog::portion_set (const ustring& selection)
// Sets the portion in the gui.
{
  // Get values for the portion.
  unsigned int chapter_from, chapter_to;
  ustring verse_from, verse_to;
  select_portion_get_values (myproject, mybook, selection, chapter_from, verse_from, chapter_to, verse_to);

  // Signal we're filling comboboxes.
  driving_combos = true;
  
  // Set the chapter combos.
  vector<unsigned int> chapters = project_get_chapters (myproject, mybook);
  listview_set_strings (treeviewchapterfrom, liststorechapterfrom, chapters);
  listview_set_strings (treeviewchapterto, liststorechapterto, chapters);
  listview_focus_string (treeviewchapterfrom, chapter_from, false);
  listview_focus_string (treeviewchapterto, chapter_to, false);

  // Set the verse combox.
  listview_set_strings (treeviewversefrom, liststoreversefrom, project_get_verses (myproject, mybook, chapter_from));
  listview_set_strings (treeviewverseto, liststoreverseto, project_get_verses (myproject, mybook, chapter_to));
  listview_focus_string (treeviewversefrom, verse_from, false);
  listview_focus_string (treeviewverseto, verse_to, false);
  
  // Ready filling combos.
  driving_combos = false;
}


void SelectChaptersDialog::verses_from_to (bool called_by_from)
{
  if (driving_combos)
    return;
  // If from/to chapters are different, don't bother further.
  unsigned int chapter = convert_to_int (listview_get_active_string (treeviewchapterfrom));
  if (convert_to_int (listview_get_active_string (treeviewchapterto)) != chapter)
    return;
  // Ensure there's no clash between the verses "from" and "to".
  vector<ustring> verses = project_get_verses (myproject, mybook, chapter);
  ustring from = listview_get_active_string (treeviewversefrom);
  ustring to = listview_get_active_string (treeviewverseto);
  unsigned int from_offset = 0;
  unsigned int to_offset = 0;
  for (unsigned int i = 0; i < verses.size(); i++) {
    if (verses[i] == from)
      from_offset = i;
    if (verses[i] == to)
      to_offset = i;
  }
  if (called_by_from) {
    if (from_offset > to_offset)
      listview_focus_string (treeviewverseto, verses[from_offset], false);
  } else {
    if (to_offset < from_offset)
      listview_focus_string (treeviewversefrom, verses[to_offset], false);
  }
}


void SelectChaptersDialog::on_treeviewchapterfrom_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
  ((SelectChaptersDialog *) user_data)->on_treeviewchapterfrom_row ();
}


void SelectChaptersDialog::on_treeviewchapterfrom_row ()
{
  on_fromchapter ();
  gtk_widget_grab_focus (treeviewversefrom);
}


gboolean SelectChaptersDialog::on_treeviewchapterfrom_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  ((SelectChaptersDialog *) user_data)->on_fromchapter ();
  return false;
}


gboolean SelectChaptersDialog::on_treeviewchapterfrom_key_release_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  ((SelectChaptersDialog *) user_data)->on_fromchapter ();
  return false;
}


void SelectChaptersDialog::on_treeviewversefrom_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
  ((SelectChaptersDialog *) user_data)->on_treeviewversefrom_row ();
}


void SelectChaptersDialog::on_treeviewversefrom_row ()
{
  on_fromverse ();
  gtk_widget_grab_focus (treeviewchapterto);
}


gboolean SelectChaptersDialog::on_treeviewversefrom_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  ((SelectChaptersDialog *) user_data)->on_fromverse ();
  return false;
}


gboolean SelectChaptersDialog::on_treeviewversefrom_key_release_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  ((SelectChaptersDialog *) user_data)->on_fromverse ();
  return false;
}


void SelectChaptersDialog::on_treeviewchapterto_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
  ((SelectChaptersDialog *) user_data)->on_treeviewchapterto_row ();
}


void SelectChaptersDialog::on_treeviewchapterto_row ()
{
  on_tochapter ();
  gtk_widget_grab_focus (treeviewverseto);
}


gboolean SelectChaptersDialog::on_treeviewchapterto_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  ((SelectChaptersDialog *) user_data)->on_tochapter ();
  return false;
}


gboolean SelectChaptersDialog::on_treeviewchapterto_key_release_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  ((SelectChaptersDialog *) user_data)->on_tochapter ();
  return false;
}


void SelectChaptersDialog::on_treeviewverseto_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
  ((SelectChaptersDialog *) user_data)->on_treeviewverseto_row ();
}


void SelectChaptersDialog::on_treeviewverseto_row ()
{
  on_toverse ();
  gtk_widget_grab_focus (okbutton);
}


gboolean SelectChaptersDialog::on_treeviewverseto_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  ((SelectChaptersDialog *) user_data)->on_toverse ();
  return false;
}


gboolean SelectChaptersDialog::on_treeviewverseto_key_release_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  ((SelectChaptersDialog *) user_data)->on_toverse ();
  return false;
}
