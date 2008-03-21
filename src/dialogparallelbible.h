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


#ifndef INCLUDED_DIALOG_PARALLEL_BIBLE_H
#define INCLUDED_DIALOG_PARALLEL_BIBLE_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "settings.h"
#include "guiselectproject.h"


class ParallelBibleDialog
{
public:
  ParallelBibleDialog (int dummy);
  ~ParallelBibleDialog ();
  int run ();
protected:
  GtkWidget *parallelbibledialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label12;
  GtkWidget *hseparator1;
  GtkWidget *hbox3;
  GtkWidget *label13;
  GtkWidget *label_main_project;
  GtkWidget *hbox4;
  GtkWidget *label16;
  GtkWidget *label_book;
  GtkWidget *hbox10;
  GtkWidget *label19;
  GtkWidget *label_chapters;
  GtkWidget *button_chapters;
  GtkWidget *alignment3;
  GtkWidget *hbox11;
  GtkWidget *image3;
  GtkWidget *label20;
  GtkWidget *hseparator2;
  GtkWidget *label15;

  // Additional project.
  vector<GtkWidget *> hboxes;
  vector<SelectProjectGui *> selectprojectguis;
  vector<GtkWidget *> buttons;
  
  GtkWidget *hseparator4;
  GtkWidget *hbox7;
  GtkWidget *button_add;
  GtkWidget *alignment2;
  GtkWidget *hbox8;
  GtkWidget *image2;
  GtkWidget *label18;
  GtkWidget *hseparator5;
  GtkWidget *checkbutton_keep_together;
  GtkWidget *checkbutton_include_verse0;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  static void on_button_additional_project_clicked (GtkButton *button, gpointer user_data);
  void on_button_additional_project (GtkButton *button);
  static void on_button_add_clicked (GtkButton *button, gpointer user_data);
  void on_button_add (const ustring& project);
  void vector_remove_content (vector<GtkWidget *>& container, int offset);
  void vector_remove_content (vector<SelectProjectGui *>& container, int offset);
  void rewrite_button_labels ();
  static void on_button_chapters_clicked (GtkButton *button, gpointer user_data);
  void on_button_chapters ();
};


#endif
