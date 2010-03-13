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


#ifndef INCLUDED_DIALOGMERGE_H
#define INCLUDED_DIALOGMERGE_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "merge_utils.h"


class MergeDialog
{
public:
  MergeDialog (const ustring& text);
  ~MergeDialog ();
  int run ();
  ustring reconciled_text;
protected:
  GtkWidget *mergedialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label;
  GtkWidget *scrolledwindow;
  GtkWidget *textview;
  GtkTextBuffer * textbuffer;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  void load_text (ustring text);
  vector <MergeButtonPair> buttonpairs;
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  static void on_textbuffer_changed (GtkTextBuffer * textbuffer, gpointer user_data);
  void on_textbuffer ();
  guint event_textbuffer;
  static bool on_textbuffer_delayed (gpointer user_data);
  void on_text_buffer_execute ();
  static void on_mergebutton_clicked (GtkButton *button, gpointer user_data);
  void on_mergebutton (GtkButton *button);
  ustring empty_text ();
  void preprocess_empty_replacements (ustring& text);
};


#endif
