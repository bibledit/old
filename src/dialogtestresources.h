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
** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#ifndef INCLUDED_DIALOGTESTRESOURCES_H
#define INCLUDED_DIALOGTESTRESOURCES_H


#include <gtk/gtk.h>
#include "ustring.h"
#include <gtkmozembed.h>
#include "resource_utils.h"


class TestResourcesDialog
{
public:
  TestResourcesDialog (int dummy);
  ~TestResourcesDialog ();
  void tests (ResourceType resource_type, const ustring& constructor, map <unsigned int, ustring>& books, const ustring& workingdirectory);
  void tests (ResourceType resource_type, const ustring& constructor, map <unsigned int, ustring>& books, map <unsigned int, ustring>& anchors, const ustring& workingdirectory);
  int run ();
protected:
  GtkWidget *dialogtestresources;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox2;
  GtkWidget *hbox6;
  GtkWidget *progressbar1;
  GtkWidget *animation;
  GtkWidget *mozembed;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
private:
  static void on_progress_all_cb (GtkMozEmbed *embed, const char *aURI, gint cur, gint max, gpointer data);
  void on_progress_all (gint cur, gint max);
  static void on_load_started_cb (GtkMozEmbed *embed, gpointer data);
  void on_load_started ();
  static void on_load_finished_cb (GtkMozEmbed *embed, gpointer data);
  void on_load_finished ();
  static void on_status_change_cb (GtkMozEmbed *embed, gpointer request, gint status, gpointer message, gpointer data);
  void on_status_change (gpointer request, gint status, gpointer message);
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  gint event_id;
  static bool on_timeout (gpointer user_data);
  void timeout ();
  vector <ustring> test_actions;
  vector <ustring> files_to_unlink;
  bool loading;
  unsigned int delay;
};


#endif
