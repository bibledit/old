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


DisplayChangesGui::DisplayChangesGui(GtkWidget * box, const gchar * text)
// Creates a GUI that displays changes on the screen.
// box: the parent box where it goes in.
// text: the text to put in the label above the differences.
{
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox);
  gtk_box_pack_start(GTK_BOX(box), hbox, TRUE, TRUE, 0);

  if (text) {
    label_user = gtk_label_new_with_mnemonic(text);
    gtk_widget_show(label_user);
    gtk_box_pack_start(GTK_BOX(hbox), label_user, FALSE, FALSE, 0);
  }

  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_box_pack_start(GTK_BOX(hbox), scrolledwindow, TRUE, TRUE, 0);
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
  gtk_widget_destroy (hbox);
}


void DisplayChangesGui::display(const vector <ustring>& differences)
{
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
      if (tag)
        line.erase (0, 1);
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
}


void DisplayChangesGui::clear()
{
  gtk_text_buffer_set_text(textbuffer, "", 0);
}

