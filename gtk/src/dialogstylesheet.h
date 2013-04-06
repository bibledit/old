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


#ifndef INCLUDED_DIALOGSTYLESHEET_H
#define INCLUDED_DIALOGSTYLESHEET_H


#include <gtk/gtk.h>
#include "style.h"


class StylesheetDialog
{
public:
  StylesheetDialog (const ustring& stylesheet, const ustring& style);
  ~ StylesheetDialog ();
  int run ();
  ustring name;
protected:
  ustring mystylesheet;
  ustring mystyle;
private:
  GtkWidget *stylesheetdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *scrolledwindow_main;
  GtkWidget *viewport_main;
  GtkWidget *vbox1;
  GtkWidget *vbox2;
  GtkWidget *hbox1;
  GtkWidget *label6;
  GtkWidget *labelstyle;
  GtkWidget *label37;
  GtkWidget *entryname;
  GtkWidget *label36;
  GtkWidget *scrolledwindow1;
  GtkWidget *textview1;
  GtkWidget *hseparator1;
  GtkWidget *hbox8;
  GtkWidget *vbox5;
  GtkWidget *label21;
  GtkWidget *vbox3;
  GtkWidget *radiobutton_id;
  GtkWidget *radiobutton_paragraph;
  GtkWidget *radiobutton_inline;
  GtkWidget *radiobutton_chapter;
  GtkWidget *radiobutton_verse;
  GtkWidget *radiobutton_footendnote;
  GtkWidget *radiobutton_crossreference;
  GtkWidget *radiobutton_peripheral;
  GtkWidget *radiobutton_picture;
  GtkWidget *radiobutton_pagebreak;
  GtkWidget *radiobutton_table_element;
  GtkWidget *radiobutton_wordlist_element;
  GtkWidget *vseparator1;
  GtkWidget *notebook_subtype;
  GtkWidget *empty_notebook_page;
  GtkWidget *label47;
  GtkWidget *vbox8;
  GtkWidget *label50;
  GtkWidget *radiobutton_footnote;
  GtkWidget *radiobutton_endnote;
  GtkWidget *radiobutton_note_content_standard;
  GtkWidget *radiobutton_note_content;
  GtkWidget *radiobutton_note_content_endmarker;
  GtkWidget *radiobutton_note_paragraph;
  GtkWidget *label48;
  GtkWidget *vbox9;
  GtkWidget *label51;
  GtkWidget *radiobutton_xref;
  GtkWidget *radiobutton_xref_content_standard;
  GtkWidget *radiobutton_xref_content;
  GtkWidget *radiobutton_xref_content_endmarker;
  GtkWidget *label49;
  GtkWidget *vbox13;
  GtkWidget *label55;
  GtkWidget *radiobutton_paragraph_type_main_title;
  GtkWidget *radiobutton_paragraph_type_subtitle;
  GtkWidget *radiobutton_paragraph_type_section_heading;
  GtkWidget *radiobutton_paragraph_type_text;
  GtkWidget *label54;
  GtkWidget *vbox14;
  GtkWidget *labelperiph58;
  GtkWidget *radiobutton_peripheral_pub;
  GtkWidget *radiobutton_peripheral_toc;
  GtkWidget *radiobutton_peripheral_pref;
  GtkWidget *radiobutton_peripheral_intro;
  GtkWidget *radiobutton_peripheral_conc;
  GtkWidget *radiobutton_peripheral_glo;
  GtkWidget *radiobutton_peripheral_idx;
  GtkWidget *radiobutton_peripheral_maps;
  GtkWidget *radiobutton_peripheral_cov;
  GtkWidget *radiobutton_peripheral_spine;
  GtkWidget *label56;
  
  GtkWidget *vbox15;
  GtkWidget *label_id_58;
  GtkWidget *radiobutton_id_book;
  GtkWidget *radiobutton_id_encoding;
  GtkWidget *radiobutton_id_comment;
  GtkWidget *radiobutton_id_comment_with_endmarker;
  GtkWidget *radiobutton_id_running_header;
  GtkWidget *radiobutton_is_long_toc_text;
  GtkWidget *radiobutton_is_short_toc_text;
  GtkWidget *radiobutton_is_book_abbrev;
  GtkWidget *radiobutton_is_chapter_label;
  GtkWidget *radiobutton_is_published_chapter_marker;
  GtkWidget *label57;

  // Table subtype
  GtkWidget *vbox17;
  GtkWidget *label_table_sub;
  GtkWidget *radiobutton_table_sub_new_row;
  GtkWidget *radiobutton_table_sub_heading;
  GtkWidget *radiobutton_table_sub_cell;
  GtkWidget *label61;

  // Word list subtype.
  GtkWidget *vbox_subtype_wordlist;
  GtkWidget *label_subtype_wordlist;
  GtkWidget *radiobutton_subtype_wordlist_glossary_dictionary_entry;
  GtkWidget *radiobutton_subtype_hebrew_wordlist_entry;
  GtkWidget *radiobutton_subtype_greek_wordlist_entry;
  GtkWidget *radiobutton_subtype_subject_index_entry;
  GtkWidget *label63;

  GtkWidget *vseparator2;
  GtkWidget *vbox6;

  // Fontsize in points.
  GtkWidget *hbox2;
  GtkWidget *label8;
  GtkObject *spinbuttonfontsize_adj;
  GtkWidget *spinbuttonfontsize;
  GtkWidget *label9;
  void fontsize_points_create ();
  double fontsize_points;

  // Italic
  ustring italic;
  GtkWidget *checkbutton_italic;
  void italic_simple_create ();
  
  // Bold
  ustring bold;
  GtkWidget *checkbutton_bold;
  void bold_simple_create ();
  
  // Underline
  ustring underline;
  GtkWidget *checkbutton_underline;
  void underline_simple_create ();
  
  // Small caps
  ustring smallcaps;
  GtkWidget *checkbutton_small_caps;
  void smallcaps_simple_create ();

  // Italic, bold, underline, small caps - extended.
  GtkWidget *table2;
  GtkWidget *label40;
  GtkWidget *label41;
  GtkWidget *label42;
  GtkWidget *radiobutton_italics_on;
  GtkWidget *radiobutton_bold_on;
  GtkWidget *radiobutton_underline_on;
  GtkWidget *radiobutton_italics_toggle;
  GtkWidget *radiobutton_bold_toggle;
  GtkWidget *radiobutton_underline_toggle;
  GtkWidget *radiobutton_italics_inherit;
  GtkWidget *radiobutton_bold_inherit;
  GtkWidget *radiobutton_underline_inherit;
  GtkWidget *radiobutton_italics_off;
  GtkWidget *radiobutton_bold_off;
  GtkWidget *radiobutton_underline_off;
  GtkWidget *label_small_caps;
  GtkWidget *radiobutton_small_caps_off;
  GtkWidget *radiobutton_small_caps_on;
  GtkWidget *radiobutton_small_caps_inherit;
  GtkWidget *radiobutton_small_caps_toggle;
  void italic_bold_underline_smallcaps_extended_create ();

  // Superscript
  GtkWidget *checkbutton_superscript;
  bool superscript;
  void superscript_create ();

  // Paragraph
  GtkWidget *vbox4;
  GtkWidget *hbox4;
  GtkWidget *label10;
  GtkWidget *radiobutton_left;
  GtkWidget *radiobutton_center;
  GtkWidget *radiobutton_right;
  GtkWidget *radiobutton_full;
  GtkWidget *table1;
  GtkWidget *label11;
  GtkWidget *label12;
  GtkWidget *label13;
  GtkWidget *label14;
  GtkWidget *label15;
  GtkWidget *label16;
  GtkWidget *label17;
  GtkWidget *label18;
  GtkWidget *label19;
  GtkWidget *label20;
  GtkObject *spinbutton_first_adj;
  GtkWidget *spinbutton_first;
  GtkObject *spinbutton_before_adj;
  GtkWidget *spinbutton_before;
  GtkObject *spinbutton_after_adj;
  GtkWidget *spinbutton_after;
  GtkObject *spinbutton_left_adj;
  GtkWidget *spinbutton_left;
  GtkObject *spinbutton_right_adj;
  GtkWidget *spinbutton_right;
  ustring justification;
  double spacebefore;
  double spaceafter;
  double leftmargin;
  double rightmargin;
  double firstlineindent;
  void paragraph_create (const gchar * label, bool grey_out_justify);

  // Span columns.
  GtkWidget *checkbutton_span;
  bool spancolumns;
  void span_columns_create ();
  
  // Apocrypha
  GtkWidget *checkbutton_apocrypha;
  void apocrypha_create ();

  // Note numbering type.
  GtkWidget *vbox10;
  GtkWidget *hbox9;
  GtkWidget *label52;
  GtkWidget *vbox11;
  GtkWidget *radiobutton_note_numbering_1;
  GtkWidget *radiobutton_note_numbering_a;
  GtkWidget *radiobutton_note_numbering_user;
  GtkWidget *entry_note_numbering;
  void note_numbering_type_create ();
  
  // Note numbering restart.
  GtkWidget *hbox10;
  GtkWidget *label53;
  GtkWidget *vbox12;
  GtkWidget *radiobutton_note_numbering_restart_never;
  GtkWidget *radiobutton_note_numbering_restart_book;
  GtkWidget *radiobutton_note_numbering_restart_chapter;
  void note_numering_restart_create ();
  
  // Print chapternumber at first verse.
  GtkWidget *print_chapter_at_first_verse;
  void print_chapter_at_first_verse_create ();
  static void on_radiobutton_print_chapter_at_first_verse_create_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton_print_chapter_at_first_verse_create ();

  // \id : Book starting new page.
  GtkWidget *hbox_id;
  GtkWidget *checkbutton_id_newpage;
  GtkWidget *checkbutton_id_oddpage;
  void book_id_new_page_create ();
  
  // Colour
  GtkWidget *hbox_colour;
  GtkWidget *button_colour;
  unsigned int color;
  void colour_create ();  

  // Print
  GtkWidget *checkbutton_print;
  bool print;
  void print_create ();

  // Endnotes placement.
  GtkWidget *hbox13;
  GtkWidget *label60;
  GtkWidget *vbox16;
  GtkWidget *radiobutton_print_endnotes_after_book;
  GtkWidget *radiobutton_print_endnotes_after_everything_else;
  GtkWidget *hbox14;
  GtkWidget *radiobutton_print_endnotes_at_marker;
  GtkWidget *entry_print_endnotes_marker;
  void end_note_placement_create ();
  static void on_radiobutton_print_endnotes_at_marker_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton_print_endnotes_at_marker ();

  // Column number.
  GtkWidget *hbox15;
  GtkWidget *label62;
  GtkObject *spinbutton_column_number_adj;
  GtkWidget *spinbutton_column_number;
  void column_number_create ();

  // Chapter number (and other text) in running headers
  GtkWidget *checkbutton_print_in_running_header_left;
  GtkWidget *checkbutton_print_in_running_header_right;
  void print_in_running_header_create ();
  
  // Text to add after wordlist entries.
  GtkWidget *hbox16;
  GtkWidget *label64;
  GtkWidget *entry_wordlist_addition;
  GtkWidget *label65;
  void wordlist_add_text_create ();

  // Restart paragraph.
  GtkWidget *checkbutton_restarts_paragraph;
  void restarts_paragraph_create ();

  // Action area.
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton;
  
  // User variables.
  bool userbool1, userbool2, userbool3;
  int userint1, userint2, userint3;
  ustring userstring1, userstring2, userstring3;
  
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_ok();
  static void on_style_type_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_style_type (GtkToggleButton *togglebutton);
  static void on_alignment_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_alignment (GtkToggleButton *togglebutton);
  void set_gui ();
  void save_style ();
  void set_justification (const ustring& justification);
  ustring get_justification ();
  void set_italic ();
  ustring get_italic ();
  void set_bold ();
  ustring get_bold ();
  void set_underline ();
  ustring get_underline ();
  void set_small_caps ();
  ustring get_small_caps ();
  StyleType get_styletype ();
  static void on_radiobutton_italic_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton_italic (GtkWidget *togglebutton);
  static void on_radiobutton_bold_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton_bold (GtkWidget *togglebutton);
  static void on_radiobutton_underline_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton_underline (GtkWidget *togglebutton);
  static void on_radiobutton_small_caps_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton_small_caps (GtkWidget *togglebutton);
  void set_subtype (StyleType maintype, int subtype);
  int get_subtype ();
  static void on_radiobutton_note_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton_note ();
  static void on_radiobutton_xref_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton_xref ();
  void destroy_optional_widgets ();
  static void on_radiobutton_note_numbering_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton_note_numbering ();
  static void on_radiobutton_identifier_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton_identifier ();
  static void on_checkbutton_id_newpage_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_id_newpage ();
  static void on_checkbutton_table_element_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_table_element ();
  static void on_radiobutton_subtype_wordlist_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton_subtype_wordlist ();
};


#endif
