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


#include "dialogfontcolor.h"
#include "help.h"
#include "shortcuts.h"
#include "color.h"


FontColorDialog::FontColorDialog(bool initial_use_default_font, const ustring & initial_font, unsigned int initial_line_spacing, bool initial_use_default_color, unsigned int initial_normal_text_color, unsigned int initial_background_color, unsigned int initial_selected_text_color, unsigned int initial_selection_color)
{
  Shortcuts shortcuts(0);

  fontcolordialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(fontcolordialog), "Font and Color");
  gtk_window_set_position(GTK_WINDOW(fontcolordialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(fontcolordialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (fontcolordialog));
  gtk_widget_show(dialog_vbox1);

  checkbutton_font = gtk_check_button_new_with_mnemonic("Use default font");
  gtk_widget_show(checkbutton_font);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_font, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_font);

  fontselection = gtk_font_selection_new();
  gtk_widget_show(fontselection);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), fontselection, TRUE, TRUE, 0);

  hbox_line_spacing = gtk_hbox_new(FALSE, 5);
  gtk_widget_show(hbox_line_spacing);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox_line_spacing, TRUE, TRUE, 0);

  label7 = gtk_label_new_with_mnemonic("Line spacing");
  gtk_widget_show(label7);
  gtk_box_pack_start(GTK_BOX(hbox_line_spacing), label7, FALSE, FALSE, 0);

  shortcuts.label(label7);

  spinbutton_line_spacing_adj = gtk_adjustment_new(initial_line_spacing, 50, 500, 1, 10, 0);
  spinbutton_line_spacing = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_line_spacing_adj), 1, 0);
  gtk_widget_show(spinbutton_line_spacing);
  gtk_box_pack_start(GTK_BOX(hbox_line_spacing), spinbutton_line_spacing, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_line_spacing), TRUE);

  label8 = gtk_label_new("%");
  gtk_widget_show(label8);
  gtk_box_pack_start(GTK_BOX(hbox_line_spacing), label8, FALSE, FALSE, 0);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hseparator1, FALSE, FALSE, 0);

  checkbutton_colors = gtk_check_button_new_with_mnemonic("Use default colors");
  gtk_widget_show(checkbutton_colors);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_colors, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_colors);

  table1 = gtk_table_new(4, 2, FALSE);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), table1, TRUE, TRUE, 0);
  gtk_table_set_col_spacings(GTK_TABLE(table1), 5);

  label3 = gtk_label_new("Normal text color");
  gtk_widget_show(label3);
  gtk_table_attach(GTK_TABLE(table1), label3, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label3), 0, 0.5);

  shortcuts.label(label3);

  label4 = gtk_label_new("Background color");
  gtk_widget_show(label4);
  gtk_table_attach(GTK_TABLE(table1), label4, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label4), 0, 0.5);

  shortcuts.label(label4);

  label5 = gtk_label_new("Selected text color");
  gtk_widget_show(label5);
  gtk_table_attach(GTK_TABLE(table1), label5, 0, 1, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label5), 0, 0.5);

  shortcuts.label(label5);

  label6 = gtk_label_new("Selection color");
  gtk_widget_show(label6);
  gtk_table_attach(GTK_TABLE(table1), label6, 0, 1, 3, 4, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label6), 0, 0.5);

  shortcuts.label(label6);

  colorbutton_normal_text = gtk_color_button_new();
  gtk_widget_show(colorbutton_normal_text);
  gtk_table_attach(GTK_TABLE(table1), colorbutton_normal_text, 1, 2, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  colorbutton_background = gtk_color_button_new();
  gtk_widget_show(colorbutton_background);
  gtk_table_attach(GTK_TABLE(table1), colorbutton_background, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  colorbutton_selected_text = gtk_color_button_new();
  gtk_widget_show(colorbutton_selected_text);
  gtk_table_attach(GTK_TABLE(table1), colorbutton_selected_text, 1, 2, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  colorbutton_selection = gtk_color_button_new();
  gtk_widget_show(colorbutton_selection);
  gtk_table_attach(GTK_TABLE(table1), colorbutton_selection, 1, 2, 3, 4, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(fontcolordialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(fontcolordialog, NULL, &shortcuts, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(fontcolordialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(fontcolordialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  shortcuts.stockbutton(cancelbutton);
  shortcuts.stockbutton(okbutton);
  shortcuts.process();

  g_signal_connect((gpointer) checkbutton_font, "toggled", G_CALLBACK(on_checkbutton_font_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_colors, "toggled", G_CALLBACK(on_checkbutton_colors_toggled), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label3), colorbutton_normal_text);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label4), colorbutton_background);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label5), colorbutton_selected_text);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label6), colorbutton_selection);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label7), spinbutton_line_spacing);

  gtk_widget_grab_focus(okbutton);
  gtk_widget_grab_default(okbutton);

  // Make the settings in the GUI.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_font), initial_use_default_font);
  gtk_font_selection_set_font_name(GTK_FONT_SELECTION(fontselection), initial_font.c_str());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_colors), initial_use_default_color);
  GdkColor gdkcolor;
  color_decimal_to_gdk(initial_normal_text_color, &gdkcolor);
  gtk_color_button_set_color(GTK_COLOR_BUTTON(colorbutton_normal_text), &gdkcolor);
  color_decimal_to_gdk(initial_background_color, &gdkcolor);
  gtk_color_button_set_color(GTK_COLOR_BUTTON(colorbutton_background), &gdkcolor);
  color_decimal_to_gdk(initial_selected_text_color, &gdkcolor);
  gtk_color_button_set_color(GTK_COLOR_BUTTON(colorbutton_selected_text), &gdkcolor);
  color_decimal_to_gdk(initial_selection_color, &gdkcolor);
  gtk_color_button_set_color(GTK_COLOR_BUTTON(colorbutton_selection), &gdkcolor);
}


FontColorDialog::~FontColorDialog()
{
  gtk_widget_destroy(fontcolordialog);
}


int FontColorDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(fontcolordialog));
}


void FontColorDialog::on_checkbutton_font_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((FontColorDialog *) user_data)->on_checkbutton_font();
}


void FontColorDialog::on_checkbutton_font()
{
  bool sensitive = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_font));
  gtk_widget_set_sensitive(fontselection, sensitive);
  gtk_widget_set_sensitive(hbox_line_spacing, sensitive);
}


void FontColorDialog::on_checkbutton_colors_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((FontColorDialog *) user_data)->on_checkbutton_colors();
}


void FontColorDialog::on_checkbutton_colors()
{
  bool sensitive = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_colors));
  gtk_widget_set_sensitive(label3, sensitive);
  gtk_widget_set_sensitive(label4, sensitive);
  gtk_widget_set_sensitive(label5, sensitive);
  gtk_widget_set_sensitive(label6, sensitive);
  gtk_widget_set_sensitive(colorbutton_normal_text, sensitive);
  gtk_widget_set_sensitive(colorbutton_background, sensitive);
  gtk_widget_set_sensitive(colorbutton_selected_text, sensitive);
  gtk_widget_set_sensitive(colorbutton_selection, sensitive);
}


void FontColorDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((FontColorDialog *) user_data)->on_okbutton();
}


void FontColorDialog::on_okbutton()
{
  new_use_default_font = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_font));
  new_font = gtk_font_selection_get_font_name(GTK_FONT_SELECTION(fontselection));
  gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton_line_spacing));
  new_line_spacing = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton_line_spacing));
  new_use_default_color = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_colors));
  GdkColor gdkcolor;
  gtk_color_button_get_color(GTK_COLOR_BUTTON(colorbutton_normal_text), &gdkcolor);
  new_normal_text_color = color_gdk_to_decimal(&gdkcolor);
  gtk_color_button_get_color(GTK_COLOR_BUTTON(colorbutton_background), &gdkcolor);
  new_background_color = color_gdk_to_decimal(&gdkcolor);
  gtk_color_button_get_color(GTK_COLOR_BUTTON(colorbutton_selected_text), &gdkcolor);
  new_selected_text_color = color_gdk_to_decimal(&gdkcolor);
  gtk_color_button_get_color(GTK_COLOR_BUTTON(colorbutton_selection), &gdkcolor);
  new_selection_color = color_gdk_to_decimal(&gdkcolor);
}


