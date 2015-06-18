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

#include "libraries.h"
#include <glib.h>
#include "dialognewstylesheet.h"
#include "help.h"
#include "shortcuts.h"
#include "stylesheetutils.h"
#include "listview.h"
#include "utilities.h"
#include <glib/gi18n.h>

NewStylesheetDialog::NewStylesheetDialog(const ustring & worksheet)
{
  myworksheet = worksheet;

  Shortcuts shortcuts(0);

  newstylesheetdialog = gtk_dialog_new();
  if (!worksheet.empty())
    gtk_window_set_title(GTK_WINDOW(newstylesheetdialog), _("New style"));
  else
    gtk_window_set_title(GTK_WINDOW(newstylesheetdialog), _("New stylesheet"));
  gtk_window_set_position(GTK_WINDOW(newstylesheetdialog), GTK_WIN_POS_CENTER_ON_PARENT);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (newstylesheetdialog));
  gtk_widget_show(dialog_vbox1);

  hbox2 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox2);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox2, TRUE, TRUE, 0);

  vbox1 = gtk_vbox_new(FALSE, 4);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(hbox2), vbox1, TRUE, TRUE, 0);

  if (myworksheet.empty()) {
    hbox1 = gtk_hbox_new(FALSE, 4);
    gtk_widget_show(hbox1);
    gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);

    label1 = gtk_label_new(_("Name"));
    gtk_widget_show(label1);
    gtk_box_pack_start(GTK_BOX(hbox1), label1, FALSE, FALSE, 0);

    shortcuts.label(label1);

    entry1 = gtk_entry_new();
    gtk_widget_show(entry1);
    gtk_box_pack_start(GTK_BOX(hbox1), entry1, TRUE, TRUE, 0);
  }

  ustring info;
  if (!myworksheet.empty())
    info = _("Select one or more styles from the list");
  label_info = gtk_label_new(info.c_str());
  gtk_widget_show(label_info);
  gtk_box_pack_start(GTK_BOX(vbox1), label_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_info), 0, 0.5);

  GSList *radiobutton_basic_group = NULL;

  radiobutton_basic = gtk_radio_button_new_with_mnemonic(NULL, _("Basic"));
  gtk_widget_show(radiobutton_basic);
  gtk_box_pack_start(GTK_BOX(vbox1), radiobutton_basic, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_basic), radiobutton_basic_group);
  radiobutton_basic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_basic));

  shortcuts.button(radiobutton_basic);

  radiobutton_paragraph = gtk_radio_button_new_with_mnemonic(NULL, _("Basic, paragraph"));
  gtk_widget_show(radiobutton_paragraph);
  gtk_box_pack_start(GTK_BOX(vbox1), radiobutton_paragraph, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_paragraph), radiobutton_basic_group);
  radiobutton_basic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_paragraph));

  shortcuts.button(radiobutton_paragraph);

  radiobutton_word_note = gtk_radio_button_new_with_mnemonic(NULL, _("Basic, paragraph, word, note"));
  gtk_widget_show(radiobutton_word_note);
  gtk_box_pack_start(GTK_BOX(vbox1), radiobutton_word_note, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_word_note), radiobutton_basic_group);
  radiobutton_basic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_word_note));

  shortcuts.button(radiobutton_word_note);

  radiobutton_sbp = gtk_radio_button_new_with_mnemonic(NULL, _("SIL best practice"));
  gtk_widget_show(radiobutton_sbp);
  gtk_box_pack_start(GTK_BOX(vbox1), radiobutton_sbp, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_sbp), radiobutton_basic_group);
  radiobutton_basic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_sbp));

  shortcuts.button(radiobutton_sbp);

  radiobutton_full = gtk_radio_button_new_with_mnemonic(NULL, _("Full"));
  gtk_widget_show(radiobutton_full);
  gtk_box_pack_start(GTK_BOX(vbox1), radiobutton_full, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_full), radiobutton_basic_group);
  radiobutton_basic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_full));

  shortcuts.button(radiobutton_full);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_full), true);

  if (!myworksheet.empty()) {
    expander1 = gtk_expander_new(NULL);
    gtk_widget_show(expander1);
    gtk_box_pack_start(GTK_BOX(vbox1), expander1, TRUE, TRUE, 0);

    label2 = gtk_label_new(_("Add non-standard style"));
    gtk_widget_show(label2);
    gtk_expander_set_label_widget(GTK_EXPANDER(expander1), label2);

    shortcuts.label(label2);

    hbox1 = gtk_hbox_new(FALSE, 4);
    gtk_widget_show(hbox1);
    gtk_container_add(GTK_CONTAINER(expander1), hbox1);

    label1 = gtk_label_new(_("Name"));
    gtk_widget_show(label1);
    gtk_box_pack_start(GTK_BOX(hbox1), label1, FALSE, FALSE, 0);

    shortcuts.label(label1);

    entry1 = gtk_entry_new();
    gtk_widget_show(entry1);
    gtk_box_pack_start(GTK_BOX(hbox1), entry1, TRUE, TRUE, 0);

    scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show(scrolledwindow1);
    gtk_box_pack_start(GTK_BOX(hbox2), scrolledwindow1, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_SHADOW_IN);

    treeview1 = gtk_tree_view_new();
    gtk_widget_show(treeview1);
    gtk_container_add(GTK_CONTAINER(scrolledwindow1), treeview1);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview1), FALSE);

    store1 = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview1), GTK_TREE_MODEL(store1));
    g_object_unref(store1);
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    column1 = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview1), column1);
    select1 = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview1));
    gtk_tree_selection_set_mode(select1, GTK_SELECTION_SINGLE);
  }

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(newstylesheetdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(newstylesheetdialog, NULL, &shortcuts, "file/styles/stylesheet/new");

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(newstylesheetdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton1), true);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(newstylesheetdialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  shortcuts.stockbutton(cancelbutton1);
  shortcuts.stockbutton(okbutton);
  shortcuts.process();

  g_signal_connect((gpointer) entry1, "changed", G_CALLBACK(on_entry_changed), gpointer(this));
  g_signal_connect((gpointer) radiobutton_basic, "toggled", G_CALLBACK(on_radiobutton_basic_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_paragraph, "toggled", G_CALLBACK(on_radiobutton_paragraph_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_word_note, "toggled", G_CALLBACK(on_radiobutton_word_note_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_sbp, "toggled", G_CALLBACK(on_radiobutton_sbp_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_full, "toggled", G_CALLBACK(on_radiobutton_full_toggled), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_focus(entry1);
  gtk_widget_grab_default(okbutton);

  gtk_label_set_mnemonic_widget(GTK_LABEL(label1), entry1);

  on_entry();
  on_radiobutton();
}

NewStylesheetDialog::~NewStylesheetDialog()
{
  gtk_widget_destroy(newstylesheetdialog);
}

int NewStylesheetDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(newstylesheetdialog));
}

void NewStylesheetDialog::on_entry_changed(GtkEditable * editable, gpointer user_data)
{
  ((NewStylesheetDialog *) user_data)->on_entry();
}

void NewStylesheetDialog::on_entry()
// Activates the ok button only when we've a non-existing stylesheet.
// Or when there is a style given.
{
  name = gtk_entry_get_text(GTK_ENTRY(entry1));
  bool sensitive = !name.empty();
  if (myworksheet.empty()) {
    if (sensitive)
      sensitive = !stylesheet_exists(name);
    ustring message;
    if (!sensitive)
      message = _("A stylesheet with this name already exists");
    else
      message.clear();
    gtk_label_set_text(GTK_LABEL(label_info), message.c_str());
  } else {
    if (!markers.empty())
      sensitive = true;
    // Only lowercase styles.
    name = lowerCase(name);
  }
  gtk_widget_set_sensitive(okbutton, sensitive);
}

void NewStylesheetDialog::on_radiobutton_basic_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((NewStylesheetDialog *) user_data)->on_radiobutton();
}

void NewStylesheetDialog::on_radiobutton_paragraph_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((NewStylesheetDialog *) user_data)->on_radiobutton();
}

void NewStylesheetDialog::on_radiobutton_word_note_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((NewStylesheetDialog *) user_data)->on_radiobutton();
}

void NewStylesheetDialog::on_radiobutton_sbp_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((NewStylesheetDialog *) user_data)->on_radiobutton();
}

void NewStylesheetDialog::on_radiobutton_full_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((NewStylesheetDialog *) user_data)->on_radiobutton();
}

void NewStylesheetDialog::on_radiobutton()
{
  // Set the right stylesheet type.
  stylesheettype = stFull;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_basic)))
    stylesheettype = stBasic;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_paragraph)))
    stylesheettype = stBasicParagraph;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_word_note)))
    stylesheettype = stBasicParagraphWordNote;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_sbp)))
    stylesheettype = stSilBestPractice;
  // If in stylesheet mode, bail out.
  if (myworksheet.empty())
    return;
  // Show a list with the available styles.
  markers.clear();
  set < ustring > markers_of_type_set = stylesheet_get_styles_of_type(stylesheettype);
  vector < ustring > markers_of_type(markers_of_type_set.begin(), markers_of_type_set.end());
  vector < ustring > markers_in_sheet = stylesheet_get_markers(myworksheet, NULL);
  set < ustring > markers_in_sheet_set(markers_in_sheet.begin(), markers_in_sheet.end());
  for (unsigned int i = 0; i < markers_of_type.size(); i++) {
    if (markers_in_sheet_set.find(markers_of_type[i]) == markers_in_sheet_set.end())
      markers.push_back(markers_of_type[i]);
  }
  sort(markers.begin(), markers.end());
  listview_set_strings(treeview1, store1, markers);
  // Focus first item.
  if (!markers.empty())
    listview_focus_string(treeview1, markers[0]);
  // GUI.
  on_entry();
}

void NewStylesheetDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((NewStylesheetDialog *) user_data)->on_okbutton();
}

void NewStylesheetDialog::on_okbutton()
{
  if (!myworksheet.empty()) {
    if (name.empty())
      name = listview_get_active_string(treeview1);
  }
}
