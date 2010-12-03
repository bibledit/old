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


#include "settings.h"
#include "directories.h"
#include "gwrappers.h"
#include "sqlite3.h"
#include "sqlite_reader.h"
#include "utilities.h"
#include "date_time_utils.h"
#include "shell.h"
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include "config.xml.h"
#include "windowsoutpost.h"


ustring general_configuration_filename()
{
  return gw_build_filename(directories_get_configuration(), "configuration.1.xml");
}


void upgrade_configuration()
{
}


/*
 A test was done on to see if the cause of irregular crashes of Bibledit, 
 when used a lot, is, that GeneralConfiguration was always left open in mainwindow.
 It is now always closed.
 Several versions later it appeared that the random crashes had stopped.
 The conclusion is that the database should always be closed after use, 
 and not left open.
 Note: The above is still true in general, but the configuration no longer 
 uses the database, hence it is not relevant in this particular case.
 */


GeneralConfiguration::GeneralConfiguration(bool save_on_destroy)
/*
 This version of the general configuration uses a fast, in-memory system 
 for storing and retrieving the data.
 It loads the values from file only when needed. 
 Once the values have been loaded in memory, they remain there,
 and the next times the values are queried, they are taken from memory,
 instead of loading them from disk again. This speeds the system up greatly.
 It has "getters" and "setters" to facilitate this system.
 On object destruction, it saves the values if they were modified.
 */
{
  // Save parameters.
  my_save_on_destroy = save_on_destroy;

  // Function definition for initializing variables.
#define INITIALIZE(parameter) parameter##_loaded = false

  // Initialize variables.
  INITIALIZE(screen_width);
  INITIALIZE(screen_height);
  INITIALIZE (window_width);
  INITIALIZE (window_height);
  INITIALIZE (window_x_position);
  INITIALIZE (window_y_position);
  INITIALIZE (window_maximized);
  INITIALIZE(window_widths);
  INITIALIZE(window_heights);
  INITIALIZE(window_x_positions);
  INITIALIZE(window_y_positions);
  INITIALIZE(window_ids);
  INITIALIZE(window_titles);
  INITIALIZE(window_shows);
  INITIALIZE(edit_note_standard_text_one);
  INITIALIZE(edit_note_standard_text_two);
  INITIALIZE(edit_note_standard_text_three);
  INITIALIZE(edit_note_standard_text_four);
  INITIALIZE(project);
  INITIALIZE(book);
  INITIALIZE(chapter);
  INITIALIZE(verse);
  INITIALIZE(stylesheet);
  INITIALIZE(references_file);
  INITIALIZE(export_to_bibleworks_filename);
  INITIALIZE(export_to_sword_module_path);
  INITIALIZE(export_to_sword_install_path);
  INITIALIZE(paper_format);
  INITIALIZE(paper_width);
  INITIALIZE(paper_height);
  INITIALIZE(paper_inside_margin);
  INITIALIZE(paper_outside_margin);
  INITIALIZE(paper_top_margin);
  INITIALIZE(paper_bottom_margin);
  INITIALIZE(printdate);
  INITIALIZE(header_font_size);
  INITIALIZE(print_engine_use_intermediate_text);
  INITIALIZE(print_changes_only);
  INITIALIZE(project_to_compare_with);
  INITIALIZE(notes_selection_reference);
  INITIALIZE(notes_selection_edited);
  INITIALIZE(notes_selection_date_from);
  INITIALIZE(notes_selection_date_to);
  INITIALIZE(notes_selection_category);
  INITIALIZE(notes_selection_current_project);
  INITIALIZE(notes_display_project);
  INITIALIZE(notes_display_category);
  INITIALIZE(notes_display_date_created);
  INITIALIZE(notes_display_created_by);
  INITIALIZE(notes_display_summary);
  INITIALIZE(notes_display_reference_text);
  INITIALIZE(check_markers_compare_project);
  INITIALIZE(check_markers_compare_all_markers);
  INITIALIZE(check_markers_compare_include_only);
  INITIALIZE(check_markers_compare_ignore);
  INITIALIZE(check_markers_compare_ignore_verse_zero);
  INITIALIZE(check_capitalization_punctuation);
  INITIALIZE(check_capitalization_ignore);
  INITIALIZE(check_capitalization_allow_any_prefixes);
  INITIALIZE(check_repetition_ignore_case);
  INITIALIZE(check_repetition_show_only_these);
  INITIALIZE(check_repetition_ignore_these);
  INITIALIZE(check_matching_pairs_ignore);
  INITIALIZE(check_words_inventory_not_include_words_count);
  INITIALIZE(check_words_inventory_word_forming_characters);
  INITIALIZE(check_markers_spacing_include);
  INITIALIZE(styles_category_expanded);
  INITIALIZE(insert_footnote_template);
  INITIALIZE(insert_endnote_template);
  INITIALIZE(insert_xref_template);
  INITIALIZE(parallel_bible_keep_verses_together);
  INITIALIZE(parallel_bible_chapters_verses);
  INITIALIZE(parallel_bible_include_verse_zero);
  INITIALIZE(printing_fonts);
  INITIALIZE(parallel_bible_projects);
  INITIALIZE(parallel_bible_enabled);
  INITIALIZE(use_outpost);
  INITIALIZE(mychecks);
  INITIALIZE(tidy_translate);
  INITIALIZE(tidy_books);
  INITIALIZE(tidy_texts);
  INITIALIZE(tidy_normalize_hyphens);
  INITIALIZE(tidy_space_between_chapter_verse);
  INITIALIZE(tidy_space_series_verses);
  INITIALIZE(tidy_full_stop_ends_text);
  INITIALIZE(tidy_ampersand_semicolon);
  INITIALIZE(tidy_space_before_punctuation);
  INITIALIZE(wordlist_process_general);
  INITIALIZE(wordlist_general_asterisk);
  INITIALIZE(wordlist_general_asterisk_first);
  INITIALIZE(wordlist_process_hebrew);
  INITIALIZE(wordlist_hebrew_asterisk);
  INITIALIZE(wordlist_hebrew_asterisk_first);
  INITIALIZE(wordlist_process_greek);
  INITIALIZE(wordlist_greek_asterisk);
  INITIALIZE(wordlist_greek_asterisk_first);
  INITIALIZE(wordlist_process_index);
  INITIALIZE(wordlist_index_asterisk);
  INITIALIZE(wordlist_index_asterisk_first);
  INITIALIZE(text_editor_font_default);
  INITIALIZE(text_editor_font_name);
  INITIALIZE(text_editor_default_color);
  INITIALIZE(text_editor_normal_text_color);
  INITIALIZE(text_editor_background_color);
  INITIALIZE(text_editor_selected_text_color);
  INITIALIZE(text_editor_selection_color);
  INITIALIZE(encoding);
  INITIALIZE(features_mode);
  INITIALIZE(features_list);
  INITIALIZE(remember_verse_per_chapter);
  INITIALIZE(administration_password);
  INITIALIZE(print_references_projects);
  INITIALIZE(dialogpositions_x);
  INITIALIZE(dialogpositions_y);
  INITIALIZE(text_replacement);
  INITIALIZE(text_replacement_originals);
  INITIALIZE(text_replacement_replacements);
  INITIALIZE(pdf_viewer_automatic);
  INITIALIZE(pdf_viewer_path);
  INITIALIZE(pdf_viewer_arguments);
  INITIALIZE(project_tasks_names);
  INITIALIZE(project_tasks_durations);
  INITIALIZE(print_job);
  INITIALIZE(projects_displaying_verses);
  INITIALIZE(compare_disregard_notes);
  INITIALIZE(source_language_names);
  INITIALIZE(reference_window_show_verse_text);
  INITIALIZE(reference_window_show_relevant_bits);
  INITIALIZE(consultation_notes_git_use_remote_repository);
  INITIALIZE(consultation_notes_git_remote_repository_url);
  INITIALIZE(consultation_notes_git_remote_repository_conflict_handling);
  INITIALIZE(bibledit_web_url);
  INITIALIZE(bibledit_web_user);
}


GeneralConfiguration::~GeneralConfiguration()
{
  if (my_save_on_destroy) {
    save();
  }
}


void GeneralConfiguration::save()
// Saves all settings to disk.
{
  vector < ConfigXmlPair > values;

#define SAVE_VALUE(item) if (item##_loaded) config_xml_values_set_assemble (values, item##_key(), item)

  SAVE_VALUE(screen_width);
  SAVE_VALUE(screen_height);
  SAVE_VALUE (window_width);
  SAVE_VALUE (window_height);
  SAVE_VALUE (window_x_position);
  SAVE_VALUE (window_y_position);
  SAVE_VALUE (window_maximized);
  SAVE_VALUE(window_widths);
  SAVE_VALUE(window_heights);
  SAVE_VALUE(window_x_positions);
  SAVE_VALUE(window_y_positions);
  SAVE_VALUE(window_ids);
  SAVE_VALUE(window_titles);
  SAVE_VALUE(window_shows);
  SAVE_VALUE(edit_note_standard_text_one);
  SAVE_VALUE(edit_note_standard_text_two);
  SAVE_VALUE(edit_note_standard_text_three);
  SAVE_VALUE(edit_note_standard_text_four);
  SAVE_VALUE(project);
  SAVE_VALUE(book);
  SAVE_VALUE(chapter);
  SAVE_VALUE(verse);
  SAVE_VALUE(stylesheet);
  SAVE_VALUE(references_file);
  SAVE_VALUE(export_to_bibleworks_filename);
  SAVE_VALUE(export_to_sword_module_path);
  SAVE_VALUE(export_to_sword_install_path);
  SAVE_VALUE(paper_format);
  SAVE_VALUE(paper_width);
  SAVE_VALUE(paper_height);
  SAVE_VALUE(paper_inside_margin);
  SAVE_VALUE(paper_outside_margin);
  SAVE_VALUE(paper_top_margin);
  SAVE_VALUE(paper_bottom_margin);
  SAVE_VALUE(printdate);
  SAVE_VALUE(header_font_size);
  SAVE_VALUE(print_engine_use_intermediate_text);
  SAVE_VALUE(print_changes_only);
  SAVE_VALUE(project_to_compare_with);
  SAVE_VALUE(notes_selection_reference);
  SAVE_VALUE(notes_selection_edited);
  SAVE_VALUE(notes_selection_date_from);
  SAVE_VALUE(notes_selection_date_to);
  SAVE_VALUE(notes_selection_category);
  SAVE_VALUE(notes_selection_current_project);
  SAVE_VALUE(notes_display_project);
  SAVE_VALUE(notes_display_category);
  SAVE_VALUE(notes_display_date_created);
  SAVE_VALUE(notes_display_created_by);
  SAVE_VALUE(notes_display_summary);
  SAVE_VALUE(notes_display_reference_text);
  SAVE_VALUE(check_markers_compare_project);
  SAVE_VALUE(check_markers_compare_all_markers);
  SAVE_VALUE(check_markers_compare_include_only);
  SAVE_VALUE(check_markers_compare_ignore);
  SAVE_VALUE(check_markers_compare_ignore_verse_zero);
  SAVE_VALUE(check_capitalization_punctuation);
  SAVE_VALUE(check_capitalization_ignore);
  SAVE_VALUE(check_capitalization_allow_any_prefixes);
  SAVE_VALUE(check_repetition_ignore_case);
  SAVE_VALUE(check_repetition_show_only_these);
  SAVE_VALUE(check_repetition_ignore_these);
  SAVE_VALUE(check_matching_pairs_ignore);
  SAVE_VALUE(check_words_inventory_not_include_words_count);
  SAVE_VALUE(check_words_inventory_word_forming_characters);
  SAVE_VALUE(check_markers_spacing_include);
  SAVE_VALUE(styles_category_expanded);
  SAVE_VALUE(insert_footnote_template);
  SAVE_VALUE(insert_endnote_template);
  SAVE_VALUE(insert_xref_template);
  SAVE_VALUE(parallel_bible_keep_verses_together);
  SAVE_VALUE(parallel_bible_chapters_verses);
  SAVE_VALUE(parallel_bible_include_verse_zero);
  SAVE_VALUE(printing_fonts);
  SAVE_VALUE(parallel_bible_projects);
  SAVE_VALUE(parallel_bible_enabled);
  SAVE_VALUE(mychecks);
  SAVE_VALUE(tidy_translate);
  SAVE_VALUE(tidy_books);
  SAVE_VALUE(tidy_texts);
  SAVE_VALUE(tidy_normalize_hyphens);
  SAVE_VALUE(tidy_space_between_chapter_verse);
  SAVE_VALUE(tidy_space_series_verses);
  SAVE_VALUE(tidy_full_stop_ends_text);
  SAVE_VALUE(tidy_ampersand_semicolon);
  SAVE_VALUE(tidy_space_before_punctuation);
  SAVE_VALUE(wordlist_process_general);
  SAVE_VALUE(wordlist_general_asterisk);
  SAVE_VALUE(wordlist_general_asterisk_first);
  SAVE_VALUE(wordlist_process_hebrew);
  SAVE_VALUE(wordlist_hebrew_asterisk);
  SAVE_VALUE(wordlist_hebrew_asterisk_first);
  SAVE_VALUE(wordlist_process_greek);
  SAVE_VALUE(wordlist_greek_asterisk);
  SAVE_VALUE(wordlist_greek_asterisk_first);
  SAVE_VALUE(wordlist_process_index);
  SAVE_VALUE(wordlist_index_asterisk);
  SAVE_VALUE(wordlist_index_asterisk_first);
  SAVE_VALUE(text_editor_font_default);
  SAVE_VALUE(text_editor_font_name);
  SAVE_VALUE(text_editor_default_color);
  SAVE_VALUE(text_editor_normal_text_color);
  SAVE_VALUE(text_editor_background_color);
  SAVE_VALUE(text_editor_selected_text_color);
  SAVE_VALUE(text_editor_selection_color);
  SAVE_VALUE(encoding);
  SAVE_VALUE(features_mode);
  SAVE_VALUE(features_list);
  SAVE_VALUE(remember_verse_per_chapter); 
  SAVE_VALUE(administration_password);
  SAVE_VALUE(print_references_projects);
  SAVE_VALUE(dialogpositions_x);
  SAVE_VALUE(dialogpositions_y);
  SAVE_VALUE(text_replacement);
  SAVE_VALUE(text_replacement_originals);
  SAVE_VALUE(text_replacement_replacements);
  SAVE_VALUE(pdf_viewer_automatic);
  SAVE_VALUE(pdf_viewer_path);
  SAVE_VALUE(pdf_viewer_arguments);
  SAVE_VALUE(project_tasks_names);
  SAVE_VALUE(project_tasks_durations);
  SAVE_VALUE(print_job);
  SAVE_VALUE(projects_displaying_verses);
  SAVE_VALUE(compare_disregard_notes);
  SAVE_VALUE(source_language_names);
  SAVE_VALUE(reference_window_show_verse_text);
  SAVE_VALUE(reference_window_show_relevant_bits);
  SAVE_VALUE(consultation_notes_git_use_remote_repository);
  SAVE_VALUE(consultation_notes_git_remote_repository_url);
  SAVE_VALUE(consultation_notes_git_remote_repository_conflict_handling);
  SAVE_VALUE(bibledit_web_url);
  SAVE_VALUE(bibledit_web_user);

  config_xml_values_set_execute(general_configuration_filename(), values);
}


bool GeneralConfiguration::bool_get(const gchar * key, bool & store, bool & loaded, bool standard)
{
  if (!loaded) {
    store = config_xml_bool_get(general_configuration_filename(), key, standard);
    loaded = true;
  }
  return store;
}


int GeneralConfiguration::int_get(const gchar * key, int &store, bool & loaded, int standard)
{
  if (!loaded) {
    store = config_xml_int_get(general_configuration_filename(), key, standard);
    loaded = true;
  }
  return store;
}


ustring GeneralConfiguration::string_get(const gchar * key, ustring & store, bool & loaded, const ustring & standard)
{
  if (!loaded) {
    store = config_xml_string_get(general_configuration_filename(), key, standard);
    loaded = true;
  }
  return store;
}


double GeneralConfiguration::double_get(const gchar * key, double &store, bool & loaded, double standard)
{
  if (!loaded) {
    store = config_xml_double_get(general_configuration_filename(), key, standard);
    loaded = true;
  }
  return store;
}


vector < bool > GeneralConfiguration::vector_bool_get(const gchar * key, vector < bool > &store, bool & loaded, void *dummy)
{
  if (!loaded) {
    store = config_xml_vector_bool_get(general_configuration_filename(), key);
    loaded = true;
  }
  return store;
}


vector < ustring > GeneralConfiguration::vector_string_get(const gchar * key, vector < ustring > &store, bool & loaded, void *dummy)
{
  if (!loaded) {
    store = config_xml_vector_string_get(general_configuration_filename(), key);
    loaded = true;
  }
  return store;
}


vector < int >GeneralConfiguration::vector_int_get(const gchar * key, vector < int >&store, bool & loaded, void *dummy)
{
  if (!loaded) {
    store = config_xml_vector_int_get(general_configuration_filename(), key);
    loaded = true;
  }
  return store;
}


vector < double >GeneralConfiguration::vector_double_get(const gchar * key, vector < double >&store, bool & loaded, void *dummy)
{
  if (!loaded) {
    store = config_xml_vector_double_get(general_configuration_filename(), key);
    loaded = true;
  }
  return store;
}


// Definitions of the implementation of the code in the general configuration.
#define IMPLEMENT(type, getter, store, defaultvalue) \
const gchar * GeneralConfiguration::store##_key () \
{ \
  return #store; \
} \
type GeneralConfiguration::store##_get () \
{ \
  return getter (store##_key (), store, store##_loaded, defaultvalue); \
} \
void GeneralConfiguration::store##_set (type value) \
{ \
  store = value; \
}

// Code to make everything work.
IMPLEMENT(int, int_get, screen_width, 0)
IMPLEMENT(int, int_get, screen_height, 0)
IMPLEMENT (int, int_get, window_width, 0)
IMPLEMENT (int, int_get, window_height, 0)
IMPLEMENT (int, int_get, window_x_position, 0)
IMPLEMENT (int, int_get, window_y_position, 0)
IMPLEMENT (bool, bool_get, window_maximized, true)
IMPLEMENT(vector < int >, vector_int_get, window_widths, NULL)
IMPLEMENT(vector < int >, vector_int_get, window_heights, NULL)
IMPLEMENT(vector < int >, vector_int_get, window_x_positions, NULL)
IMPLEMENT(vector < int >, vector_int_get, window_y_positions, NULL)
IMPLEMENT(vector < int >, vector_int_get, window_ids, NULL)
IMPLEMENT(vector < ustring >, vector_string_get, window_titles, NULL)
IMPLEMENT(vector < bool >, vector_bool_get, window_shows, NULL)
IMPLEMENT(ustring, string_get, edit_note_standard_text_one, "Exegetical problem: ")
IMPLEMENT(ustring, string_get, edit_note_standard_text_two, "Checking question: ")
IMPLEMENT(ustring, string_get, edit_note_standard_text_three, "Checking feedback: ")
IMPLEMENT(ustring, string_get, edit_note_standard_text_four, "PROBLEM: ")
IMPLEMENT(ustring, string_get, project, "")
IMPLEMENT(int, int_get, book, 0)
IMPLEMENT(ustring, string_get, chapter, "")
IMPLEMENT(ustring, string_get, verse, "")
IMPLEMENT(ustring, string_get, stylesheet, "")
IMPLEMENT(ustring, string_get, references_file, "")
IMPLEMENT(ustring, string_get, export_to_bibleworks_filename, "")
IMPLEMENT(ustring, string_get, export_to_sword_module_path, g_get_home_dir())
IMPLEMENT(ustring, string_get, export_to_sword_install_path, gw_build_filename(g_get_home_dir(), ".sword"))
IMPLEMENT(ustring, string_get, paper_format, "A4")
IMPLEMENT(double, double_get, paper_width, 21.0)
IMPLEMENT(double, double_get, paper_height, 29.7)
IMPLEMENT(double, double_get, paper_inside_margin, 2)
IMPLEMENT(double, double_get, paper_outside_margin, 1)
IMPLEMENT(double, double_get, paper_top_margin, 2)
IMPLEMENT(double, double_get, paper_bottom_margin, 2)
IMPLEMENT(bool, bool_get, printdate, true)
IMPLEMENT(double, double_get, header_font_size, 12)
IMPLEMENT(bool, bool_get, print_engine_use_intermediate_text, false)
IMPLEMENT(bool, bool_get, print_changes_only, false)
IMPLEMENT(ustring, string_get, project_to_compare_with, "")
IMPLEMENT(int, int_get, notes_selection_reference, nsrtCurrentVerse)
IMPLEMENT(int, int_get, notes_selection_edited, nsetAny)
IMPLEMENT(int, int_get, notes_selection_date_from, date_time_julian_day_get_current())
IMPLEMENT(int, int_get, notes_selection_date_to, date_time_julian_day_get_current())
IMPLEMENT(ustring, string_get, notes_selection_category, "")
IMPLEMENT(bool, bool_get, notes_selection_current_project, false)
IMPLEMENT(bool, bool_get, notes_display_project, false)
IMPLEMENT(bool, bool_get, notes_display_category, false)
IMPLEMENT(bool, bool_get, notes_display_date_created, false)
IMPLEMENT(bool, bool_get, notes_display_created_by, false)
IMPLEMENT(bool, bool_get, notes_display_summary, false)
IMPLEMENT(bool, bool_get, notes_display_reference_text, false)
IMPLEMENT(ustring, string_get, check_markers_compare_project, "")
IMPLEMENT(bool, bool_get, check_markers_compare_all_markers, true)
IMPLEMENT(ustring, string_get, check_markers_compare_include_only, "")
IMPLEMENT(ustring, string_get, check_markers_compare_ignore, "")
IMPLEMENT(bool, bool_get, check_markers_compare_ignore_verse_zero, false)
IMPLEMENT(ustring, string_get, check_capitalization_punctuation, CAPITALIZATION_PUNCTUATION)
IMPLEMENT(ustring, string_get, check_capitalization_ignore, CAPITALIZATION_LOWERCASE)
IMPLEMENT(bool, bool_get, check_capitalization_allow_any_prefixes, false)
IMPLEMENT(bool, bool_get, check_repetition_ignore_case, false)
IMPLEMENT(bool, bool_get, check_repetition_show_only_these, false)
IMPLEMENT(bool, bool_get, check_repetition_ignore_these, false)
IMPLEMENT(ustring, string_get, check_matching_pairs_ignore, "")
IMPLEMENT(int, int_get, check_words_inventory_not_include_words_count, 0)
IMPLEMENT(ustring, string_get, check_words_inventory_word_forming_characters, "")
IMPLEMENT(ustring, string_get, check_markers_spacing_include, "xo")
IMPLEMENT(vector < bool >, vector_bool_get, styles_category_expanded, NULL)
IMPLEMENT(ustring, string_get, insert_footnote_template, "")
IMPLEMENT(ustring, string_get, insert_endnote_template, "")
IMPLEMENT(ustring, string_get, insert_xref_template, "")
IMPLEMENT(bool, bool_get, parallel_bible_keep_verses_together, true)
IMPLEMENT(ustring, string_get, parallel_bible_chapters_verses, "")
IMPLEMENT(bool, bool_get, parallel_bible_include_verse_zero, false)
IMPLEMENT(vector < ustring >, vector_string_get, printing_fonts, NULL)
IMPLEMENT(vector < ustring >, vector_string_get, parallel_bible_projects, NULL)
IMPLEMENT(vector < bool >, vector_bool_get, parallel_bible_enabled, NULL)
IMPLEMENT(bool, bool_get, use_outpost, false)
IMPLEMENT(ustring, string_get, mychecks, "")
IMPLEMENT(bool, bool_get, tidy_translate, false)
IMPLEMENT(vector < int >, vector_int_get, tidy_books, NULL)
IMPLEMENT(vector < ustring >, vector_string_get, tidy_texts, NULL)
IMPLEMENT(bool, bool_get, tidy_normalize_hyphens, false)
IMPLEMENT(bool, bool_get, tidy_space_between_chapter_verse, false)
IMPLEMENT(bool, bool_get, tidy_space_series_verses, false)
IMPLEMENT(bool, bool_get, tidy_full_stop_ends_text, false)
IMPLEMENT(bool, bool_get, tidy_ampersand_semicolon, false)
IMPLEMENT(bool, bool_get, tidy_space_before_punctuation, false)
IMPLEMENT(bool, bool_get, wordlist_process_general, false)
IMPLEMENT(bool, bool_get, wordlist_general_asterisk, false)
IMPLEMENT(bool, bool_get, wordlist_general_asterisk_first, false)
IMPLEMENT(bool, bool_get, wordlist_process_hebrew, false)
IMPLEMENT(bool, bool_get, wordlist_hebrew_asterisk, false)
IMPLEMENT(bool, bool_get, wordlist_hebrew_asterisk_first, false)
IMPLEMENT(bool, bool_get, wordlist_process_greek, false)
IMPLEMENT(bool, bool_get, wordlist_greek_asterisk, false)
IMPLEMENT(bool, bool_get, wordlist_greek_asterisk_first, false)
IMPLEMENT(bool, bool_get, wordlist_process_index, false)
IMPLEMENT(bool, bool_get, wordlist_index_asterisk, false)
IMPLEMENT(bool, bool_get, wordlist_index_asterisk_first, false)
IMPLEMENT(bool, bool_get, text_editor_font_default, true)
IMPLEMENT(ustring, string_get, text_editor_font_name, "Sans 14")
IMPLEMENT(bool, bool_get, text_editor_default_color, true)
IMPLEMENT(int, int_get, text_editor_normal_text_color, 0)
IMPLEMENT(int, int_get, text_editor_background_color, 16777215)
IMPLEMENT(int, int_get, text_editor_selected_text_color, 16777215)
IMPLEMENT(int, int_get, text_editor_selection_color, 4294343)
IMPLEMENT(ustring, string_get, encoding, "WINDOWS-1258")
IMPLEMENT(int, int_get, features_mode, 1)
IMPLEMENT(ustring, string_get, features_list, "")
IMPLEMENT(bool, bool_get, remember_verse_per_chapter, false) 
IMPLEMENT(ustring, string_get, administration_password, "")
IMPLEMENT(vector < ustring >, vector_string_get, print_references_projects, NULL)
IMPLEMENT(vector < int >, vector_int_get, dialogpositions_x, NULL)
IMPLEMENT(vector < int >, vector_int_get, dialogpositions_y, NULL)
IMPLEMENT(bool, bool_get, text_replacement, false)
IMPLEMENT(vector < ustring >, vector_string_get, text_replacement_originals, NULL)
IMPLEMENT(vector < ustring >, vector_string_get, text_replacement_replacements, NULL)
IMPLEMENT(bool, bool_get, pdf_viewer_automatic, true)
IMPLEMENT(ustring, string_get, pdf_viewer_path, "") 
IMPLEMENT(ustring, string_get, pdf_viewer_arguments, "") 
IMPLEMENT(vector < ustring >, vector_string_get, project_tasks_names, NULL) 
IMPLEMENT(vector < double >, vector_double_get, project_tasks_durations, NULL) 
IMPLEMENT(int, int_get, print_job, 0) 
IMPLEMENT(vector < ustring >, vector_string_get, projects_displaying_verses, NULL)
IMPLEMENT(bool, bool_get, compare_disregard_notes, false)
IMPLEMENT(vector <ustring>, vector_string_get, source_language_names, NULL)
IMPLEMENT(bool, bool_get, reference_window_show_verse_text, true)
IMPLEMENT(bool, bool_get, reference_window_show_relevant_bits, false)
IMPLEMENT(bool, bool_get, consultation_notes_git_use_remote_repository, false)
IMPLEMENT(ustring, string_get, consultation_notes_git_remote_repository_url, "")
IMPLEMENT(int, int_get, consultation_notes_git_remote_repository_conflict_handling, 0)
IMPLEMENT(ustring, string_get, bibledit_web_url, "http://localhost/bibledit")
IMPLEMENT(ustring, string_get, bibledit_web_user, "")
