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


#include "libraries.h"
#include <glib.h>
#include "dialogstylesheet.h"
#include "utilities.h"
#include "stylesheetutils.h"
#include "dialogentry.h"
#include "projectutils.h"
#include "directories.h"
#include "usfm.h"
#include "gtkwrappers.h"
#include "constants.h"
#include <glib.h>
#include "color.h"
#include "help.h"
#include "tiny_utilities.h"
#include "screen.h"


/*
 Usage of user variables.

 userint1
 - Footnote / crossreference / endnote: NoteNumberingType
 - Table Element, heading and cell: column number

 userint2
 - Footnote / crossreference: NoteNumberingRestartType
 - Endnote: EndnotePositionType

 userbool1
 - Chapter number: Whether to print at first verse.
 - \id: whether to start a new page.
 - Footnote/endnote/xref: whether it applies to the apocrypha.
 - Verse number: Whether to restart the paragraph.

 userbool2
 - \id: whether to start an odd page number.
 - Chapter number (\c): Whether to include it in the left running header.
 - Running header (\h(#): Whether to include it in the left running header.

 userbool3
 - Chapter number (\c): Whether to include it in the right running header.
 - Running header (\h(#): Whether to include it in the right running header.

 userstring1
 - Footnotes / xrefs / endnotes: numbering sequence.
 - Chapter label: the label.
 - Word list entries: text to add after entry.

 userstring2
 - Endnotes: dump notes upon encountering this marker.
 */


StylesheetDialog::StylesheetDialog(const ustring & stylesheet, const ustring & style)
{
  // Save variables.
  mystylesheet = stylesheet;
  mystyle = style;

  stylesheetdialog = gtk_dialog_new();
  ustring s;
  s = "Stylesheet " + stylesheet + ", style " + style;
  gtk_window_set_title(GTK_WINDOW(stylesheetdialog), s.c_str());
  gtk_window_set_position(GTK_WINDOW(stylesheetdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(stylesheetdialog), TRUE);
  gtk_window_set_destroy_with_parent(GTK_WINDOW(stylesheetdialog), TRUE);
  gtk_window_set_type_hint(GTK_WINDOW(stylesheetdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(stylesheetdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  scrolledwindow_main = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_main);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), scrolledwindow_main, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_main), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_main), GTK_SHADOW_IN);

  viewport_main = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport_main);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_main), viewport_main);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (viewport_main), vbox1);

  vbox2 = gtk_vbox_new(FALSE, 6);
  gtk_widget_show(vbox2);
  gtk_box_pack_start(GTK_BOX(vbox1), vbox2, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(vbox2), hbox1, false, false, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hbox1), 2);

  label6 = gtk_label_new_with_mnemonic("Style:");
  gtk_widget_show(label6);
  gtk_box_pack_start(GTK_BOX(hbox1), label6, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label6), 0, 0.5);

  labelstyle = gtk_label_new("");
  gtk_widget_show(labelstyle);
  gtk_box_pack_start(GTK_BOX(hbox1), labelstyle, TRUE, TRUE, 0);
  gtk_misc_set_alignment(GTK_MISC(labelstyle), 0, 0.5);

  label37 = gtk_label_new_with_mnemonic("N_ame");
  gtk_widget_show(label37);
  gtk_box_pack_start(GTK_BOX(vbox2), label37, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label37), 0, 0.5);

  entryname = gtk_entry_new();
  gtk_widget_show(entryname);
  gtk_box_pack_start(GTK_BOX(vbox2), entryname, FALSE, FALSE, 0);

  label36 = gtk_label_new_with_mnemonic("D_escription");
  gtk_widget_show(label36);
  gtk_box_pack_start(GTK_BOX(vbox2), label36, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label36), 0, 0.5);

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_box_pack_start(GTK_BOX(vbox2), scrolledwindow1, TRUE, TRUE, 0);
  gtk_widget_set_size_request(scrolledwindow1, -1, 50);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  textview1 = gtk_text_view_new();
  gtk_widget_show(textview1);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), textview1);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview1), GTK_WRAP_WORD);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(vbox2), hseparator1, TRUE, TRUE, 0);

  hbox8 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox8);
  gtk_box_pack_start(GTK_BOX(vbox2), hbox8, TRUE, TRUE, 0);

  vbox5 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox5);
  gtk_box_pack_start(GTK_BOX(hbox8), vbox5, TRUE, TRUE, 0);

  label21 = gtk_label_new_with_mnemonic("_This style:");
  gtk_widget_show(label21);
  gtk_box_pack_start(GTK_BOX(vbox5), label21, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label21), 0, 0.5);

  vbox3 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox3);
  gtk_box_pack_start(GTK_BOX(vbox5), vbox3, FALSE, TRUE, 0);

  GSList *radiobutton_id_group = NULL;

  radiobutton_id = gtk_radio_button_new_with_mnemonic(NULL, "is an identifier");
  gtk_widget_show(radiobutton_id);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_id, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_id), radiobutton_id_group);
  radiobutton_id_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_id));

  radiobutton_paragraph = gtk_radio_button_new_with_mnemonic(NULL, "starts a new paragraph");
  gtk_widget_show(radiobutton_paragraph);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_paragraph, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_paragraph), radiobutton_id_group);
  radiobutton_id_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_paragraph));

  radiobutton_inline = gtk_radio_button_new_with_mnemonic(NULL, "is inline text with endmarker");
  gtk_widget_show(radiobutton_inline);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_inline, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_inline), radiobutton_id_group);
  radiobutton_id_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_inline));

  radiobutton_chapter = gtk_radio_button_new_with_mnemonic(NULL, "is a chapter number");
  gtk_widget_show(radiobutton_chapter);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_chapter, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_chapter), radiobutton_id_group);
  radiobutton_id_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_chapter));

  radiobutton_verse = gtk_radio_button_new_with_mnemonic(NULL, "is a verse number");
  gtk_widget_show(radiobutton_verse);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_verse, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_verse), radiobutton_id_group);
  radiobutton_id_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_verse));

  radiobutton_footendnote = gtk_radio_button_new_with_mnemonic(NULL, "is a footnote or endnote");
  gtk_widget_show(radiobutton_footendnote);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_footendnote, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_footendnote), radiobutton_id_group);
  radiobutton_id_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_footendnote));

  radiobutton_crossreference = gtk_radio_button_new_with_mnemonic(NULL, "is a crossreference");
  gtk_widget_show(radiobutton_crossreference);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_crossreference, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_crossreference), radiobutton_id_group);
  radiobutton_id_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_crossreference));

  radiobutton_peripheral = gtk_radio_button_new_with_mnemonic(NULL, "is a peripheral element");
  gtk_widget_show(radiobutton_peripheral);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_peripheral, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_peripheral), radiobutton_id_group);
  radiobutton_id_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_peripheral));

  radiobutton_picture = gtk_radio_button_new_with_mnemonic(NULL, "is a picture");
  gtk_widget_show(radiobutton_picture);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_picture, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_picture), radiobutton_id_group);
  radiobutton_id_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_picture));

  radiobutton_pagebreak = gtk_radio_button_new_with_mnemonic(NULL, "starts a new page");
  gtk_widget_show(radiobutton_pagebreak);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_pagebreak, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_pagebreak), radiobutton_id_group);
  radiobutton_id_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_pagebreak));

  radiobutton_table_element = gtk_radio_button_new_with_mnemonic(NULL, "is a table element");
  gtk_widget_show(radiobutton_table_element);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_table_element, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_table_element), radiobutton_id_group);
  radiobutton_id_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_table_element));

  radiobutton_wordlist_element = gtk_radio_button_new_with_mnemonic(NULL, "is a word list element");
  gtk_widget_show(radiobutton_wordlist_element);
  gtk_box_pack_start(GTK_BOX(vbox3), radiobutton_wordlist_element, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_wordlist_element), radiobutton_id_group);
  radiobutton_id_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_wordlist_element));

  vseparator1 = gtk_vseparator_new();
  gtk_widget_show(vseparator1);
  gtk_box_pack_start(GTK_BOX(hbox8), vseparator1, FALSE, FALSE, 0);

  notebook_subtype = gtk_notebook_new();
  gtk_widget_show(notebook_subtype);
  gtk_box_pack_start(GTK_BOX(hbox8), notebook_subtype, TRUE, TRUE, 0);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook_subtype), FALSE);
  GTK_WIDGET_UNSET_FLAGS(notebook_subtype, GTK_CAN_FOCUS);
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook_subtype), FALSE);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook_subtype), FALSE);

  empty_notebook_page = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(empty_notebook_page);
  gtk_container_add(GTK_CONTAINER(notebook_subtype), empty_notebook_page);

  label47 = gtk_label_new("");
  gtk_widget_show(label47);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_subtype), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_subtype), 0), label47);

  vbox8 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox8);
  gtk_container_add(GTK_CONTAINER(notebook_subtype), vbox8);

  label50 = gtk_label_new("This one:");
  gtk_widget_show(label50);
  gtk_box_pack_start(GTK_BOX(vbox8), label50, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label50), 0, 0.5);

  GSList *radiobutton_footnote_group = NULL;

  radiobutton_footnote = gtk_radio_button_new_with_mnemonic(NULL, "starts a footnote");
  gtk_widget_show(radiobutton_footnote);
  gtk_box_pack_start(GTK_BOX(vbox8), radiobutton_footnote, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_footnote), radiobutton_footnote_group);
  radiobutton_footnote_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_footnote));

  radiobutton_endnote = gtk_radio_button_new_with_mnemonic(NULL, "starts an endnote");
  gtk_widget_show(radiobutton_endnote);
  gtk_box_pack_start(GTK_BOX(vbox8), radiobutton_endnote, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_endnote), radiobutton_footnote_group);
  radiobutton_footnote_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_endnote));

  radiobutton_note_content_standard = gtk_radio_button_new_with_mnemonic(NULL, "is standard content");
  gtk_widget_show(radiobutton_note_content_standard);
  gtk_box_pack_start(GTK_BOX(vbox8), radiobutton_note_content_standard, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_note_content_standard), radiobutton_footnote_group);
  radiobutton_footnote_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_note_content_standard));

  radiobutton_note_content = gtk_radio_button_new_with_mnemonic(NULL, "is content");
  gtk_widget_show(radiobutton_note_content);
  gtk_box_pack_start(GTK_BOX(vbox8), radiobutton_note_content, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_note_content), radiobutton_footnote_group);
  radiobutton_footnote_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_note_content));

  radiobutton_note_content_endmarker = gtk_radio_button_new_with_mnemonic(NULL, "is content with endmarker");
  gtk_widget_show(radiobutton_note_content_endmarker);
  gtk_box_pack_start(GTK_BOX(vbox8), radiobutton_note_content_endmarker, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_note_content_endmarker), radiobutton_footnote_group);
  radiobutton_footnote_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_note_content_endmarker));

  radiobutton_note_paragraph = gtk_radio_button_new_with_mnemonic(NULL, "starts another paragraph");
  gtk_widget_show(radiobutton_note_paragraph);
  gtk_box_pack_start(GTK_BOX(vbox8), radiobutton_note_paragraph, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_note_paragraph), radiobutton_footnote_group);
  radiobutton_footnote_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_note_paragraph));

  label48 = gtk_label_new("");
  gtk_widget_show(label48);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_subtype), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_subtype), 1), label48);

  vbox9 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox9);
  gtk_container_add(GTK_CONTAINER(notebook_subtype), vbox9);

  label51 = gtk_label_new("This one:");
  gtk_widget_show(label51);
  gtk_box_pack_start(GTK_BOX(vbox9), label51, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label51), 0, 0.5);

  GSList *radiobutton_xref_group = NULL;

  radiobutton_xref = gtk_radio_button_new_with_mnemonic(NULL, "starts a crossreference");
  gtk_widget_show(radiobutton_xref);
  gtk_box_pack_start(GTK_BOX(vbox9), radiobutton_xref, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_xref), radiobutton_xref_group);
  radiobutton_xref_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_xref));

  radiobutton_xref_content_standard = gtk_radio_button_new_with_mnemonic(NULL, "is standard content");
  gtk_widget_show(radiobutton_xref_content_standard);
  gtk_box_pack_start(GTK_BOX(vbox9), radiobutton_xref_content_standard, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_xref_content_standard), radiobutton_xref_group);
  radiobutton_xref_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_xref_content_standard));

  radiobutton_xref_content = gtk_radio_button_new_with_mnemonic(NULL, "is content");
  gtk_widget_show(radiobutton_xref_content);
  gtk_box_pack_start(GTK_BOX(vbox9), radiobutton_xref_content, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_xref_content), radiobutton_xref_group);
  radiobutton_xref_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_xref_content));

  radiobutton_xref_content_endmarker = gtk_radio_button_new_with_mnemonic(NULL, "is content with endmarker");
  gtk_widget_show(radiobutton_xref_content_endmarker);
  gtk_box_pack_start(GTK_BOX(vbox9), radiobutton_xref_content_endmarker, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_xref_content_endmarker), radiobutton_xref_group);
  radiobutton_xref_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_xref_content_endmarker));

  label49 = gtk_label_new("");
  gtk_widget_show(label49);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_subtype), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_subtype), 2), label49);

  vbox13 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox13);
  gtk_container_add(GTK_CONTAINER(notebook_subtype), vbox13);
  gtk_notebook_set_tab_label_packing(GTK_NOTEBOOK(notebook_subtype), vbox13, FALSE, FALSE, GTK_PACK_START);

  label55 = gtk_label_new("This one:");
  gtk_widget_show(label55);
  gtk_box_pack_start(GTK_BOX(vbox13), label55, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label55), 0, 0.5);

  GSList *radiobutton_paragraph_type_main_title_group = NULL;

  radiobutton_paragraph_type_main_title = gtk_radio_button_new_with_mnemonic(NULL, "is a main title");
  gtk_widget_show(radiobutton_paragraph_type_main_title);
  gtk_box_pack_start(GTK_BOX(vbox13), radiobutton_paragraph_type_main_title, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_paragraph_type_main_title), radiobutton_paragraph_type_main_title_group);
  radiobutton_paragraph_type_main_title_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_paragraph_type_main_title));

  radiobutton_paragraph_type_subtitle = gtk_radio_button_new_with_mnemonic(NULL, "is a subtitle");
  gtk_widget_show(radiobutton_paragraph_type_subtitle);
  gtk_box_pack_start(GTK_BOX(vbox13), radiobutton_paragraph_type_subtitle, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_paragraph_type_subtitle), radiobutton_paragraph_type_main_title_group);
  radiobutton_paragraph_type_main_title_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_paragraph_type_subtitle));

  radiobutton_paragraph_type_section_heading = gtk_radio_button_new_with_mnemonic(NULL, "is a section heading");
  gtk_widget_show(radiobutton_paragraph_type_section_heading);
  gtk_box_pack_start(GTK_BOX(vbox13), radiobutton_paragraph_type_section_heading, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_paragraph_type_section_heading), radiobutton_paragraph_type_main_title_group);
  radiobutton_paragraph_type_main_title_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_paragraph_type_section_heading));

  radiobutton_paragraph_type_text = gtk_radio_button_new_with_mnemonic(NULL, "is a normal paragraph");
  gtk_widget_show(radiobutton_paragraph_type_text);
  gtk_box_pack_start(GTK_BOX(vbox13), radiobutton_paragraph_type_text, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_paragraph_type_text), radiobutton_paragraph_type_main_title_group);
  radiobutton_paragraph_type_main_title_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_paragraph_type_text));

  label54 = gtk_label_new("");
  gtk_widget_show(label54);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_subtype), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_subtype), 3), label54);

  vbox14 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox14);
  gtk_container_add(GTK_CONTAINER(notebook_subtype), vbox14);

  labelperiph58 = gtk_label_new("This one:");
  gtk_widget_show(labelperiph58);
  gtk_box_pack_start(GTK_BOX(vbox14), labelperiph58, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(labelperiph58), 0, 0.5);

  GSList *radiobutton_peripheral_pub_group = NULL;

  radiobutton_peripheral_pub = gtk_radio_button_new_with_mnemonic(NULL, "starts publication data");
  gtk_widget_show(radiobutton_peripheral_pub);
  gtk_box_pack_start(GTK_BOX(vbox14), radiobutton_peripheral_pub, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_peripheral_pub), radiobutton_peripheral_pub_group);
  radiobutton_peripheral_pub_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_peripheral_pub));

  radiobutton_peripheral_toc = gtk_radio_button_new_with_mnemonic(NULL, "starts table of contents");
  gtk_widget_show(radiobutton_peripheral_toc);
  gtk_box_pack_start(GTK_BOX(vbox14), radiobutton_peripheral_toc, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_peripheral_toc), radiobutton_peripheral_pub_group);
  radiobutton_peripheral_pub_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_peripheral_toc));

  radiobutton_peripheral_pref = gtk_radio_button_new_with_mnemonic(NULL, "starts preface");
  gtk_widget_show(radiobutton_peripheral_pref);
  gtk_box_pack_start(GTK_BOX(vbox14), radiobutton_peripheral_pref, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_peripheral_pref), radiobutton_peripheral_pub_group);
  radiobutton_peripheral_pub_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_peripheral_pref));

  radiobutton_peripheral_intro = gtk_radio_button_new_with_mnemonic(NULL, "starts introduction");
  gtk_widget_show(radiobutton_peripheral_intro);
  gtk_box_pack_start(GTK_BOX(vbox14), radiobutton_peripheral_intro, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_peripheral_intro), radiobutton_peripheral_pub_group);
  radiobutton_peripheral_pub_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_peripheral_intro));

  radiobutton_peripheral_conc = gtk_radio_button_new_with_mnemonic(NULL, "starts concordance");
  gtk_widget_show(radiobutton_peripheral_conc);
  gtk_box_pack_start(GTK_BOX(vbox14), radiobutton_peripheral_conc, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_peripheral_conc), radiobutton_peripheral_pub_group);
  radiobutton_peripheral_pub_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_peripheral_conc));

  radiobutton_peripheral_glo = gtk_radio_button_new_with_mnemonic(NULL, "starts glossary");
  gtk_widget_show(radiobutton_peripheral_glo);
  gtk_box_pack_start(GTK_BOX(vbox14), radiobutton_peripheral_glo, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_peripheral_glo), radiobutton_peripheral_pub_group);
  radiobutton_peripheral_pub_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_peripheral_glo));

  radiobutton_peripheral_idx = gtk_radio_button_new_with_mnemonic(NULL, "starts index");
  gtk_widget_show(radiobutton_peripheral_idx);
  gtk_box_pack_start(GTK_BOX(vbox14), radiobutton_peripheral_idx, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_peripheral_idx), radiobutton_peripheral_pub_group);
  radiobutton_peripheral_pub_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_peripheral_idx));

  radiobutton_peripheral_maps = gtk_radio_button_new_with_mnemonic(NULL, "starts map index");
  gtk_widget_show(radiobutton_peripheral_maps);
  gtk_box_pack_start(GTK_BOX(vbox14), radiobutton_peripheral_maps, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_peripheral_maps), radiobutton_peripheral_pub_group);
  radiobutton_peripheral_pub_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_peripheral_maps));

  radiobutton_peripheral_cov = gtk_radio_button_new_with_mnemonic(NULL, "starts cover");
  gtk_widget_show(radiobutton_peripheral_cov);
  gtk_box_pack_start(GTK_BOX(vbox14), radiobutton_peripheral_cov, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_peripheral_cov), radiobutton_peripheral_pub_group);
  radiobutton_peripheral_pub_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_peripheral_cov));

  radiobutton_peripheral_spine = gtk_radio_button_new_with_mnemonic(NULL, "starts spine");
  gtk_widget_show(radiobutton_peripheral_spine);
  gtk_box_pack_start(GTK_BOX(vbox14), radiobutton_peripheral_spine, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_peripheral_spine), radiobutton_peripheral_pub_group);
  radiobutton_peripheral_pub_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_peripheral_spine));

  label56 = gtk_label_new("");
  gtk_widget_show(label56);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_subtype), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_subtype), 4), label56);

  vbox15 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox15);
  gtk_container_add(GTK_CONTAINER(notebook_subtype), vbox15);

  label_id_58 = gtk_label_new("This one:");
  gtk_widget_show(label_id_58);
  gtk_box_pack_start(GTK_BOX(vbox15), label_id_58, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_id_58), 0, 0.5);

  GSList *radiobutton_id_book_group = NULL;

  radiobutton_id_book = gtk_radio_button_new_with_mnemonic(NULL, "identifies the book");
  gtk_widget_show(radiobutton_id_book);
  gtk_box_pack_start(GTK_BOX(vbox15), radiobutton_id_book, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_id_book), radiobutton_id_book_group);
  radiobutton_id_book_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_id_book));

  radiobutton_id_encoding = gtk_radio_button_new_with_mnemonic(NULL, "identifies the encoding");
  gtk_widget_show(radiobutton_id_encoding);
  gtk_box_pack_start(GTK_BOX(vbox15), radiobutton_id_encoding, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_id_encoding), radiobutton_id_book_group);
  radiobutton_id_book_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_id_encoding));

  radiobutton_id_comment = gtk_radio_button_new_with_mnemonic(NULL, "is a comment");
  gtk_widget_show(radiobutton_id_comment);
  gtk_box_pack_start(GTK_BOX(vbox15), radiobutton_id_comment, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_id_comment), radiobutton_id_book_group);
  radiobutton_id_book_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_id_comment));

  radiobutton_id_comment_with_endmarker = gtk_radio_button_new_with_mnemonic(NULL, "is a comment with an endmarker");
  gtk_widget_show(radiobutton_id_comment_with_endmarker);
  gtk_box_pack_start(GTK_BOX(vbox15), radiobutton_id_comment_with_endmarker, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_id_comment_with_endmarker), radiobutton_id_book_group);
  radiobutton_id_book_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_id_comment_with_endmarker));

  radiobutton_id_running_header = gtk_radio_button_new_with_mnemonic(NULL, "is a running header");
  gtk_widget_show(radiobutton_id_running_header);
  gtk_box_pack_start(GTK_BOX(vbox15), radiobutton_id_running_header, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_id_running_header), radiobutton_id_book_group);
  radiobutton_id_book_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_id_running_header));

  radiobutton_is_long_toc_text = gtk_radio_button_new_with_mnemonic(NULL, "is long table of contents text");
  gtk_widget_show(radiobutton_is_long_toc_text);
  gtk_box_pack_start(GTK_BOX(vbox15), radiobutton_is_long_toc_text, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_is_long_toc_text), radiobutton_id_book_group);
  radiobutton_id_book_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_is_long_toc_text));

  radiobutton_is_short_toc_text = gtk_radio_button_new_with_mnemonic(NULL, "is short table of contents text");
  gtk_widget_show(radiobutton_is_short_toc_text);
  gtk_box_pack_start(GTK_BOX(vbox15), radiobutton_is_short_toc_text, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_is_short_toc_text), radiobutton_id_book_group);
  radiobutton_id_book_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_is_short_toc_text));

  radiobutton_is_book_abbrev = gtk_radio_button_new_with_mnemonic(NULL, "is the book abbreviation");
  gtk_widget_show(radiobutton_is_book_abbrev);
  gtk_box_pack_start(GTK_BOX(vbox15), radiobutton_is_book_abbrev, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_is_book_abbrev), radiobutton_id_book_group);
  radiobutton_id_book_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_is_book_abbrev));

  radiobutton_is_chapter_label = gtk_radio_button_new_with_mnemonic(NULL, "is the chapter label");
  gtk_widget_show(radiobutton_is_chapter_label);
  gtk_box_pack_start(GTK_BOX(vbox15), radiobutton_is_chapter_label, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_is_chapter_label), radiobutton_id_book_group);
  radiobutton_id_book_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_is_chapter_label));

  radiobutton_is_published_chapter_marker = gtk_radio_button_new_with_mnemonic(NULL, "is the published chapter marker");
  gtk_widget_show(radiobutton_is_published_chapter_marker);
  gtk_box_pack_start(GTK_BOX(vbox15), radiobutton_is_published_chapter_marker, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_is_published_chapter_marker), radiobutton_id_book_group);
  radiobutton_id_book_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_is_published_chapter_marker));

  label57 = gtk_label_new("");
  gtk_widget_show(label57);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_subtype), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_subtype), 5), label57);

  // Here starts the table subtype.
  vbox17 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox17);
  gtk_container_add(GTK_CONTAINER(notebook_subtype), vbox17);

  label_table_sub = gtk_label_new("This one:");
  gtk_widget_show(label_table_sub);
  gtk_box_pack_start(GTK_BOX(vbox17), label_table_sub, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_table_sub), 0, 0.5);

  GSList *radiobutton_table_sub_new_row_group = NULL;

  radiobutton_table_sub_new_row = gtk_radio_button_new_with_mnemonic(NULL, "starts a new row");
  gtk_widget_show(radiobutton_table_sub_new_row);
  gtk_box_pack_start(GTK_BOX(vbox17), radiobutton_table_sub_new_row, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_table_sub_new_row), radiobutton_table_sub_new_row_group);
  radiobutton_table_sub_new_row_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_table_sub_new_row));

  radiobutton_table_sub_heading = gtk_radio_button_new_with_mnemonic(NULL, "is a column heading");
  gtk_widget_show(radiobutton_table_sub_heading);
  gtk_box_pack_start(GTK_BOX(vbox17), radiobutton_table_sub_heading, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_table_sub_heading), radiobutton_table_sub_new_row_group);
  radiobutton_table_sub_new_row_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_table_sub_heading));

  radiobutton_table_sub_cell = gtk_radio_button_new_with_mnemonic(NULL, "is cell data");
  gtk_widget_show(radiobutton_table_sub_cell);
  gtk_box_pack_start(GTK_BOX(vbox17), radiobutton_table_sub_cell, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_table_sub_cell), radiobutton_table_sub_new_row_group);
  radiobutton_table_sub_new_row_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_table_sub_cell));

  label61 = gtk_label_new("");
  gtk_widget_show(label61);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_subtype), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_subtype), 6), label61);
  // Here ends the table subtype.

  // Word list subtype.

  vbox_subtype_wordlist = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox_subtype_wordlist);
  gtk_container_add(GTK_CONTAINER(notebook_subtype), vbox_subtype_wordlist);

  label_subtype_wordlist = gtk_label_new("This one:");
  gtk_widget_show(label_subtype_wordlist);
  gtk_box_pack_start(GTK_BOX(vbox_subtype_wordlist), label_subtype_wordlist, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_subtype_wordlist), 0, 0.5);

  GSList *radiobutton_subtype_wordlist_group = NULL;

  radiobutton_subtype_wordlist_glossary_dictionary_entry = gtk_radio_button_new_with_mnemonic(NULL, "Wordlist / glossary / dictionary entry");
  gtk_widget_show(radiobutton_subtype_wordlist_glossary_dictionary_entry);
  gtk_box_pack_start(GTK_BOX(vbox_subtype_wordlist), radiobutton_subtype_wordlist_glossary_dictionary_entry, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_subtype_wordlist_glossary_dictionary_entry), radiobutton_subtype_wordlist_group);
  radiobutton_subtype_wordlist_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_subtype_wordlist_glossary_dictionary_entry));

  radiobutton_subtype_hebrew_wordlist_entry = gtk_radio_button_new_with_mnemonic(NULL, "Hebrew wordlist entry");
  gtk_widget_show(radiobutton_subtype_hebrew_wordlist_entry);
  gtk_box_pack_start(GTK_BOX(vbox_subtype_wordlist), radiobutton_subtype_hebrew_wordlist_entry, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_subtype_hebrew_wordlist_entry), radiobutton_subtype_wordlist_group);
  radiobutton_subtype_wordlist_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_subtype_hebrew_wordlist_entry));

  radiobutton_subtype_greek_wordlist_entry = gtk_radio_button_new_with_mnemonic(NULL, "Greek wordlist entry");
  gtk_widget_show(radiobutton_subtype_greek_wordlist_entry);
  gtk_box_pack_start(GTK_BOX(vbox_subtype_wordlist), radiobutton_subtype_greek_wordlist_entry, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_subtype_greek_wordlist_entry), radiobutton_subtype_wordlist_group);
  radiobutton_subtype_wordlist_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_subtype_greek_wordlist_entry));

  radiobutton_subtype_subject_index_entry = gtk_radio_button_new_with_mnemonic(NULL, "Subject index entry");
  gtk_widget_show(radiobutton_subtype_subject_index_entry);
  gtk_box_pack_start(GTK_BOX(vbox_subtype_wordlist), radiobutton_subtype_subject_index_entry, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_subtype_subject_index_entry), radiobutton_subtype_wordlist_group);
  radiobutton_subtype_wordlist_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_subtype_subject_index_entry));

  label63 = gtk_label_new("");
  gtk_widget_show(label63);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook_subtype), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook_subtype), 7), label63);

  // Here ends the word list subtype.

  vseparator2 = gtk_vseparator_new();
  gtk_widget_show(vseparator2);
  gtk_box_pack_start(GTK_BOX(hbox8), vseparator2, TRUE, TRUE, 0);

  vbox6 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox6);
  gtk_box_pack_start(GTK_BOX(hbox8), vbox6, TRUE, TRUE, 0);

  // Optional widgets, set them all to NULL.
  hbox2 = NULL;
  checkbutton_italic = NULL;
  checkbutton_bold = NULL;
  checkbutton_underline = NULL;
  checkbutton_small_caps = NULL;
  table2 = NULL;
  checkbutton_superscript = NULL;
  vbox4 = NULL;
  checkbutton_apocrypha = NULL;
  checkbutton_span = NULL;
  vbox10 = NULL;
  hbox10 = NULL;
  print_chapter_at_first_verse = NULL;
  hbox_id = NULL;
  hbox_colour = NULL;
  checkbutton_print = NULL;
  hbox13 = NULL;
  hbox15 = NULL;
  checkbutton_print_in_running_header_left = NULL;
  hbox16 = NULL;
  checkbutton_restarts_paragraph = NULL;

  dialog_action_area1 = GTK_DIALOG(stylesheetdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(stylesheetdialog, NULL, NULL, NULL);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(stylesheetdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(stylesheetdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) radiobutton_id, "toggled", G_CALLBACK(on_style_type_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_paragraph, "toggled", G_CALLBACK(on_style_type_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_inline, "toggled", G_CALLBACK(on_style_type_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_chapter, "toggled", G_CALLBACK(on_style_type_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_verse, "toggled", G_CALLBACK(on_style_type_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_footendnote, "toggled", G_CALLBACK(on_style_type_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_crossreference, "toggled", G_CALLBACK(on_style_type_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_peripheral, "toggled", G_CALLBACK(on_style_type_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_picture, "toggled", G_CALLBACK(on_style_type_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_pagebreak, "toggled", G_CALLBACK(on_style_type_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_table_element, "toggled", G_CALLBACK(on_style_type_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_wordlist_element, "toggled", G_CALLBACK(on_style_type_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_footnote, "toggled", G_CALLBACK(on_radiobutton_note_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_endnote, "toggled", G_CALLBACK(on_radiobutton_note_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_note_content_standard, "toggled", G_CALLBACK(on_radiobutton_note_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_note_content, "toggled", G_CALLBACK(on_radiobutton_note_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_note_content_endmarker, "toggled", G_CALLBACK(on_radiobutton_note_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_note_paragraph, "toggled", G_CALLBACK(on_radiobutton_note_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_xref, "toggled", G_CALLBACK(on_radiobutton_xref_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_xref_content_standard, "toggled", G_CALLBACK(on_radiobutton_xref_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_xref_content, "toggled", G_CALLBACK(on_radiobutton_xref_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_xref_content_endmarker, "toggled", G_CALLBACK(on_radiobutton_xref_toggled), gpointer(this));

  g_signal_connect((gpointer) radiobutton_id_book, "toggled", G_CALLBACK(on_radiobutton_identifier_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_id_encoding, "toggled", G_CALLBACK(on_radiobutton_identifier_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_id_comment, "toggled", G_CALLBACK(on_radiobutton_identifier_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_id_running_header, "toggled", G_CALLBACK(on_radiobutton_identifier_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_is_long_toc_text, "toggled", G_CALLBACK(on_radiobutton_identifier_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_is_short_toc_text, "toggled", G_CALLBACK(on_radiobutton_identifier_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_is_book_abbrev, "toggled", G_CALLBACK(on_radiobutton_identifier_toggled), gpointer(this));

  g_signal_connect((gpointer) radiobutton_table_sub_new_row, "toggled", G_CALLBACK(on_checkbutton_table_element_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_table_sub_heading, "toggled", G_CALLBACK(on_checkbutton_table_element_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_table_sub_cell, "toggled", G_CALLBACK(on_checkbutton_table_element_toggled), gpointer(this));

  g_signal_connect((gpointer) radiobutton_subtype_wordlist_glossary_dictionary_entry, "toggled", G_CALLBACK(on_radiobutton_subtype_wordlist_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_subtype_hebrew_wordlist_entry, "toggled", G_CALLBACK(on_radiobutton_subtype_wordlist_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_subtype_greek_wordlist_entry, "toggled", G_CALLBACK(on_radiobutton_subtype_wordlist_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_subtype_subject_index_entry, "toggled", G_CALLBACK(on_radiobutton_subtype_wordlist_toggled), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label37), entryname);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label36), textview1);

  gtk_widget_grab_default(okbutton);

  // Display the style.
  set_gui();

  new DialogAutoScaler (stylesheetdialog, G_MAXINT);
}


StylesheetDialog::~StylesheetDialog()
{
  gtk_widget_destroy(stylesheetdialog);
}


int StylesheetDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(stylesheetdialog));
}


void StylesheetDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_ok();
}


void StylesheetDialog::on_ok()
{
  save_style();
  name = gtk_entry_get_text(GTK_ENTRY(entryname));
}


void StylesheetDialog::on_style_type_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_style_type(togglebutton);
}


void StylesheetDialog::on_style_type(GtkToggleButton * togglebutton)
{
  // Set this togglebutton as the widget that gets activated on the shortcut.
  gtk_label_set_mnemonic_widget(GTK_LABEL(label21), GTK_WIDGET(togglebutton));
  // Destroy any optional widgets.
  destroy_optional_widgets();
  // Get the type of the style.
  StyleType type = get_styletype();
  switch (type) {
  case stIdentifier:
  case stNotUsedComment:
  case stNotUsedRunningHeader:
    {
      // Notebook subtype
      // 0: hidden
      // 1: footnote/endnote
      // 2: crossreference
      // 3: paragraph
      // 4: peripheral
      // 5: identifier
      gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 5);
      on_radiobutton_identifier();
      break;
    }
  case stStartsParagraph:
    {
      gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 3);
      fontsize_points_create();
      italic_simple_create();
      bold_simple_create();
      underline_simple_create();
      smallcaps_simple_create();
      paragraph_create(NULL, false);
      span_columns_create();
      break;
    }
  case stInlineText:
    {
      gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 0);
      italic_bold_underline_smallcaps_extended_create();
      colour_create();
      superscript_create();
      break;
    }
  case stChapterNumber:
    {
      gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 0);
      fontsize_points_create();
      italic_simple_create();
      bold_simple_create();
      underline_simple_create();
      smallcaps_simple_create();
      paragraph_create("chapter", false);
      span_columns_create();
      print_chapter_at_first_verse_create();
      print_in_running_header_create();
      break;
    }
  case stVerseNumber:
    {
      gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 0);
      italic_bold_underline_smallcaps_extended_create();
      colour_create();
      superscript_create();
      restarts_paragraph_create();
      print_create();
      break;
    }
  case stFootEndNote:
    {
      on_radiobutton_note();
      break;
    }
  case stCrossreference:
    {
      on_radiobutton_xref();
      break;
    }
  case stPeripheral:
    {
      gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 4);
      break;
    }
  case stPicture:
    {
      gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 0);
      break;
    }
  case stPageBreak:
    {
      gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 0);
      break;
    }
  case stTableElement:
    {
      gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 6);
      break;
    }
  case stWordlistElement:
    {
      gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 7);
      wordlist_add_text_create();
      break;
    }
  }
}


void StylesheetDialog::on_alignment_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_alignment(togglebutton);
}


void StylesheetDialog::on_alignment(GtkToggleButton * togglebutton)
{
  // Set this togglebutton as the widget that gets activated on the shortcut.
  gtk_label_set_mnemonic_widget(GTK_LABEL(label10), GTK_WIDGET(togglebutton));
}


void StylesheetDialog::set_gui()
{
  // Get all available styles in this stylesheet.
  vector < Style > styles;
  stylesheet_get_styles(mystylesheet, styles);
  // Get the style we now display.
  int pointer = stylesheet_style_get_pointer(styles, mystyle);
  if (pointer >= 0) {
    // Store values in the object.
    fontsize_points = styles[pointer].fontsize;
    italic = styles[pointer].italic;
    bold = styles[pointer].bold;
    underline = styles[pointer].underline;
    smallcaps = styles[pointer].smallcaps;
    superscript = styles[pointer].superscript;
    justification = styles[pointer].justification;
    spacebefore = styles[pointer].spacebefore;
    spaceafter = styles[pointer].spaceafter;
    leftmargin = styles[pointer].leftmargin;
    rightmargin = styles[pointer].rightmargin;
    firstlineindent = styles[pointer].firstlineindent;
    spancolumns = styles[pointer].spancolumns;
    color = styles[pointer].color;
    print = styles[pointer].print;
    userbool1 = styles[pointer].userbool1;
    userbool2 = styles[pointer].userbool2;
    userbool3 = styles[pointer].userbool3;
    userint1 = styles[pointer].userint1;
    userint2 = styles[pointer].userint2;
    userint3 = styles[pointer].userint3;
    userstring1 = styles[pointer].userstring1;
    userstring2 = styles[pointer].userstring2;
    userstring3 = styles[pointer].userstring3;
    // Display information about this style.
    ustring info;
    info = "\\" + mystyle;
    gtk_label_set_text(GTK_LABEL(labelstyle), info.c_str());
    gtk_entry_set_text(GTK_ENTRY(entryname), styles[pointer].name.c_str());
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1)), styles[pointer].info.c_str(), -1);
    // Set publication information.
    GtkWidget *togglebutton = NULL;
    switch (styles[pointer].type) {
    case stIdentifier:
    case stNotUsedComment:
    case stNotUsedRunningHeader:
      togglebutton = radiobutton_id;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_id), true);
      gtk_label_set_mnemonic_widget(GTK_LABEL(label21), radiobutton_id);
      break;
    case stStartsParagraph:
      togglebutton = radiobutton_paragraph;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_paragraph), true);
      gtk_label_set_mnemonic_widget(GTK_LABEL(label21), radiobutton_paragraph);
      break;
    case stInlineText:
      togglebutton = radiobutton_inline;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_inline), true);
      gtk_label_set_mnemonic_widget(GTK_LABEL(label21), radiobutton_inline);
      break;
    case stChapterNumber:
      togglebutton = radiobutton_chapter;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_chapter), true);
      gtk_label_set_mnemonic_widget(GTK_LABEL(label21), radiobutton_chapter);
      break;
    case stVerseNumber:
      togglebutton = radiobutton_verse;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_verse), true);
      gtk_label_set_mnemonic_widget(GTK_LABEL(label21), radiobutton_verse);
      break;
    case stFootEndNote:
      togglebutton = radiobutton_footendnote;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_footendnote), true);
      gtk_label_set_mnemonic_widget(GTK_LABEL(label21), radiobutton_footendnote);
      break;
    case stCrossreference:
      togglebutton = radiobutton_crossreference;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_crossreference), true);
      gtk_label_set_mnemonic_widget(GTK_LABEL(label21), radiobutton_crossreference);
      break;
    case stPeripheral:
      togglebutton = radiobutton_peripheral;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral), true);
      gtk_label_set_mnemonic_widget(GTK_LABEL(label21), radiobutton_peripheral);
      break;
    case stPicture:
      togglebutton = radiobutton_picture;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_picture), true);
      gtk_label_set_mnemonic_widget(GTK_LABEL(label21), radiobutton_picture);
      break;
    case stPageBreak:
      togglebutton = radiobutton_pagebreak;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_pagebreak), true);
      gtk_label_set_mnemonic_widget(GTK_LABEL(label21), radiobutton_pagebreak);
      break;
    case stTableElement:
      togglebutton = radiobutton_table_element;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_table_element), true);
      gtk_label_set_mnemonic_widget(GTK_LABEL(label21), radiobutton_table_element);
      break;
    case stWordlistElement:
      togglebutton = radiobutton_wordlist_element;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_wordlist_element), true);
      gtk_label_set_mnemonic_widget(GTK_LABEL(label21), radiobutton_wordlist_element);
      break;
    }
    if (togglebutton) {
      on_style_type(GTK_TOGGLE_BUTTON(togglebutton));
      set_subtype(styles[pointer].type, styles[pointer].subtype);
    }
  }
}


void StylesheetDialog::save_style()
{
  // Destroy relevant widgets, so saving their value.
  destroy_optional_widgets();
  // Get the type of this style.
  StyleType type = get_styletype();
  // Get remaining value of this style.
  ustring info;
  GtkTextBuffer *buffer;
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
  GtkTextIter startiter;
  GtkTextIter enditer;
  gtk_text_buffer_get_start_iter(buffer, &startiter);
  gtk_text_buffer_get_end_iter(buffer, &enditer);
  info = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &startiter, &enditer, false);
  info = trim(info);
  // Save this style.
  stylesheet_save_style(mystylesheet, mystyle, gtk_entry_get_text(GTK_ENTRY(entryname)), info, type, get_subtype(), fontsize_points, italic, bold, underline, smallcaps, superscript, justification, spacebefore, spaceafter, leftmargin, rightmargin, firstlineindent, spancolumns, color, print, userbool1, userbool2, userbool3, userint1, userint2, userint3, userstring1, userstring2, userstring3);
}


void StylesheetDialog::set_justification(const ustring & justification)
// Sets the GUI.
{
  if (justification == CENTER) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_center), true);
    on_alignment(GTK_TOGGLE_BUTTON(radiobutton_center));
  } else if (justification == RIGHT) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_right), true);
    on_alignment(GTK_TOGGLE_BUTTON(radiobutton_right));
  } else if (justification == JUSTIFIED) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_full), true);
    on_alignment(GTK_TOGGLE_BUTTON(radiobutton_full));
  } else {
    // Defaults to left.
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_left), true);
    on_alignment(GTK_TOGGLE_BUTTON(radiobutton_left));
  }
}


ustring StylesheetDialog::get_justification()
// Get the string that indicates the justification, from the GUI.
{
  ustring result;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_center))) {
    result = CENTER;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_right))) {
    result = RIGHT;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_full))) {
    result = JUSTIFIED;
  } else {
    // Defaults to left.
    result = LEFT;
  }
  return result;
}


void StylesheetDialog::set_italic()
{
  if (italic == ON) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_italics_on), true);
    on_radiobutton_italic(radiobutton_italics_on);
  } else if (italic == INHERIT) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_italics_inherit), true);
    on_radiobutton_italic(radiobutton_italics_inherit);
  } else if (italic == TOGGLE) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_italics_toggle), true);
    on_radiobutton_italic(radiobutton_italics_toggle);
  } else {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_italics_off), true);
    on_radiobutton_italic(radiobutton_italics_off);
  }
}


ustring StylesheetDialog::get_italic()
{
  // Defaults to off.
  ustring result = OFF;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_italics_on))) {
    result = ON;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_italics_inherit))) {
    result = INHERIT;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_italics_toggle))) {
    result = TOGGLE;
  }
  return result;
}


void StylesheetDialog::set_bold()
{
  if (bold == ON) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_bold_on), true);
    on_radiobutton_bold(radiobutton_bold_on);
  } else if (bold == INHERIT) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_bold_inherit), true);
    on_radiobutton_bold(radiobutton_bold_inherit);
  } else if (bold == TOGGLE) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_bold_toggle), true);
    on_radiobutton_bold(radiobutton_bold_toggle);
  } else {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_bold_off), true);
    on_radiobutton_bold(radiobutton_bold_off);
  }
}


ustring StylesheetDialog::get_bold()
{
  // Defaults to off.
  ustring result = OFF;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_bold_on))) {
    result = ON;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_bold_inherit))) {
    result = INHERIT;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_bold_toggle))) {
    result = TOGGLE;
  }
  return result;
}


void StylesheetDialog::set_underline()
{
  if (underline == ON) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_underline_on), true);
    on_radiobutton_underline(radiobutton_underline_on);
  } else if (underline == INHERIT) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_underline_inherit), true);
    on_radiobutton_underline(radiobutton_underline_inherit);
  } else if (underline == TOGGLE) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_underline_toggle), true);
    on_radiobutton_underline(radiobutton_underline_toggle);
  } else {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_underline_off), true);
    on_radiobutton_underline(radiobutton_underline_off);
  }
}


ustring StylesheetDialog::get_underline()
{
  // Defaults to off.
  ustring result = OFF;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_underline_on))) {
    result = ON;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_underline_inherit))) {
    result = INHERIT;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_underline_toggle))) {
    result = TOGGLE;
  }
  return result;
}


void StylesheetDialog::set_small_caps()
{
  if (smallcaps == ON) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_small_caps_on), true);
    on_radiobutton_small_caps(radiobutton_small_caps_on);
  } else if (smallcaps == INHERIT) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_small_caps_inherit), true);
    on_radiobutton_small_caps(radiobutton_small_caps_inherit);
  } else if (smallcaps == TOGGLE) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_small_caps_toggle), true);
    on_radiobutton_small_caps(radiobutton_small_caps_toggle);
  } else {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_small_caps_off), true);
    on_radiobutton_small_caps(radiobutton_small_caps_off);
  }
}


ustring StylesheetDialog::get_small_caps()
{
  // Defaults to off.
  ustring result = OFF;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_small_caps_on))) {
    result = ON;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_small_caps_inherit))) {
    result = INHERIT;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_small_caps_toggle))) {
    result = TOGGLE;
  }
  return result;
}


StyleType StylesheetDialog::get_styletype()
{
  StyleType type = stNotUsedComment;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_id))) {
    type = stIdentifier;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_paragraph))) {
    type = stStartsParagraph;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_inline))) {
    type = stInlineText;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_chapter))) {
    type = stChapterNumber;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_verse))) {
    type = stVerseNumber;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_footendnote))) {
    type = stFootEndNote;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_crossreference))) {
    type = stCrossreference;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral))) {
    type = stPeripheral;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_picture))) {
    type = stPicture;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_pagebreak))) {
    type = stPageBreak;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_table_element))) {
    type = stTableElement;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_wordlist_element))) {
    type = stWordlistElement;
  }
  return type;
}


void StylesheetDialog::on_radiobutton_italic_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_radiobutton_italic(GTK_WIDGET(togglebutton));
}


void StylesheetDialog::on_radiobutton_italic(GtkWidget * togglebutton)
{
  gtk_label_set_mnemonic_widget(GTK_LABEL(label40), togglebutton);
}


void StylesheetDialog::on_radiobutton_bold_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_radiobutton_bold(GTK_WIDGET(togglebutton));
}


void StylesheetDialog::on_radiobutton_bold(GtkWidget * togglebutton)
{
  gtk_label_set_mnemonic_widget(GTK_LABEL(label41), togglebutton);
}


void StylesheetDialog::on_radiobutton_underline_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_radiobutton_underline(GTK_WIDGET(togglebutton));
}


void StylesheetDialog::on_radiobutton_underline(GtkWidget * togglebutton)
{
  gtk_label_set_mnemonic_widget(GTK_LABEL(label42), togglebutton);
}


void StylesheetDialog::on_radiobutton_small_caps_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_radiobutton_small_caps(GTK_WIDGET(togglebutton));
}


void StylesheetDialog::on_radiobutton_small_caps(GtkWidget * togglebutton)
{
  gtk_label_set_mnemonic_widget(GTK_LABEL(label_small_caps), togglebutton);
}


void StylesheetDialog::set_subtype(StyleType maintype, int subtype)
{
  switch (maintype) {
  case stIdentifier:
  case stNotUsedComment:
  case stNotUsedRunningHeader:
    {
      IdentifierType identifiertype = (IdentifierType) subtype;
      switch (identifiertype) {
      case itBook:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_id_book), true);
        break;
      case itEncoding:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_id_encoding), true);
        break;
      case itComment:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_id_comment), true);
        break;
      case itRunningHeader:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_id_running_header), true);
        break;
      case itLongTOC:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_is_long_toc_text), true);
        break;
      case itShortTOC:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_is_short_toc_text), true);
        break;
      case itBookAbbrev:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_is_book_abbrev), true);
        break;
      case itChapterLabel:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_is_chapter_label), true);
        break;
      case itPublishedChapterMarker:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_is_published_chapter_marker), true);
        break;
      case itCommentWithEndmarker:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_id_comment_with_endmarker), true);
        break;
      }
      break;
    }
  case stStartsParagraph:
    {
      ParagraphType paragraphtype = (ParagraphType) subtype;
      switch (paragraphtype) {
      case ptMainTitle:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_paragraph_type_main_title), true);
        break;
      case ptSubTitle:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_paragraph_type_subtitle), true);
        break;
      case ptSectionHeading:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_paragraph_type_section_heading), true);
        break;
      case ptNormalParagraph:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_paragraph_type_text), true);
        break;
      }
      break;
    }
  case stInlineText:
    break;
  case stChapterNumber:
    break;
  case stVerseNumber:
    break;
  case stFootEndNote:
    {
      FootEndNoteType footnotetype = (FootEndNoteType) subtype;
      switch (footnotetype) {
      case fentFootnote:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_footnote), true);
        break;
      case fentEndnote:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_endnote), true);
        break;
      case fentStandardContent:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_note_content_standard), true);
        break;
      case fentContent:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_note_content), true);
        break;
      case fentContentWithEndmarker:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_note_content_endmarker), true);
        break;
      case fentParagraph:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_note_paragraph), true);
        break;
      }
      break;
    }
  case stCrossreference:
    {
      CrossreferenceType crossreferencetype = (CrossreferenceType) subtype;
      switch (crossreferencetype) {
      case ctCrossreference:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_xref), true);
        break;
      case ctStandardContent:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_xref_content_standard), true);
        break;
      case ctContent:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_xref_content), true);
        break;
      case ctContentWithEndmarker:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_xref_content_endmarker), true);
        break;
      }
      break;
    }
  case stPeripheral:
    {
      PeripheralType peripheraltype = (PeripheralType) subtype;
      switch (peripheraltype) {
      case ptPublication:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_pub), true);
        break;
      case ptTableOfContents:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_toc), true);
        break;
      case ptPreface:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_pref), true);
        break;
      case ptIntroduction:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_intro), true);
        break;
      case ptGlossary:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_conc), true);
        break;
      case ptConcordance:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_glo), true);
        break;
      case ptIndex:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_idx), true);
        break;
      case ptMapIndex:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_maps), true);
        break;
      case ptCover:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_cov), true);
        break;
      case ptSpine:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_spine), true);
        break;
      }
      break;
    }
  case stPicture:
    break;
  case stPageBreak:
    break;
  case stTableElement:
    {
      TableElementType table_element_type = (TableElementType) subtype;
      switch (table_element_type) {
      case tetRow:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_table_sub_new_row), true);
        break;
      case tetHeading:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_table_sub_heading), true);
        break;
      case tetCell:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_table_sub_cell), true);
        break;
      }
      break;
    }
  case stWordlistElement:
    {
      WordListElementType wordlist_element_type = (WordListElementType) subtype;
      switch (wordlist_element_type) {
      case wltWordlistGlossaryDictionary:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_subtype_wordlist_glossary_dictionary_entry), true);
        break;
      case wltHebrewWordlistEntry:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_subtype_hebrew_wordlist_entry), true);
        break;
      case wltGreekWordlistEntry:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_subtype_greek_wordlist_entry), true);
        break;
      case wltSubjectIndexEntry:
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_subtype_subject_index_entry), true);
        break;
      }
      break;
    }
    break;
  }
}


int StylesheetDialog::get_subtype()
{
  int subtype = 0;
  switch (gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook_subtype))) {
  case 0:
    // Subtype is not relevant here.
    break;
  case 1:
    // Get subtype for footnotes and endnotes.
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_footnote)))
      subtype = fentFootnote;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_endnote)))
      subtype = fentEndnote;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_note_content)))
      subtype = fentContent;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_note_content_standard)))
      subtype = fentStandardContent;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_note_content_endmarker)))
      subtype = fentContentWithEndmarker;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_note_paragraph)))
      subtype = fentParagraph;
    break;
  case 2:
    // Get subtype for crossreferences.
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_xref)))
      subtype = ctCrossreference;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_xref_content_standard)))
      subtype = ctStandardContent;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_xref_content)))
      subtype = ctContent;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_xref_content_endmarker)))
      subtype = ctContentWithEndmarker;
    break;
  case 3:
    // Get subtype for paragraphs.
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_paragraph_type_main_title)))
      subtype = ptMainTitle;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_paragraph_type_subtitle)))
      subtype = ptSubTitle;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_paragraph_type_section_heading)))
      subtype = ptSectionHeading;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_paragraph_type_text)))
      subtype = ptNormalParagraph;
    break;
  case 4:
    // Get subtype for peripherals.
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_pub)))
      subtype = ptPublication;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_toc)))
      subtype = ptTableOfContents;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_pref)))
      subtype = ptPreface;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_intro)))
      subtype = ptIntroduction;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_glo)))
      subtype = ptGlossary;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_conc)))
      subtype = ptConcordance;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_idx)))
      subtype = ptIndex;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_maps)))
      subtype = ptMapIndex;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_cov)))
      subtype = ptCover;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_peripheral_spine)))
      subtype = ptSpine;
    break;
  case 5:
    // Get subtype for identifiers.
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_id_book)))
      subtype = itBook;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_id_encoding)))
      subtype = itEncoding;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_id_comment)))
      subtype = itComment;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_id_running_header)))
      subtype = itRunningHeader;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_is_long_toc_text)))
      subtype = itLongTOC;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_is_short_toc_text)))
      subtype = itShortTOC;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_is_book_abbrev)))
      subtype = itBookAbbrev;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_is_chapter_label)))
      subtype = itChapterLabel;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_is_published_chapter_marker)))
      subtype = itPublishedChapterMarker;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_id_comment_with_endmarker)))
      subtype = itCommentWithEndmarker;
    break;
  case 6:
    // Get subtype for table elements.
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_table_sub_new_row)))
      subtype = tetRow;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_table_sub_heading)))
      subtype = tetHeading;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_table_sub_cell)))
      subtype = tetCell;
    break;
  case 7:
    // Get subtype for wordlist elements.
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_subtype_wordlist_glossary_dictionary_entry)))
      subtype = wltWordlistGlossaryDictionary;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_subtype_hebrew_wordlist_entry)))
      subtype = wltHebrewWordlistEntry;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_subtype_greek_wordlist_entry)))
      subtype = wltGreekWordlistEntry;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_subtype_subject_index_entry)))
      subtype = wltSubjectIndexEntry;
    break;
  }
  return subtype;
}


void StylesheetDialog::on_radiobutton_note_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_radiobutton_note();
}


void StylesheetDialog::on_radiobutton_note()
{
  // Destroy any optional widgets.
  destroy_optional_widgets();
  // Show/hide notebooks with visual settings.
  FootEndNoteType subtype = (FootEndNoteType) get_subtype();
  switch (subtype) {
  case fentFootnote:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 1);
    italic_bold_underline_smallcaps_extended_create();
    superscript_create();
    note_numbering_type_create();
    note_numering_restart_create();
    print_create();
    break;
  case fentEndnote:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 1);
    italic_bold_underline_smallcaps_extended_create();
    superscript_create();
    note_numbering_type_create();
    end_note_placement_create();
    print_create();
    break;
  case fentStandardContent:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 1);
    fontsize_points_create();
    italic_simple_create();
    bold_simple_create();
    underline_simple_create();
    smallcaps_simple_create();
    superscript_create();
    paragraph_create(NULL, true);
    apocrypha_create();
    break;
  case fentContent:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 1);
    italic_bold_underline_smallcaps_extended_create();
    colour_create();
    superscript_create();
    apocrypha_create();
    break;
  case fentContentWithEndmarker:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 1);
    italic_bold_underline_smallcaps_extended_create();
    colour_create();
    superscript_create();
    apocrypha_create();
    break;
  case fentParagraph:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 1);
    fontsize_points_create();
    italic_simple_create();
    bold_simple_create();
    underline_simple_create();
    smallcaps_simple_create();
    superscript_create();
    paragraph_create(NULL, true);
    apocrypha_create();
    break;
  }
}


void StylesheetDialog::on_radiobutton_xref_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_radiobutton_xref();
}


void StylesheetDialog::on_radiobutton_xref()
{
  // Destroy any optional widgets.
  destroy_optional_widgets();
  // Show/hide notebooks with visual settings.
  CrossreferenceType subtype = (CrossreferenceType) get_subtype();
  switch (subtype) {
  case ctCrossreference:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 2);
    italic_bold_underline_smallcaps_extended_create();
    superscript_create();
    note_numbering_type_create();
    note_numering_restart_create();
    print_create();
    break;
  case ctStandardContent:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 2);
    fontsize_points_create();
    italic_simple_create();
    bold_simple_create();
    underline_simple_create();
    smallcaps_simple_create();
    superscript_create();
    paragraph_create(NULL, true);
    apocrypha_create();
    break;
  case ctContent:
  case ctContentWithEndmarker:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 2);
    italic_bold_underline_smallcaps_extended_create();
    colour_create();
    apocrypha_create();
    break;
  }
}


void StylesheetDialog::destroy_optional_widgets()
/*
 Store the values of the optional widgets, and destroy them.

 General strategy.
 - If a widget is not NULL, it means it is in use and we need to handle it.
 - Update all spin buttons to get the correct value in them after manual edit.
 If this were not done, manual changes in a spinbutton might get lost.
 - Extract the value and store it.
 - Destroy the top widget, which will destroy all its children as well.
 - Set the top widget to NULL.

 */
{
  // Fontsize in points.
  if (hbox2) {
    gtk_spin_button_update(GTK_SPIN_BUTTON(spinbuttonfontsize));
    fontsize_points = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbuttonfontsize));
    gtk_widget_destroy(hbox2);
    hbox2 = NULL;
  }
  // Simple italic;
  if (checkbutton_italic) {
    italic = OFF;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_italic)))
      italic = ON;
    gtk_widget_destroy(checkbutton_italic);
    checkbutton_italic = NULL;
  }
  // Simple bold;
  if (checkbutton_bold) {
    bold = OFF;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_bold)))
      bold = ON;
    gtk_widget_destroy(checkbutton_bold);
    checkbutton_bold = NULL;
  }
  // Simple underline;
  if (checkbutton_underline) {
    underline = OFF;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_underline)))
      underline = ON;
    gtk_widget_destroy(checkbutton_underline);
    checkbutton_underline = NULL;
  }
  // Simple small caps;
  if (checkbutton_small_caps) {
    smallcaps = OFF;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_small_caps)))
      smallcaps = ON;
    gtk_widget_destroy(checkbutton_small_caps);
    checkbutton_small_caps = NULL;
  }
  // Extended italic/bold/underline/small caps.
  if (table2) {
    italic = get_italic();
    bold = get_bold();
    underline = get_underline();
    smallcaps = get_small_caps();
    gtk_widget_destroy(table2);
    table2 = NULL;
  }
  // Superscript
  if (checkbutton_superscript) {
    superscript = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_superscript));
    gtk_widget_destroy(checkbutton_superscript);
    checkbutton_superscript = NULL;
  }
  // Paragraph
  if (vbox4) {
    gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton_before));
    gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton_after));
    gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton_left));
    gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton_right));
    gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton_first));
    justification = get_justification();
    spacebefore = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_before));
    spaceafter = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_after));
    leftmargin = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_left));
    rightmargin = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_right));
    firstlineindent = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_first));
    gtk_widget_destroy(vbox4);
    vbox4 = NULL;
  }
  // Span columns
  if (checkbutton_span) {
    spancolumns = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_span)), gtk_widget_destroy(checkbutton_span);
    checkbutton_span = NULL;
  }
  // Apocrypha
  if (checkbutton_apocrypha) {
    userbool1 = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_apocrypha));
    gtk_widget_destroy(checkbutton_apocrypha);
    checkbutton_apocrypha = NULL;
  }
  // Note numbering type
  if (vbox10) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_note_numbering_1)))
      userint1 = nntNumerical;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_note_numbering_a)))
      userint1 = nntAlphabetical;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_note_numbering_user)))
      userint1 = nntUserDefined;
    userstring1 = gtk_entry_get_text(GTK_ENTRY(entry_note_numbering));
    gtk_widget_destroy(vbox10);
    vbox10 = NULL;
  }
  // Note numbering restart
  if (hbox10) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_note_numbering_restart_never)))
      userint2 = nnrtNever;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_note_numbering_restart_book)))
      userint2 = nnrtBook;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_note_numbering_restart_chapter)))
      userint2 = nnrtChapter;
    gtk_widget_destroy(hbox10);
    hbox10 = NULL;
  }
  // Printing of chapter number at first verse.
  if (print_chapter_at_first_verse) {
    userbool1 = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(print_chapter_at_first_verse));
    gtk_widget_destroy(print_chapter_at_first_verse);
    print_chapter_at_first_verse = NULL;
  }
  // Whether and how to start a new book on a new page.
  if (hbox_id) {
    userbool1 = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_id_newpage));
    userbool2 = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_id_oddpage));
    gtk_widget_destroy(hbox_id);
    hbox_id = NULL;
  }
  // Save the color.
  if (hbox_colour) {
    GdkColor colour;
    gtk_color_button_get_color(GTK_COLOR_BUTTON(button_colour), &colour);
    color = color_gdk_to_decimal(&colour);
    gtk_widget_destroy(hbox_colour);
    hbox_colour = NULL;
  }
  // Save the print property.
  if (checkbutton_print) {
    print = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_print));
    gtk_widget_destroy(checkbutton_print);
    checkbutton_print = NULL;
  }
  // Endnote placement: save.
  if (hbox13) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_print_endnotes_after_book)))
      userint2 = eptAfterBook;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_print_endnotes_after_everything_else)))
      userint2 = eptAfterEverything;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_print_endnotes_at_marker)))
      userint2 = eptAtMarker;
    userstring2 = gtk_entry_get_text(GTK_ENTRY(entry_print_endnotes_marker));
    gtk_widget_destroy(hbox13);
    hbox13 = NULL;
  }
  // Table column number: save.
  if (hbox15) {
    userint1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton_column_number));
    gtk_widget_destroy(hbox15);
    hbox15 = NULL;
  }
  // Printing of chapter number in running headers.
  if (checkbutton_print_in_running_header_left) {
    userbool2 = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_print_in_running_header_left));
    gtk_widget_destroy(checkbutton_print_in_running_header_left);
    checkbutton_print_in_running_header_left = NULL;
    userbool3 = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_print_in_running_header_right));
    gtk_widget_destroy(checkbutton_print_in_running_header_right);
  }
  // Text added after wordlist entry.
  if (hbox16) {
    userstring1 = gtk_entry_get_text(GTK_ENTRY(entry_wordlist_addition));
    gtk_widget_destroy(hbox16);
    hbox16 = NULL;
  }
  // Save the restart-paragraph property.
  if (checkbutton_restarts_paragraph) {
    userbool1 = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_restarts_paragraph));
    gtk_widget_destroy(checkbutton_restarts_paragraph);
    checkbutton_restarts_paragraph = NULL;
  }
}


void StylesheetDialog::fontsize_points_create()
{
  hbox2 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox2);
  gtk_box_pack_start(GTK_BOX(vbox6), hbox2, TRUE, TRUE, 0);

  label8 = gtk_label_new_with_mnemonic("_Font size");
  gtk_widget_show(label8);
  gtk_box_pack_start(GTK_BOX(hbox2), label8, FALSE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label8), 4, 0);

  spinbuttonfontsize_adj = gtk_adjustment_new(12, 5, 60, 0.1, 1, 0);
  spinbuttonfontsize = gtk_spin_button_new(GTK_ADJUSTMENT(spinbuttonfontsize_adj), 1, 1);
  gtk_widget_show(spinbuttonfontsize);
  gtk_box_pack_start(GTK_BOX(hbox2), spinbuttonfontsize, FALSE, FALSE, 0);

  label9 = gtk_label_new("points");
  gtk_widget_show(label9);
  gtk_box_pack_start(GTK_BOX(hbox2), label9, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label9), 0, 0.5);
  gtk_misc_set_padding(GTK_MISC(label9), 4, 0);

  gtk_label_set_mnemonic_widget(GTK_LABEL(label8), spinbuttonfontsize);

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbuttonfontsize), fontsize_points);
}


void StylesheetDialog::italic_simple_create()
{
  checkbutton_italic = gtk_check_button_new_with_mnemonic("_Italic");
  gtk_widget_show(checkbutton_italic);
  gtk_box_pack_start(GTK_BOX(vbox6), checkbutton_italic, FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_italic), italic == ON);
}


void StylesheetDialog::bold_simple_create()
{
  checkbutton_bold = gtk_check_button_new_with_mnemonic("_Bold");
  gtk_widget_show(checkbutton_bold);
  gtk_box_pack_start(GTK_BOX(vbox6), checkbutton_bold, FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_bold), bold == ON);
}


void StylesheetDialog::underline_simple_create()
{
  checkbutton_underline = gtk_check_button_new_with_mnemonic("_Underline");
  gtk_widget_show(checkbutton_underline);
  gtk_box_pack_start(GTK_BOX(vbox6), checkbutton_underline, FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_underline), underline == ON);
}


void StylesheetDialog::smallcaps_simple_create()
{
  checkbutton_small_caps = gtk_check_button_new_with_mnemonic("S_mall caps");
  gtk_widget_show(checkbutton_small_caps);
  gtk_box_pack_start(GTK_BOX(vbox6), checkbutton_small_caps, FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_small_caps), smallcaps == ON);
}


void StylesheetDialog::italic_bold_underline_smallcaps_extended_create()
{
  table2 = gtk_table_new(4, 5, FALSE);
  gtk_widget_show(table2);
  gtk_box_pack_start(GTK_BOX(vbox6), table2, TRUE, TRUE, 0);
  gtk_table_set_row_spacings(GTK_TABLE(table2), 1);
  gtk_table_set_col_spacings(GTK_TABLE(table2), 10);

  label40 = gtk_label_new_with_mnemonic("_Italic");
  gtk_widget_show(label40);
  gtk_table_attach(GTK_TABLE(table2), label40, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label40), 0, 0.5);

  label41 = gtk_label_new_with_mnemonic("_Bold");
  gtk_widget_show(label41);
  gtk_table_attach(GTK_TABLE(table2), label41, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label41), 0, 0.5);

  label42 = gtk_label_new_with_mnemonic("_Underline");
  gtk_widget_show(label42);
  gtk_table_attach(GTK_TABLE(table2), label42, 0, 1, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label42), 0, 0.5);

  label_small_caps = gtk_label_new_with_mnemonic("S_mall caps");
  gtk_widget_show(label_small_caps);
  gtk_table_attach(GTK_TABLE(table2), label_small_caps, 0, 1, 3, 4, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label_small_caps), 0, 0.5);

  GSList *radiobutton_italics_group = NULL;
  GSList *radiobutton_bold_group = NULL;
  GSList *radiobutton_underline_group = NULL;
  GSList *radiobutton_small_caps_group = NULL;

  radiobutton_italics_off = gtk_radio_button_new_with_mnemonic(NULL, "Off");
  gtk_widget_show(radiobutton_italics_off);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_italics_off, 1, 2, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_italics_off), radiobutton_italics_group);
  radiobutton_italics_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_italics_off));

  radiobutton_italics_on = gtk_radio_button_new_with_mnemonic(NULL, "On");
  gtk_widget_show(radiobutton_italics_on);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_italics_on, 2, 3, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_italics_on), radiobutton_italics_group);
  radiobutton_italics_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_italics_on));

  radiobutton_italics_inherit = gtk_radio_button_new_with_mnemonic(NULL, "Inherit");
  gtk_widget_show(radiobutton_italics_inherit);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_italics_inherit, 3, 4, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_italics_inherit), radiobutton_italics_group);
  radiobutton_italics_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_italics_inherit));

  radiobutton_italics_toggle = gtk_radio_button_new_with_mnemonic(NULL, "Toggle");
  gtk_widget_show(radiobutton_italics_toggle);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_italics_toggle, 4, 5, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_italics_toggle), radiobutton_italics_group);
  radiobutton_italics_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_italics_toggle));

  radiobutton_bold_off = gtk_radio_button_new_with_mnemonic(NULL, "Off");
  gtk_widget_show(radiobutton_bold_off);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_bold_off, 1, 2, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_bold_off), radiobutton_bold_group);
  radiobutton_bold_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_bold_off));

  radiobutton_bold_on = gtk_radio_button_new_with_mnemonic(NULL, "On");
  gtk_widget_show(radiobutton_bold_on);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_bold_on, 2, 3, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_bold_on), radiobutton_bold_group);
  radiobutton_bold_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_bold_on));

  radiobutton_bold_inherit = gtk_radio_button_new_with_mnemonic(NULL, "Inherit");
  gtk_widget_show(radiobutton_bold_inherit);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_bold_inherit, 3, 4, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_bold_inherit), radiobutton_bold_group);
  radiobutton_bold_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_bold_inherit));

  radiobutton_bold_toggle = gtk_radio_button_new_with_mnemonic(NULL, "Toggle");
  gtk_widget_show(radiobutton_bold_toggle);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_bold_toggle, 4, 5, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_bold_toggle), radiobutton_bold_group);
  radiobutton_bold_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_bold_toggle));

  radiobutton_underline_off = gtk_radio_button_new_with_mnemonic(NULL, "Off");
  gtk_widget_show(radiobutton_underline_off);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_underline_off, 1, 2, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_underline_off), radiobutton_underline_group);
  radiobutton_underline_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_underline_off));

  radiobutton_underline_on = gtk_radio_button_new_with_mnemonic(NULL, "On");
  gtk_widget_show(radiobutton_underline_on);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_underline_on, 2, 3, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_underline_on), radiobutton_underline_group);
  radiobutton_underline_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_underline_on));

  radiobutton_underline_inherit = gtk_radio_button_new_with_mnemonic(NULL, "Inherit");
  gtk_widget_show(radiobutton_underline_inherit);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_underline_inherit, 3, 4, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_underline_inherit), radiobutton_underline_group);
  radiobutton_underline_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_underline_inherit));

  radiobutton_underline_toggle = gtk_radio_button_new_with_mnemonic(NULL, "Toggle");
  gtk_widget_show(radiobutton_underline_toggle);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_underline_toggle, 4, 5, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_underline_toggle), radiobutton_underline_group);
  radiobutton_underline_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_underline_toggle));

  radiobutton_small_caps_off = gtk_radio_button_new_with_mnemonic(NULL, "Off");
  gtk_widget_show(radiobutton_small_caps_off);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_small_caps_off, 1, 2, 3, 4, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_small_caps_off), radiobutton_small_caps_group);
  radiobutton_small_caps_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_small_caps_off));

  radiobutton_small_caps_on = gtk_radio_button_new_with_mnemonic(NULL, "On");
  gtk_widget_show(radiobutton_small_caps_on);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_small_caps_on, 2, 3, 3, 4, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_small_caps_on), radiobutton_small_caps_group);
  radiobutton_small_caps_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_small_caps_on));

  radiobutton_small_caps_inherit = gtk_radio_button_new_with_mnemonic(NULL, "Inherit");
  gtk_widget_show(radiobutton_small_caps_inherit);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_small_caps_inherit, 3, 4, 3, 4, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_small_caps_inherit), radiobutton_small_caps_group);
  radiobutton_small_caps_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_small_caps_inherit));

  radiobutton_small_caps_toggle = gtk_radio_button_new_with_mnemonic(NULL, "Toggle");
  gtk_widget_show(radiobutton_small_caps_toggle);
  gtk_table_attach(GTK_TABLE(table2), radiobutton_small_caps_toggle, 4, 5, 3, 4, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_small_caps_toggle), radiobutton_small_caps_group);
  radiobutton_small_caps_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_small_caps_toggle));

  g_signal_connect((gpointer) radiobutton_italics_off, "toggled", G_CALLBACK(on_radiobutton_italic_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_italics_on, "toggled", G_CALLBACK(on_radiobutton_italic_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_italics_inherit, "toggled", G_CALLBACK(on_radiobutton_italic_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_italics_toggle, "toggled", G_CALLBACK(on_radiobutton_italic_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_bold_off, "toggled", G_CALLBACK(on_radiobutton_bold_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_bold_on, "toggled", G_CALLBACK(on_radiobutton_bold_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_bold_inherit, "toggled", G_CALLBACK(on_radiobutton_bold_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_bold_toggle, "toggled", G_CALLBACK(on_radiobutton_bold_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_underline_off, "toggled", G_CALLBACK(on_radiobutton_underline_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_underline_on, "toggled", G_CALLBACK(on_radiobutton_underline_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_underline_inherit, "toggled", G_CALLBACK(on_radiobutton_underline_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_underline_toggle, "toggled", G_CALLBACK(on_radiobutton_underline_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_small_caps_off, "toggled", G_CALLBACK(on_radiobutton_small_caps_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_small_caps_on, "toggled", G_CALLBACK(on_radiobutton_small_caps_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_small_caps_inherit, "toggled", G_CALLBACK(on_radiobutton_small_caps_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_small_caps_toggle, "toggled", G_CALLBACK(on_radiobutton_small_caps_toggled), gpointer(this));

  set_italic();
  set_bold();
  set_underline();
  set_small_caps();
}


void StylesheetDialog::superscript_create()
{
  checkbutton_superscript = gtk_check_button_new_with_mnemonic("Su_perscript");
  gtk_widget_show(checkbutton_superscript);
  gtk_box_pack_start(GTK_BOX(vbox6), checkbutton_superscript, FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_superscript), superscript);
}


void StylesheetDialog::paragraph_create(const gchar * label, bool grey_out_justify)
/*
 Creates the paragraph settings.
 label: the label to use. By default it uses "paragraph", but if text is given
 here, it uses that label, e.g. "cell".
 */
{
  if (!label)
    label = (const gchar *)"paragraph";
  ustring text;

  vbox4 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox4);
  gtk_box_pack_start(GTK_BOX(vbox6), vbox4, TRUE, TRUE, 0);

  hbox4 = gtk_hbox_new(FALSE, 10);
  gtk_widget_show(hbox4);
  gtk_box_pack_start(GTK_BOX(vbox4), hbox4, TRUE, TRUE, 0);

  label10 = gtk_label_new_with_mnemonic("Te_xt alignment");
  gtk_widget_show(label10);
  gtk_box_pack_start(GTK_BOX(hbox4), label10, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label10), 0, 0.5);

  GSList *radiobutton_left_group = NULL;

  radiobutton_left = gtk_radio_button_new_with_mnemonic(NULL, "Left");
  gtk_widget_show(radiobutton_left);
  gtk_box_pack_start(GTK_BOX(hbox4), radiobutton_left, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_left), radiobutton_left_group);
  radiobutton_left_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_left));

  radiobutton_center = gtk_radio_button_new_with_mnemonic(NULL, "Center");
  gtk_widget_show(radiobutton_center);
  gtk_box_pack_start(GTK_BOX(hbox4), radiobutton_center, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_center), radiobutton_left_group);
  radiobutton_left_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_center));

  radiobutton_right = gtk_radio_button_new_with_mnemonic(NULL, "Right");
  gtk_widget_show(radiobutton_right);
  gtk_box_pack_start(GTK_BOX(hbox4), radiobutton_right, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_right), radiobutton_left_group);
  radiobutton_left_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_right));

  radiobutton_full = gtk_radio_button_new_with_mnemonic(NULL, "Justified");
  gtk_widget_show(radiobutton_full);
  gtk_box_pack_start(GTK_BOX(hbox4), radiobutton_full, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_full), radiobutton_left_group);
  radiobutton_left_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_full));

  if (grey_out_justify)
    gtk_widget_set_sensitive(radiobutton_full, false);

  table1 = gtk_table_new(5, 4, FALSE);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(vbox4), table1, TRUE, TRUE, 0);
  gtk_table_set_row_spacings(GTK_TABLE(table1), 4);
  gtk_table_set_col_spacings(GTK_TABLE(table1), 4);

  text = "S_pace before ";
  text.append(label);
  label11 = gtk_label_new_with_mnemonic(text.c_str());
  gtk_widget_show(label11);
  gtk_table_attach(GTK_TABLE(table1), label11, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label11), 1, 0.5);

  text = "Space afte_r ";
  text.append(label);
  label12 = gtk_label_new_with_mnemonic(text.c_str());
  gtk_widget_show(label12);
  gtk_table_attach(GTK_TABLE(table1), label12, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label12), 1, 0.5);

  label13 = gtk_label_new_with_mnemonic("_Left margin");
  gtk_widget_show(label13);
  gtk_table_attach(GTK_TABLE(table1), label13, 0, 1, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label13), 1, 0.5);

  label14 = gtk_label_new_with_mnemonic("Ri_ght margin");
  gtk_widget_show(label14);
  gtk_table_attach(GTK_TABLE(table1), label14, 0, 1, 3, 4, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label14), 1, 0.5);

  label15 = gtk_label_new_with_mnemonic("_1st Line indent");
  gtk_widget_show(label15);
  gtk_table_attach(GTK_TABLE(table1), label15, 0, 1, 4, 5, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label15), 1, 0.5);

  label16 = gtk_label_new("mm");
  gtk_widget_show(label16);
  gtk_table_attach(GTK_TABLE(table1), label16, 2, 3, 0, 1, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label16), 0, 0.5);

  label17 = gtk_label_new("mm");
  gtk_widget_show(label17);
  gtk_table_attach(GTK_TABLE(table1), label17, 2, 3, 1, 2, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label17), 0, 0.5);

  label18 = gtk_label_new("mm");
  gtk_widget_show(label18);
  gtk_table_attach(GTK_TABLE(table1), label18, 2, 3, 2, 3, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label18), 0, 0.5);

  label19 = gtk_label_new("mm");
  gtk_widget_show(label19);
  gtk_table_attach(GTK_TABLE(table1), label19, 2, 3, 3, 4, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label19), 0, 0.5);

  label20 = gtk_label_new("mm");
  gtk_widget_show(label20);
  gtk_table_attach(GTK_TABLE(table1), label20, 2, 3, 4, 5, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label20), 0, 0.5);

  spinbutton_first_adj = gtk_adjustment_new(0, -100, 100, 0.1, 1, 0);
  spinbutton_first = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_first_adj), 1, 1);
  gtk_widget_show(spinbutton_first);
  gtk_table_attach(GTK_TABLE(table1), spinbutton_first, 1, 2, 4, 5, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);

  spinbutton_before_adj = gtk_adjustment_new(0, 0, 100, 0.1, 1, 0);
  spinbutton_before = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_before_adj), 1, 1);
  gtk_widget_show(spinbutton_before);
  gtk_table_attach(GTK_TABLE(table1), spinbutton_before, 1, 2, 0, 1, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);

  spinbutton_after_adj = gtk_adjustment_new(0, 0, 100, 0.1, 1, 0);
  spinbutton_after = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_after_adj), 1, 1);
  gtk_widget_show(spinbutton_after);
  gtk_table_attach(GTK_TABLE(table1), spinbutton_after, 1, 2, 1, 2, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);

  spinbutton_left_adj = gtk_adjustment_new(0, -100, 100, 0.1, 1, 0);
  spinbutton_left = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_left_adj), 1, 1);
  gtk_widget_show(spinbutton_left);
  gtk_table_attach(GTK_TABLE(table1), spinbutton_left, 1, 2, 2, 3, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);

  spinbutton_right_adj = gtk_adjustment_new(0, -100, 100, 0.1, 1, 0);
  spinbutton_right = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_right_adj), 1, 1);
  gtk_widget_show(spinbutton_right);
  gtk_table_attach(GTK_TABLE(table1), spinbutton_right, 1, 2, 3, 4, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);

  gtk_label_set_mnemonic_widget(GTK_LABEL(label11), spinbutton_before);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label12), spinbutton_after);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label13), spinbutton_left);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label14), spinbutton_right);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label15), spinbutton_first);

  g_signal_connect((gpointer) radiobutton_left, "toggled", G_CALLBACK(on_alignment_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_center, "toggled", G_CALLBACK(on_alignment_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_right, "toggled", G_CALLBACK(on_alignment_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_full, "toggled", G_CALLBACK(on_alignment_toggled), gpointer(this));

  set_justification(justification);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_before), spacebefore);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_after), spaceafter);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_left), leftmargin);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_right), rightmargin);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_first), firstlineindent);
}


void StylesheetDialog::span_columns_create()
{
  checkbutton_span = gtk_check_button_new_with_mnemonic("Spans the t_wo columns");
  gtk_widget_show(checkbutton_span);
  gtk_box_pack_start(GTK_BOX(vbox6), checkbutton_span, FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_span), spancolumns);
}


void StylesheetDialog::apocrypha_create()
{
  checkbutton_apocrypha = gtk_check_button_new_with_mnemonic("Refers to the Apocrypha");
  gtk_widget_show(checkbutton_apocrypha);
  gtk_box_pack_start(GTK_BOX(vbox6), checkbutton_apocrypha, FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_apocrypha), userbool1);
}


void StylesheetDialog::note_numbering_type_create()
{
  vbox10 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox10);
  gtk_box_pack_start(GTK_BOX(vbox6), vbox10, TRUE, TRUE, 0);

  hbox9 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox9);
  gtk_box_pack_start(GTK_BOX(vbox10), hbox9, TRUE, TRUE, 0);

  label52 = gtk_label_new("Numbering");
  gtk_widget_show(label52);
  gtk_box_pack_start(GTK_BOX(hbox9), label52, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label52), 0, 0);

  vbox11 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox11);
  gtk_box_pack_start(GTK_BOX(hbox9), vbox11, TRUE, TRUE, 0);

  GSList *radiobutton_note_numbering_1_group = NULL;

  radiobutton_note_numbering_1 = gtk_radio_button_new_with_mnemonic(NULL, "1, 2, 3 ...");
  gtk_widget_show(radiobutton_note_numbering_1);
  gtk_box_pack_start(GTK_BOX(vbox11), radiobutton_note_numbering_1, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_note_numbering_1), radiobutton_note_numbering_1_group);
  radiobutton_note_numbering_1_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_note_numbering_1));

  radiobutton_note_numbering_a = gtk_radio_button_new_with_mnemonic(NULL, "a, b, c ...");
  gtk_widget_show(radiobutton_note_numbering_a);
  gtk_box_pack_start(GTK_BOX(vbox11), radiobutton_note_numbering_a, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_note_numbering_a), radiobutton_note_numbering_1_group);
  radiobutton_note_numbering_1_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_note_numbering_a));

  radiobutton_note_numbering_user = gtk_radio_button_new_with_mnemonic(NULL, "User defined");
  gtk_widget_show(radiobutton_note_numbering_user);
  gtk_box_pack_start(GTK_BOX(vbox11), radiobutton_note_numbering_user, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_note_numbering_user), radiobutton_note_numbering_1_group);
  radiobutton_note_numbering_1_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_note_numbering_user));

  entry_note_numbering = gtk_entry_new();
  gtk_widget_show(entry_note_numbering);
  gtk_box_pack_start(GTK_BOX(vbox10), entry_note_numbering, FALSE, FALSE, 0);
  gtk_entry_set_activates_default(GTK_ENTRY(entry_note_numbering), TRUE);

  g_signal_connect((gpointer) radiobutton_note_numbering_1, "toggled", G_CALLBACK(on_radiobutton_note_numbering_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_note_numbering_a, "toggled", G_CALLBACK(on_radiobutton_note_numbering_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_note_numbering_user, "toggled", G_CALLBACK(on_radiobutton_note_numbering_toggled), gpointer(this));

  NoteNumberingType note_numbering_type = (NoteNumberingType) userint1;
  switch (note_numbering_type) {
  case nntNumerical:
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_note_numbering_1), true);
      break;
    }
  case nntAlphabetical:
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_note_numbering_a), true);
      break;
    }
  case nntUserDefined:
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_note_numbering_user), true);
      break;
    }
  }
  gtk_entry_set_text(GTK_ENTRY(entry_note_numbering), userstring1.c_str());
  on_radiobutton_note_numbering();
}


void StylesheetDialog::note_numering_restart_create()
{
  hbox10 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox10);
  gtk_box_pack_start(GTK_BOX(vbox6), hbox10, TRUE, TRUE, 0);

  label53 = gtk_label_new("Restart");
  gtk_widget_show(label53);
  gtk_box_pack_start(GTK_BOX(hbox10), label53, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label53), 0, 0);

  vbox12 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox12);
  gtk_box_pack_start(GTK_BOX(hbox10), vbox12, TRUE, TRUE, 0);

  GSList *radiobutton_note_numbering_restart_never_group = NULL;

  radiobutton_note_numbering_restart_never = gtk_radio_button_new_with_mnemonic(NULL, "never");
  gtk_widget_show(radiobutton_note_numbering_restart_never);
  gtk_box_pack_start(GTK_BOX(vbox12), radiobutton_note_numbering_restart_never, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_note_numbering_restart_never), radiobutton_note_numbering_restart_never_group);
  radiobutton_note_numbering_restart_never_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_note_numbering_restart_never));

  radiobutton_note_numbering_restart_book = gtk_radio_button_new_with_mnemonic(NULL, "every book");
  gtk_widget_show(radiobutton_note_numbering_restart_book);
  gtk_box_pack_start(GTK_BOX(vbox12), radiobutton_note_numbering_restart_book, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_note_numbering_restart_book), radiobutton_note_numbering_restart_never_group);
  radiobutton_note_numbering_restart_never_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_note_numbering_restart_book));

  radiobutton_note_numbering_restart_chapter = gtk_radio_button_new_with_mnemonic(NULL, "every chapter");
  gtk_widget_show(radiobutton_note_numbering_restart_chapter);
  gtk_box_pack_start(GTK_BOX(vbox12), radiobutton_note_numbering_restart_chapter, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_note_numbering_restart_chapter), radiobutton_note_numbering_restart_never_group);
  radiobutton_note_numbering_restart_never_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_note_numbering_restart_chapter));

  NoteNumberingRestartType note_numbering_type = (NoteNumberingRestartType) userint2;
  switch (note_numbering_type) {
  case nnrtNever:
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_note_numbering_restart_never), true);
      break;
    }
  case nnrtBook:
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_note_numbering_restart_book), true);
      break;
    }
  default:
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_note_numbering_restart_chapter), true);
      break;
    }
  }
}


void StylesheetDialog::on_radiobutton_note_numbering_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_radiobutton_note_numbering();
}


void StylesheetDialog::on_radiobutton_note_numbering()
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_note_numbering_user))) {
    gtk_widget_set_sensitive(entry_note_numbering, true);
    gtk_widget_grab_focus(entry_note_numbering);
  } else {
    gtk_widget_set_sensitive(entry_note_numbering, false);
  }
}


void StylesheetDialog::print_chapter_at_first_verse_create()
{
  print_chapter_at_first_verse = gtk_check_button_new_with_mnemonic("Print chapter number at the first verse");
  gtk_widget_show(print_chapter_at_first_verse);
  gtk_box_pack_start(GTK_BOX(vbox6), print_chapter_at_first_verse, FALSE, FALSE, 0);

  g_signal_connect((gpointer) print_chapter_at_first_verse, "toggled", G_CALLBACK(on_radiobutton_print_chapter_at_first_verse_create_toggled), gpointer(this));

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(print_chapter_at_first_verse), userbool1);
}


void StylesheetDialog::on_radiobutton_print_chapter_at_first_verse_create_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_radiobutton_print_chapter_at_first_verse_create();
}


void StylesheetDialog::on_radiobutton_print_chapter_at_first_verse_create()
{
  bool sensitive = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(print_chapter_at_first_verse));
  gtk_widget_set_sensitive(hbox2, sensitive);
  gtk_widget_set_sensitive(checkbutton_small_caps, sensitive);
  gtk_widget_set_sensitive(vbox4, sensitive);   // Paragraph.
  gtk_widget_set_sensitive(checkbutton_span, sensitive);
}


void StylesheetDialog::book_id_new_page_create()
{
  hbox_id = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox_id);
  gtk_box_pack_start(GTK_BOX(vbox6), hbox_id, false, false, 0);

  checkbutton_id_newpage = gtk_check_button_new_with_mnemonic("Start on a new page");
  gtk_widget_show(checkbutton_id_newpage);
  gtk_box_pack_start(GTK_BOX(hbox_id), checkbutton_id_newpage, FALSE, FALSE, 0);

  checkbutton_id_oddpage = gtk_check_button_new_with_mnemonic("with an odd number");
  gtk_widget_show(checkbutton_id_oddpage);
  gtk_box_pack_start(GTK_BOX(hbox_id), checkbutton_id_oddpage, FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_id_newpage), userbool1);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_id_oddpage), userbool2);

  g_signal_connect((gpointer) checkbutton_id_newpage, "toggled", G_CALLBACK(on_checkbutton_id_newpage_toggled), gpointer(this));

  on_checkbutton_id_newpage();
}


void StylesheetDialog::on_radiobutton_identifier_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_radiobutton_identifier();
}


void StylesheetDialog::on_radiobutton_identifier()
{
  // Destroy any optional widgets.
  destroy_optional_widgets();
  // Show/hide notebooks with visual settings.
  IdentifierType subtype = (IdentifierType) get_subtype();
  switch (subtype) {
  case itBook:
    book_id_new_page_create();
    break;
  case itEncoding:
  case itComment:
    break;
  case itRunningHeader:
    print_in_running_header_create();
    break;
  case itLongTOC:
  case itShortTOC:
  case itBookAbbrev:
  case itChapterLabel:
  case itPublishedChapterMarker:
  case itCommentWithEndmarker:
    break;
  }
}


void StylesheetDialog::on_checkbutton_id_newpage_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_checkbutton_id_newpage();
}


void StylesheetDialog::on_checkbutton_id_newpage()
{
  bool newpage = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_id_newpage));
  gtk_widget_set_sensitive(checkbutton_id_oddpage, newpage);
}


void StylesheetDialog::colour_create()
{
  hbox_colour = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox_colour);
  gtk_box_pack_start(GTK_BOX(vbox6), hbox_colour, false, false, 0);

  button_colour = gtk_color_button_new();
  gtk_widget_show(button_colour);
  gtk_box_pack_start(GTK_BOX(hbox_colour), button_colour, FALSE, FALSE, 0);

  GdkColor colour;
  color_decimal_to_gdk(color, &colour);
  gtk_color_button_set_color(GTK_COLOR_BUTTON(button_colour), &colour);
}


void StylesheetDialog::print_create()
{
  checkbutton_print = gtk_check_button_new_with_mnemonic("Print");
  gtk_widget_show(checkbutton_print);
  gtk_box_pack_start(GTK_BOX(vbox6), checkbutton_print, FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_print), print);
}


void StylesheetDialog::end_note_placement_create()
{
  // Build the interface.
  hbox13 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox13);
  gtk_box_pack_start(GTK_BOX(vbox6), hbox13, TRUE, TRUE, 0);

  label60 = gtk_label_new("Print");
  gtk_widget_show(label60);
  gtk_box_pack_start(GTK_BOX(hbox13), label60, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label60), 0.5, 0);

  vbox16 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox16);
  gtk_box_pack_start(GTK_BOX(hbox13), vbox16, TRUE, TRUE, 0);

  GSList *radiobutton_print_endnotes_after_book_group = NULL;

  radiobutton_print_endnotes_after_book = gtk_radio_button_new_with_mnemonic(NULL, "after each book");
  gtk_widget_show(radiobutton_print_endnotes_after_book);
  gtk_box_pack_start(GTK_BOX(vbox16), radiobutton_print_endnotes_after_book, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_print_endnotes_after_book), radiobutton_print_endnotes_after_book_group);
  radiobutton_print_endnotes_after_book_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_print_endnotes_after_book));

  radiobutton_print_endnotes_after_everything_else = gtk_radio_button_new_with_mnemonic(NULL, "after everything else");
  gtk_widget_show(radiobutton_print_endnotes_after_everything_else);
  gtk_box_pack_start(GTK_BOX(vbox16), radiobutton_print_endnotes_after_everything_else, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_print_endnotes_after_everything_else), radiobutton_print_endnotes_after_book_group);
  radiobutton_print_endnotes_after_book_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_print_endnotes_after_everything_else));

  hbox14 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox14);
  gtk_box_pack_start(GTK_BOX(vbox16), hbox14, TRUE, TRUE, 0);

  radiobutton_print_endnotes_at_marker = gtk_radio_button_new_with_mnemonic(NULL, "upon encountering marker");
  gtk_widget_show(radiobutton_print_endnotes_at_marker);
  gtk_box_pack_start(GTK_BOX(hbox14), radiobutton_print_endnotes_at_marker, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_print_endnotes_at_marker), radiobutton_print_endnotes_after_book_group);
  radiobutton_print_endnotes_after_book_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_print_endnotes_at_marker));

  entry_print_endnotes_marker = gtk_entry_new();
  gtk_widget_show(entry_print_endnotes_marker);
  gtk_box_pack_start(GTK_BOX(hbox14), entry_print_endnotes_marker, FALSE, FALSE, 0);
  gtk_entry_set_width_chars(GTK_ENTRY(entry_print_endnotes_marker), 15);

  g_signal_connect((gpointer) radiobutton_print_endnotes_at_marker, "toggled", G_CALLBACK(on_radiobutton_print_endnotes_at_marker_toggled), gpointer(this));

  // Make the settings.
  EndnotePositionType endnote_position = (EndnotePositionType) userint2;
  switch (endnote_position) {
  case eptAfterBook:
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_print_endnotes_after_book), true);
      break;
    }
  case eptAfterEverything:
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_print_endnotes_after_everything_else), true);
      break;
    }
  case eptAtMarker:
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_print_endnotes_at_marker), true);
      break;
    }
  }
  on_radiobutton_print_endnotes_at_marker();
  gtk_entry_set_text(GTK_ENTRY(entry_print_endnotes_marker), userstring2.c_str());
}


void StylesheetDialog::on_radiobutton_print_endnotes_at_marker_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_radiobutton_print_endnotes_at_marker();
}


void StylesheetDialog::on_radiobutton_print_endnotes_at_marker()
{
  gtk_widget_set_sensitive(entry_print_endnotes_marker, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_print_endnotes_at_marker)));
}


void StylesheetDialog::on_checkbutton_table_element_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_checkbutton_table_element();
}


void StylesheetDialog::on_checkbutton_table_element()
{
  // Destroy any optional widgets.
  destroy_optional_widgets();
  // Show/hide notebooks with visual settings.
  TableElementType subtype = (TableElementType) get_subtype();
  switch (subtype) {
  case tetRow:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 6);
    break;
  case tetHeading:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 6);
    fontsize_points_create();
    italic_simple_create();
    bold_simple_create();
    underline_simple_create();
    smallcaps_simple_create();
    paragraph_create("heading", false);
    column_number_create();
    break;
  case tetCell:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 6);
    fontsize_points_create();
    italic_simple_create();
    bold_simple_create();
    underline_simple_create();
    smallcaps_simple_create();
    paragraph_create("cell", false);
    column_number_create();
    break;
  }
}


void StylesheetDialog::on_radiobutton_subtype_wordlist_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((StylesheetDialog *) user_data)->on_radiobutton_subtype_wordlist();
}


void StylesheetDialog::on_radiobutton_subtype_wordlist()
{
  // Destroy any optional widgets.
  destroy_optional_widgets();
  // Show/hide notebooks with visual settings.
  WordListElementType subtype = (WordListElementType) get_subtype();
  switch (subtype) {
  case wltWordlistGlossaryDictionary:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 7);
    wordlist_add_text_create();
    break;
  case wltHebrewWordlistEntry:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 7);
    wordlist_add_text_create();
    break;
  case wltGreekWordlistEntry:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 7);
    wordlist_add_text_create();
    break;
  case wltSubjectIndexEntry:
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook_subtype), 7);
    wordlist_add_text_create();
    break;
  }
}


void StylesheetDialog::column_number_create()
{
  // Build the interface.
  hbox15 = gtk_hbox_new(FALSE, 5);
  gtk_widget_show(hbox15);
  gtk_box_pack_start(GTK_BOX(vbox6), hbox15, FALSE, FALSE, 0);

  label62 = gtk_label_new("Column number");
  gtk_widget_show(label62);
  gtk_box_pack_start(GTK_BOX(hbox15), label62, FALSE, FALSE, 0);

  spinbutton_column_number_adj = gtk_adjustment_new(1, 1, 10, 1, 10, 0);
  spinbutton_column_number = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_column_number_adj), 1, 0);
  gtk_widget_show(spinbutton_column_number);
  gtk_box_pack_start(GTK_BOX(hbox15), spinbutton_column_number, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_column_number), TRUE);

  // Make the settings.
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_column_number), userint1);
}


void StylesheetDialog::print_in_running_header_create()
{
  checkbutton_print_in_running_header_left = gtk_check_button_new_with_mnemonic("Print this in running header of left page");
  gtk_widget_show(checkbutton_print_in_running_header_left);
  gtk_box_pack_start(GTK_BOX(vbox6), checkbutton_print_in_running_header_left, FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_print_in_running_header_left), userbool2);

  checkbutton_print_in_running_header_right = gtk_check_button_new_with_mnemonic("Print this in running header of right page");
  gtk_widget_show(checkbutton_print_in_running_header_right);
  gtk_box_pack_start(GTK_BOX(vbox6), checkbutton_print_in_running_header_right, FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_print_in_running_header_right), userbool3);
}


void StylesheetDialog::wordlist_add_text_create()
{
  hbox16 = gtk_hbox_new(FALSE, 5);
  gtk_widget_show(hbox16);
  gtk_box_pack_start(GTK_BOX(vbox6), hbox16, TRUE, TRUE, 0);

  label64 = gtk_label_new("Add");
  gtk_widget_show(label64);
  gtk_box_pack_start(GTK_BOX(hbox16), label64, FALSE, FALSE, 0);

  entry_wordlist_addition = gtk_entry_new();
  gtk_widget_show(entry_wordlist_addition);
  gtk_box_pack_start(GTK_BOX(hbox16), entry_wordlist_addition, FALSE, TRUE, 0);
  gtk_entry_set_width_chars(GTK_ENTRY(entry_wordlist_addition), 5);

  label65 = gtk_label_new("after entry");
  gtk_widget_show(label65);
  gtk_box_pack_start(GTK_BOX(hbox16), label65, FALSE, FALSE, 0);

  gtk_entry_set_text(GTK_ENTRY(entry_wordlist_addition), userstring1.c_str());
}


void StylesheetDialog::restarts_paragraph_create()
{
  checkbutton_restarts_paragraph = gtk_check_button_new_with_mnemonic("Restart paragraph");
  gtk_widget_show(checkbutton_restarts_paragraph);
  gtk_box_pack_start(GTK_BOX(vbox6), checkbutton_restarts_paragraph, FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_restarts_paragraph), userbool1);
}


