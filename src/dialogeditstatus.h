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


#ifndef INCLUDED_DIALOGEDITSTATUS_H
#define INCLUDED_DIALOGEDITSTATUS_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "libraries.h"
#include "reporting.h"


class EditStatusDialog
{
public:
  EditStatusDialog (const ustring& project_in, unsigned int book, unsigned int chapter);
  ~EditStatusDialog ();
  int run ();
protected:
  GtkWidget *editstatusdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *scrolledwindow1;
  GtkWidget *viewport1;
  GtkWidget *hbox1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *button_project;
  GtkWidget *alignment3;
  GtkWidget *hbox4;
  GtkWidget *progressbar1;
  GtkWidget *vseparator1;
  GtkWidget *vbox2;
  GtkWidget *label2;
  GtkWidget *togglebutton_book;
  GtkWidget *alignment2;
  GtkWidget *hbox3;
  GtkWidget *progressbar_book;
  GtkWidget *vseparator2;
  GtkWidget *vbox3;
  GtkWidget *label3;
  GtkWidget *togglebutton_chapter;
  GtkWidget *alignment4;
  GtkWidget *hbox5;
  GtkWidget *progressbar_chapter;
  GtkWidget *vseparator3;
  GtkWidget *vbox4;
  GtkWidget *label4;
  GtkWidget *button_verse;
  GtkWidget *alignment5;
  GtkWidget *hbox6;
  GtkWidget *progressbar_verse;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void on_button_project_clicked (GtkButton *button, gpointer user_data);
  void on_button_project ();
  static void on_togglebutton_book_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_togglebutton_book_toggle (GtkToggleButton *togglebutton);
  static void on_togglebutton_book_clicked (GtkButton *button, gpointer user_data);
  void on_togglebutton_book_click (GtkButton *button);
  static void on_togglebutton_chapter_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_togglebutton_chapter_toggle (GtkToggleButton *togglebutton);
  static void on_togglebutton_chapter_clicked (GtkButton *button, gpointer user_data);
  void on_togglebutton_chapter_click (GtkButton *button);
  static void on_button_verse_clicked (GtkButton *button, gpointer user_data);
  void on_button_verse (GtkButton *button);
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton (GtkButton *button);
  static bool on_depress_button_timeout (gpointer data);
  ustring project;
  vector <ustring> alltasks;
  vector <unsigned int> allpercentages;
  ProjectStatus * projectstatus;
  void build_books ();
  vector <GtkWidget *> bookbuttons;
  vector <bool> bookhistories;
  vector <GtkWidget *> bookprogressbars;
  unsigned int currentbook;
  bool settingbooks;
  void build_chapters ();
  vector <GtkWidget *> allchapterbuttons;
  vector <GtkWidget *> allchapterprogressbars;
  vector <GtkWidget *> chapterbuttons;
  vector <bool> chapterhistories;
  vector <GtkWidget *> chapterprogressbars;
  unsigned int currentchapter;
  bool settingchapters;
  void build_verses ();
  vector <GtkWidget *> allversebuttons;
  vector <GtkWidget *> allverseprogressbars;
  vector <GtkWidget *> versebuttons;
  vector <GtkWidget *> verseprogressbars;
  void setpercentages ();
};


#endif
