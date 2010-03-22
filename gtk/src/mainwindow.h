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


#ifndef INCLUDED_MAINWINDOW_H
#define INCLUDED_MAINWINDOW_H


#include <gtk/gtk.h>
#include "windowsoutpost.h"
#include "ustring.h"
#include "usfm.h"
#include "note_editor.h"
#include "editor.h"
#include "httpd.h"
#include "reference.h"
#include "gui_navigation.h"
#include "displayprojectnotes.h"
#include "git.h"
#include "windowshowrelatedverses.h"
#include "windowmerge.h"
#include "windowresource.h"
#include "windowoutline.h"
#include "windowcheckkeyterms.h"
#include "windowstyles.h"
#include "windownotes.h"
#include "windowreferences.h"
#include "windoweditor.h"
#include "assistantimportkeyterms.h"
#include "assistantdeletekeyterms.h"
#include "assistantchanges.h"
#include "windowcheckusfm.h"
#include "assistantremoterepository.h"
#include "assistantresource.h"
#include "assistantbackup.h"
#include "assistantrestore.h"
#include "assistantexport.h"
#include "assistantimport.h"
#include "floatingwindow.h"
#include "urltransport.h"


class MainWindow
{
public:
  MainWindow(unsigned long xembed, GtkAccelGroup *accelerator_group);
  ~MainWindow();
  int run();
protected:
  /* IMPORTANT If more menu items are added see whether they should be made 
   * in(sensitive) depending on the context whether something has been opened. */
  GtkWidget *window_main;
  GtkWidget *vbox_main;
  GtkWidget *menubar1;
  GtkWidget *menuitem_file;
  GtkWidget *menuitem_file_menu;
  GtkWidget *file_project;
  GtkWidget *image463;
  GtkWidget *file_project_menu;
  GtkWidget *new1;
  GtkWidget *image903;
  GtkWidget *open1;
  GtkWidget *delete1;
  GtkWidget *properties1;
  GtkWidget *image4995;
  GtkWidget *copy_project_to;
  GtkWidget *image2688;
  GtkWidget *compare_with1;
  GtkWidget *image2764;
  GtkWidget *project_changes;
  GtkWidget *image19115;
  GtkWidget *file_projects_merge;
  GtkWidget *projects_send_receive1;
  GtkWidget *image34440;
  GtkWidget *style;
  GtkWidget *image10735;
  GtkWidget *style_menu;
  GtkWidget *stylesheet_open;
  GtkWidget *image31346;
  GtkWidget *stylesheets_expand_all;
  GtkWidget *stylesheets_collapse_all;
  GtkWidget *style_insert;
  GtkWidget *stylesheet_edit_mode;
  GtkWidget *style_new;
  GtkWidget *style_properties;
  GtkWidget *style_delete;
  GtkWidget *menu_stylesheet;
  GtkWidget *menu_stylesheet_menu;
  GtkWidget *stylesheet_switch;
  GtkWidget *stylesheets_new;
  GtkWidget *stylesheets_delete;
  GtkWidget *stylesheets_rename;
  GtkWidget *notes2;
  GtkWidget *image936;
  GtkWidget *notes2_menu;
  GtkWidget *new_note;
  GtkWidget *delete_note;
  GtkWidget *image963;
  GtkWidget *consultation_notes_send_receive;
  GtkWidget *file_resources;
  GtkWidget *image27365;
  GtkWidget *file_resources_menu;
  GtkWidget *file_resources_open;
  GtkWidget *image27366;
  GtkWidget *file_resources_close;
  GtkWidget *image27367;
  GtkWidget *file_resources_new;
  GtkWidget *image27514;
  GtkWidget *file_resources_edit;
  GtkWidget *image27515;
  GtkWidget *file_resources_delete;
  GtkWidget *image27664;
  GtkWidget *file_keyterms;
  GtkWidget *image32231;
  GtkWidget *file_keyterms_menu;
  GtkWidget *keyterms_delete;
  GtkWidget *print;
  GtkWidget *file_import;
  GtkWidget *image36797;
  GtkWidget *file_export;
  GtkWidget *image35236;
  GtkWidget *file_backup;
  GtkWidget *image34724;
  GtkWidget *file_restore;
  GtkWidget *image34980;
  GtkWidget *quit1;
  GtkWidget *menuitem_edit;
  GtkWidget *menuitem_edit_menu;
  GtkWidget *cut1;
  GtkWidget *copy1;
  GtkWidget *copy_with_formatting;
  GtkWidget *image18220;
  GtkWidget *paste1;
  GtkWidget *separator2;
  GtkWidget *undo1;
  GtkWidget *image295;
  GtkWidget *redo1;
  GtkWidget *image296;
  GtkWidget *separator4;
  GtkWidget *find1;
  GtkWidget *find_and_replace1;
  GtkWidget *find_in_notes1;
  GtkWidget *image1430;
  GtkWidget *separator7;
  GtkWidget *get_references_from_note;
  GtkWidget *image3158;
  GtkWidget *separator15;
  GtkWidget *edit_revert;
  GtkWidget *image19262;
  GtkWidget *separator17;
  GtkWidget *edit_bible_note;
  GtkWidget *image20483;
  GtkWidget *separator21;
  GtkWidget *edit_planning;
  GtkWidget *image26801;
  GtkWidget *menuitem_view;
  GtkWidget *menuitem_view_menu;
  GtkWidget *view_font;
  GtkWidget *image20234;
  GtkWidget *view_font_menu;
  GtkWidget *view_text_font;
  GtkWidget *image20235;
  GtkWidget *viewnotes;
  GtkWidget *image2627;
  GtkWidget *view_usfm_code;
  GtkWidget *view_planning;
  GtkWidget *image26812;
  GtkWidget *view_references;
  GtkWidget *view_related_verses;
  GtkWidget *view_outline;
  GtkWidget *insert1;
  GtkWidget *insert1_menu;
  GtkWidget *standard_text_1;
  GtkWidget *image1963;
  GtkWidget *standard_text_2;
  GtkWidget *image1964;
  GtkWidget *standard_text_3;
  GtkWidget *image1965;
  GtkWidget *standard_text_4;
  GtkWidget *image1966;
  GtkWidget *separator9;
  GtkWidget *current_reference1;
  GtkWidget *image3797;
  GtkWidget *separator20;
  GtkWidget *insert_special_character;
  GtkWidget *image25281;
  GtkWidget *check1;
  GtkWidget *check1_menu;
  GtkWidget *chapters_and_verses1;
  GtkWidget *image5580;
  GtkWidget *markers1;
  GtkWidget *image5578;
  GtkWidget *markers1_menu;
  GtkWidget *validate_usfms1;
  GtkWidget *image5579;
  GtkWidget *count_usfms1;
  GtkWidget *image6239;
  GtkWidget *compare_usfm1;
  GtkWidget *image6748;
  GtkWidget *check_markers_spacing;
  GtkWidget *image17930;
  GtkWidget *characters1;
  GtkWidget *image6867;
  GtkWidget *characters1_menu;
  GtkWidget *count_characters;
  GtkWidget *image6868;
  GtkWidget *unwanted_patterns;
  GtkWidget *image7494;
  GtkWidget *check_words;
  GtkWidget *image7111;
  GtkWidget *check_words_menu;
  GtkWidget *check_capitalization;
  GtkWidget *image7112;
  GtkWidget *check_repetition;
  GtkWidget *image7238;
  GtkWidget *unwanted_words;
  GtkWidget *image7631;
  GtkWidget *word_count_inventory;
  GtkWidget *image13715;
  GtkWidget *check_punctuation;
  GtkWidget *image7366;
  GtkWidget *check_punctuation_menu;
  GtkWidget *check_matching_pairs;
  GtkWidget *image7367;
  GtkWidget *check_sentence_structure;
  GtkWidget *image28475;
  GtkWidget *check_references;
  GtkWidget *image21826;
  GtkWidget *check_references_menu;
  GtkWidget *check_references_inventory;
  GtkWidget *image21827;
  GtkWidget *check_references_validate;
  GtkWidget *image21828;
  GtkWidget *checks_passages;
  GtkWidget *image24103;
  GtkWidget *checks_passages_menu;
  GtkWidget *check_nt_quotations_from_the_ot;
  GtkWidget *image24104;
  GtkWidget *synoptic_parallel_passages_from_the_nt;
  GtkWidget *image24105;
  GtkWidget *parallels_from_the_ot;
  GtkWidget *image24106;
  GtkWidget *check_key_terms;
  GtkWidget *my_checks;
  GtkWidget *image15438;
  GtkWidget *check_usfm;
  GtkWidget *check_spelling_error;
  GtkWidget *image34138;
  GtkWidget *check_spelling_error_menu;
  GtkWidget *check_spelling_error_next;
  GtkWidget *image34139;
  GtkWidget *check_spelling_error_previous;
  GtkWidget *image34140;
  GtkWidget *check_spelling_bulk;
  GtkWidget *image35887;
  GtkWidget *menutools;
  GtkWidget *menutools_menu;
  GtkWidget *notes_transfer;
  GtkWidget *image14659;
  GtkWidget *tool_origin_references_in_bible_notes;
  GtkWidget *image16248;
  GtkWidget *tool_project_notes_mass_update1;
  GtkWidget *image17187;
  GtkWidget *tool_generate_word_lists;
  GtkWidget *image20671;
  GtkWidget *tool_simple_text_corrections;
  GtkWidget *image21054;
  GtkWidget *tool_transfer_project_notes_to_text;
  GtkWidget *image29089;
  GtkWidget *tool_go_to_reference;
  GtkWidget *image36137;
  GtkWidget *tools_maintenance;
  GtkWidget *image36259;
  GtkWidget *tool_send_reference;
  GtkWidget *image37446;
  GtkWidget *tools_receive_reference;
  GtkWidget *menuitem_preferences;
  GtkWidget *menuitem_preferences_menu;
  GtkWidget *notes_preferences;
  GtkWidget *image2116;
  GtkWidget *printingprefs;
  GtkWidget *image3493;
  GtkWidget *reference_exchange1;
  GtkWidget *image5972;
  GtkWidget *prefs_books;
  GtkWidget *image12167;
  GtkWidget *preferences_windows_outpost;
  GtkWidget *image14287;
  GtkWidget *preferences_tidy_text;
  GtkWidget *image16359;
  GtkWidget *preferences_remote_repository;
  GtkWidget *image33654;
  GtkWidget *preferences_gui;
  GtkWidget *image20936;
  GtkWidget *preferences_password;
  GtkWidget *image20937;
  GtkWidget *preferences_text_replacement;
  GtkWidget *image23181;
  GtkWidget *pdf_viewer1;
  GtkWidget *image24540;
  GtkWidget *preferences_planning;
  GtkWidget *image26888;
  GtkWidget *preferences_filters;
  GtkWidget *image28360;
  GtkWidget *preferences_compare;
  GtkWidget *image32676;
  GtkWidget *menuitem_help;
  GtkWidget *menuitem_help_menu;
  GtkWidget *help_main;
  GtkWidget *image17520;
  GtkWidget *system_log1;
  GtkWidget *image4388;
  GtkWidget *about1;
  GtkWidget *toolbar;
  GtkWidget *scrolledwindow_layout;
  GtkWidget *layout;

  // Navigator object.
  GuiNavigation navigation;

  /* Initialization */
  void enable_or_disable_widgets(bool enable);

  /* Menu window */
  static void on_open1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void open();
  static void on_new1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void newproject();
  static void on_properties1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void editproject();
  static void on_delete1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void deleteproject();
  static void on_quit1_activate(GtkMenuItem * menuitem, gpointer user_data);
  static void on_system_log1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void viewlog();
  static void on_help_main_activate(GtkMenuItem * menuitem, gpointer user_data);
  void on_help_main();
  static void on_about1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void showabout();
  static void on_undo1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void menu_accelerator_undo();
  static void on_redo1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void menu_accelerator_redo();
  static void on_edit1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void menu_edit();
  static void on_find_and_replace1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void menu_replace();
  static void on_findspecial1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void menu_findspecial();
  static void on_insert1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_menu_insert();
  static void on_notes_preferences_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_notes_preferences();
  static void on_copy_project_to_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_copy_project_to();
  static void on_compare_with1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_compare_with();
  static void on_printingprefs_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_printing_preferences();
  static void on_prefs_books_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_prefs_books();
  static void on_preferences_tidy_text_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_preferences_tidy_text();
  static void on_tool_generate_word_lists_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_tool_generate_word_lists();
  static void on_preferences_gui_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_preferences_gui();
  static void on_preferences_password_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_preferences_password();
  static void on_tool_simple_text_corrections_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_tool_simple_text_corrections();
  static void on_preferences_text_replacement_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_preferences_text_replacement();
  static void on_pdf_viewer1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_pdf_viewer();
  static void on_insert_special_character_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_insert_special_character();
  static void on_preferences_compare_activate (GtkMenuItem *menuitem, gpointer user_data);
  void on_preferences_compare ();
  
  // Navigation
  static void on_navigation_new_reference_clicked(GtkButton *button, gpointer user_data);
  void on_navigation_new_reference();
  void goto_next_verse();
  void goto_previous_verse();
  void goto_next_chapter();
  void goto_previous_chapter();
  void goto_next_book();
  void goto_previous_book();
  void goto_reference_interactive();
  static void on_new_verse_signalled(GtkButton *button, gpointer user_data);
  void on_editor_another_verse();
  void on_text_area_activate();
  void on_tools_area_activate();
  void on_notes_area_activate();
  static void on_tool_go_to_reference_activate (GtkMenuItem *menuitem, gpointer user_data);

  /* Clipboard */
  static void on_cut1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void on_cut();
  static void on_copy1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void on_copy();
  static void on_copy_with_formatting_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_copy_with_formatting();
  static void on_paste1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void on_paste();

  /* References */
  bool references_management_enabled;
  static void on_view_references_activate (GtkMenuItem *menuitem, gpointer user_data);
  void on_view_references ();
  void show_references_window();
  WindowReferences * window_references;
  static void on_window_references_delete_button_clicked(GtkButton *button, gpointer user_data);
  void on_window_references_delete_button();
  static void on_window_references_signal_button_clicked(GtkButton *button, gpointer user_data);
  void on_window_references_signal_button();
  void on_next_reference();
  void on_previous_reference();
  static void on_show_quick_references_signal_button_clicked(GtkButton * button, gpointer user_data);
  void on_show_quick_references_signal_button(GtkButton * button);

  // Bibledit Windows Outpost
  // BibleTime
  WindowsOutpost * windowsoutpost;
  static void on_reference_exchange1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_reference_exchange();
  static void on_send_word_to_toolbox_signalled(GtkButton *button, gpointer user_data);
  void send_word_to_toolbox();
  static void on_preferences_windows_outpost_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_preferences_windows_outpost();
  static void on_tool_send_reference_activate (GtkMenuItem *menuitem, gpointer user_data);
  void on_tool_send_reference ();
  static void on_tools_receive_reference_activate (GtkMenuItem *menuitem, gpointer user_data);
  void on_tools_receive_reference ();
  guint event_id_receive_reference;
  static bool on_tools_receive_reference_timeout(gpointer data);
  void tools_receive_reference_timeout();
  Reference * previously_received_reference;

  /* Project notes */
  bool project_notes_enabled;
  bool project_notes_management_enabled;
  void view_project_notes();
  WindowNotes * window_notes;
  static void on_window_notes_delete_button_clicked(GtkButton *button, gpointer user_data);
  void on_window_notes_delete_button();
  static void on_new_note_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_new_note();
  static void on_delete_note_activate(GtkMenuItem *menuitem, gpointer user_data);
  static void on_consultant_notes_send_receive_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_consultant_notes_send_receive ();
  static void on_viewnotes_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_view_notes();
  void notes_redisplay();
  static void on_find_in_notes1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void find_in_notes();
  void on_import_notes();
  static void on_standard_text_1_activate(GtkMenuItem *menuitem, gpointer user_data);
  static void on_standard_text_2_activate(GtkMenuItem *menuitem, gpointer user_data);
  static void on_standard_text_3_activate(GtkMenuItem *menuitem, gpointer user_data);
  static void on_standard_text_4_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_insert_standard_text(GtkMenuItem *menuitem);
  static void on_current_reference1_activate(GtkMenuItem *menuitem, gpointer user_data);
  static void on_menuitem_view_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_menuitem_view();
  static void on_get_references_from_note_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_get_references_from_note();
  static void on_window_notes_references_available_button_clicked(GtkButton *button, gpointer user_data);
  void on_window_notes_references_available_button();

  /* Export */
  static void on_file_export_activate (GtkMenuItem *menuitem, gpointer user_data);
  void on_file_export ();
  ExportAssistant * export_assistant;

  /* Checks */
  static void on_validate_usfms1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_menu_check_markers_validate();
  static void on_count_usfms1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_menu_check_markers_count();
  static void on_compare_usfm1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_menu_check_markers_compare();
  static void on_chapters_and_verses1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_menu_check_chapters_and_verses();
  static void on_count_characters_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_count_characters();
  static void on_unwanted_patterns_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_unwanted_patterns();
  static void on_check_capitalization_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_check_capitalization();
  static void on_check_repetition_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_check_repetition();
  static void on_check_matching_pairs_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_check_matching_pairs();
  static void on_unwanted_words_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_unwanted_words();
  static void on_word_count_inventory_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_word_count_inventory();
  static bool on_check_httpd_timeout(gpointer data);
  void on_check_httpd();
  static void on_my_checks_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_my_checks();
  static void on_check_markers_spacing_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_check_markers_spacing();
  static void on_check_references_inventory_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_check_references_inventory();
  static void on_check_references_validate_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_check_references_validate();
  static void on_check_nt_quotations_from_the_ot_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_check_nt_quotations_from_the_ot();
  static void on_synoptic_parallel_passages_from_the_nt_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_synoptic_parallel_passages_from_the_nt();
  static void on_parallels_from_the_ot_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_parallels_from_the_ot();
  static void on_check_sentence_structure_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_check_sentence_structure();
  static void on_check_spelling_error_next_activate (GtkMenuItem *menuitem, gpointer user_data);
  static void on_check_spelling_error_previous_activate (GtkMenuItem *menuitem, gpointer user_data);
  void on_check_spelling_error(bool next, bool extremity);
  static void on_editor_spelling_checked_button_clicked(GtkButton *button, gpointer user_data);
  void on_editor_spelling_checked_button();
  bool check_spelling_at_start;
  bool check_spelling_at_end;
  static void on_check_spelling_bulk_activate (GtkMenuItem *menuitem, gpointer user_data);
  void on_check_spelling_bulk ();

  /* Styles */
  static void on_stylesheet_open_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_stylesheet_open();
  void on_goto_styles_area();
  void display_window_styles();
  WindowStyles * window_styles;
  static void on_window_styles_delete_button_clicked(GtkButton *button, gpointer user_data);
  void on_window_styles_delete_button();
  void stylesheet_open_named(const ustring& stylesheet);
  static void on_style_button_open_clicked(GtkButton *button, gpointer user_data);
  void on_style_button_open();
  static void on_style_button_apply_clicked(GtkButton *button, gpointer user_data);
  void on_style_apply();
  static void on_editor_style_changed(GtkButton *button, gpointer user_data);
  void editor_style_changed();
  static void on_style_edited(GtkButton *button, gpointer user_data);
  void reload_styles();
  bool style_management_enabled;

  /* Footnotes, endnotes, crossreferences */
  static void on_edit_bible_note_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_edit_bible_note();

  /* Tools */
  static void on_menutools_activate(GtkMenuItem *menuitem, gpointer user_data);
  static void on_notes_transfer_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_notes_transfer();
  static void on_tool_origin_references_in_bible_notes_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_tool_origin_references_in_bible_notes();
  static void on_tool_project_notes_mass_update1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_tool_project_notes_mass_update();
  static void on_tool_transfer_project_notes_to_text_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_tool_transfer_project_notes_to_text();
  static void on_tools_maintenance_activate (GtkMenuItem *menuitem, gpointer user_data);
  void on_tools_maintenance ();

  /* Webserver */
  Httpd httpd;

  /* Keyterms */
  static void on_check_key_terms_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_check_key_terms();
  WindowCheckKeyterms * window_check_keyterms;
  static void on_window_check_keyterms_delete_button_clicked(GtkButton *button, gpointer user_data);
  void on_window_check_keyterms_delete_button();
  static void on_keyterms_signal_button_clicked(GtkButton *button, gpointer user_data);
  void keyterms_check_action();
  static void on_view_related_verses_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_view_related_verses();
  WindowShowRelatedVerses * window_show_related_verses;
  static void on_window_show_related_verses_delete_button_clicked(GtkButton *button, gpointer user_data);
  void on_window_show_related_verses_delete_button();
  static void on_window_show_related_verses_item_button_clicked(GtkButton *button, gpointer user_data);
  void on_window_show_related_verses_item_button();
  void on_keyterms_import ();
  ImportKeytermsAssistant * import_keyterms_assistant;
  static void on_keyterms_delete_activate (GtkMenuItem *menuitem, gpointer user_data);
  void on_keyterms_delete ();
  DeleteKeytermsAssistant * delete_keyterms_assistant;

  /* Backup and restore */
  static void on_file_backup_activate (GtkMenuItem *menuitem, gpointer user_data);
  void on_file_backup ();
  BackupAssistant * backup_assistant;
  static void on_file_restore_activate (GtkMenuItem *menuitem, gpointer user_data);
  void on_file_restore ();
  RestoreAssistant * restore_assistant;

  /* Git */
  static void on_preferences_remote_repository_activate (GtkMenuItem *menuitem, gpointer user_data);
  void on_preferences_remote_repository();  
  RemoteRepositoryAssistant * remote_repository_assistant;
  static void on_project_changes_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_project_changes();
  ChangesAssistant * changes_assistant;
  static void on_edit_revert_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_edit_revert();
  static void on_projects_send_receive1_activate (GtkMenuItem *menuitem, gpointer user_data);
  void on_projects_send_receive ();

  /* Fonts */
  static void on_view_text_font_activate(GtkMenuItem * menuitem, gpointer user_data);
  void on_text_font();
  void set_fonts();

  /* Outline */
  static void on_view_outline_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_view_outline();
  WindowOutline * window_outline;
  static void on_window_outline_delete_button_clicked(GtkButton *button, gpointer user_data);
  void on_window_outline_delete_button();
  static void on_button_outline_clicked(GtkButton *button, gpointer user_data);
  void on_button_outline();

  /* Planning */
  static void on_edit_planning_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_edit_planning();
  static void on_view_planning_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_view_planning();
  static void on_preferences_planning_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_preferences_planning();

  /* Resources */
  vector <WindowResource *> resource_windows;
  static void on_window_resource_delete_button_clicked(GtkButton *button, gpointer user_data);
  void on_window_resource_delete_button(GtkButton *button);
  WindowResource * last_focused_resource_window();
  static void on_file_resources_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_file_resources();
  static void on_file_resources_open_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_file_resources_open(ustring resource, bool startup);
  static void on_file_resources_close_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_file_resources_close();
  static void on_file_resources_new_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_file_resources_new();
  static void on_file_resources_edit_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_file_resources_edit();
  static void on_file_resources_delete_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_file_resources_delete();
  ResourceAssistant * resource_assistant;

  /* Text Editors */
  vector <WindowEditor *> editor_windows;
  static void on_window_editor_delete_button_clicked(GtkButton *button, gpointer user_data);
  void on_window_editor_delete_button(GtkButton *button);
  WindowEditor * last_focused_editor_window();
  void on_file_project_open(const ustring& project, bool startup);
  void handle_editor_focus();
  ustring previously_focused_project_name;
  void save_editors();
  void goto_next_previous_project(bool next);
  static void on_editor_reload_clicked(GtkButton *button, gpointer user_data);
  static void on_editor_changed_clicked(GtkButton *button, gpointer user_data);
  void on_editor_changed();
  static void on_view_usfm_code_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_view_usfm_code();
  void reload_all_editors(bool take_chapter_from_focused_editor);

  /* Merge */
  static void on_file_projects_merge_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_file_projects_merge();
  WindowMerge * window_merge;
  static void on_window_merge_delete_button_clicked(GtkButton *button, gpointer user_data);
  void on_window_merge_delete_button();
  static void on_merge_window_get_text_button_clicked(GtkButton *button, gpointer user_data);
  void on_merge_window_get_text_button();
  static void on_merge_window_new_reference_button_clicked(GtkButton *button, gpointer user_data);
  void on_merge_window_new_reference_button();
  static void on_merge_window_save_editors_button_clicked(GtkButton *button, gpointer user_data);
  void on_merge_window_save_editors_button();

  /* Diglot */
  static void on_preferences_filters_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_preferences_filters();

  /* Print */
  static void on_print_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_print();

  // Accelerators.
  static void accelerator_undo_callback(gpointer user_data);
  static void accelerator_redo_callback(gpointer user_data);
  static void accelerator_cut_callback(gpointer user_data);
  static void accelerator_copy_callback(gpointer user_data);
  static void accelerator_paste_callback(gpointer user_data);
  static void accelerator_standard_text_1_callback(gpointer user_data);
  static void accelerator_standard_text_2_callback(gpointer user_data);
  static void accelerator_standard_text_3_callback(gpointer user_data);
  static void accelerator_standard_text_4_callback(gpointer user_data);
  void accelerator_standard_text_n(unsigned int selector);
  static void accelerator_new_project_note_callback(gpointer user_data);
  static void accelerator_next_verse_callback(gpointer user_data);
  static void accelerator_previous_verse_callback(gpointer user_data);
  static void accelerator_next_chapter_callback(gpointer user_data);
  static void accelerator_previous_chapter_callback(gpointer user_data);
  static void accelerator_next_book_callback(gpointer user_data);
  static void accelerator_previous_book_callback(gpointer user_data);
  static void accelerator_next_reference_in_history_callback(gpointer user_data);
  void accelerator_next_reference_in_history();
  static void accelerator_previous_reference_in_history_callback(gpointer user_data);
  void accelerator_previous_reference_in_history();
  static void accelerator_go_to_reference_callback(gpointer user_data);
  static void accelerator_close_window_callback(gpointer user_data);
  void accelerator_close_window();
  static void accelerator_goto_styles_area_callback(gpointer user_data);
  static void accelerator_quit_program_callback(gpointer user_data);
  static void accelerator_activate_text_area_callback(gpointer user_data);
  static void accelerator_activate_tools_area_callback(gpointer user_data);
  static void accelerator_activate_notes_area_callback(gpointer user_data);
  static void accelerator_next_reference_in_reference_area_callback(gpointer user_data);
  static void accelerator_previous_reference_in_reference_area_callback(gpointer user_data);
  static void accelerator_next_project_callback(gpointer user_data);
  static void accelerator_previous_project_callback(gpointer user_data);
  static void accelerator_open_project_callback(gpointer user_data);
  static void accelerator_print_callback(gpointer user_data);
  static void accelerator_find_callback(gpointer user_data);
  static void accelerator_replace_callback(gpointer user_data);
  static void accelerator_main_help_callback(gpointer user_data);
  static void accelerator_view_usfm_code(gpointer user_data);
  void accelerator_view_usfm_code_toggle();
  static void accelerator_left_square_bracket(gpointer user_data);
  void left_square_bracket ();
  static void accelerator_right_square_bracket(gpointer user_data);  
  void right_square_bracket ();
  
  // Shutdown.
  bool shutting_down;
  static bool on_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
  void initiate_shutdown();
  
  // Assistants.
  static void on_assistant_ready_signal (GtkButton *button, gpointer user_data);
  void on_assistant_ready ();

  // Check USFM
  static void on_check_usfm_activate (GtkMenuItem *menuitem, gpointer user_data);
  void on_check_usfm ();
  WindowCheckUSFM * window_check_usfm;
  static void on_window_check_usfm_delete_button_clicked(GtkButton *button, gpointer user_data);
  void on_window_check_usfm_delete_button();
  void check_usfm_window_ping();

  /* Import */
  static void on_file_import_activate (GtkMenuItem *menuitem, gpointer user_data);
  void on_file_import ();
  ImportAssistant * import_assistant;
  
  /* Floating windows */
  static void on_scrolledwindow_layout_size_allocate (GtkWidget *widget, GdkRectangle *allocation, gpointer user_data);
  void scrolledwindow_layout_size_allocate (GdkRectangle *allocation);
  ustring focused_project_last_session;
  static bool on_windows_startup_timeout(gpointer data);
  bool on_windows_startup();
  unsigned int windows_startup_pointer;
  static void on_window_focus_button_clicked(GtkButton *button, gpointer user_data);
  void on_window_focus_button(GtkButton *button);
  GtkWidget * focused_editor_button;
  GtkWidget * focused_resource_button;
  GtkWidget * focused_tool_button;
  void store_last_focused_tool_button (GtkButton * button);
  void shutdown_windows();
  
  // Interprocess communications.
  guint interprocess_communications_initiate_listener_event_id;
  static bool on_interprocess_communications_initiate_listener_timeout(gpointer data);
  void interprocess_communications_initiate_listener ();
  static void on_interprocess_communications_listener_button_clicked(GtkButton *button, gpointer user_data);
  void on_interprocess_communications_listener_button(GtkButton *button);
  

};


#endif
