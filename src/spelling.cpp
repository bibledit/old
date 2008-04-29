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


ustring spelling_global_dictionary ()
{
  return "Global Dictionary";
}


gchar * spelling_project_dictionary_prefix ()
{
  return "Project ";
}


gchar * spelling_project_dictionary_suffix ()
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


gchar * spelling_tag_name ()
{
  return "misspelling";
}


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
}


SpellingChecker::~SpellingChecker ()
{
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
  
  // Free the broker.
  enchant_broker_free (broker);
}


void SpellingChecker::set_dictionaries (const vector <ustring>& dictionaries)
// Sets the dictionaries to be used for the spelling checker.
{
  free_enchant ();
  broker = enchant_broker_init();
  for (unsigned int i = 0; i < dictionaries.size (); i++) {
    ustring filename = spelling_dictionary_filename (dictionaries[i]);
    EnchantDict * dict = NULL;
    if (filename.empty ()) {
      dict = enchant_broker_request_dict (broker, dictionaries[i].c_str());
    } else {
      dict = enchant_broker_request_pwl_dict (broker, filename.c_str ());
    }
    if (dict) {
      dicts.push_back (dict);
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


void SpellingChecker::attach (GtkTextBuffer * textbuffer)
{
}


/*

Todo spell check


Add a right-clicked menu. If chosen, it looks at the context to see if a word is misspelled.


Also check the embedded buffers.


The right-click menu, if we are not sure to which user dictionary the word to add,
that is, if there is more than one available, then we need to make a menu like
Add
Add to -> list of user dictionaries.


When the spelling checker underlines a misspelled word, it will insert a tag,
and that in turn will set the buffer to modified. Make a mechanism, 
similar to highlighting words, that does not modify the buffer.


Have our own code and dialogs for spelling checking.


Add libenchant-dev to the installation documentation. All distros.


See ~/work/spelling for a lot of example code.


We need to intelligently see from the tags in the buffer whether the word to check is
in the vernacular language. For example things like \id JHN, or \fig ...\fig* should be skipped.
There are some usfm categories that can be checked, these should be checked only, and the rest
skipped.


Use GtkSpell's right click menu for handling misspelled words.
The menu of Screem comes in when doing a check of the whole project.
This check is there too, as a Tool, and allows both editing and checking. Buttons
forward, add, etc, see Screems dialog.


While collecting words we need to handle special cases such as apostrophies, etc.


Apostrophe stuff: http://bugzilla.gnome.org/show_bug.cgi?id=97545 Probably in helpfile, if needed.


If there's a change in the dictionaries, or generally when clicking OK in the 
project dialog, the project should reload the dictionaries, as these may have 
been changed.




*/
