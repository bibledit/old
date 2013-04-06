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


#ifndef INCLUDED_ASSISTANT_CHANGES_H
#define INCLUDED_ASSISTANT_CHANGES_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "assistants.h"
#include "widgetdate.h"
#include "windowreferences.h"


class ChangesAssistant : public AssistantBase
{
public:
  ChangesAssistant(WindowReferences * references_window);
  virtual ~ChangesAssistant();
private:
  GtkWidget *vbox1;
  GtkWidget *radiobutton_since_last_review;
  GtkWidget *radiobutton_since_date;
  GtkWidget *radiobutton_between_dates;
  int question_page;

  GtkWidget *label_last_review;
  int last_review_page;

  unsigned int date_from_seconds;
  DateWidget * date_from_object;
  int date_from_page;

  unsigned int date_to_seconds;
  DateWidget * date_to_object;
  int date_to_page;

  GtkWidget *label_confirm;
  int confirm_page;
  GtkWidget *label_progress;
  GtkWidget *label_summary;
  unsigned int summary_page_number;
  static gint assistant_forward_function (gint current_page, gpointer user_data);
  gint assistant_forward (gint current_page);

  static void on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data);
  void on_assistant_apply ();
  ustring temporal_from_project;
  ustring temporal_to_project;  
  WindowReferences * my_references_window;
  ustring copy_project_and_move_back_in_history (const ustring& project, unsigned int second);
  void view_changes(const ustring& current_stage_project, const ustring& previous_stage_project);
};

#endif
