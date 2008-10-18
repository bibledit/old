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

#ifndef INCLUDED_MAINWINDOW_H
#define INCLUDED_MAINWINDOW_H

#include <gtk/gtk.h>
#include "windowsoutpost.h"
#include "ustring.h"
#include "bibletime.h"
#include "usfm.h"
#include "note_editor.h"
#include "editor.h"
#include "httpd.h"
#include "reference.h"
#include "keytermsgui.h"
#include "gui_navigation.h"
#include "outline.h"
#include "gui_styles.h"
#include "displayprojectnotes.h"
#include "git.h"
#include "resourceviewergui.h"
#include "editorsgui.h"
#include "mergegui.h"
#include "windowlayout.h"
#include "windowshowkeyterms.h"
#include "windowshowquickrefs.h"

class MainWindow
  {
public:
  MainWindow(unsigned long xembed);
  ~MainWindow();
  int run();
  GtkWidget *mainwindow;
protected:
  /* IMPORTANT If more menu items are added see whether they should be made 
   * in(sensitive) depending on whether a project has been opened. */
  GtkWidget *vbox1;
  GtkWidget *menubar1;
  GtkWidget *menuitem_file;
  GtkWidget *menuitem_file_menu;
  GtkWidget *file_project;
  GtkWidget *image463;
  GtkWidget *file_project_menu;
  GtkWidget *new1;
  GtkWidget *image903;
  GtkWidget *open1;
  GtkWidget *close1;
  GtkWidget *delete1;
  GtkWidget *properties1;
  GtkWidget *image4995;
  GtkWidget *import1;
  GtkWidget *image464;
  GtkWidget *export_project;
  GtkWidget *image3298;
  GtkWidget *export_project_menu;
  GtkWidget *export_usfm_files;
  GtkWidget *image12814;
  GtkWidget *export_zipped_unified_standard_format_markers1;
  GtkWidget *image17639;
  GtkWidget *to_bibleworks_version_database_compiler;
  GtkWidget *image3299;
  GtkWidget *export_to_sword_module;
  GtkWidget *image11392;
  GtkWidget *export_opendocument;
  GtkWidget *image15162;
  GtkWidget *copy_project_to;
  GtkWidget *image2688;
  GtkWidget *compare_with1;
  GtkWidget *image2764;
  GtkWidget *project_backup;
  GtkWidget *image18535;
  GtkWidget *project_backup_menu;
  GtkWidget *project_backup_incremental;
  GtkWidget *image18536;
  GtkWidget *project_backup_flexible;
  GtkWidget *image18537;
  GtkWidget *project_changes;
  GtkWidget *image19115;
  GtkWidget *file_projects_merge;
  GtkWidget *image28085;
  GtkWidget *file_references;
  GtkWidget *image465;
  GtkWidget *file_references_menu;
  GtkWidget *open_references1;
  GtkWidget *image466;
  GtkWidget *references_save_as;
  GtkWidget *close_references;
  GtkWidget *image468;
  GtkWidget *delete_references;
  GtkWidget *image469;
  GtkWidget *reference_hide;
  GtkWidget *image6483;
  GtkWidget *style;
  GtkWidget *image10735;
  // Some of the widgets for the style menu have been moved to the GuiStyles 
  // object. Other ones are in the constructor of MainWindow.
  GtkWidget *notes2;
  GtkWidget *image936;
  GtkWidget *notes2_menu;
  GtkWidget *new_note;
  GtkWidget *delete_note;
  GtkWidget *image963;
  GtkWidget *import_notes;
  GtkWidget *image1455;
  GtkWidget *export_notes;
  GtkWidget *image4068;
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
  GtkWidget *print;
  GtkWidget *quit1;
  GtkWidget *menuitem_edit;
  GtkWidget *menuitem_edit_menu;
  GtkWidget *cut1;
  GtkWidget *copy1;
  GtkWidget *copy_without_formatting;
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
  GtkWidget *editstatus;
  GtkWidget *image25815;
  GtkWidget *edit_planning;
  GtkWidget *image26801;
  GtkWidget *menuitem_view;
  GtkWidget *menuitem_view_menu;
  GtkWidget *view_font;
  GtkWidget *image20234;
  GtkWidget *view_font_menu;
  GtkWidget *view_text_font;
  GtkWidget *image20235;
  /* Because of switching to GtkHtml for displaying and editing fonts, the fonts no longer can be set in the menu.
   GtkWidget *view_notes_font;
   GtkWidget *image20236;
   */
  GtkWidget *viewnotes;
  GtkWidget *image2627;
  GtkWidget *screen_layout;
  GtkWidget *image11944;
  GtkWidget *view_git_tasks;
  GtkWidget *image18685;
  GtkWidget *parallel_passages1;
  GtkWidget *view_usfm_code;
  GtkWidget *image25006;
  GtkWidget *view_status;
  GtkWidget *image25963;
  GtkWidget *view_planning;
  GtkWidget *image26812;
  GtkWidget *view_screen_layout;
  GtkWidget *view_keyterms;
  GtkWidget *view_quick_references;
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
  GtkWidget *menuitem_goto;
  GtkWidget *menuitem_goto_menu;
  GtkWidget *next_verse1;
  GtkWidget *image95;
  GtkWidget *previous_verse1;
  GtkWidget *image96;
  GtkWidget *next_chapter1;
  GtkWidget *image97;
  GtkWidget *previous_chapter1;
  GtkWidget *image98;
  GtkWidget *next_book1;
  GtkWidget *image99;
  GtkWidget *previous_book1;
  GtkWidget *image100;
  GtkWidget *next_reference_in_history1;
  GtkWidget *image5687;
  GtkWidget *previous_reference_in_history1;
  GtkWidget *image5688;
  GtkWidget *separator18;
  GtkWidget *next_reference1;
  GtkWidget *image608;
  GtkWidget *previous_reference1;
  GtkWidget *image609;
  GtkWidget *goto_next_project;
  GtkWidget *image19528;
  GtkWidget *goto_previous_project;
  GtkWidget *image19529;
  GtkWidget *reference1;
  GtkWidget *image101;
  GtkWidget *separator19;
  GtkWidget *text_area1;
  GtkWidget *image4721;
  GtkWidget *goto_bible_notes_area1;
  GtkWidget *image11878;
  GtkWidget *references_area1;
  GtkWidget *image4722;
  GtkWidget *insert_style;
  GtkWidget *image11111;
  GtkWidget *notes_area1;
  GtkWidget *image4723;
  GtkWidget *separator11;
  GtkWidget *synchronize_other_programs2;
  GtkWidget *image4931;
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
  GtkWidget *image17074;
  GtkWidget *my_checks;
  GtkWidget *image15438;
  GtkWidget *menutools;
  GtkWidget *menutools_menu;
  GtkWidget *line_cutter_for_hebrew_text1;
  GtkWidget *image13532;
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
  GtkWidget *menuitem_preferences;
  GtkWidget *menuitem_preferences_menu;
  GtkWidget *notes_preferences;
  GtkWidget *image2116;
  GtkWidget *printingprefs;
  GtkWidget *image3493;
  GtkWidget *reference_exchange1;
  GtkWidget *image5972;
  GtkWidget *ignored_references1;
  GtkWidget *image6467;
  GtkWidget *prefs_books;
  GtkWidget *image12167;
  GtkWidget *preferences_windows_outpost;
  GtkWidget *image14287;
  GtkWidget *preferences_tidy_text;
  GtkWidget *image16359;
  GtkWidget *preferences_remote_git_repository;
  GtkWidget *image18977;
  GtkWidget *preferences_features;
  GtkWidget *image20936;
  GtkWidget *preferences_password;
  GtkWidget *image20937;
  GtkWidget *preferences_text_replacement;
  GtkWidget *image23181;
  GtkWidget *pdf_viewer1;
  GtkWidget *image24540;
  GtkWidget *preferences_reporting;
  GtkWidget *image25623;
  GtkWidget *preferences_planning;
  GtkWidget *image26888;
  GtkWidget *preferences_graphical_interface;
  GtkWidget *image27031;
  GtkWidget *preferences_filters;
  GtkWidget *image28360;
  GtkWidget *menuitem_help;
  GtkWidget *menuitem_help_menu;
  GtkWidget *help_main;
  GtkWidget *image17520;
  GtkWidget *help_context;
  GtkWidget *image16053;
  GtkWidget *system_log1;
  GtkWidget *image4388;
  GtkWidget *about1;
  GtkWidget *toolbar1;
  GtkWidget *hbox1;
  GtkWidget *hpaned1;
  GtkWidget *vbox_left;
  GtkWidget *vpaned1;
  GtkWidget *vboxeditors;
  GtkWidget *notebook_notes_area;
  GtkWidget *notebook1;
  GtkWidget *scrolledwindow_notes;
  GtkWidget *htmlview_notes;
  GtkWidget *label1;
  GtkWidget *vbox_note_editor;
  GtkWidget *toolbar_note_editor;
  GtkWidget *toolitem_note_edit_font_size;
  GtkWidget *combobox_note_edit_font_size;
  GtkWidget *toolitem_note_edit_paragraph_style;
  GtkWidget *combobox_note_edit_paragraph_style;
  GtkWidget *toolitem_note_edit_bold;
  GtkWidget *togglebutton_note_edit_bold;
  GtkWidget *image29121;
  GtkWidget *toolitem_note_edit_italics;
  GtkWidget *togglebutton_note_edit_italics;
  GtkWidget *image29122;
  GtkWidget *toolitem_note_edit_underline;
  GtkWidget *togglebutton_note_edit_underline;
  GtkWidget *image29114;
  GtkWidget *toolitem_note_edit_strike_through;
  GtkWidget *togglebutton_note_edit_strike_through;
  GtkWidget *image29123;
  GtkWidget *toolitem_note_edit_left_justify;
  GtkWidget *togglebutton_note_edit_left_justify;
  GtkWidget *image29124;
  GtkWidget *toolitem_note_edit_center_justify;
  GtkWidget *togglebutton_note_edit_center_justify;
  GtkWidget *image29125;
  GtkWidget *toolitem_note_edit_right_justify;
  GtkWidget *togglebutton_note_edit_right_justify;
  GtkWidget *image29126;
  GtkWidget *toolitem_note_edit_decrease_indent;
  GtkWidget *button_note_edit_decrease_indent;
  GtkWidget *image29127;
  GtkWidget *toolitem_note_edit_increase_indent;
  GtkWidget *button_note_edit_increase_indent;
  GtkWidget *image29128;
  GtkWidget *toolitem_note_edit_color;
  GtkWidget *colorbutton_note_edit;
  GtkWidget *hbox_note_editor;
  GtkWidget *scrolledwindow_note_editor;
  GtkWidget *htmlview_note_editor;
  GtkWidget *vbox4;
  GtkWidget *button_note_ok;
  GtkWidget *button_note_cancel;
  GtkWidget *label2;
  GtkWidget *label30;
  GtkWidget *scrolledwindow_keyterm_text;
  GtkWidget *textview_keyterm_text;
  GtkWidget *label31;
  GtkWidget *vbox_right;
  GtkWidget *notebook_tools;
  GtkWidget *scrolledwindow_references;
  GtkWidget *treeview_references;
  GtkWidget *label13;
  GtkWidget *vbox_styles;
  GtkWidget *label14;
  GtkWidget *vbox_project_note;
  GtkWidget *label_note_category;
  GtkWidget *combobox_note_category;
  GtkWidget *label_note_references;
  GtkWidget *scrolledwindow8;
  GtkWidget *textview_note_references;
  GtkWidget *label_note_project;
  GtkWidget *combobox_note_project;
  GtkWidget *label_note_created_on;
  GtkWidget *label_note_created_by;
  GtkWidget *label_note_edited_on;
  GtkWidget *label_note_logbook;
  GtkWidget *scrolledwindow9;
  GtkWidget *textview_note_logbook;
  GtkWidget *label_notetools;
  GtkWidget *scrolledwindow_keyterms;
  GtkWidget *vbox_keyterms;
  // Widgets that belong here are moved to the KeytermsGUI object.  
  GtkWidget *label22;
  GtkWidget *vbox_outline;
  // Widgets that belong here have been moved to the Outline object.
  GtkWidget *label36;
  GtkWidget *scrolledwindow_resources;
  GtkWidget *vbox_resources;
  GtkWidget *scrolledwindow_merge;
  GtkWidget *vboxmerge;
  GtkWidget *labelmerge;
  GtkWidget *label_tool_resources;
  GtkWidget *hbox5;
  GtkWidget *hbox7;
  GtkWidget *statuslabel_stylesheet;
  GtkWidget *statuslabel_style;
  GtkWidget *label_git;
  GtkWidget *statusbar1;
  GtkAccelGroup *accel_group;
  // Gtk variables manually added.
  GtkListStore *liststore_references;
  GtkTreeViewColumn *treecolumn_references;
  GtkTreeSelection *treeselect_references;

  // Navigator object.
  GuiNavigation navigation;

  /* Initialization */
  void enable_or_disable_widgets(bool enable);

  /* Menu callbacks */
  static void on_open1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void open();
  static void on_close1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void close();
  static void on_new1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void newproject();
  static void
      on_properties1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void editproject();
  static void on_delete1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void deleteproject();
  static void on_quit1_activate(GtkMenuItem * menuitem, gpointer user_data);
  static void
      on_system_log1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void viewlog();
  static void on_help_context_activate(GtkMenuItem * menuitem,
      gpointer user_data);
  void on_help_context();
  static void on_help_main_activate(GtkMenuItem * menuitem, gpointer user_data);
  void on_help_main();
  static void on_about1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void showabout();
  static void on_undo1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void menu_undo();
  static void on_redo1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void menu_redo();
  static void on_edit1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void menu_edit();
  static void on_find_and_replace1_activate(GtkMenuItem * menuitem,
      gpointer user_data);
  void menu_replace();
  static void on_findspecial1_activate(GtkMenuItem * menuitem,
      gpointer user_data);
  void menu_findspecial();
  static void on_import1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void menu_import();
  static gboolean on_mainwindow_delete_event(GtkWidget *widget,
      GdkEvent *event, gpointer user_data);
  static gboolean on_mainwindow_focus_in_event(GtkWidget *widget,
      GdkEventFocus *event, gpointer user_data);
  void on_mainwindow_focus_in(GdkEventFocus *event);
  static void on_insert1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_menu_insert();
  static void on_notes_preferences_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_notes_preferences();
  static void on_copy_project_to_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_copy_project_to();
  static void on_compare_with1_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_compare_with();
  static void on_printingprefs_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_printing_preferences();
  static void on_screen_layout_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_screen_layout();
  static void
      on_prefs_books_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_prefs_books();
  static void on_preferences_tidy_text_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_preferences_tidy_text();
  static void on_tool_generate_word_lists_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_tool_generate_word_lists();
  static void on_preferences_features_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_preferences_features();
  static void on_preferences_password_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_preferences_password();
  static void on_tool_simple_text_corrections_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_tool_simple_text_corrections();
  static void on_preferences_text_replacement_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_preferences_text_replacement();
  static void on_parallel_passages1_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_parallel_passages1();
  static void
      on_pdf_viewer1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_pdf_viewer();
  static void on_view_usfm_code_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_view_usfm_code();
  static void on_insert_special_character_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_insert_special_character();
  static void on_preferences_graphical_interface_activate(
      GtkMenuItem *menuitem, gpointer user_data);
  void on_preferences_graphical_interface();

  /* Navigation  */
  static void on_navigation_new_reference_clicked(GtkButton *button,
      gpointer user_data);
  void on_navigation_new_reference();
  static void
      on_next_verse_activate(GtkMenuItem * menuitem, gpointer user_data);
  void goto_next_verse();
  static void on_previous_verse_activate(GtkMenuItem * menuitem,
      gpointer user_data);
  void goto_previous_verse();
  static void on_next_chapter_activate(GtkMenuItem * menuitem,
      gpointer user_data);
  void goto_next_chapter();
  static void on_previous_chapter_activate(GtkMenuItem * menuitem,
      gpointer user_data);
  void goto_previous_chapter();
  static void on_next_book_activate(GtkMenuItem * menuitem, gpointer user_data);
  void goto_next_book();
  static void on_previous_book_activate(GtkMenuItem * menuitem,
      gpointer user_data);
  void goto_previous_book();
  static void on_reference_activate(GtkMenuItem * menuitem, gpointer user_data);
  void goto_reference_interactive();
  void go_to_new_reference();
  static void on_new_verse_signalled(GtkButton *button, gpointer user_data);
  void on_new_verse();
  static void on_text_area1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_text_area_activate();
  static void on_goto_bible_notes_area1_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_bible_notes_area_activate();
  static void
      on_tools_area1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_tools_area_activate();
  static void
      on_notes_area1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_notes_area_activate();
  static void on_synchronize_other_programs2_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_synchronize_other_programs();
  static void on_goto_next_project_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  static void on_goto_previous_project_activate(GtkMenuItem *menuitem,
      gpointer user_data);

  /* Clipboard */
  static void on_cut1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void on_cut();
  static void on_copy1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void on_copy();
  static void on_copy_without_formatting_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_copy_without_formatting();
  static void on_paste1_activate(GtkMenuItem * menuitem, gpointer user_data);
  void on_paste();

  /* Tools Area */
  static void on_notebook_tools_switch_page(GtkNotebook *notebook,
      GtkNotebookPage *page, guint page_num, gpointer user_data);
  void notebook_tools_switch_page(guint page_num);
  static void on_file_references_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_file_references();

  /* List store and reference handling */
  static gboolean on_treeview_references_key_press_event(GtkWidget * widget,
      GdkEventKey * event, gpointer user_data);
  static gboolean on_treeview_references_button_press_event(GtkWidget * widget,
      GdkEventButton * event, gpointer user_data);
  void on_list_goto();
  static void mainwindow_selection_foreach_function(GtkTreeModel * model,
      GtkTreePath * path, GtkTreeIter * iter, gpointer data);
  static void on_open_references1_activate(GtkMenuItem * menuitem,
      gpointer user_data);
  void on_open_references();
  Reference selected_reference;
  static void on_references_save_as_activate(GtkMenuItem * menuitem,
      gpointer user_data);
  void on_save_references();
  static void on_close_references_activate(GtkMenuItem * menuitem,
      gpointer user_data);
  void on_clear_references();
  static void on_delete_references_activate(GtkMenuItem * menuitem,
      gpointer user_data);
  void on_delete_references();
  static void on_references_collect_iters(GtkTreeModel * model,
      GtkTreePath * path, GtkTreeIter * iter, gpointer data);
  static void on_next_reference1_activate(GtkMenuItem * menuitem,
      gpointer user_data);
  void on_next_reference();
  static void on_previous_reference1_activate(GtkMenuItem * menuitem,
      gpointer user_data);
  void on_previous_reference();
  static void on_ignored_references1_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_ignored_references();
  static gboolean on_treeview_references_popup_menu(GtkWidget *widget,
      gpointer user_data);
  void treeview_references_popup_menu(GtkWidget *widget);
  static void on_reference_hide_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_reference_hide();
  void show_references_popup_menu(GtkWidget *my_widget, GdkEventButton *event);
  static gboolean on_treeview_references_move_cursor(GtkTreeView *treeview,
      GtkMovementStep step, gint count, gpointer user_data);
  static void on_treeview_references_cursor_changed(GtkTreeView *treeview,
      gpointer user_data);
  void treeview_references_display_quick_reference();

  // Bibledit Windows Outpost
  // BibleTime
  static bool mainwindow_on_external_programs_timeout(gpointer data);
  bool on_external_programs_timeout();
  WindowsOutpost * windowsoutpost;
  static void on_reference_exchange1_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_reference_exchange();
  BibleTime bibletime;
  int got_new_bt_reference;
  ustring bibledit_bt_previous_reference;
  ustring bibletime_previous_reference;
  static void on_send_word_to_toolbox_signalled(GtkButton *button,
      gpointer user_data);
  void send_word_to_toolbox();
  static void on_preferences_windows_outpost_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_preferences_windows_outpost();

  /* Title bar and status bar, GUI */
  void set_titlebar(const ustring& project);
  static bool on_gui_timeout(gpointer data);
  void on_gui();
  gint editor_undo_redo_accelerator_state;
  static gboolean on_mainwindow_window_state_event(GtkWidget *widget,
      GdkEvent *event, gpointer user_data);
  void on_mainwindow_window_state(GdkEvent *event);
  static void on_window_size_allocated(GtkWidget *widget,
      GtkAllocation *allocation, gpointer user_data);
  void window_size_allocated(GtkWidget *widget, GtkAllocation *allocation);
  gint mainwindow_width;
  bool mainwindow_width_safe;

  /* Notes editor */
  static void on_new_note_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_new_note();
  static void
      on_delete_note_activate(GtkMenuItem *menuitem, gpointer user_data);
  static void on_viewnotes_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_view_notes();
  void notes_redisplay();
  guint notes_redisplay_source_id;
  static bool on_notes_redisplay_timeout(gpointer data);
  void notes_redisplay_timeout();
  DisplayProjectNotes * displayprojectnotes;
  static void on_find_in_notes1_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void find_in_notes();
  static void
      on_import_notes_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_import_notes();
  static void
      on_export_notes_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_export_notes();
  static void on_button_cancel_clicked(GtkButton *button, gpointer user_data);
  void on_notes_button_cancel();
  static void on_button_ok_clicked(GtkButton *button, gpointer user_data);
  void on_notes_button_ok();
  void on_notes_button_ok_cancel();
  void notes_fill_edit_screen(int id, bool newnote);
  NoteEditor * note_editor;
  static void on_standard_text_1_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  static void on_standard_text_2_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  static void on_standard_text_3_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  static void on_standard_text_4_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_insert_standard_text(GtkMenuItem *menuitem);
  static void on_current_reference1_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  static void on_menuitem_view_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_menuitem_view();
  void stop_displaying_more_notes();
  static void on_get_references_from_note_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_get_references_from_note();
  void notes_get_references_from_id(gint id);
  static gboolean note_save_receiver(const HTMLEngine * engine,
      const char *data, unsigned int len, void *user_data);
  bool project_notes_editable;
  static gboolean on_notes_html_link_clicked(GtkHTML *html, const gchar * url,
      gpointer user_data);
  void notes_html_link_clicked(GtkHTML *html, const gchar * url);
  void notes_delete_ids(const vector<gint>& ids);

  /* Notes formatting */
  static void on_combobox_note_edit_font_size_changed(GtkComboBox *combobox,
      gpointer user_data);
  void combobox_note_edit_font_size_changed();
  static void on_note_editor_insertion_font_style_changed(GtkHTML * html,
      GtkHTMLFontStyle style, gpointer user_data);
  void note_editor_insertion_font_style_changed(GtkHTMLFontStyle style);
  static void on_combobox_note_edit_paragraph_style_changed(
      GtkComboBox *combobox, gpointer user_data);
  void combobox_note_edit_paragraph_style_changed();
  static void on_note_editor_current_paragraph_style_changed(GtkHTML * html,
      GtkHTMLParagraphStyle style, gpointer user_data);
  void note_editor_current_paragraph_style_changed(GtkHTMLParagraphStyle style);
  static void on_togglebutton_note_edit_bold_toggled(
      GtkToggleButton *togglebutton, gpointer user_data);
  void togglebutton_note_edit_bold_toggled();
  static void on_togglebutton_note_edit_italics_toggled(
      GtkToggleButton *togglebutton, gpointer user_data);
  void togglebutton_note_edit_italics_toggled();
  static void on_togglebutton_note_edit_underline_toggled(
      GtkToggleButton *togglebutton, gpointer user_data);
  void togglebutton_note_edit_underline_toggled();
  static void on_togglebutton_note_edit_strike_through_toggled(
      GtkToggleButton *togglebutton, gpointer user_data);
  void togglebutton_note_edit_strike_through_toggled();
  static void on_togglebutton_note_edit_left_justify_toggled(
      GtkToggleButton *togglebutton, gpointer user_data);
  void togglebutton_note_edit_left_justify_toggled();
  static void on_togglebutton_note_edit_center_justify_toggled(
      GtkToggleButton *togglebutton, gpointer user_data);
  void togglebutton_note_edit_center_justify_toggled();
  static void on_togglebutton_note_edit_right_justify_toggled(
      GtkToggleButton *togglebutton, gpointer user_data);
  void togglebutton_note_edit_right_justify_toggled();
  static void on_current_paragraph_alignment_changed(GtkHTML *html,
      GtkHTMLParagraphAlignment new_alignment, gpointer user_data);
  void current_paragraph_alignment_changed(
      GtkHTMLParagraphAlignment new_alignment);
  static void on_button_note_edit_decrease_indent_clicked(GtkButton *button,
      gpointer user_data);
  void button_note_edit_decrease_indent_clicked();
  static void on_button_note_edit_increase_indent_clicked(GtkButton *button,
      gpointer user_data);
  void button_note_edit_increase_indent_clicked();
  static void on_current_paragraph_indentation_changed(GtkHTML *html,
      guint new_indentation, gpointer user_data);
  void current_paragraph_indentation_changed(guint new_indentation);
  static void on_colorbutton_note_edit_color_set(GtkColorButton *colorbutton,
      gpointer user_data);
  void colorbutton_note_edit_color_set(GtkColorButton *colorbutton);
  static void on_insertion_color_changed(GtkHTML *html, GdkColor *color,
      gpointer user_data);
  void insertion_color_changed(GdkColor *color);

  /* Export */
  static void on_export_usfm_files_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  static void on_export_zipped_unified_standard_format_markers1_activate(
      GtkMenuItem *menuitem, gpointer user_data);
  void on_export_usfm_files(bool zipped);
  static void on_to_bibleworks_version_compiler_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_to_bibleworks_version_compiler();
  static void on_export_to_sword_module_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_export_to_sword_module();
  static void on_export_opendocument_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_export_opendocument();

  /* Checks */
  static void on_check1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_menu_check();
  static void on_markers1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_menu_check_markers();
  static void on_validate_usfms1_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_menu_check_markers_validate();
  static void
      on_count_usfms1_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_menu_check_markers_count();
  static void on_compare_usfm1_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_menu_check_markers_compare();
  static void on_chapters_and_verses1_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_menu_check_chapters_and_verses();
  static void on_count_characters_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_count_characters();
  static void on_unwanted_patterns_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_unwanted_patterns();
  static void on_check_capitalization_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_check_capitalization();
  static void on_check_repetition_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_check_repetition();
  static void on_check_matching_pairs_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_check_matching_pairs();
  static void on_unwanted_words_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_unwanted_words();
  static void on_word_count_inventory_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_word_count_inventory();
  static bool on_check_httpd_timeout(gpointer data);
  void on_check_httpd();
  static void on_my_checks_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_my_checks();
  static void on_check_key_terms_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_check_key_terms();
  static void on_check_markers_spacing_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_check_markers_spacing();
  static void on_check_references_inventory_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_check_references_inventory();
  static void on_check_references_validate_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_check_references_validate();
  static void on_check_nt_quotations_from_the_ot_activate(
      GtkMenuItem *menuitem, gpointer user_data);
  void on_check_nt_quotations_from_the_ot();
  static void on_synoptic_parallel_passages_from_the_nt_activate(
      GtkMenuItem *menuitem, gpointer user_data);
  void on_synoptic_parallel_passages_from_the_nt();
  static void on_parallels_from_the_ot_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_parallels_from_the_ot();
  static void on_check_sentence_structure_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_check_sentence_structure();

  /* Styles */
  GuiStyles * styles;
  static void on_goto_styles_area_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_goto_styles_area();
  static void
      on_file_styles_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_file_styles();
  void stylesheet_open_named(const ustring& stylesheet);
  static void
      on_style_button_open_clicked(GtkButton *button, gpointer user_data);
  void on_style_button_open();
  static void on_style_button_apply_clicked(GtkButton *button,
      gpointer user_data);
  void on_style_apply();
  static void on_editor_style_changed(GtkButton *button, gpointer user_data);
  void editor_style_changed();
  static void on_style_edited(GtkButton *button, gpointer user_data);

  /* Footnotes, endnotes, crossreferences */
  static void on_edit_bible_note_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_edit_bible_note();

  /* Tools */
  static void on_menutools_activate(GtkMenuItem *menuitem, gpointer user_data);
  static void on_line_cutter_for_hebrew_text1_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_line_cutter_for_hebrew_text();
  static void on_notes_transfer_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_notes_transfer();
  static void on_tool_origin_references_in_bible_notes_activate(
      GtkMenuItem *menuitem, gpointer user_data);
  void on_tool_origin_references_in_bible_notes();
  static void on_tool_project_notes_mass_update1_activate(
      GtkMenuItem *menuitem, gpointer user_data);
  void on_tool_project_notes_mass_update();
  static void on_tool_transfer_project_notes_to_text_activate(
      GtkMenuItem *menuitem, gpointer user_data);
  void on_tool_transfer_project_notes_to_text();

  /* Webserver */
  Httpd httpd;

  /* Keyterms */
  KeytermsGUI * keytermsgui;
  void activate_keyterms_object();
  void destroy_keyterms_object();
  static void on_keyterms_new_reference(GtkButton *button, gpointer user_data);
  void check_move_new_reference();
  static void on_view_keyterms_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_view_keyterms();
  WindowShowKeyterms * window_show_keyterms;
  static void on_window_show_keyterms_button_clicked(GtkButton *button,
      gpointer user_data);
  void on_window_show_keyterms_button();

  /* Backups */
  static void on_project_backup_incremental_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_project_backup_incremental();
  static void on_project_backup_flexible_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_project_backup_flexible();

  /* Git */
  static void on_view_git_tasks_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_view_git_tasks();
  static void on_preferences_remote_git_repository_activate(
      GtkMenuItem *menuitem, gpointer user_data);
  void on_preferences_remote_git_repository();
  void on_git_reopen_project();
  static void on_project_changes_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_project_changes();
  static void
      on_edit_revert_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_edit_revert();
  void git_update_intervals_initialize();
  map <ustring, unsigned int> git_update_intervals;
  unsigned int git_update_interval_event_id;
  static bool on_git_update_timeout(gpointer user_data);
  void git_update_timeout();
  GitChapterState * gitchapterstate;
  bool git_reopen_project;

  /* Fonts */
  static void on_view_text_font_activate(GtkMenuItem * menuitem,
      gpointer user_data);
  void on_text_font();
  static void on_view_notes_font_activate(GtkMenuItem * menuitem,
      gpointer user_data);
  void on_notes_font();
  void set_fonts();

  /* Outline */
  Outline * outline;
  void outline_initialize();
  void outline_destroy();
  static void on_button_outline_clicked(GtkButton *button, gpointer user_data);
  void on_button_outline();

  /* Interprocess communications */
  static void on_ipc_method_called(GtkButton *button, gpointer user_data);
  void on_ipc_method();

  /* Reporting and Planning */
  static void on_preferences_reporting_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_preferences_reporting();
  static void on_editstatus_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_editstatus();
  static void
      on_view_status_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_view_status();
  static void on_edit_planning_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_edit_planning();
  static void on_view_planning_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_view_planning();
  static void on_preferences_planning_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_preferences_planning();

  /* Resources */
  ResourcesGUI * resourcesgui;
  void activate_resources_object();
  void destroy_resources_object();
  static void on_file_resources_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_file_resources();
  static void on_file_resources_open_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_file_resources_open();
  static void on_file_resources_close_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_file_resources_close();
  static void on_file_resources_new_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_file_resources_new();
  static void on_file_resources_edit_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_file_resources_edit();
  static void on_file_resources_delete_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_file_resources_delete();

  /* Text editors */
  static void on_editorsgui_focus_button_clicked(GtkButton *button,
      gpointer user_data);
  void on_editorsgui_focus_button();
  static void on_editor_reload_clicked(GtkButton *button, gpointer user_data);
  void on_editor_reload();
  EditorsGUI * editorsgui;
  void jump_start_editors(const ustring& project);
  static void on_editorsgui_changed_clicked(GtkButton *button,
      gpointer user_data);
  void on_editorsgui_changed();
  void reload_project();

  /* Merge */
  static void on_file_projects_merge_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_file_projects_merge();
  MergeGUI * mergegui;
  static void on_mergegui_get_text_button_clicked(GtkButton *button,
      gpointer user_data);
  void on_mergegui_get_text_button();
  static void on_mergegui_new_reference_button_clicked(GtkButton *button,
      gpointer user_data);
  void on_mergegui_new_reference_button();
  static void on_mergegui_save_editors_button_clicked(GtkButton *button,
      gpointer user_data);
  void on_mergegui_save_editors_button();

  /* Diglot */
  static void on_preferences_filters_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_preferences_filters();

  /* Print */
  static void on_print_activate(GtkMenuItem *menuitem, gpointer user_data);
  void on_print();

  /* Screen layout */
  static void on_view_screen_layout_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_view_screen_layout();
  WindowLayout * window_screen_layout;
  static void on_window_screen_layout_button_clicked(GtkButton *button,
      gpointer user_data);
  void on_window_screen_layout_button();
  static bool on_windows_startup_timeout(gpointer data);
  bool on_windows_startup();
  unsigned int windows_startup_pointer;
  void shutdown_windows();

  /* Quick references */
  static void on_view_quick_references_activate(GtkMenuItem *menuitem,
      gpointer user_data);
  void on_view_quick_references();
  WindowShowQuickReferences * window_show_quick_references;
  static void on_window_show_quick_references_button_clicked(GtkButton *button,
      gpointer user_data);
  void on_window_show_quick_references_button();

  };

#endif
