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

#include "libraries.h"
#include <glib.h>
#include "assistantchanges.h"
#include "help.h"
#include "settings.h"
#include "git.h"
#include "date_time_utils.h"


ChangesAssistant::ChangesAssistant(int dummy) :
AssistantBase("Keyterms")

// Assistant for adding keyterms.
{
  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));

  // Configurations.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());

  // Introduction.
  introduction ("You can view the changes that have been made in the project.");

  // Ask user what he wants.
  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  question_page = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox1);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox1, "Which changes in the project would you like to see?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox1, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox1, true);

  GSList *radiobutton_since_last_review_group = NULL;

  radiobutton_since_last_review = gtk_radio_button_new_with_mnemonic (NULL, "The ones made since my last review");
  gtk_widget_show (radiobutton_since_last_review);
  gtk_box_pack_start (GTK_BOX (vbox1), radiobutton_since_last_review, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_since_last_review), radiobutton_since_last_review_group);
  radiobutton_since_last_review_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_since_last_review));

  radiobutton_since_date = gtk_radio_button_new_with_mnemonic (NULL, "The ones made since a certain date I will give");
  gtk_widget_show (radiobutton_since_date);
  gtk_box_pack_start (GTK_BOX (vbox1), radiobutton_since_date, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_since_date), radiobutton_since_last_review_group);
  radiobutton_since_last_review_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_since_date));

  radiobutton_between_dates = gtk_radio_button_new_with_mnemonic (NULL, "The ones made between two dates I will give");
  gtk_widget_show (radiobutton_between_dates);
  gtk_box_pack_start (GTK_BOX (vbox1), radiobutton_between_dates, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_between_dates), radiobutton_since_last_review_group);
  radiobutton_since_last_review_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_between_dates));

  // Date last review page.
  label_last_review = gtk_label_new ("");
  gtk_widget_show (label_last_review);
  last_review_page = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_last_review);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_last_review, "Information");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_last_review, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_last_review, true);

  // Show the date and time of last review. 
  // If that is uninitialized, take the date/time from the oldest revision in the git repository.
  unsigned int last_review_seconds = projectconfig->changes_last_review_get();
  if (last_review_seconds == 0) {
    last_review_seconds = git_oldest_commit(settings->genconfig.project_get());
    if (last_review_seconds == 0)
      last_review_seconds--;
    projectconfig->changes_last_review_set(last_review_seconds);
  }
  ustring last_review_label = "Last review was on " + date_time_seconds_human_readable(last_review_seconds, true);
  gtk_label_set_text(GTK_LABEL(label_last_review), last_review_label.c_str());





  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox2);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox2, "Since what date would Information");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox2, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox2, true);

  button_from = gtk_button_new ();
  gtk_widget_show (button_from);
  gtk_box_pack_start (GTK_BOX (vbox2), button_from, FALSE, FALSE, 0);

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button_from), alignment2);

  hbox5 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox5);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox5);

  image2 = gtk_image_new_from_stock ("gtk-goto-first", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox5), image2, FALSE, FALSE, 0);

  label15 = gtk_label_new_with_mnemonic ("date/time");
  gtk_widget_show (label15);
  gtk_box_pack_start (GTK_BOX (hbox5), label15, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_from, "clicked", G_CALLBACK (on_button_from_clicked), gpointer (this));


  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox3);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox3, "Information");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox3, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox3, true);

  button_to = gtk_button_new ();
  gtk_widget_show (button_to);
  gtk_box_pack_start (GTK_BOX (vbox3), button_to, FALSE, FALSE, 0);

  alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (button_to), alignment3);

  hbox6 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox6);
  gtk_container_add (GTK_CONTAINER (alignment3), hbox6);

  image3 = gtk_image_new_from_stock ("gtk-goto-last", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image3);
  gtk_box_pack_start (GTK_BOX (hbox6), image3, FALSE, FALSE, 0);

  label17 = gtk_label_new_with_mnemonic ("date/time");
  gtk_widget_show (label17);
  gtk_box_pack_start (GTK_BOX (hbox6), label17, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_to, "clicked",  G_CALLBACK (on_button_to_clicked), gpointer (this));


  label_confirm = gtk_label_new ("Ready for delete");
  gtk_widget_show (label_confirm);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_confirm);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_confirm, "Ready for delete");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_confirm, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_confirm, true);

  
  label_progress = gtk_label_new ("Deleting...");
  gtk_widget_show (label_progress);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_progress);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_progress, "Deleting");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_progress, GTK_ASSISTANT_PAGE_PROGRESS);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_progress, true);

  
  label_summary = gtk_label_new ("Delete done.");
  gtk_widget_show (label_summary);
  summary_page_number = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_summary);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_summary, "Ready");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_summary, GTK_ASSISTANT_PAGE_SUMMARY);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_summary, true);
  
  // Finish assistant.
  
  gtk_widget_show_all (assistant);
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), 0);
}

ChangesAssistant::~ChangesAssistant()
{
}

void ChangesAssistant::on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((ChangesAssistant *) user_data)->on_assistant_apply();
}


void ChangesAssistant::on_assistant_apply ()
{

  // Show summary.
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), summary_page_number);
}


gint ChangesAssistant::assistant_forward_function (gint current_page, gpointer user_data)
{
  return ((ChangesAssistant *) user_data)->assistant_forward (current_page);
}

gint ChangesAssistant::assistant_forward (gint current_page)
{
  //if (question_page) return last_review_page;
    
  // Default behaviour is to go to the next page.
  return ++current_page;
}


void ChangesAssistant::on_button_from_clicked (GtkButton *button, gpointer user_data)
{
  ((ChangesAssistant *) user_data)->on_button_from ();
}


void ChangesAssistant::on_button_from()
{
}


void ChangesAssistant::on_button_to_clicked (GtkButton *button, gpointer user_data)
{
  ((ChangesAssistant *) user_data)->on_button_to ();
}


void ChangesAssistant::on_button_to()
{
}

