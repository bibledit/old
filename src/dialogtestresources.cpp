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
#include "dialogtestresources.h"
#include "help.h"
#include "directories.h"
#include "gwrappers.h"
#include "books.h"
#include "utilities.h"
#include "tiny_utilities.h"


TestResourcesDialog::TestResourcesDialog (int dummy)
{
  dialogtestresources = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialogtestresources), "Test resources");
  gtk_window_set_position (GTK_WINDOW (dialogtestresources), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint (GTK_WINDOW (dialogtestresources), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialogtestresources)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox2, TRUE, TRUE, 0);

  hbox6 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox6);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox6, TRUE, TRUE, 0);

  progressbar1 = gtk_progress_bar_new ();
  gtk_widget_show (progressbar1);
  gtk_box_pack_start (GTK_BOX (hbox6), progressbar1, TRUE, TRUE, 0);
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar1), 1);

  animation = gtk_image_new_from_file (gw_build_filename (directories_get_package_data (), "animation.gif").c_str());
  gtk_widget_show (animation);
  gtk_box_pack_start (GTK_BOX (hbox6), animation, FALSE, FALSE, 0);

  mozembed = gtk_moz_embed_new ();
  gtk_widget_show (mozembed);
  gtk_box_pack_start (GTK_BOX (vbox2), mozembed, TRUE, TRUE, 0);
  gtk_widget_set_size_request (mozembed, 640, 480);

  dialog_action_area1 = GTK_DIALOG (dialogtestresources)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (dialogtestresources, NULL, NULL);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialogtestresources), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialogtestresources), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);
  
  g_signal_connect ((gpointer) mozembed, "progress_all", G_CALLBACK (on_progress_all_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "net_start", G_CALLBACK (on_load_started_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "net_stop", G_CALLBACK (on_load_finished_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "status_change", G_CALLBACK (on_status_change_cb), gpointer(this));
  g_signal_connect ((gpointer) okbutton1, "clicked", G_CALLBACK (on_okbutton_clicked), gpointer (this));

  gtk_widget_grab_focus (okbutton1);
  gtk_widget_grab_default (okbutton1);
  gtk_widget_hide (animation);
  
  loading = false;
  delay = 0;
  event_id = g_timeout_add_full (G_PRIORITY_DEFAULT, 100, GSourceFunc (on_timeout), gpointer(this), NULL);
}


TestResourcesDialog::~TestResourcesDialog ()
{
  gw_destroy_source (event_id);
  gtk_widget_destroy (dialogtestresources);
  for (unsigned int i = 0; i < files_to_unlink.size (); i++) {
    unlink (files_to_unlink[i].c_str());
  }
}


void TestResourcesDialog::tests (ResourceType resource_type, const ustring& constructor, map <unsigned int, ustring>& books, const ustring& workingdirectory)
{
  vector <unsigned int> ids = books_type_to_ids (btUnknown);
  for (unsigned int i = 0; i < ids.size (); i++) {
    ustring book = books [ids[i]];
    if (!book.empty ()) {
      Reference reference (ids[i], 1, "1");
      ustring url;
      switch (resource_type) {
        case rtForeignDataURLForEachVerse:
        {
          url = resource_construct_url (constructor, books, reference);
          break;
        }
        case rtIndexFileOnFlyForEachVerseWithDifferentAnchors:
        {
          url = resource_construct_index_file (workingdirectory, constructor, books, reference, true);
          break;
        }
        case rtURLForEachVerse:
        {
          url = resource_construct_url (constructor, books, reference);
          url = resource_url_modifier (url, resource_type, gw_build_filename (workingdirectory, "x"));
          break;
        }
        case rtEnd:
        {
          break;
        }
      }
      if (!url.empty ()) {
        test_actions.push_back (url);
        test_actions.push_back ("10");
      }
      files_to_unlink.push_back (url);
    }
  }
}


void TestResourcesDialog::tests (ResourceType resource_type, const ustring& index_file_constructor, map <unsigned int, ustring>& books, map <unsigned int, ustring>& anchors, const ustring& workingdirectory)
{
  vector <unsigned int> ids = books_type_to_ids (btUnknown);
  for (unsigned int i = 0; i < ids.size (); i++) {
    ustring book = books [ids[i]];
    if (!book.empty ()) {
      Reference reference (ids[i], 1, "1000");
      ustring url = resource_construct_index_file (workingdirectory, index_file_constructor, books, anchors, reference, true);
      if (!url.empty ()) {
        test_actions.push_back (url);
        test_actions.push_back ("10");
      }
      files_to_unlink.push_back (url);
      for (unsigned int i = 1; i < 5; i++) {
        reference.verse = convert_to_string (i);
        url = resource_construct_index_file (workingdirectory, index_file_constructor, books, anchors, reference, true);
        if (!url.empty ()) {
          test_actions.push_back (url);
        }
        files_to_unlink.push_back (url);
      }
    }
  }
}


int TestResourcesDialog::run ()
{
  return gtk_dialog_run (GTK_DIALOG (dialogtestresources));
}


bool TestResourcesDialog::on_timeout (gpointer user_data)
{
  ((TestResourcesDialog *) user_data)->timeout ();
  return true;
}


void TestResourcesDialog::timeout ()
{
  if (loading) return;
  if (delay) {
    delay--;
    return;
  }
  if (test_actions.empty ()) return;
  ustring action = test_actions[0];
  test_actions.erase (test_actions.begin ());
  if (action == number_in_string (action)) {
    delay = convert_to_int (action);
    return;
  }
  gtk_moz_embed_load_url (GTK_MOZ_EMBED (mozembed), action.c_str ());
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar1), action.c_str());
}


void TestResourcesDialog::on_progress_all_cb (GtkMozEmbed *embed, const char *aURI, gint cur, gint max, gpointer data)
{
  ((TestResourcesDialog *) data)->on_progress_all (cur, max);
}


void TestResourcesDialog::on_progress_all (gint cur, gint max)
{
  static gint previousmax = 0;
  static double factor = 1;
  if (max != previousmax) {
    previousmax = max;
    factor = 1;
  }
  double fraction = 0.5;
  if (max != -1) fraction = (double) cur / (double) max * factor;
  if (fraction > 1) {
    factor *= 0.9;
    fraction *= 0.9;
  }
  if (fraction > 1) {
    fraction = 1;
  }
  if (fraction < 0) fraction = 0;
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar1), fraction);
}


void TestResourcesDialog::on_load_started_cb (GtkMozEmbed *embed, gpointer data)
{
  ((TestResourcesDialog *) data)->on_load_started ();
}


void TestResourcesDialog::on_load_started ()
{
  gtk_widget_show (animation);
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar1), 0);
  loading = true;
}


void TestResourcesDialog::on_load_finished_cb (GtkMozEmbed *embed, gpointer data)
{
  ((TestResourcesDialog *) data)->on_load_finished ();
}


void TestResourcesDialog::on_load_finished ()
{
  gtk_widget_hide (animation);
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar1), 1);
  loading = false;
  delay = 10;
}


void TestResourcesDialog::on_status_change_cb (GtkMozEmbed *embed, gpointer request, gint status, gpointer message, gpointer data)
{
  ((TestResourcesDialog *) data)->on_status_change (request, status, message);
}


void TestResourcesDialog::on_status_change (gpointer request, gint status, gpointer message)
{
}


void TestResourcesDialog::on_okbutton_clicked (GtkButton *button, gpointer user_data)
{
  ((TestResourcesDialog *) user_data)->on_okbutton ();
}


void TestResourcesDialog::on_okbutton ()
{
}
