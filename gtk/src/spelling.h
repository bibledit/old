/*
** Copyright (©) 2003-2013 Teus Benschop.
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


#ifndef INCLUDED_SPELLING_H
#define INCLUDED_SPELLING_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include <enchant.h>


ustring spelling_global_dictionary ();
const gchar * spelling_project_dictionary_prefix ();
const gchar * spelling_project_dictionary_suffix ();
ustring spelling_project_dictionary (const ustring& project);
vector <ustring> spelling_enchant_dictionaries ();
bool spelling_dictionary_editable (const ustring& dictionary);
ustring spelling_dictionary_filename (ustring dictionary);
const gchar * spelling_tag_name ();


class SpellingChecker
{
public:
  SpellingChecker (GtkTextTagTable *texttagtable);
  ~SpellingChecker ();
  GtkTextTag *misspelling_tag;
  void attach (GtkWidget * textview);
  void set_dictionaries (const vector <ustring>& dictionaries);
  void check (GtkTextBuffer* textbuffer);
  GtkWidget * check_signal;
  void set_checkable_tags (const vector <ustring>& tags);
  bool move_cursor_to_spelling_error (GtkTextBuffer* textbuffer, bool next, bool extremity);
  vector <ustring> get_misspellings (GtkTextBuffer * textbuffer);
  void add_to_dictionary (const gchar * word);
private:
  void collect_words (GtkTextBuffer* textbuffer);
  bool includes_note_caller (GtkTextIter & iter);
  void check_word (GtkTextBuffer* textbuffer, GtkTextIter *start, GtkTextIter *end);
  set <ustring> correct_words;
  set <ustring> incorrect_words;
  EnchantBroker * broker;
  vector <EnchantDict *> dicts;
  vector <bool> pwls;
  void free_enchant ();
  static gboolean on_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  void button_press_event (GtkWidget *widget, GdkEventButton *event);
  static void on_populate_popup (GtkTextView *textview, GtkMenu *menu, gpointer user_data);
  void populate_popup (GtkTextView *textview, GtkMenu *menu);
  static gboolean on_popup_menu_event (GtkTextView *view, gpointer user_data);
  void popup_menu_event (GtkTextView *view);
  GtkTextIter right_clicked_iter;
  void build_suggestion_menu (GtkWidget * menu, GtkTextBuffer *buffer, const char *word);
  void right_clicked_word_get_extends (GtkTextIter * start, GtkTextIter *end);
  static void on_add_to_dictionary (GtkWidget *menuitem, gpointer user_data);
  void add_to_dictionary (GtkWidget *menuitem);
  static void on_ignore_all (GtkWidget *menuitem, gpointer user_data);
  void ignore_all (GtkWidget *menuitem);
  static void on_replace_word (GtkWidget *menuitem, gpointer user_data);
  void replace_word (GtkWidget *menuitem);
  set <ustring> checkable_tags;
  vector <ustring> misspellings;
};


#endif
