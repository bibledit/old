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


#ifndef INCLUDED_DIALOGEDITSTATUS_H
#define INCLUDED_DIALOGEDITSTATUS_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "libraries.h"
#include "reporting.h"


enum EditStatusType {estNone, estBook, estBooks, estChapter, estChapters, estVerse, estVerses};


class EditStatusDialog
{
public:
  EditStatusDialog(const ustring& project_in, unsigned int book, unsigned int chapter);
  ~EditStatusDialog();
  int run();
protected:
  GtkWidget *editstatusdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox;
  GtkWidget *scrolledwindow_books;
  GtkWidget *treeview_books;
  GtkWidget *scrolledwindow_chapters;
  GtkWidget *treeview_chapters;
  GtkWidget *scrolledwindow_verses;
  GtkWidget *treeview_verses;
  GtkWidget *scrolledwindow_status;
  GtkWidget *viewport_status;
  GtkWidget *vbox_status;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  ustring project;
  ProjectStatus * projectstatus;
  unsigned int alltasks_size;
  
  GtkListStore *liststore_books;
  static gboolean on_treeview_books_button_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  static gboolean on_treeview_books_key_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  void on_treeview_books_cursor();
  vector <int> currentbooks;

  GtkListStore *liststore_chapters;
  void load_chapters();
  static gboolean on_treeview_chapters_button_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  static gboolean on_treeview_chapters_key_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  void on_treeview_chapters_cursor();
  vector <int> currentchapters;

  GtkListStore *liststore_verses;
  void load_verses();
  static gboolean on_treeview_verses_button_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  static gboolean on_treeview_verses_key_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  void on_treeview_verses_cursor();
  vector <int> currentverses;

  EditStatusType editstatustype();
  vector <GtkWidget *> statusbuttons;
  void show_status();
  bool setting_status;
  void set_status (ProjectStatusRecord * statusrecord);
  static void on_checkbutton_status_toggled(GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_status(GtkToggleButton *togglebutton);

  static void on_okbutton_clicked(GtkButton *button, gpointer user_data);
  void on_okbutton();
};


#endif

