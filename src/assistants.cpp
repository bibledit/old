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
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **  
 */

#include "assistants.h"
#include "settings.h"
#include "gwrappers.h"
#include "unixwrappers.h"
#include "tiny_utilities.h"
#include "screen.h"

AssistantBase::AssistantBase(const ustring& title, const gchar * helptopic)
// Base class for each window.
{
  // Variables.
  process_id = 0;
  topic = helptopic;  

  // If no help is given, take a default one.
  if (!topic)
    topic = "none";
  
  // Signalling button.
  signal_button = gtk_button_new ();

  // Create the assistant.
  assistant = gtk_assistant_new();
  gtk_widget_show (assistant);
  gtk_window_set_modal (GTK_WINDOW (assistant), true);
  gtk_window_set_title (GTK_WINDOW (assistant), title.c_str());

  // Let it remain above other windows.
  gdk_window_set_keep_above (gtk_widget_get_root_window (assistant), true);
  
  // Introduction.
  label_intro = gtk_label_new (title.c_str());
  gtk_widget_show (label_intro);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_intro);
  gtk_label_set_line_wrap (GTK_LABEL (label_intro), TRUE);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_intro, "Introduction");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_intro, GTK_ASSISTANT_PAGE_INTRO);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_intro, true);

  // Help button.
  button_help = gtk_toggle_button_new();
  gtk_widget_show(button_help);
  gtk_assistant_add_action_widget (GTK_ASSISTANT (assistant), button_help);

  GtkWidget *alignment = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment);
  gtk_container_add(GTK_CONTAINER(button_help), alignment);

  GtkWidget *hbox = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox);
  gtk_container_add(GTK_CONTAINER(alignment), hbox);

  GtkWidget *image = gtk_image_new_from_stock("gtk-help", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image);
  gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);

  GtkWidget *label = gtk_label_new_with_mnemonic("_Help");
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
  
  // Signal handlers.
  g_signal_connect((gpointer) button_help, "clicked", G_CALLBACK(on_button_help_activated), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "cancel", G_CALLBACK (on_assistant_cancel_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "close", G_CALLBACK (on_assistant_close_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "delete_event", G_CALLBACK(on_assistant_delete_event), gpointer(this));
}

AssistantBase::~AssistantBase()
{
  gtk_widget_destroy (assistant);
  gtk_widget_destroy (signal_button);
  if (process_id) {
    unix_kill(process_id);
  }
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


void AssistantBase::on_button_help_activated(GtkButton * button, gpointer user_data)
{
  ((AssistantBase *) user_data)->on_button_help();
}

void AssistantBase::on_button_help()
{
  bool button_on = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button_help));
  if (button_on) {

    // Calculate the position and size for the help viewer.
    gint width, height, x, y;
    window_position_get_left_space(assistant, width, height, x, y);
    
    // If there is not enough space, make space by moving the assistant.
    if (width < 200) {
      width = 200;
      window_position_make_left_space (assistant, width);
    }

    // Assemble the url to load.    
    ustring url = "http://localhost:51516/olh_";
    url.append(topic);
    url.append(".html");

    // Start the helpviewer.
    GwSpawn spawn("bibledit-help");
    spawn.arg(url);
    spawn.arg(convert_to_string(width));
    spawn.arg(convert_to_string(height));
    spawn.arg(convert_to_string(x));
    spawn.arg(convert_to_string(y));
    spawn.async();
    spawn.run();
    process_id = spawn.pid;

    // Present the window.
    g_usleep(500000);
    gtk_window_present(GTK_WINDOW(assistant));

  } else {
    if (process_id) {
      unix_kill(process_id);
      process_id = 0;
    }
  }
}

