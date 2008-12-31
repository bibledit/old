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

#include "assistants.h"
#include "settings.h"
#include "gwrappers.h"

AssistantBase::AssistantBase(const ustring& title)
// Base class for each window.
{
  // Signalling button.
  signal_button = gtk_button_new ();

  // Create the assistant.
  assistant = gtk_assistant_new();
  gtk_widget_show (assistant);
  gtk_window_set_modal (GTK_WINDOW (assistant), true);
  gtk_window_set_title (GTK_WINDOW (assistant), title.c_str());

  // Let it remain above other windows.
  gdk_window_set_keep_above (gtk_widget_get_root_window (assistant), true);
  
  // The extra window should not appear in the taskbar in addition to the main window of Bibledit.
  // Later it was thought better to show it in the taskbar, else the assistant could be hidden.
  //gtk_window_set_skip_taskbar_hint(GTK_WINDOW(assistant), true);

  // Introduction.
  label_intro = gtk_label_new (title.c_str());
  gtk_widget_show (label_intro);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_intro);
  gtk_label_set_line_wrap (GTK_LABEL (label_intro), TRUE);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_intro, "Introduction");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_intro, GTK_ASSISTANT_PAGE_INTRO);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_intro, true);

  // Signal handlers.
  g_signal_connect (G_OBJECT (assistant), "cancel", G_CALLBACK (on_assistant_cancel_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "close", G_CALLBACK (on_assistant_close_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "delete_event", G_CALLBACK(on_assistant_delete_event), gpointer(this));
}

AssistantBase::~AssistantBase()
{
  gtk_widget_destroy (assistant);
  gtk_widget_destroy (signal_button);
}


void AssistantBase::introduction (const ustring& text)
// Sets the introduction text.
{
  gtk_label_set_text (GTK_LABEL (label_intro), text.c_str());
}


void AssistantBase::on_assistant_cancel_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((AssistantBase *) user_data)->on_assistant_end();
}


void AssistantBase::on_assistant_close_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((AssistantBase *) user_data)->on_assistant_end();
}


bool AssistantBase::on_assistant_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
  ((AssistantBase *) user_data)->on_assistant_end();
  return true;
}


void AssistantBase::on_assistant_end ()
{
  gtk_button_clicked (GTK_BUTTON (signal_button));
}


