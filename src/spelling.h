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
** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#ifndef INCLUDED_SPELLING_H
#define INCLUDED_SPELLING_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include <enchant.h>


ustring spelling_global_dictionary ();
gchar * spelling_project_dictionary_prefix ();
gchar * spelling_project_dictionary_suffix ();
ustring spelling_project_dictionary (const ustring& project);
vector <ustring> spelling_enchant_dictionaries ();
bool spelling_dictionary_editable (const ustring& dictionary);
ustring spelling_dictionary_filename (ustring dictionary);
gchar * spelling_tag_name ();


class SpellingChecker
{
public:
  SpellingChecker (GtkTextTagTable *texttagtable);
  ~SpellingChecker ();
  GtkTextTag *misspelling_tag;
  void attach (GtkTextBuffer * textbuffer);
  void set_dictionaries (const vector <ustring>& dictionaries);
  void check (GtkTextBuffer* textbuffer);
private:
  void collect_words (GtkTextBuffer* textbuffer);
  void check_word (GtkTextBuffer* textbuffer, GtkTextIter *start, GtkTextIter *end);
  set <ustring> correct_words;
  set <ustring> incorrect_words;
  EnchantBroker * broker;
  vector <EnchantDict *> dicts;
  void free_enchant ();
};


#endif
