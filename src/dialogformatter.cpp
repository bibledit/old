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


#include "libraries.h"
#include <glib.h>
#include "dialogformatter.h"
#include "utilities.h"
#include "java.h"
#include "gtkwrappers.h"
#include "gwrappers.h"
#include "uname.h"
#include "unixwrappers.h"
#include "directories.h"
#include "gwrappers.h"
#include "shell.h"
#include "settings.h"
#include "gui.h"
#include "help.h"
#include "xep.h"
#include "tiny_utilities.h"


FormatterDialog::FormatterDialog (int dummy)
{
  // Save and initialize variables.
  extern Settings * settings;
  java_binary_path = settings->genconfig.java_binary_get ();
  xep_package_path = g_get_home_dir ();
  xep_package_path.append ("/");
  license_path = g_get_home_dir ();
  license_path.append ("/");
  xep_home = settings->genconfig.xep_home_get ();
  
  // Build GUI.
  formatterdialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (formatterdialog), "Formatter Setup");
  gtk_window_set_position (GTK_WINDOW (formatterdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal (GTK_WINDOW (formatterdialog), TRUE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (formatterdialog), TRUE);

  dialog_vbox1 = GTK_DIALOG (formatterdialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  expander1 = gtk_expander_new (NULL);
  gtk_widget_show (expander1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), expander1, TRUE, TRUE, 0);

  label_expander = gtk_label_new ("XEP has already been installed\nIf you would like to upgrade to a newer version, click here");
  gtk_widget_show (label_expander);
  gtk_expander_set_label_widget (GTK_EXPANDER (expander1), label_expander);

  vbox1 = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (expander1), vbox1);

  table1 = gtk_table_new (8, 4, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table1), 4);
  gtk_table_set_col_spacings (GTK_TABLE (table1), 4);

  image_java = gtk_image_new_from_stock ("gtk-yes", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_java);
  gtk_table_attach (GTK_TABLE (table1), image_java, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label_java_gui = gtk_label_new ("");
  gtk_widget_show (label_java_gui);
  gtk_table_attach (GTK_TABLE (table1), label_java_gui, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_java_gui), 0, 0.5);

  label24 = gtk_label_new ("Open the Java binary");
  gtk_widget_show (label24);
  gtk_table_attach (GTK_TABLE (table1), label24, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label24), 0, 0.5);

  button_java = gtk_button_new ();
  gtk_widget_show (button_java);
  gtk_table_attach (GTK_TABLE (table1), button_java, 3, 4, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (button_java), alignment3);

  hbox6 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox6);
  gtk_container_add (GTK_CONTAINER (alignment3), hbox6);

  image3 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image3);
  gtk_box_pack_start (GTK_BOX (hbox6), image3, FALSE, FALSE, 0);

  label25 = gtk_label_new_with_mnemonic ("_Java");
  gtk_widget_show (label25);
  gtk_box_pack_start (GTK_BOX (hbox6), label25, FALSE, FALSE, 0);

  label_install = gtk_label_new ("");
  gtk_widget_show (label_install);
  gtk_table_attach (GTK_TABLE (table1), label_install, 0, 3, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_install), 0, 0.5);

  image_install = gtk_image_new_from_stock ("gtk-no", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_install);
  gtk_table_attach (GTK_TABLE (table1), image_install, 0, 1, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label_install_gui = gtk_label_new ("");
  gtk_widget_show (label_install_gui);
  gtk_table_attach (GTK_TABLE (table1), label_install_gui, 1, 2, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_install_gui), 0, 0.5);

  label31 = gtk_label_new ("Install or upgrade XEP");
  gtk_widget_show (label31);
  gtk_table_attach (GTK_TABLE (table1), label31, 2, 3, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label31), 0, 0.5);

  button_install = gtk_button_new ();
  gtk_widget_show (button_install);
  gtk_table_attach (GTK_TABLE (table1), button_install, 3, 4, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  alignment6 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment6);
  gtk_container_add (GTK_CONTAINER (button_install), alignment6);

  hbox11 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox11);
  gtk_container_add (GTK_CONTAINER (alignment6), hbox11);

  image10 = gtk_image_new_from_stock ("gtk-execute", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image10);
  gtk_box_pack_start (GTK_BOX (hbox11), image10, FALSE, FALSE, 0);

  label32 = gtk_label_new_with_mnemonic ("_Install");
  gtk_widget_show (label32);
  gtk_box_pack_start (GTK_BOX (hbox11), label32, FALSE, FALSE, 0);

  label_license = gtk_label_new ("");
  gtk_widget_show (label_license);
  gtk_table_attach (GTK_TABLE (table1), label_license, 0, 3, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_license), 0, 0.5);

  image_license = gtk_image_new_from_stock ("gtk-no", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_license);
  gtk_table_attach (GTK_TABLE (table1), image_license, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label_license_gui = gtk_label_new ("ok");
  gtk_widget_show (label_license_gui);
  gtk_table_attach (GTK_TABLE (table1), label_license_gui, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_license_gui), 0, 0.5);

  label29 = gtk_label_new ("Open the XEP license");
  gtk_widget_show (label29);
  gtk_table_attach (GTK_TABLE (table1), label29, 2, 3, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label29), 0, 0.5);

  button_license = gtk_button_new ();
  gtk_widget_show (button_license);
  gtk_table_attach (GTK_TABLE (table1), button_license, 3, 4, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  alignment5 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment5);
  gtk_container_add (GTK_CONTAINER (button_license), alignment5);

  hbox10 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox10);
  gtk_container_add (GTK_CONTAINER (alignment5), hbox10);

  image8 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image8);
  gtk_box_pack_start (GTK_BOX (hbox10), image8, FALSE, FALSE, 0);

  label30 = gtk_label_new_with_mnemonic ("_License");
  gtk_widget_show (label30);
  gtk_box_pack_start (GTK_BOX (hbox10), label30, FALSE, FALSE, 0);

  label_package = gtk_label_new ("");
  gtk_widget_show (label_package);
  gtk_table_attach (GTK_TABLE (table1), label_package, 0, 3, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_package), 0, 0.5);

  image_xep = gtk_image_new_from_stock ("gtk-justify-fill", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_xep);
  gtk_table_attach (GTK_TABLE (table1), image_xep, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label_xep_gui = gtk_label_new ("ok");
  gtk_widget_show (label_xep_gui);
  gtk_table_attach (GTK_TABLE (table1), label_xep_gui, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_xep_gui), 0, 0.5);

  label27 = gtk_label_new ("Open the XEP installation package");
  gtk_widget_show (label27);
  gtk_table_attach (GTK_TABLE (table1), label27, 2, 3, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label27), 0, 0.5);

  button_xep = gtk_button_new ();
  gtk_widget_show (button_xep);
  gtk_table_attach (GTK_TABLE (table1), button_xep, 3, 4, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  alignment4 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment4);
  gtk_container_add (GTK_CONTAINER (button_xep), alignment4);

  hbox8 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox8);
  gtk_container_add (GTK_CONTAINER (alignment4), hbox8);

  image6 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image6);
  gtk_box_pack_start (GTK_BOX (hbox8), image6, FALSE, FALSE, 0);

  label28 = gtk_label_new_with_mnemonic ("_XEP");
  gtk_widget_show (label28);
  gtk_box_pack_start (GTK_BOX (hbox8), label28, FALSE, FALSE, 0);

  label_java = gtk_label_new ("");
  gtk_widget_show (label_java);
  gtk_table_attach (GTK_TABLE (table1), label_java, 0, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_java), 0, 0.5);

  dialog_action_area1 = GTK_DIALOG (formatterdialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (formatterdialog, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (formatterdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (formatterdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton, GTK_CAN_DEFAULT);

  g_signal_connect ((gpointer) button_java, "clicked",
                    G_CALLBACK (on_button_java_clicked),
                    gpointer(this));
  g_signal_connect ((gpointer) button_xep, "clicked",
                    G_CALLBACK (on_button_xep_clicked),
                    gpointer(this));
  g_signal_connect ((gpointer) button_license, "clicked",
                    G_CALLBACK (on_button_license_clicked),
                    gpointer(this));
  g_signal_connect ((gpointer) button_install, "clicked",
                    G_CALLBACK (on_button_install_clicked),
                    gpointer(this));
  g_signal_connect ((gpointer) okbutton, "clicked",
                    G_CALLBACK (on_okbutton_clicked),
                    gpointer(this));
  
  gtk_widget_grab_default (okbutton);
  
  // Set gui.
  set_gui ();
}


FormatterDialog::~FormatterDialog ()
{
  gtk_widget_destroy (formatterdialog);
}


int FormatterDialog::run ()
{
  return gtk_dialog_run (GTK_DIALOG (formatterdialog));
}


void FormatterDialog::on_okbutton_clicked (GtkButton *button, gpointer user_data)
{
  ((FormatterDialog *) user_data)->on_okbutton ();
}


void FormatterDialog::on_okbutton ()
{
  extern Settings * settings;
  // Store Java just to remember for next time.
  settings->genconfig.java_binary_set (java_binary_path);
  // Store xep home for printing.
  settings->genconfig.xep_home_set (xep_home);
}


void FormatterDialog::on_button_java_clicked (GtkButton *button, gpointer user_data)
{
  ((FormatterDialog *) user_data)->on_button_java ();
}


void FormatterDialog::on_button_java ()
{
  ustring filename = gtkw_file_chooser_open (formatterdialog, "Select a Java binary", java_binary_path);
  if (filename.empty()) return;  
  java_binary_path = filename;
  set_gui ();  
}


void FormatterDialog::on_button_xep_clicked (GtkButton *button, gpointer user_data)
{
  ((FormatterDialog *) user_data)->on_button_xep ();
}


void FormatterDialog::on_button_xep ()
{
  ustring filename = gtkw_file_chooser_open (formatterdialog, "Select the XEP package", xep_package_path);
  if (filename.empty()) return;
  xep_package_path = filename;
  set_gui ();  
}


void FormatterDialog::on_button_license_clicked (GtkButton *button, gpointer user_data)
{
  ((FormatterDialog *) user_data)->on_button_license ();
}


void FormatterDialog::on_button_license ()
{
  ustring filename = gtkw_file_chooser_open (formatterdialog, "Select the XEP license", license_path);
  if (filename.empty ()) return;
  license_path = filename;
  set_gui ();  
}


void FormatterDialog::on_button_install_clicked (GtkButton *button, gpointer user_data)
{
  ((FormatterDialog *) user_data)->on_button_install ();
}


void FormatterDialog::on_button_install ()
{
  xep_home = xep_install (java_binary_path, xep_package_path, license_path);
  set_gui ();  
}


void FormatterDialog::set_gui ()
{
  // Try to get a correct Java binary. XEP needs the Sun one, not GNU gij.
  bool java_ok = false;
  ustring java_information;
  if (java_search (java_binary_path)) {
    if (!java_binary_path.empty())
      java_ok = java_is_sun (java_binary_path, java_information);
  }

  // Set in gui whether java was ok.
  gui_okay (image_java, label_java_gui, java_ok);
  java_information = trim (java_information);
  if (java_ok)
    java_information.insert (0, "Binary: " + java_binary_path + "\n");
  else
    java_information.insert (0, "Binary not found in " + java_binary_path + "\n");
  gtk_label_set_text (GTK_LABEL (label_java), java_information.c_str());
 
  // See whether the XEP package is found.
  bool xep_ok = xep_find_installation_package (xep_package_path);
  
  // Set in gui whether the package is fine.
  gui_okay (image_xep, label_xep_gui, xep_ok);
  ustring xep_information (xep_package_path);
  if (xep_ok) 
    xep_information.insert (0, "Package is ");
  else
    xep_information.insert (0, "Package not found at ");
  gtk_label_set_text (GTK_LABEL (label_package), xep_information.c_str());
    
  // See whether the license is found.
  bool license_ok = xep_find_license (license_path);
  
  // Set in gui whether the license is fine.
  gui_okay (image_license, label_license_gui, license_ok);
  ustring license_information (license_path);
  if (license_ok) 
    license_information.insert (0, "License is ");
  else
    license_information.insert (0, "License not found at ");
  gtk_label_set_text (GTK_LABEL (label_license), license_information.c_str());
    
  // Set Install button sensitive is all's ready.
  gtk_widget_set_sensitive (button_install, java_ok && xep_ok && license_ok);
  
  // See whether XEP has been installed.
  bool xep_installed = xep_present ();
  
  // Info about installation.
  gtk_widget_set_sensitive (okbutton, xep_installed);
  ustring xep = xep_home;
  if (xep_installed)
    xep.insert (0, xep_version () + " installed in ");
  else
    xep.insert (0, "XEP installation not found in ");
  gtk_label_set_text (GTK_LABEL (label_install), xep.c_str());
  gui_okay (image_install, label_install_gui, xep_installed);
  
  // If Java or XEP are not installed, expand the gui.
  if ((!java_ok) || (!xep_installed)) {
    gtk_label_set_text (GTK_LABEL (label_expander), "");
    gtk_expander_set_expanded (GTK_EXPANDER (expander1), TRUE);
  }
}
