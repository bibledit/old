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


#ifndef INCLUDED_GENERALCONFIGURATION_H
#define INCLUDED_GENERALCONFIGURATION_H


#include "libraries.h"
#include "constants.h"


ustring general_configuration_filename ();
void upgrade_configuration ();


// Definitions of various types of variables in the general configuration.
#define DEFINE_GENCONFIG_H(type, parameter) \
private: \
  type    parameter; \
  bool    parameter##_loaded; \
  gchar * parameter##_key (); \
public: \
  type    parameter##_get (); \
  void    parameter##_set (type value) \


class GeneralConfiguration
// Manages program settings in memory and on disk.
{
public:
  GeneralConfiguration (bool save_on_destroy);
  ~GeneralConfiguration ();
  void save ();
  // Private and public definitions follow.
  DEFINE_GENCONFIG_H (int, clear_up_day);
  DEFINE_GENCONFIG_H (int, screen_width);
  DEFINE_GENCONFIG_H (int, screen_height);
  DEFINE_GENCONFIG_H (bool, window_maximized);
  DEFINE_GENCONFIG_H (int, window_width);
  DEFINE_GENCONFIG_H (int, window_height);
  DEFINE_GENCONFIG_H (int, window_x_position);
  DEFINE_GENCONFIG_H (int, window_y_position);
  DEFINE_GENCONFIG_H (int, hpane_position);
  DEFINE_GENCONFIG_H (int, vpane_editor_position);
  DEFINE_GENCONFIG_H (int, vpane_quick_references_position);
  DEFINE_GENCONFIG_H (bool, tools_area_left);
  DEFINE_GENCONFIG_H (ustring, edit_note_standard_text_one);
  DEFINE_GENCONFIG_H (ustring, edit_note_standard_text_two);
  DEFINE_GENCONFIG_H (ustring, edit_note_standard_text_three);
  DEFINE_GENCONFIG_H (ustring, edit_note_standard_text_four);
  DEFINE_GENCONFIG_H (vector<ustring>, projects);
  DEFINE_GENCONFIG_H (vector<int>, project_pages);
  DEFINE_GENCONFIG_H (ustring, project);
  DEFINE_GENCONFIG_H (int, book);
  DEFINE_GENCONFIG_H (ustring, chapter);
  DEFINE_GENCONFIG_H (ustring, verse);
  DEFINE_GENCONFIG_H (ustring, stylesheet);
  DEFINE_GENCONFIG_H (ustring, references_file);
  DEFINE_GENCONFIG_H (ustring, export_to_bibleworks_filename);
  DEFINE_GENCONFIG_H (ustring, export_to_sword_module_path);
  DEFINE_GENCONFIG_H (ustring, export_to_sword_install_path);
  DEFINE_GENCONFIG_H (ustring, paper_format);
  DEFINE_GENCONFIG_H (double, paper_width);
  DEFINE_GENCONFIG_H (double, paper_height);
  DEFINE_GENCONFIG_H (double, paper_inside_margin);
  DEFINE_GENCONFIG_H (double, paper_outside_margin);
  DEFINE_GENCONFIG_H (double, paper_top_margin);
  DEFINE_GENCONFIG_H (double, paper_bottom_margin);
  DEFINE_GENCONFIG_H (bool, notes_print_full_page_width);
  DEFINE_GENCONFIG_H (bool, printdate);
  DEFINE_GENCONFIG_H (bool, print_changes_only);
  DEFINE_GENCONFIG_H (ustring, project_to_compare_with);
  DEFINE_GENCONFIG_H (int, notes_selection_reference);
  DEFINE_GENCONFIG_H (int, notes_selection_edited);
  DEFINE_GENCONFIG_H (int, notes_selection_date_from);
  DEFINE_GENCONFIG_H (int, notes_selection_date_to);
  DEFINE_GENCONFIG_H (ustring, notes_selection_category);
  DEFINE_GENCONFIG_H (bool, notes_selection_current_project);
  DEFINE_GENCONFIG_H (bool, notes_display_center_around_reference);
  DEFINE_GENCONFIG_H (bool, notes_display_project);
  DEFINE_GENCONFIG_H (bool, notes_display_category);
  DEFINE_GENCONFIG_H (bool, notes_display_date_created);
  DEFINE_GENCONFIG_H (bool, notes_display_created_by);
  DEFINE_GENCONFIG_H (bool, notes_display_summary);
  DEFINE_GENCONFIG_H (bool, notes_display_reference_text);
  DEFINE_GENCONFIG_H (ustring, bibleworks_executable);
  DEFINE_GENCONFIG_H (bool, reference_exchange_send_to_bibleworks);
  DEFINE_GENCONFIG_H (bool, reference_exchange_receive_from_bibleworks);
  DEFINE_GENCONFIG_H (bool, reference_exchange_send_to_bibletime);
  DEFINE_GENCONFIG_H (bool, reference_exchange_receive_from_bibletime);
  DEFINE_GENCONFIG_H (bool, reference_exchange_send_to_santafefocus);
  DEFINE_GENCONFIG_H (bool, reference_exchange_send_to_bibledit_resource_viewer);
  DEFINE_GENCONFIG_H (bool, reference_exchange_receive_from_santafefocus);
  DEFINE_GENCONFIG_H (ustring, check_markers_compare_project);
  DEFINE_GENCONFIG_H (bool, check_markers_compare_all_markers);
  DEFINE_GENCONFIG_H (ustring, check_markers_compare_include_only);
  DEFINE_GENCONFIG_H (ustring, check_markers_compare_ignore);
  DEFINE_GENCONFIG_H (bool, check_markers_compare_ignore_verse_zero);
  DEFINE_GENCONFIG_H (ustring, check_capitalization_punctuation);
  DEFINE_GENCONFIG_H (ustring, check_capitalization_ignore);
  DEFINE_GENCONFIG_H (bool, check_capitalization_allow_any_prefixes);
  DEFINE_GENCONFIG_H (bool, check_repetition_ignore_case);
  DEFINE_GENCONFIG_H (bool, check_repetition_show_only_these);
  DEFINE_GENCONFIG_H (bool, check_repetition_ignore_these);
  DEFINE_GENCONFIG_H (ustring, check_matching_pairs_ignore);
  DEFINE_GENCONFIG_H (int, check_words_inventory_not_include_words_count);
  DEFINE_GENCONFIG_H (ustring, check_words_inventory_word_forming_characters);
  DEFINE_GENCONFIG_H (ustring, check_markers_spacing_include);
  DEFINE_GENCONFIG_H (int, tools_area_page_number);
  DEFINE_GENCONFIG_H (vector<bool>, styles_category_expanded);
  DEFINE_GENCONFIG_H (ustring, insert_footnote_template);
  DEFINE_GENCONFIG_H (ustring, insert_endnote_template);
  DEFINE_GENCONFIG_H (ustring, insert_xref_template);
  DEFINE_GENCONFIG_H (bool, parallel_bible_keep_verses_together);
  DEFINE_GENCONFIG_H (ustring, parallel_bible_chapters_verses);
  DEFINE_GENCONFIG_H (bool, parallel_bible_include_verse_zero);
  DEFINE_GENCONFIG_H (vector<ustring>, printing_fonts);
  DEFINE_GENCONFIG_H (vector<ustring>, parallel_bible_projects);
  DEFINE_GENCONFIG_H (bool, use_outpost);
  DEFINE_GENCONFIG_H (bool, outpost_networked);
  DEFINE_GENCONFIG_H (ustring, outpost_host);
  DEFINE_GENCONFIG_H (ustring, outpost_path);
  DEFINE_GENCONFIG_H (ustring, wine_path);
  DEFINE_GENCONFIG_H (ustring, outpost_command);
  DEFINE_GENCONFIG_H (ustring, mychecks);
  DEFINE_GENCONFIG_H (bool, tidy_translate);
  DEFINE_GENCONFIG_H (vector<int>, tidy_books);
  DEFINE_GENCONFIG_H (vector<ustring>, tidy_texts);
  DEFINE_GENCONFIG_H (bool, tidy_normalize_hyphens);
  DEFINE_GENCONFIG_H (bool, tidy_space_between_chapter_verse);
  DEFINE_GENCONFIG_H (bool, tidy_space_series_verses);
  DEFINE_GENCONFIG_H (bool, tidy_full_stop_ends_text);
  DEFINE_GENCONFIG_H (bool, tidy_ampersand_semicolon);
  DEFINE_GENCONFIG_H (bool, tidy_space_before_punctuation);
  DEFINE_GENCONFIG_H (bool, wordlist_process_general);
  DEFINE_GENCONFIG_H (bool, wordlist_general_asterisk);
  DEFINE_GENCONFIG_H (bool, wordlist_general_asterisk_first);
  DEFINE_GENCONFIG_H (bool, wordlist_process_hebrew);
  DEFINE_GENCONFIG_H (bool, wordlist_hebrew_asterisk);
  DEFINE_GENCONFIG_H (bool, wordlist_hebrew_asterisk_first);
  DEFINE_GENCONFIG_H (bool, wordlist_process_greek);
  DEFINE_GENCONFIG_H (bool, wordlist_greek_asterisk);
  DEFINE_GENCONFIG_H (bool, wordlist_greek_asterisk_first);
  DEFINE_GENCONFIG_H (bool, wordlist_process_index);
  DEFINE_GENCONFIG_H (bool, wordlist_index_asterisk);
  DEFINE_GENCONFIG_H (bool, wordlist_index_asterisk_first);
  DEFINE_GENCONFIG_H (bool, notes_editor_font_default);
  DEFINE_GENCONFIG_H (ustring, notes_editor_font_name);
  DEFINE_GENCONFIG_H (bool, notes_editor_default_color);
  DEFINE_GENCONFIG_H (int, notes_editor_normal_text_color);
  DEFINE_GENCONFIG_H (int, notes_editor_background_color);
  DEFINE_GENCONFIG_H (int, notes_editor_selected_text_color);
  DEFINE_GENCONFIG_H (int, notes_editor_selection_color);
  DEFINE_GENCONFIG_H (bool, text_editor_font_default);
  DEFINE_GENCONFIG_H (ustring, text_editor_font_name);
  DEFINE_GENCONFIG_H (bool, text_editor_default_color);
  DEFINE_GENCONFIG_H (int, text_editor_normal_text_color);
  DEFINE_GENCONFIG_H (int, text_editor_background_color);
  DEFINE_GENCONFIG_H (int, text_editor_selected_text_color);
  DEFINE_GENCONFIG_H (int, text_editor_selection_color);
  DEFINE_GENCONFIG_H (ustring, encoding);
  DEFINE_GENCONFIG_H (int, features_mode);
  DEFINE_GENCONFIG_H (ustring, features_list);
  DEFINE_GENCONFIG_H (ustring, administration_password);
  DEFINE_GENCONFIG_H (vector<ustring>, print_references_projects);
  DEFINE_GENCONFIG_H (vector<int>, dialogpositions_x);
  DEFINE_GENCONFIG_H (vector<int>, dialogpositions_y);
  DEFINE_GENCONFIG_H (bool, text_replacement);
  DEFINE_GENCONFIG_H (vector<ustring>, text_replacement_originals);
  DEFINE_GENCONFIG_H (vector<ustring>, text_replacement_replacements);
  DEFINE_GENCONFIG_H (bool, pdf_viewer_automatic);
  DEFINE_GENCONFIG_H (ustring, pdf_viewer_path);
  DEFINE_GENCONFIG_H (ustring, pdf_viewer_arguments);
  DEFINE_GENCONFIG_H (vector<ustring>, project_tasks_names);
  DEFINE_GENCONFIG_H (vector<double>, project_tasks_durations);
  DEFINE_GENCONFIG_H (bool, inserting_xref_shows_references);
  DEFINE_GENCONFIG_H (vector<ustring>, resources);
  DEFINE_GENCONFIG_H (vector<int>, resource_pages);
  DEFINE_GENCONFIG_H (int, split_view_open_method);
  DEFINE_GENCONFIG_H (bool, split_view_editor_top_down_layout);
  DEFINE_GENCONFIG_H (int, print_job);
  DEFINE_GENCONFIG_H (int, git_health);
private:
  bool my_save_on_destroy;
  bool bool_get (gchar * key, bool& store, bool& loaded, bool standard);
  int int_get (gchar * key, int& store, bool& loaded, int standard);
  ustring string_get (gchar * key, ustring& store, bool& loaded, const ustring& standard);
  double double_get (gchar * key, double& store, bool& loaded, double standard);
  vector<bool> vector_bool_get (gchar * key, vector<bool>& store, bool& loaded, void * dummy);
  vector<ustring> vector_string_get (gchar * key, vector<ustring>& store, bool& loaded, void * dummy);
  vector<int> vector_int_get (gchar * key, vector<int>& store, bool& loaded, void * dummy);
  vector<double> vector_double_get (gchar * key, vector<double>& store, bool& loaded, void * dummy);
};


#endif
