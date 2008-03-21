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



#include "oldgenconfig.h"
#include "directories.h"
#include "gwrappers.h"
#include "sqlite3.h"
#include "sqlite_reader.h"
#include "utilities.h"
#include <libxml/xmlwriter.h>
#include "tiny_utilities.h"


ustring old_configuration_filename ()
{
  return gw_build_filename (directories_get_configuration (), "configuration.sqlite.2");
}


ustring new_configuration_filename ()
{
  return gw_build_filename (directories_get_configuration (), "configuration.1.xml");
}


vector<ustring> old_valuelist_get (const char * key)
{
  vector<ustring> value;
  vector<unsigned int> sequence;
  sqlite3 *db;
  sqlite3_open (old_configuration_filename ().c_str (), &db);
  sqlite3_busy_timeout (db, 1000);
  SqliteReader sqlitereader (0);
  char * sql = g_strdup_printf ("select value, sequence from data where key = '%s';", key);
  sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, NULL);
  g_free (sql);
  value.assign (sqlitereader.ustring0.begin(), sqlitereader.ustring0.end());
  for (unsigned int i = 0; i < sqlitereader.ustring1.size(); i++) {
    sequence.push_back (convert_to_int (sqlitereader.ustring1[i]));
  }
  quick_sort (sequence, value, 0, sequence.size());
  sqlite3_close (db);
  return value;
}


ustring new_config_element (char * name) 
{
  ustring element (name);
  replace_text (element, "_", "-");
  return element;
}


typedef struct {
  char *key;   // The key, as it is stored in the old configuration database.
  char *name;  // The name, as it will be used in the new code.
} old_config_record;


typeof (old_config_record) old_config_table [] = {
 { "sync_on_open", "synchronize_project_on_open" },
 { "sync_on_close", "synchronize_project_on_close" },
 { "sync_regularly", "synchronize_project_regularly" },
 { "sync_minutes", "synchronize_project_minutes" },
 { "sync_startup", "synchronize_on_startup" },
 { "sync_shutdown", "synchronize_on_shutdown" },
 { "startup_time", "startup_time" },
 { "screen_width", "screen_width" },
 { "screen_height", "screen_height" },
 { "window_maximized", "window_maximized" },
 { "window_width", "window_width" },
 { "window_height", "window_height" },
 { "window_x_pos", "window_x_position" },
 { "window_y_pos", "window_y_position" },
 { "hpane_pos", "hpane_position" },
 { "vpane_edit_pos", "vpane_editor_position" },
 { "vpane_qref_pos", "vpane_quick_references_position" },
 { "tools_area_left", "tools_area_left" },
 { "note_std_txt1", "edit_note_standard_text_one" },
 { "note_std_txt2", "edit_note_standard_text_two" },
 { "note_std_txt3", "edit_note_standard_text_three" },
 { "note_std_txt4", "edit_note_standard_text_four" },
 { "projects", "projects" },
 { "project", "project" },
 { "book", "book" },
 { "chapter", "chapter" },
 { "verse", "verse" },
 { "stylesheet", "stylesheet" },
 { "ref_file", "references_file" },
 { "exp_bw_file", "export_to_bibleworks_filename" },
 { "exp_sword_mod", "export_to_sword_module_path" },
 { "exp_sword_inst", "export_to_sword_install_path" },
 { "paper_format", "paper_format" },
 { "paper_width", "paper_width" },
 { "paper_height", "paper_height" },
 { "paper_l_margin", "paper_left_margin" },
 { "paper_r_margin", "paper_right_margin" },
 { "paper_t_margin", "paper_top_margin" },
 { "paper_b_margin", "paper_bottom_margin" },
 { "print_note_full_width", "notes_print_full_page_width" },
 { "java_bin", "java_binary" },
 { "xep_home", "xep_home" },
 { "print_date", "printdate" },
 { "print_changes_only", "print_changes_only" },
 { "project_compare", "project_to_compare_with" },
 { "show_note_select", "show_notes_selector" },
 { "show_note_curr_proj_only", "show_notes_for_current_project_only" },
 { "note_show_proj", "notes_show_project" },
 { "sort_notes_round_ref", "notes_sort_round_ref" },
 { "notes_ref_text", "notes_show_reference_text" },
 { "show_note_cat", "show_notes_category" },
 { "show_note_date_from", "show_notes_date_range_from" },
 { "show_note_date_to", "show_notes_date_range_to" },
 { "notes_show_summary", "notes_show_summary" },
 { "bw_exe", "bibleworks_executable" },
 { "ref_to_bw", "reference_exchange_send_to_bibleworks" },
 { "ref_from_bw", "reference_exchange_receive_from_bibleworks" },
 { "ref_to_bt", "reference_exchange_send_to_bibletime" },
 { "ref_from_bt", "reference_exchange_receive_from_bibletime" },
 { "ref_to_sff", "reference_exchange_send_to_santafefocus" },
 { "ref_from_sff", "reference_exchange_receive_from_santafefocus" },
 { "ref_to_brv", "reference_exchange_send_to_bibledit_resource_viewer" },
 { "chk_mark_comp_with", "check_markers_compare_project" },
 { "chk_mark_comp_all", "check_markers_compare_all_markers" },
 { "chk_mark_comp_only", "check_markers_compare_include_only" },
 { "chk_mark_comp_ign", "check_markers_compare_ignore" },
 { "chk_mark_ign_v0", "check_markers_compare_ignore_verse_zero" },
 { "chk_cap_punct", "check_capitalization_punctuation" },
 { "chk_cap_ign", "check_capitalization_ignore" },
 { "chk_cap_any_pref", "check_capitalization_allow_any_prefixes" },
 { "chk_repeat_ign_case", "check_repetition_ignore_case" },
 { "chk_repeat_only_these", "check_repetition_show_only_these" },
 { "chk_repeat_ign_these", "check_repetition_ignore_these" },
 { "chk_pairs_ign", "check_matching_pairs_ignore" },
 { "chk_wrd_inv_excl_count", "check_words_inventory_not_include_words_count" },
 { "chk_wrd_extra_chars", "check_words_inventory_word_forming_characters" },
 { "chk_mark_spac", "check_markers_spacing_include" },
 { "tools_area_page", "tools_area_page_number" },
 { "style_cat_exp", "styles_category_expanded" },
 { "ins_footnote_templ", "insert_footnote_template" },
 { "ins_endnote_templ", "insert_endnote_template" },
 { "ins_xref_templ", "insert_xref_template" },
 { "par_bible_keep_vs", "parallel_bible_keep_verses_together" },
 { "par_bible_ch_vs", "parallel_bible_chapters_verses" },
 { "par_bible_vs0", "parallel_bible_include_verse_zero" },
 { "print_fonts2", "printing_fonts" },
 { "par_bible_prj", "parallel_bible_projects" },
 { "use_outpost", "use_outpost" },
 { "outpost_networked", "outpost_networked" },
 { "outpost_host", "outpost_host" },
 { "outpost_path", "outpost_path" },
 { "wine_path", "wine_path" },
 { "outpost_command", "outpost_command" },
 { "mychecks", "mychecks" },
 { "tidy_translate", "tidy_translate" },
 { "tidy_books", "tidy_books" },
 { "tidy_texts", "tidy_texts" },
 { "tidy_n_hyphens", "tidy_normalize_hyphens" },
 { "tidy_space_btw_ch_vs", "tidy_space_between_chapter_verse" },
 { "tidy_space_verses", "tidy_space_series_verses" },
 { "tidy_end_full_stop", "tidy_full_stop_ends_text" },
 { "tidy_amp_sem", "tidy_ampersand_semicolon" },
 { "tidy_rem_sp_punct", "tidy_space_before_punctuation" },
 { "wl_gen", "wordlist_process_general" },
 { "wl_gen_asterisk", "wordlist_general_asterisk" },
 { "wl_gen_asterisk_first", "wordlist_general_asterisk_first" },
 { "wl_heb", "wordlist_process_hebrew" },
 { "wl_heb_asterisk", "wordlist_hebrew_asterisk" },
 { "wl_heb_asterisk_first", "wordlist_hebrew_asterisk_first" },
 { "wl_grk", "wordlist_process_greek" },
 { "wl_grk_asterisk", "wordlist_greek_asterisk" },
 { "wl_grk_asterisk_first", "wordlist_greek_asterisk_first" },
 { "notes_font_default", "notes_editor_font_default" },
 { "notes_font", "notes_editor_font_name" },
 { "notes_color_default", "notes_editor_default_color" },
 { "notes_normal_text_color", "notes_editor_normal_text_color" },
 { "notes_background_color", "notes_editor_background_color" },
 { "notes_selected_text_color", "notes_editor_selected_text_color" },
 { "notes_selection_color", "notes_editor_selection_color" },
 { "edit_font_default", "text_editor_font_default" },
 { "edit_font", "text_editor_font_name" },
 { "edit_color_default", "text_editor_default_color" },
 { "edit_normal_text_color", "text_editor_normal_text_color" },
 { "edit_background_color", "text_editor_background_color" },
 { "edit_selected_text_color", "text_editor_selected_text_color" },
 { "edit_selection_color", "text_editor_selection_color" },
 { "encoding", "encoding" }
};


void upgrade_old_general_configuration ()
{
  // No old configuration to upgrade: bail out.
  if (!g_file_test (old_configuration_filename ().c_str(), G_FILE_TEST_IS_REGULAR)) return;
  
  cout << "Upgrading general configuration" << endl;
  
  // Get a list of all distinct keys stored in the configuration.
  vector <ustring> keys;
  {
    sqlite3 *db;
    sqlite3_open (old_configuration_filename ().c_str (), &db);
    sqlite3_busy_timeout (db, 1000);
    SqliteReader sqlitereader (0);
    char * sql = g_strdup_printf ("select distinct key from data");
    sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, NULL);
    g_free (sql);
    keys = sqlitereader.ustring0;
    sqlite3_close (db);
  }
  
  // Create a new XML buffer, to which the document will be written
  xmlBufferPtr buffer = xmlBufferCreate();
  
  // Create a new XmlWriter for memory, with no compression.
  xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);

  // Create first bit of the document.
  xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
  
  // Indent the output.
  xmlTextWriterSetIndent(writer, 1);
    
  // Write the main element.
  xmlTextWriterStartElement (writer, BAD_CAST "bibledit-general-configuration");
    
  // Get the value of all the keys, and write them to the document.
  for (unsigned int i = 0; i < keys.size(); i++) {
    ustring key = keys[i];
    ustring name;
    for (unsigned int i2 = 0; i2 < sizeof (old_config_table) / sizeof (*old_config_table); i2++) {
      if (key == old_config_table[i2].key) {
        name = new_config_element (old_config_table[i2].name);
      }
    }
    if (name.empty()) continue;
    xmlTextWriterStartElement (writer, BAD_CAST name.c_str());
    vector <ustring> values = old_valuelist_get (key.c_str());
    for (unsigned int i2 = 0; i2 < values.size(); i2++) {
      xmlTextWriterStartElement (writer, BAD_CAST "value");
      xmlTextWriterWriteFormatString (writer, "%s", values[i2].c_str());
      xmlTextWriterEndElement (writer);      
    }
    xmlTextWriterEndElement (writer);
  }
  
  // We could close any open elements by hand, but closing the document does it all.
  xmlTextWriterEndDocument (writer);
  
  // Flush the writer, else nothing gets transferred to the buffer.
  xmlTextWriterFlush (writer);

  // Write the lines to disk.
  ParseLine parseline ((const char *) buffer->content);
  write_lines (new_configuration_filename (), parseline.lines);

  // Free memory.
  if (writer) xmlFreeTextWriter (writer);
  if (buffer) xmlBufferFree (buffer);

  // Remove old configuration file.
  unlink (old_configuration_filename ().c_str());
}
