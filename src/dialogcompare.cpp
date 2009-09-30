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


#include "libraries.h"
#include "dialogcompare.h"
#include "projectutils.h"
#include "directories.h"
#include "combobox.h"
#include "keyboard.h"
#include "settings.h"
#include "generalconfig.h"
#include "help.h"
#include "compareutils.h"
#include "dialogselectbooks.h"
#include "scriptureportions.h"
#include "shortcuts.h"


CompareDialog::CompareDialog(WindowReferences * references_window)
{
  // Store variables
  my_references_window = references_window;
  extern Settings *settings;

  Shortcuts shortcuts(0);

  comparedialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(comparedialog), "Compare project");
  gtk_window_set_position(GTK_WINDOW(comparedialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(comparedialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(comparedialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new("Select a project to compare the current project with\n" "and then press OK");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 4);
  gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

  selectprojectgui = new SelectProjectGui(0);
  selectprojectgui->build(vbox1, "", settings->genconfig.project_to_compare_with_get());

  checkbutton1 = gtk_check_button_new_with_mnemonic("Print changed verses only");
  gtk_widget_show(checkbutton1);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton1, FALSE, FALSE, 0);

  shortcuts.button(checkbutton1);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton1), settings->genconfig.print_changes_only_get());

  hbox1 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);

  label2 = gtk_label_new("Portion");
  gtk_widget_show(label2);
  gtk_box_pack_start(GTK_BOX(hbox1), label2, FALSE, FALSE, 0);

  label_portion = gtk_label_new("");
  gtk_widget_show(label_portion);
  gtk_box_pack_start(GTK_BOX(hbox1), label_portion, FALSE, FALSE, 0);

  vseparator1 = gtk_vseparator_new();
  gtk_widget_show(vseparator1);
  gtk_box_pack_start(GTK_BOX(hbox1), vseparator1, TRUE, TRUE, 0);

  button_change = gtk_button_new();
  gtk_widget_show(button_change);
  gtk_box_pack_start(GTK_BOX(hbox1), button_change, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(button_change), alignment1);

  hbox2 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox2);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox2);

  image1 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox2), image1, FALSE, FALSE, 0);

  label4 = gtk_label_new_with_mnemonic("C_hange");
  gtk_widget_show(label4);
  gtk_box_pack_start(GTK_BOX(hbox2), label4, FALSE, FALSE, 0);

  shortcuts.label(label4);

  dialog_action_area1 = GTK_DIALOG(comparedialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(comparedialog, NULL, &shortcuts, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(comparedialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(comparedialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton);
  shortcuts.stockbutton(okbutton);
  shortcuts.process();

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(static_on_okbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) button_change, "clicked", G_CALLBACK(on_button_portion_clicked), gpointer(this));

  selectprojectgui->focus();
  gtk_widget_grab_default(okbutton);

  // Set gui.
  gui_reorder_include();
}


CompareDialog::~CompareDialog()
{
  delete selectprojectgui;
  gtk_widget_destroy(comparedialog);
}


int CompareDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(comparedialog));
}


bool CompareDialog::on_combobox1_key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  return ((CompareDialog *) user_data)->on_combobox1_key_press(event);
}


bool CompareDialog::on_combobox1_key_press(GdkEventKey * event)
{
  if (keyboard_enter_pressed(event)) {
    on_okbutton_clicked();
    gtk_dialog_response(GTK_DIALOG(comparedialog), GTK_RESPONSE_OK);
    return true;
  }
  return false;
}


void CompareDialog::static_on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((CompareDialog *) user_data)->on_okbutton_clicked();
}


void CompareDialog::on_okbutton_clicked()
// Ok handler: do the comparison.
{
  // Store user entered data.
  extern Settings *settings;
  settings->genconfig.project_to_compare_with_set(selectprojectgui->project);
  settings->genconfig.print_changes_only_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton1)));
  // Run comparison.
  compare_with(my_references_window, settings->genconfig.project_to_compare_with_get(), settings->genconfig.project_get(), settings->genconfig.print_changes_only_get());
}


void CompareDialog::on_button_portion_clicked(GtkButton * button, gpointer user_data)
{
  ((CompareDialog *) user_data)->on_button_portion();
}


void CompareDialog::on_button_portion()
{
  SelectBooksDialog dialog(true);
  if (dialog.run() == GTK_RESPONSE_OK) {
    gui_reorder_include();
  }
}


void CompareDialog::gui_reorder_include()
// Set labels whether the order of the books is standard and everything is included.
{
  extern Settings *settings;
  ScripturePortions scriptureportions(settings->genconfig.project_get());
  if (scriptureportions.reordering_portions_all())
    gtk_label_set_text(GTK_LABEL(label_portion), CHAPTER_VERSE_SELECTION_ALL);
  else
    gtk_label_set_text(GTK_LABEL(label_portion), "part");
}


