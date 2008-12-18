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


#include "spelling.h"
#include <enchant-provider.h>
#include "gwrappers.h"
#include "directories.h"
#include "shell.h"
#include "utilities.h"
#include "tiny_utilities.h"
#include "projectutils.h"
#include "editor_aids.h"


ustring spelling_global_dictionary ()
{
  return "Global Dictionary";
}


const gchar * spelling_project_dictionary_prefix ()
{
  return "Project ";
}


const gchar * spelling_project_dictionary_suffix ()
{
  return " Shared Dictionary";
}


ustring spelling_project_dictionary (const ustring& project)
{
  return spelling_project_dictionary_prefix () + project + spelling_project_dictionary_suffix ();
}


static void enumerate_dicts (const char * const lang_tag, const char * const provider_name, const char * const provider_desc, const char * const provider_file, void * user_data)
{
  ((vector <ustring> *) user_data)->push_back (lang_tag);
}


vector <ustring> spelling_enchant_dictionaries ()
{
  vector <ustring> dictionaries;
  EnchantBroker *broker = enchant_broker_init ();
  enchant_broker_list_dicts (broker, enumerate_dicts, &dictionaries);
  enchant_broker_free (broker);
  return dictionaries;

  /* 
  
  Note: Enchant 1.4.1 gives a segmentation fault
  Note: The code below can be used if the library call above crashes
  
  ustring filename = gw_build_filename (directories_get_temp (), "enchant-dictionaries");
  ustring command = "enchant-lsmod -list-dicts >" + shell_quote_space (filename) + "2>&1";
  system (command.c_str ());
  ReadText rt (filename, true, true);
  for (unsigned int i = 0; i < rt.lines.size (); i++) {
    rt.lines[i] = trim (rt.lines[i]);
    Parse parse (rt.lines[i]);
    if (!parse.words.empty ()) {
      ustring dictionary = trim (parse.words[0]);
      if (dictionary.empty ()) continue;
      if (dictionary == "**") continue;
      dictionaries.push_back (dictionary);
    }
  }
  */
}


bool spelling_dictionary_editable (const ustring& dictionary)
{
  if (dictionary == spelling_global_dictionary ()) 
    return true;
  if (g_str_has_prefix (dictionary.c_str(), spelling_project_dictionary_prefix ()))
    if (g_str_has_suffix (dictionary.c_str(), spelling_project_dictionary_suffix ()))
      return true;
  return false;
}


ustring spelling_dictionary_filename (ustring dictionary)
{
  if (dictionary == spelling_global_dictionary ()) {
    return gw_build_filename (directories_get_configuration (), "global_dictionary");
  }
  if (g_str_has_prefix (dictionary.c_str(), spelling_project_dictionary_prefix ())) {
    dictionary.erase (0, strlen (spelling_project_dictionary_prefix ()));
    if (g_str_has_suffix (dictionary.c_str(), spelling_project_dictionary_suffix ())) {
      size_t length = strlen (spelling_project_dictionary_suffix ());
      dictionary.erase (dictionary.length () - length, length);
      ustring project (dictionary);
      ustring filename = gw_build_filename (project_data_directory_project (project), "shared_dictionary");
      return filename;
    }
  }
  return "";
}


const gchar * spelling_tag_name ()
{
  return "misspelling";
}


// The spelling checker code was written while closely looking at GtkSpell.


SpellingChecker::SpellingChecker (GtkTextTagTable *texttagtable)
{
  misspelling_tag = gtk_text_tag_new (spelling_tag_name ()); 
  gtk_text_tag_table_add (texttagtable, misspelling_tag);
  g_object_unref (misspelling_tag);
  GValue gvalue = {0, };
  g_value_init (&gvalue, PANGO_TYPE_UNDERLINE);
  g_value_set_enum (&gvalue, PANGO_UNDERLINE_ERROR);
  g_object_set_property (G_OBJECT (misspelling_tag), "underline", &gvalue);
  g_value_unset (&gvalue);    
  broker = NULL;
  check_signal = gtk_button_new ();
  something_was_added_to_dictionary = false;
}


SpellingChecker::~SpellingChecker ()
{
  gtk_widget_destroy (check_signal);
  free_enchant ();
}


void SpellingChecker::free_enchant ()
{
  // Bail out if there's no broker.
  if (!broker) return;
    
  // Free the dictionaries.
  for (unsigned int i = 0; i < dicts.size (); i++) {
    enchant_broker_free_dict (broker, dicts[i]);
  }
  dicts.clear ();
  pwls.clear ();
  
  // Free the broker.
  enchant_broker_free (broker);
  
  // Clear internal buffers.
  correct_words.clear ();
  incorrect_words.clear ();
}


void SpellingChecker::set_dictionaries (const vector <ustring>& dictionaries)
// Sets the dictionaries to be used for the spelling checker.
{
  free_enchant ();
  broker = enchant_broker_init();
  for (unsigned int i = 0; i < dictionaries.size (); i++) {
    ustring filename = spelling_dictionary_filename (dictionaries[i]);
    EnchantDict * dict = NULL;
    bool pwl = false;
    if (filename.empty ()) {
      dict = enchant_broker_request_dict (broker, dictionaries[i].c_str());
      pwl = false;
    } else {
      dict = enchant_broker_request_pwl_dict (broker, filename.c_str ());
      pwl = true;
    }
    if (dict) {
      dicts.push_back (dict);
      pwls.push_back (pwl);
    } else {
      gw_warning ("Enchant error for dictionary " + dictionaries[i]);
    }
  }
}


void SpellingChecker::check (GtkTextBuffer * textbuffer)
{
  // Erase any previous marks for spelling mistakes.
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_start_iter (textbuffer, &startiter);
  gtk_text_buffer_get_end_iter (textbuffer, &enditer);
  gtk_text_buffer_remove_tag (textbuffer, misspelling_tag, &startiter, &enditer);
  
  // Proceed with next step of speller.
  collect_words (textbuffer);
}


void SpellingChecker::collect_words (GtkTextBuffer * textbuffer)
// Collects words to be spelling checked.
{
  // Iterators for going through the buffer.
  GtkTextIter startiter, enditer;
  
  // Find all the words.
  gtk_text_buffer_get_start_iter (textbuffer, &enditer);
  while (gtk_text_iter_forward_word_end (&enditer)) {
    startiter = enditer;
    gtk_text_iter_backward_word_start (&startiter);    
    GtkTextIter iter = startiter;
    check_word (textbuffer, &startiter, &enditer);
  }
}


void SpellingChecker::check_word (GtkTextBuffer* textbuffer, GtkTextIter *start, GtkTextIter *end)
{
  // Get the word. Skip digits.
  gchar *text = gtk_text_buffer_get_text (textbuffer, start, end, FALSE);
  bool digit = g_unichar_isdigit  (* text);
  ustring word (text);
  g_free (text);  
  if (digit) return;

  // Bail out if the word is among the correct ones that have been verified.
  if (correct_words.find (word) != correct_words.end ()) 
    return;

  // Clear flag if the word is found in the incorrect words.
  bool correct = true;
  if (incorrect_words.find (word) != incorrect_words.end ())
    correct = false;
  
  // If the word is still correct at this stage, it means that it is a new word
  // that we have not seen before. Consult the dictionary to find out about it.
  if (correct) {
    correct = false;
    for (unsigned int i = 0; i < dicts.size (); i++) {
      if (correct) continue;
      correct = (enchant_dict_check(dicts[i], word.c_str(), strlen (word.c_str())) == 0);
    }
    if (correct) correct_words.insert (word);
    else incorrect_words.insert (word);
  }
  
  // If the word is found to be correct, bail out.
  if (correct) 
    return;
  
  // Mark the word as being a spelling mistake. 
  gtk_text_buffer_apply_tag (textbuffer, misspelling_tag, start, end);
}


void SpellingChecker::attach (GtkWidget * textview)
// This routine attaches the spelling checker to a textview.
{
  g_signal_connect (G_OBJECT (textview), "button-press-event", G_CALLBACK (on_button_press_event), gpointer (this));
  g_signal_connect (G_OBJECT (textview), "populate-popup", G_CALLBACK (on_populate_popup), gpointer (this));
  g_signal_connect (G_OBJECT (textview), "popup-menu", G_CALLBACK (on_popup_menu_event), gpointer (this));
}


gboolean SpellingChecker::on_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
// When the user right-clicks on a word, they want to check that word.
// Here, we do NOT  move the cursor to the location of the clicked-upon word
// since that prevents the use of edit functions on the context menu.
{
  ((SpellingChecker *) user_data)->button_press_event (widget, event);
  return false;
}


void SpellingChecker::button_press_event (GtkWidget *widget, GdkEventButton *event)
{
  if (event->button == 3) {
    gint x, y;
    gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (widget), GTK_TEXT_WINDOW_TEXT, int (event->x), int (event->y), &x, &y);
    gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (widget), &right_clicked_iter, x, y);
  } 
}


void SpellingChecker::on_populate_popup (GtkTextView *textview, GtkMenu *menu, gpointer user_data)
{
  ((SpellingChecker *) user_data)->populate_popup (textview, menu);
}


void SpellingChecker::populate_popup (GtkTextView *textview, GtkMenu *menu)
{
  // Find out whether a misspelled word was picked.
  GtkTextIter start, end;
  right_clicked_word_get_extends (&start, &end);

  // Bail out if there was no misspelled word.
  if (!gtk_text_iter_has_tag (&start, misspelling_tag))
    return;

  // Menu separator comes first.
  GtkWidget *mi;
  mi = gtk_menu_item_new ();
  gtk_widget_show (mi);
  gtk_menu_shell_prepend (GTK_MENU_SHELL(menu), mi);

  // On top of it, the suggestions menu.
  GtkWidget *img;
  img = gtk_image_new_from_stock (GTK_STOCK_SPELL_CHECK, GTK_ICON_SIZE_MENU);
  mi = gtk_image_menu_item_new_with_label ("Spelling suggestions");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (mi), img);

  GtkTextBuffer *buffer = gtk_text_view_get_buffer(textview);
  char *word;

  word = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (mi), build_suggestion_menu (buffer, word));
  g_free(word);

  gtk_widget_show_all (mi);
  gtk_menu_shell_prepend (GTK_MENU_SHELL(menu), mi);
}


gboolean SpellingChecker::on_popup_menu_event (GtkTextView *view, gpointer user_data)
// This event occurs when the popup menu is requested through a key-binding,
// the Menu Key or <shift>+F10 by default.  
{
  ((SpellingChecker *) user_data)->popup_menu_event (view);
  return FALSE;
}


void SpellingChecker::popup_menu_event (GtkTextView *view)
{
  GtkTextBuffer * textbuffer = gtk_text_view_get_buffer (view);
  GtkTextMark * textmark = gtk_text_buffer_get_insert (textbuffer);
  gtk_text_buffer_get_iter_at_mark (textbuffer, &right_clicked_iter, textmark);
}


GtkWidget* SpellingChecker::build_suggestion_menu (GtkTextBuffer *buffer, const char *word)
{
  // Top menu.
  GtkWidget *topmenu, *menu;
  topmenu = menu = gtk_menu_new();
  
  // Bail out if there are no dictionaries.
  if (dicts.empty ()) {
    return topmenu;
  }
        
  // There can be more than one dictionary. Go through them all to find suggestions.
  // The use of more than one dictionary will inevitably give double suggestions.
  // These are weeded out.
  vector <ustring> replacements;
  set <ustring> replacement_set;
  for (unsigned int d = 0; d < dicts.size (); d++) {
    size_t n_suggs;
    char **suggestions = enchant_dict_suggest (dicts[d], word, strlen(word), &n_suggs);
    if (suggestions) {
      for (size_t i = 0; i < n_suggs; i++ ) {
        if (replacement_set.find (suggestions[i]) == replacement_set.end ()) {
          replacements.push_back (suggestions[i]);
          replacement_set.insert (suggestions[i]);
        }
      }
      enchant_dict_free_suggestions (dicts[d], suggestions);
    }
  }

  GtkWidget *mi;
  if (replacements.empty ()) {
    // No suggestions. Put something in the menu anyway.
    GtkWidget *label;
    label = gtk_label_new ("");
    gtk_label_set_markup (GTK_LABEL (label), "<i>(No suggestions)</i>");

    mi = gtk_menu_item_new ();
    gtk_container_add (GTK_CONTAINER (mi), label);
    gtk_widget_show_all (mi);
    gtk_menu_shell_prepend (GTK_MENU_SHELL (menu), mi);
  } else {
    // Build a set of menus with suggestions.
    for (unsigned int i = 0; i < replacements.size (); i++) {
      if (i > 0 && i % 10 == 0) {
        
        mi = gtk_menu_item_new ();
        gtk_widget_show (mi);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);

        mi = gtk_menu_item_new_with_label ("More...");
        gtk_widget_show (mi);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);

        menu = gtk_menu_new();
        gtk_menu_item_set_submenu (GTK_MENU_ITEM (mi), menu);
      }
      
      mi = gtk_menu_item_new_with_label (replacements[i].c_str());
      g_signal_connect (G_OBJECT (mi), "activate", G_CALLBACK (on_replace_word), gpointer(this));
      gtk_widget_show (mi);
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);
    }
  }
  
  // Separator
  mi = gtk_menu_item_new ();
  gtk_widget_show (mi);
  gtk_menu_shell_append (GTK_MENU_SHELL (topmenu), mi);

  // + Add to Dictionary
  char *label;
  label = g_strdup_printf ("Add \"%s\" to Dictionary", word);
  mi = gtk_image_menu_item_new_with_label (label);
  g_free (label);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (mi), gtk_image_new_from_stock (GTK_STOCK_ADD, GTK_ICON_SIZE_MENU));
  g_signal_connect (G_OBJECT (mi), "activate", G_CALLBACK (on_add_to_dictionary), gpointer (this));
  gtk_widget_show_all (mi);
  gtk_menu_shell_append (GTK_MENU_SHELL (topmenu), mi);

  // - Ignore All
  mi = gtk_image_menu_item_new_with_label ("Ignore All");
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (mi), gtk_image_new_from_stock (GTK_STOCK_REMOVE, GTK_ICON_SIZE_MENU));
  g_signal_connect (G_OBJECT (mi), "activate", G_CALLBACK (on_ignore_all), gpointer (this));
  gtk_widget_show_all (mi);
  gtk_menu_shell_append (GTK_MENU_SHELL (topmenu), mi);
  
  // Return the top menu.
  return topmenu;
}


void SpellingChecker::right_clicked_word_get_extends (GtkTextIter * start, GtkTextIter *end)
// Get the word boundaries for the word the user right-clicked.
{
  * start = right_clicked_iter;
  if (!gtk_text_iter_starts_word (start))
    gtk_text_iter_backward_word_start (start);
  * end = * start;
  if (gtk_text_iter_inside_word (end))
    gtk_text_iter_forward_word_end (end);
}


void SpellingChecker::on_add_to_dictionary (GtkWidget *menuitem, gpointer user_data)
{
  ((SpellingChecker *) user_data)->add_to_dictionary (menuitem);
}


void SpellingChecker::add_to_dictionary (GtkWidget *menuitem)
{
  // Get a personal wordlist.
  EnchantDict * personal_wordlist = NULL;
  for (unsigned int i = 0; i < dicts.size (); i++) {
    if (!personal_wordlist) {
      if (pwls[i]) personal_wordlist = dicts[i];
    }
  }
  
  // Bail out if there was none.
  if (!personal_wordlist) {
    gw_warning ("No personal wordlist to add the word to");
    return;
  }

  GtkTextBuffer * buffer = gtk_text_iter_get_buffer (&right_clicked_iter);

  GtkTextIter start, end;
  right_clicked_word_get_extends (&start, &end);

  char *word = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
  
  enchant_dict_add_to_pwl (personal_wordlist, word, strlen (word));
  correct_words.insert (word);
  
  // Set a flag that something was added.
  something_was_added_to_dictionary = true;
  
  g_free (word);
  
  gtk_button_clicked (GTK_BUTTON (check_signal));
}


void SpellingChecker::on_ignore_all (GtkWidget *menuitem, gpointer user_data)
{
  ((SpellingChecker *) user_data)->ignore_all (menuitem);
}


void SpellingChecker::ignore_all (GtkWidget *menuitem)
{
  GtkTextBuffer * buffer = gtk_text_iter_get_buffer (&right_clicked_iter);

  GtkTextIter start, end;
  right_clicked_word_get_extends (&start, &end);

  char *word = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
  
  correct_words.insert (word);
  
  g_free (word);
  
  gtk_button_clicked (GTK_BUTTON (check_signal));
}


void SpellingChecker::on_replace_word (GtkWidget *menuitem, gpointer user_data)
{
  ((SpellingChecker *) user_data)->replace_word (menuitem);
}


void SpellingChecker::replace_word (GtkWidget *menuitem)
/*
Replaces the misspelled word in the text with the word that is chosen in the menu.
An algorithm is used that gives the replacement the same styles as the original.
*/
{
  // Bail out if there is no dictionary.
  if (dicts.empty ()) return;
    
  // Get the text buffer.
  GtkTextBuffer * buffer = gtk_text_iter_get_buffer (&right_clicked_iter);
  
  // Get the extends of the misspelled words.
  GtkTextIter start, end;
  right_clicked_word_get_extends (&start, &end);

  // Get the old and the new word.
  char *oldword = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
  const char *newword = gtk_label_get_text (GTK_LABEL (GTK_BIN (menuitem)->child));

  // Get a list of the styles applied to the old word.
  vector <ustring> paragraph_styles;
  vector <ustring> character_styles;
  GtkTextIter iter = start;
  do {
    ustring paragraphstyle, characterstyle;
    get_styles_at_iterator (iter, paragraphstyle, characterstyle);
    paragraph_styles.push_back (paragraphstyle);
    character_styles.push_back (characterstyle);
    gtk_text_iter_forward_char (&iter);
  } while (!gtk_text_iter_equal (&iter, &end));

  // Get the offset of the start of the word.
  unsigned int offset = gtk_text_iter_get_offset (&start);
  
  // Delete the old word, and insert the new one.
  gtk_text_buffer_delete (buffer, &start, &end);
  gtk_text_buffer_insert (buffer, &start, newword, -1);

  // Store this replacement in enchant.
  enchant_dict_store_replacement (dicts[0],  oldword, strlen (oldword), newword, strlen (newword));

  // Free the memory used.
  g_free (oldword);

  // Apply the tags of the old word to the new. 
  // If there are not enough tags, keep repeating the last one.
  ustring unewword (newword);
  for (unsigned int i = 0; i < unewword.length (); i++) {
    ustring paragraphstyle;
    if (i < paragraph_styles.size ()) paragraphstyle = paragraph_styles[i];
    else paragraphstyle = paragraph_styles[paragraph_styles.size()-1];
    ustring characterstyle;
    if (i < character_styles.size ()) characterstyle = character_styles[i];
    else characterstyle = character_styles[character_styles.size()-1];
    gtk_text_buffer_get_iter_at_offset (buffer, &start, offset + i);
    gtk_text_buffer_get_iter_at_offset (buffer, &end, offset + i + 1);
    gtk_text_buffer_remove_all_tags (buffer, &start, &end);
    if (!paragraphstyle.empty ()) {
      gtk_text_buffer_apply_tag_by_name (buffer, paragraphstyle.c_str(), &start, &end);
    }
    if (!characterstyle.empty ())
      gtk_text_buffer_apply_tag_by_name (buffer, characterstyle.c_str(), &start, &end);
  }    
}


void SpellingChecker::set_checkable_tags (const vector <ustring>& tags)
// Sets the tags that can be checked. This implies that text that has any tags, 
// that is, any styles, that are not listed here, is not checked.
// The purpose of this is to prevent the checking of things like \id JHN, etc.
// Not yet implemented.
{
}
