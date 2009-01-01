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
#include "windowshowquickrefs.h"
#include "help.h"
#include "windows.h"
#include "keyterms.h"
#include "tiny_utilities.h"
#include "projectutils.h"
#include "settings.h"

WindowShowQuickReferences::WindowShowQuickReferences(GtkAccelGroup * accelerator_group, bool startup):
WindowBase(widShowQuickReferences, "Quick references", startup, 0)
// Window for showing the quick references.  
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

WindowShowQuickReferences::~WindowShowQuickReferences()
{
}

void WindowShowQuickReferences::go_to(const ustring & project, vector < Reference > &references)
// Show the references.
{
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  ustring language = projectconfig->language_get();
  ustring quickreferences;
  for (unsigned int i = 0; i < references.size(); i++) {
    quickreferences.append(references[i].human_readable(language));
    quickreferences.append(" ");
    ustring text = project_retrieve_verse(project, references[i].book, references[i].chapter, references[i].verse);
    if (text.empty()) {
      quickreferences.append("<empty>");
    } else {
      CategorizeLine cl(text);
      quickreferences.append(cl.verse);
    }
    quickreferences.append("\n");
  }
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
  gtk_text_buffer_set_text(buffer, quickreferences.c_str(), -1);
}
