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
#include "dialogprintprefs.h"
#include "paper.h"
#include "combobox.h"
#include "settings.h"
#include "help.h"

PrintPreferencesDialog::PrintPreferencesDialog(int dummy)
{
  // Settings
  extern Settings *settings;

  dialogprintpreferences = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialogprintpreferences), "Printing preferences");
  gtk_window_set_position(GTK_WINDOW(dialogprintpreferences), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(dialogprintpreferences), TRUE);
  gtk_window_set_destroy_with_parent(GTK_WINDOW(dialogprintpreferences), TRUE);
  gtk_window_set_type_hint(GTK_WINDOW(dialogprintpreferences), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(dialogprintpreferences));
  gtk_widget_show(dialog_vbox1);

  notebook1 = gtk_notebook_new();
  gtk_widget_show(notebook1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), notebook1, FALSE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(notebook1), 4);

  // Page setup.
  hbox1 = gtk_hbox_new(FALSE, 10);
  gtk_widget_show(hbox1);
  gtk_container_add(GTK_CONTAINER(notebook1), hbox1);
  gtk_container_set_border_width(GTK_CONTAINER(hbox1), 10);

  table1 = gtk_table_new(4, 2, FALSE);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(hbox1), table1, TRUE, TRUE, 0);
  gtk_table_set_row_spacings(GTK_TABLE(table1), 6);
  gtk_table_set_col_spacings(GTK_TABLE(table1), 2);

  spinbutton_height_adj = gtk_adjustment_new(1, 0, 100, 0.01, 0.1, 0);
  spinbutton_height = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_height_adj), 1, 2);
  gtk_widget_show(spinbutton_height);
  gtk_table_attach(GTK_TABLE(table1), spinbutton_height, 1, 2, 3, 4, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_height), TRUE);

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_height), settings->genconfig.paper_height_get());

  label4 = gtk_label_new("Height (cm)");
  gtk_widget_show(label4);
  gtk_table_attach(GTK_TABLE(table1), label4, 0, 1, 3, 4, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label4), 1, 0.5);

  spinbutton_width_adj = gtk_adjustment_new(1, 0, 100, 0.01, 0.1, 0);
  spinbutton_width = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_width_adj), 1, 2);
  gtk_widget_show(spinbutton_width);
  gtk_table_attach(GTK_TABLE(table1), spinbutton_width, 1, 2, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_width), TRUE);

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_width), settings->genconfig.paper_width_get());

  label3 = gtk_label_new("Width (cm)");
  gtk_widget_show(label3);
  gtk_table_attach(GTK_TABLE(table1), label3, 0, 1, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label3), 1, 0.5);

  label2 = gtk_label_new("Format");
  gtk_widget_show(label2);
  gtk_table_attach(GTK_TABLE(table1), label2, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label2), 1, 0.5);

  combo_format = gtk_combo_box_new_text();
  gtk_widget_show(combo_format);
  gtk_table_attach(GTK_TABLE(table1), combo_format, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  {
    vector < ustring > items;
    for (unsigned int i = 0; i <= NUMBER_OF_PAPERSIZES; i++) {
      items.push_back(paper_size_get_name(i));
    }
    combobox_set_strings(combo_format, items);
  }
  gtk_widget_set_size_request(combo_format, 100, -1);
  combobox_set_string(combo_format, settings->genconfig.paper_format_get());

  label5 = gtk_label_new("Paper format");
  gtk_widget_show(label5);
  gtk_table_attach(GTK_TABLE(table1), label5, 0, 2, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label5), 0.06, 0.5);

  vseparator1 = gtk_vseparator_new();
  gtk_widget_show(vseparator1);
  gtk_box_pack_start(GTK_BOX(hbox1), vseparator1, FALSE, TRUE, 0);

  table2 = gtk_table_new(5, 2, FALSE);
  gtk_widget_show(table2);
  gtk_box_pack_start(GTK_BOX(hbox1), table2, TRUE, TRUE, 0);
  gtk_table_set_row_spacings(GTK_TABLE(table2), 6);
  gtk_table_set_col_spacings(GTK_TABLE(table2), 2);

  label6 = gtk_label_new("Margins (cm)");
  gtk_widget_show(label6);
  gtk_table_attach(GTK_TABLE(table2), label6, 0, 2, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label6), 0.06, 0.5);

  label7 = gtk_label_new("Inner");
  gtk_widget_show(label7);
  gtk_table_attach(GTK_TABLE(table2), label7, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label7), 1, 0.5);

  label8 = gtk_label_new("Outer");
  gtk_widget_show(label8);
  gtk_table_attach(GTK_TABLE(table2), label8, 0, 1, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label8), 1, 0.5);

  label9 = gtk_label_new("Top");
  gtk_widget_show(label9);
  gtk_table_attach(GTK_TABLE(table2), label9, 0, 1, 3, 4, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label9), 1, 0.5);

  label10 = gtk_label_new("Bottom");
  gtk_widget_show(label10);
  gtk_table_attach(GTK_TABLE(table2), label10, 0, 1, 4, 5, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label10), 1, 0.5);

  spinbutton_left_adj = gtk_adjustment_new(1, 0, 100, 0.01, 0.1, 0);
  spinbutton_left = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_left_adj), 1, 2);
  gtk_widget_show(spinbutton_left);
  gtk_table_attach(GTK_TABLE(table2), spinbutton_left, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_left), TRUE);

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_left), settings->genconfig.paper_inside_margin_get());

  spinbutton_right_adj = gtk_adjustment_new(1, 0, 100, 0.01, 0.1, 0);
  spinbutton_right = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_right_adj), 1, 2);
  gtk_widget_show(spinbutton_right);
  gtk_table_attach(GTK_TABLE(table2), spinbutton_right, 1, 2, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_right), TRUE);

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_right), settings->genconfig.paper_outside_margin_get());

  spinbutton_top_adj = gtk_adjustment_new(1, 0, 100, 0.01, 0.1, 0);
  spinbutton_top = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_top_adj), 1, 2);
  gtk_widget_show(spinbutton_top);
  gtk_table_attach(GTK_TABLE(table2), spinbutton_top, 1, 2, 3, 4, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_top), TRUE);

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_top), settings->genconfig.paper_top_margin_get());

  spinbutton_bottom_adj = gtk_adjustment_new(1, 0, 100, 0.01, 0.1, 0);
  spinbutton_bottom = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_bottom_adj), 1, 2);
  gtk_widget_show(spinbutton_bottom);
  gtk_table_attach(GTK_TABLE(table2), spinbutton_bottom, 1, 2, 4, 5, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_bottom), TRUE);

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_bottom), settings->genconfig.paper_bottom_margin_get());

  vseparator2 = gtk_vseparator_new();
  gtk_widget_show(vseparator2);
  gtk_box_pack_start(GTK_BOX(hbox1), vseparator2, TRUE, TRUE, 0);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(hbox1), vbox1, TRUE, TRUE, 0);

  checkbuttondate = gtk_check_button_new_with_mnemonic("Print date beside page number");
  gtk_widget_show(checkbuttondate);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbuttondate, FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttondate), settings->genconfig.printdate_get());

  hbox2 = gtk_hbox_new(FALSE, 5);
  gtk_widget_show(hbox2);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox2, FALSE, FALSE, 0);

  label12 = gtk_label_new("Header font size");
  gtk_widget_show(label12);
  gtk_box_pack_start(GTK_BOX(hbox2), label12, FALSE, FALSE, 0);

  spinbutton_header_font_size_adj = gtk_adjustment_new(12, 5, 25, 0.1, 1, 0);
  spinbutton_header_font_size = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_header_font_size_adj), 1, 1);
  gtk_widget_show(spinbutton_header_font_size);
  gtk_box_pack_start(GTK_BOX(hbox2), spinbutton_header_font_size, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_header_font_size), TRUE);

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_header_font_size), settings->genconfig.header_font_size_get());

  label13 = gtk_label_new("points");
  gtk_widget_show(label13);
  gtk_box_pack_start(GTK_BOX(hbox2), label13, FALSE, FALSE, 0);

  label1 = gtk_label_new("Page");
  gtk_widget_show(label1);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook1), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook1), 0), label1);

  // Formatter setup.
  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox2);
  gtk_container_add(GTK_CONTAINER(notebook1), vbox2);

  checkbutton_intermediate_text = gtk_check_button_new_with_mnemonic("Review or edit layout engine's commands");
  gtk_widget_show(checkbutton_intermediate_text);
  gtk_box_pack_start(GTK_BOX(vbox2), checkbutton_intermediate_text, FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_intermediate_text), settings->genconfig.print_engine_use_intermediate_text_get());

  label11 = gtk_label_new("Formatter");
  gtk_widget_show(label11);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook1), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook1), 1), label11);

  // Dialog action area.
  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(dialogprintpreferences));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(dialogprintpreferences, NULL, NULL, NULL);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(dialogprintpreferences), cancelbutton1, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton1), true);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(dialogprintpreferences), okbutton1, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton1), true);

  g_signal_connect((gpointer) spinbutton_height, "changed", G_CALLBACK(on_spinbutton_height_changed), gpointer(this));
  g_signal_connect((gpointer) spinbutton_width, "changed", G_CALLBACK(on_spinbutton_width_changed), gpointer(this));
  g_signal_connect((gpointer) combo_format, "changed", G_CALLBACK(on_combo_entry_format_changed), gpointer(this));
  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton1);

  // Control sizes, margins.
  on_combo_entry_format();
  adjust_margin_limits();
}

PrintPreferencesDialog::~PrintPreferencesDialog()
{
  gtk_widget_destroy(dialogprintpreferences);
}

int PrintPreferencesDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialogprintpreferences));
}

void PrintPreferencesDialog::on_spinbutton_height_changed(GtkEditable * editable, gpointer user_data)
{
  ((PrintPreferencesDialog *) user_data)->on_spinbutton_height();
}

void PrintPreferencesDialog::on_spinbutton_height()
{
  set_paper_format();
  adjust_margin_limits();
}

void PrintPreferencesDialog::on_spinbutton_width_changed(GtkEditable * editable, gpointer user_data)
{
  ((PrintPreferencesDialog *) user_data)->on_spinbutton_width();
}

void PrintPreferencesDialog::on_spinbutton_width()
{
  set_paper_format();
  adjust_margin_limits();
}

void PrintPreferencesDialog::on_combo_entry_format_changed(GtkEditable * editable, gpointer user_data)
{
  ((PrintPreferencesDialog *) user_data)->on_combo_entry_format();
}

void PrintPreferencesDialog::on_combo_entry_format()
{
  ustring format;
  format = combobox_get_active_string(combo_format);
  double width;
  width = paper_size_get_width(format);
  if (width != 0) {
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_width), width);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_height), paper_size_get_height(format));
  }
}

void PrintPreferencesDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((PrintPreferencesDialog *) user_data)->on_okbutton1();
}

void PrintPreferencesDialog::on_okbutton1()
{
  // Update all spinbuttons.
  gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton_width));
  gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton_height));
  gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton_left));
  gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton_right));
  gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton_top));
  gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton_bottom));
  gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton_header_font_size));

  // Store all values in the configuration.
  extern Settings *settings;
  settings->genconfig.paper_format_set(combobox_get_active_string(combo_format));
  settings->genconfig.paper_width_set(gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_width)));
  settings->genconfig.paper_height_set(gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_height)));
  settings->genconfig.paper_inside_margin_set(gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_left)));
  settings->genconfig.paper_outside_margin_set(gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_right)));
  settings->genconfig.paper_top_margin_set(gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_top)));
  settings->genconfig.paper_bottom_margin_set(gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_bottom)));
  settings->genconfig.printdate_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttondate)));
  settings->genconfig.header_font_size_set(gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_header_font_size)));
  settings->genconfig.print_engine_use_intermediate_text_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_intermediate_text)));
}

void PrintPreferencesDialog::adjust_margin_limits()
// Adjust the limits of the margins depending on the paper size.
{
  // Any margin should be limited to 40 percent of the corresponding height or width.
  double limit;
  double size;
  // Deal with left and right margins.
  size = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_width));
  limit = 0.4 * size;
  gtk_spin_button_set_range(GTK_SPIN_BUTTON(spinbutton_left), 0, limit);
  gtk_spin_button_set_range(GTK_SPIN_BUTTON(spinbutton_right), 0, limit);
  // Top and bottom margins.
  size = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_height));
  limit = 0.4 * size;
  gtk_spin_button_set_range(GTK_SPIN_BUTTON(spinbutton_top), 0, limit);
  gtk_spin_button_set_range(GTK_SPIN_BUTTON(spinbutton_bottom), 0, limit);
}

void PrintPreferencesDialog::set_paper_format()
// Depending on the width and height of the paper, select the right format
// belonging to it, or if none fits, choose "User defined".
{
  double width;
  width = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_width));
  double height;
  height = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton_height));
  string size;
  size = paper_size_get_name(width, height);
  combobox_set_string(combo_format, size);
}
