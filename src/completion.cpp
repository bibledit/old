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

#include "completion.h"
#include "settings.h"

void completion_setup(GtkWidget * entry, CompletionType completiontype)
// Sets up completion on an entry.
{
  // Create completion.
  GtkEntryCompletion *completion;
  completion = gtk_entry_completion_new();
  // Assign it to the entry.
  gtk_entry_set_completion(GTK_ENTRY(entry), completion);
  // Free memory.
  g_object_unref(completion);
  // Create a model / store and fill it with data.
  extern Settings *settings;
  GtkListStore *store;
  store = gtk_list_store_new(1, G_TYPE_STRING);
  GtkTreeIter iter;
  vector < ustring > completiontable;
  switch (completiontype) {
  case cpSearch:
    completiontable = settings->session.completion_search;
  case cpReplace:
    completiontable = settings->session.completion_replace;
  case cpGoto:
    completiontable = settings->session.completion_goto;
  }
  for (unsigned int i = 0; i < completiontable.size(); i++) {
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, completiontable[i].c_str(), -1);
  }
  gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(store));
  // Free memory.
  g_object_unref(store);
  // Point it to first column to use.
  gtk_entry_completion_set_text_column(completion, 0);
}

void completion_finish(GtkWidget * entry, CompletionType completiontype)
// Finalizes completion on an entry.
{
  // Get the word to add to the completion table.
  ustring word = gtk_entry_get_text(GTK_ENTRY(entry));
  // No word? Finish here.
  if (word.length() == 0)
    return;
  // Word already in table? Finish now.
  vector < ustring > completiontable;
  extern Settings *settings;
  switch (completiontype) {
  case cpSearch:
    completiontable = settings->session.completion_search;
  case cpReplace:
    completiontable = settings->session.completion_replace;
  case cpGoto:
    completiontable = settings->session.completion_goto;
  }
  for (unsigned int i = 0; i < completiontable.size(); i++)
    if (completiontable[i] == word)
      return;
  // Add word to table.
  completiontable.push_back(word);
  // Store table.
  switch (completiontype) {
  case cpSearch:
    settings->session.completion_search = completiontable;
  case cpReplace:
    settings->session.completion_replace = completiontable;
  case cpGoto:
    settings->session.completion_goto = completiontable;
  }
}
