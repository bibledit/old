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
 ** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

#ifndef INCLUDED_ASSISTANT_CHANGES_H
#define INCLUDED_ASSISTANT_CHANGES_H

#include <gtk/gtk.h>
#include "ustring.h"
#include "assistants.h"

class ChangesAssistant : public AssistantBase
{
public:
  ChangesAssistant(int dummy);
  virtual ~ChangesAssistant();
private:
  GtkWidget *vbox1;
  GtkWidget *radiobutton_since_last_review;
  GtkWidget *radiobutton_since_date;
  GtkWidget *radiobutton_between_dates;
  unsigned int question_page;

  GtkWidget *label_last_review;
  unsigned int last_review_page;

  GtkWidget *vbox2;
  GtkWidget *button_from;
  GtkWidget *alignment2;
  GtkWidget *hbox5;
  GtkWidget *image2;
  GtkWidget *label15;
  static void on_button_from_clicked (GtkButton *button, gpointer user_data);
  void on_button_from();

  GtkWidget *vbox3;
  GtkWidget *button_to;
  GtkWidget *alignment3;
  GtkWidget *hbox6;
  GtkWidget *image3;
  GtkWidget *label17;
  static void on_button_to_clicked (GtkButton *button, gpointer user_data);
  void on_button_to();

  GtkWidget *label_confirm;
  GtkWidget *label_progress;
  GtkWidget *label_summary;
  unsigned int summary_page_number;
  static void on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data);
  void on_assistant_apply ();
  static gint assistant_forward_function (gint current_page, gpointer user_data);
  gint assistant_forward (gint current_page);
  
  
  
  
};

#endif
