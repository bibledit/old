/*
** Copyright (©) 2003-2012 Teus Benschop.
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


#ifndef INCLUDED_DIALOGCHECK_H
#define INCLUDED_DIALOGCHECK_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "settings.h"
#include "guiselectproject.h"


enum CheckDialogType {cdtChaptersVerses, 
                      cdtMarkersCount, cdtMarkersValidate, cdtMarkersCompare, cdtMarkersSpacing,
                      cdtCharactersCount, cdtUnwantedPatterns,
                      cdtWordsCapitalization, cdtWordsRepetition, cdtWordsUnwanted, cdtWordsCount,
                      cdtMatchingPairs, cdtSentenceStructure,
                      cdtReferencesInventory, cdtReferencesValidate,
                      cdtNTQuotationsFromOT, cdtSynopticParallelsNT, cdtParallelsOT };


class CheckDialog
{
public:
  CheckDialog (CheckDialogType checkdialogtype);
  ~CheckDialog ();
  int run ();
protected:
  GtkWidget *checkdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;

  // Information label.
  GtkWidget *infolabel;
  void information_setup (const gchar * information);

  // Output sorting.
  GtkWidget *radiobutton_sort0;
  GtkWidget *radiobutton_sort1;
  GtkWidget *radiobutton_sort2;
  GtkWidget *radiobutton_sort3;
  void sorting_setup (const gchar * sort0, const gchar * sort1, const gchar * sort2, const gchar * sort3);

  // Book selection.
  GtkWidget *hbox1;
  GtkWidget *button_books;
  GtkWidget *alignment1;
  GtkWidget *hbox2;
  GtkWidget *image1;
  GtkWidget *label5;
  GtkWidget *label_book;
  vector <unsigned int> selectable_books;
  void book_selection_setup ();
  static void on_button_books_clicked (GtkButton *button, gpointer user_data);
  void on_booksbutton ();
  void set_gui_books ();
  void set_relevant_books();
  
  // Area selection.
  GtkWidget *hbox15;
  GtkWidget *button_area;
  GtkWidget *alignment8;
  GtkWidget *hbox16;
  GtkWidget *image8;
  GtkWidget *label36;
  GtkWidget *label_area;
  void area_selection_setup ();
  static void on_button_area_clicked (GtkButton *button, gpointer user_data);
  void on_area ();
  
  // Markers comparison.
  GtkWidget *vbox3;
  GtkWidget *hseparator2;
  SelectProjectGui * selectprojectgui;
  GtkWidget *hseparator1;
  GtkWidget *checkbutton_compare_all;
  GtkWidget *label17;
  GtkWidget *entry_compare_include_only;
  GtkWidget *hseparator3;
  GtkWidget *label18;
  GtkWidget *entry_compare_ignore;
  GtkWidget *hseparator4;
  GtkWidget *checkbutton_compare_ignore_verse_zero;
  GtkWidget *hseparator5;
  void markers_comparison_setup ();  
  static void on_checkbutton_compare_all_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_compare_all ();

  // Characters, Unwanted Patterns.
  GtkWidget *vbox12;
  GtkWidget *label45;
  GtkWidget *hseparator15;
  GtkWidget *hbox19;
  GtkWidget *label46;
  GtkWidget *button_unwanted_patterns;
  GtkWidget *alignment11;
  GtkWidget *hbox20;
  GtkWidget *image11;
  GtkWidget *label47;
  void characters_unwanted_patterns_setup ();  
  static void on_button_unwanted_patterns_clicked (GtkButton *button, gpointer user_data);
  void on_button_unwanted_patterns ();

  // Words, Capitalization.
  GtkWidget *vbox5;
  GtkWidget *hseparator6;
  GtkWidget *label23;
  GtkWidget *hbox5;
  GtkWidget *entry_capitalization_punctuation;
  GtkWidget *button_capitalization_punctuation;
  GtkWidget *alignment3;
  GtkWidget *hbox6;
  GtkWidget *image3;
  GtkWidget *label24;
  GtkWidget *hseparator7;
  GtkWidget *label25;
  GtkWidget *hbox7;
  GtkWidget *entry_capitalization_lowercase;
  GtkWidget *button_capitalization_lowercase;
  GtkWidget *alignment4;
  GtkWidget *hbox8;
  GtkWidget *image4;
  GtkWidget *label26;
  GtkWidget *hseparator8;
  GtkWidget *hbox9;
  GtkWidget *label27;
  GtkWidget *vbox6;
  GtkWidget *button_capitalization_abbreviations;
  GtkWidget *alignment5;
  GtkWidget *hbox10;
  GtkWidget *image5;
  GtkWidget *label28;
  GtkWidget *hseparator9;
  GtkWidget *hbox11;
  GtkWidget *vbox9;
  GtkWidget *label29;
  GtkWidget *checkbutton_capitalization_prefixes;
  GtkWidget *vbox7;
  GtkWidget *button_capitalization_prefixes;
  GtkWidget *alignment6;
  GtkWidget *hbox12;
  GtkWidget *image6;
  GtkWidget *label30;
  GtkWidget *hseparator10;
  GtkWidget *hbox13;
  GtkWidget *label31;
  GtkWidget *vbox8;
  GtkWidget *button_capitalization_suffixes;
  GtkWidget *alignment7;
  GtkWidget *hbox14;
  GtkWidget *image7;
  GtkWidget *label32;
  GtkWidget *hseparator11;
  void words_capitalization_setup ();
  static void on_button_capitalization_punctuation_clicked (GtkButton *button, gpointer user_data);
  void capitalization_punctuation_default ();
  static void on_button_capitalization_lowercase_clicked (GtkButton *button, gpointer user_data);
  void capitalization_lowercase_default ();
  static void on_button_capitalization_abbreviations_clicked (GtkButton *button, gpointer user_data);
  void capitalization_abbreviations_edit ();
  static void on_checkbutton_capitalization_prefixes_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_prefixes ();
  static void on_button_capitalization_prefixes_clicked (GtkButton *button, gpointer user_data);
  void button_capitalization_prefixes_edit ();
  static void on_button_capitalization_suffixes_clicked (GtkButton *button, gpointer user_data);
  void capitalization_suffixes_edit ();

  // Word Repetition.
  GtkWidget *vbox10;
  GtkWidget *label38;
  GtkWidget *hseparator12;
  GtkWidget *checkbutton_repetition_case;
  GtkWidget *hseparator13;
  GtkWidget *table1;
  GtkWidget *button_repetition_show;
  GtkWidget *alignment9;
  GtkWidget *hbox17;
  GtkWidget *image9;
  GtkWidget *label39;
  GtkWidget *checkbutton_repetition_show;
  GtkWidget *checkbutton_repetition_ignore;
  GtkWidget *button_repetition_ignore;
  GtkWidget *alignment10;
  GtkWidget *hbox18;
  GtkWidget *image10;
  GtkWidget *label40;
  void word_repetition_setup ();
  static void on_checkbutton_repetition_show_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_repetition_show ();
  static void on_button_repetition_show_clicked (GtkButton *button, gpointer user_data);
  void on_button_repetition_show ();
  static void on_checkbutton_repetition_ignore_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_repetition_ignore ();
  static void on_button_repetition_ignore_clicked (GtkButton *button, gpointer user_data);
  void on_button_repetition_ignore ();

  // Words - Unwanted ones.
  GtkWidget *vbox13;
  GtkWidget *hseparator16;
  GtkWidget *hbox21;
  GtkWidget *label50;
  GtkWidget *button_unwanted_words;
  GtkWidget *alignment12;
  GtkWidget *hbox22;
  GtkWidget *image12;
  GtkWidget *label51;
  void unwanted_words_setup ();
  static void on_button_unwanted_words_clicked (GtkButton *button, gpointer user_data);
  void on_button_unwanted_words ();

  // Punctuation - Matching pairs.
  GtkWidget *vbox11;
  GtkWidget *label43;
  GtkWidget *hseparator14;
  GtkWidget *entry_matching_pairs_ignore;
  void punctuation_matching_pairs_setup ();  

  // Words - Inventory.
  GtkWidget *wordcount_hbox1;
  GtkWidget *wordcount_checkbutton1;
  GtkObject *wordcount_spinbutton1_adj;
  GtkWidget *wordcount_spinbutton1;
  GtkWidget *wordcount_label2;
  GtkWidget *wordcount_label1;
  GtkWidget *wordcount_entry1;
  void word_count_setup ();
  static void on_wordcount_checkbutton1_activate (GtkButton *button, gpointer user_data);
  void on_wordcount_checkbutton1 ();
  static void on_wordcount_entry1_changed (GtkEditable *editable, gpointer user_data);
  void on_wordcount_entry1 ();

  // Markers - Spacing
  GtkWidget *usfm_spacing_hseparator1;
  GtkWidget *usfm_spacing_label2;
  GtkWidget *usfm_spacing_entry1;
  void markers_spacing_setup ();  

  // Inclusion of verse text.
  GtkWidget *checkbutton_include_verse_text;
  void include_verse_text_setup ();

  // Output in Old Testament order.
  GtkWidget *checkbutton_output_in_ot_order;
  void output_in_ot_order_setup ();

  // Dialog actions. 
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  CheckDialogType mycheckdialogtype;
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  void set_gui();

};


#endif
