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
#include <glib.h>
#include "windowshowkeyterms.h"
#include "help.h"
#include "windows.h"
#include "keyterms.h"
#include "tiny_utilities.h"

WindowShowKeyterms::WindowShowKeyterms(GtkAccelGroup * accelerator_group, bool startup, GtkWidget * parent_box):
WindowBase(widShowKeyterms, "Keyterms in verse", startup, 0, parent_box), myreference(0)
// Window showing keyterms.
{
  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_container_add(GTK_CONTAINER(window), scrolledwindow1);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_SHADOW_IN);

  textview1 = gtk_text_view_new();
  gtk_widget_show(textview1);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), textview1);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textview1), FALSE);
  gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(textview1), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview1), GTK_WRAP_WORD);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textview1), FALSE);

  g_signal_connect((gpointer) textview1, "visibility-notify-event", G_CALLBACK(on_visibility_notify_event), gpointer(this));
}

WindowShowKeyterms::~WindowShowKeyterms()
{
}

void WindowShowKeyterms::go_to(const ustring & project, const Reference & reference)
{
  // Bail out if there's no change in the reference.
  if (myreference.equals(reference))
    return;

  // Display the keyterms.
  vector < int >keyterms = keyterms_get_terms_in_verse(reference);
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
  gtk_text_buffer_set_text(buffer, "", -1);
  for (unsigned int i = 0; i < keyterms.size(); i++) {

    // Display the keyterm.
    ustring term;
    unsigned int dummy;
    keyterms_get_term(keyterms[i], term, dummy);
    gtk_text_buffer_insert_at_cursor(buffer, "* ", -1);
    gtk_text_buffer_insert_at_cursor(buffer, term.c_str(), -1);
    gtk_text_buffer_insert_at_cursor(buffer, ": ", -1);

    // Display the renderings.
    vector < ustring > renderings;
    vector < bool > wholewords;
    vector < bool > casesensitives;
    ustring category;
    {
      ustring dummy1;
      vector < Reference > dummy2;
      vector < ustring > dummy3;
      keyterms_get_data(keyterms[i], category, dummy1, dummy2, dummy3);
    }
    keyterms_retrieve_renderings(project, term, category, renderings, wholewords, casesensitives);
    for (unsigned int i2 = 0; i2 < renderings.size(); i2++) {
      if (i2)
        gtk_text_buffer_insert_at_cursor(buffer, ", ", -1);
      gtk_text_buffer_insert_at_cursor(buffer, renderings[i2].c_str(), -1);
    }

    // Newline.
    gtk_text_buffer_insert_at_cursor(buffer, "\n", -1);

  }

  // Store the new reference in the object.
  myreference.assign(reference);
}
