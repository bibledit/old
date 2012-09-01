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
#include "dialoginserttable.h"
#include "help.h"
#include "editor_aids.h"
#include "usfmtools.h"

InsertTableDialog::InsertTableDialog(const ustring & project)
{
  inserttabledialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(inserttabledialog), "Insert table");
  gtk_window_set_position(GTK_WINDOW(inserttabledialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(inserttabledialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(inserttabledialog));
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 6);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new("Insert a new table");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 0);

  table1 = gtk_table_new(2, 2, FALSE);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(vbox1), table1, TRUE, TRUE, 0);
  gtk_table_set_row_spacings(GTK_TABLE(table1), 4);
  gtk_table_set_col_spacings(GTK_TABLE(table1), 4);

  label2 = gtk_label_new_with_mnemonic("_Rows:");
  gtk_widget_show(label2);
  gtk_table_attach(GTK_TABLE(table1), label2, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label2), 1, 0.5);

  label3 = gtk_label_new_with_mnemonic("Co_lumns:");
  gtk_widget_show(label3);
  gtk_table_attach(GTK_TABLE(table1), label3, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label3), 1, 0.5);

  spinbutton_rows_adj = gtk_adjustment_new(2, 1, 100, 1, 10, 0);
  spinbutton_rows = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_rows_adj), 1, 0);
  gtk_widget_show(spinbutton_rows);
  gtk_table_attach(GTK_TABLE(table1), spinbutton_rows, 1, 2, 0, 1, (GtkAttachOptions) (GTK_EXPAND), (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_rows), TRUE);

  spinbutton_columns_adj = gtk_adjustment_new(2, 1, 4, 1, 10, 0);
  spinbutton_columns = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_columns_adj), 1, 0);
  gtk_widget_show(spinbutton_columns);
  gtk_table_attach(GTK_TABLE(table1), spinbutton_columns, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND), (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_columns), TRUE);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(inserttabledialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(inserttabledialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(inserttabledialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(inserttabledialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label2), spinbutton_rows);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label3), spinbutton_columns);

  gtk_widget_grab_focus(okbutton);
  gtk_widget_grab_default(okbutton);
}

InsertTableDialog::~InsertTableDialog()
{
  gtk_widget_destroy(inserttabledialog);
}

int InsertTableDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(inserttabledialog));
}

void InsertTableDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((InsertTableDialog *) user_data)->on_okbutton();
}

void InsertTableDialog::on_okbutton()
{
  unsigned int rows = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton_rows));
  unsigned int columns = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton_columns));

  for (unsigned int row = 0; row < rows; row++) {
    rawtext.append(usfm_get_full_opening_marker(style_get_table_row_marker(myproject)));
    for (unsigned int column = 1; column <= columns; column++) {
      rawtext.append(usfm_get_full_opening_marker(style_get_table_cell_marker(myproject, column)));
    }
  }
}
