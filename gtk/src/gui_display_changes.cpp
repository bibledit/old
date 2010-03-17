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

#include "libraries.h"
#include "gui_display_changes.h"
#include "constants.h"
#include "tiny_utilities.h"


DisplayChangesGui::DisplayChangesGui(GtkWidget * box)
// Creates a GUI that displays changes on the screen.
// box: the parent box where it goes in.
{
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  gtk_box_pack_start(GTK_BOX(box), vbox, TRUE, TRUE, 0);

  label = gtk_label_new_with_mnemonic("");
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_SHADOW_IN);

  textview = gtk_text_view_new();
  gtk_widget_show(textview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), textview);
  gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(textview), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD);

  // Create tags.
  textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
  heavy_weight_tag = gtk_text_buffer_create_tag(textbuffer, NULL, "weight", PANGO_WEIGHT_HEAVY, "background", "khaki", NULL);
  strike_through_tag = gtk_text_buffer_create_tag(textbuffer, NULL, "strikethrough", TRUE, "background", "khaki", NULL);
}


DisplayChangesGui::~DisplayChangesGui()
{
  gtk_widget_destroy (vbox);
}


void DisplayChangesGui::display(const vector <ustring>& differences)
{
  // Store position of scrollled window.
  GtkAdjustment * adjustment = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolledwindow));
  int scrollbar_position = gtk_adjustment_get_value (adjustment);

  // Variable for holding the number of modifications.
  unsigned int modification_count = 0;

  // Temporally removing the view from the buffer speeds loading text up a huge lot.
  g_object_ref(textbuffer);
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(textview), NULL);
  
  // Clear buffer.
  clear();

  // Load text.
  for (unsigned int i = 0; i < differences.size(); i++) {
    GtkTextIter iter;
    GtkTextTag *tag = NULL;
    ustring line = differences[i];
    while (!line.empty()) {
      // Handle insertion or deletion flag for the next character.
      tag = NULL;
      ustring character = line.substr (0, 1);
      if (!strcmp (character.c_str(), INSERTION_FLAG)) {
        tag = heavy_weight_tag;
      }
      if (!strcmp (character.c_str(), DELETION_FLAG)) {
        tag = strike_through_tag;
      }
      if (tag) {
        line.erase (0, 1);
        // Another modification.
        modification_count++;
      }
      // Print one character with optional markup.
      character = line.substr (0, 1);
      gtk_text_buffer_get_end_iter(textbuffer, &iter);
      gtk_text_buffer_insert_with_tags(textbuffer, &iter, character.c_str(), -1, tag, NULL);
      line.erase(0, 1);
    }
    // End of line.
    gtk_text_buffer_get_end_iter(textbuffer, &iter);
    gtk_text_buffer_insert(textbuffer, &iter, "\n", 1);
  }
  
  // Reconnect the view to the buffer.
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(textview), textbuffer);
  g_object_unref(textbuffer);
  
  // Show modification count.
  ustring message = "Number of modifications: ";
  message.append (convert_to_string (modification_count));
  gtk_label_set_text (GTK_LABEL (label), message.c_str());
  
  // Restore scrollled window's position.
  // The window will then show the same block of text as before loading. This makes editing the text easier.
  while (gtk_events_pending())
    gtk_main_iteration();
  gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolledwindow));
  gtk_adjustment_set_value (adjustment, scrollbar_position);
}


void DisplayChangesGui::clear()
{
  gtk_text_buffer_set_text(textbuffer, "", 0);
}


