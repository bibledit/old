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


#ifndef INCLUDED_ASSISTANTS_H
#define INCLUDED_ASSISTANTS_H


#include "libraries.h"
#include <gtk/gtk.h>


class AssistantBase
{
public:
  AssistantBase(const ustring& title, const gchar * helptopic);
  virtual ~AssistantBase();
  GtkWidget * signal_button;
  GtkWidget * assistant;
  void introduction (const ustring& text);
private:
  GtkWidget *label_intro;
  static void on_assistant_cancel_signal (GtkAssistant *assistant, gpointer user_data);
  static void on_assistant_close_signal (GtkAssistant *assistant, gpointer user_data);
  static bool on_assistant_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
  void on_assistant_end ();
  GtkWidget *button_help;
  static void on_button_help_activated (GtkButton * button, gpointer user_data);
  void on_button_help ();
  const gchar * topic;
  GPid process_id;
};


#endif
