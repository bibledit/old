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
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **  
 */

#include "libraries.h"
#include <glib.h>
#include "windowcheckusfm.h"
#include "windows.h"
#include "settings.h"
#include "shortcuts.h"


WindowCheckUSFM::WindowCheckUSFM(GtkAccelGroup * accelerator_group, bool startup):
WindowBase(widCheckUSFM, "Check USFM", startup, 0)
// Window for checking USFM.
{
  Shortcuts shortcuts (0);
  
  vbox = gtk_vbox_new (FALSE, 6);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  checkbutton_verses_at_start = gtk_check_button_new_with_mnemonic ("Verses are at line start");
  gtk_widget_show (checkbutton_verses_at_start);
  gtk_box_pack_start (GTK_BOX (vbox), checkbutton_verses_at_start, FALSE, FALSE, 0);
  
  shortcuts.button (checkbutton_verses_at_start);

  vbox_filter = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_filter);
  gtk_box_pack_start (GTK_BOX (vbox), vbox_filter, TRUE, TRUE, 0);

  label_filter = gtk_label_new ("Filter");
  gtk_widget_show (label_filter);
  gtk_box_pack_start (GTK_BOX (vbox_filter), label_filter, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_filter), 0, 0.5);
  
  shortcuts.label (label_filter);

  combobox_filter = gtk_combo_box_new_text ();
  gtk_widget_show (combobox_filter);
  gtk_box_pack_start (GTK_BOX (vbox_filter), combobox_filter, FALSE, FALSE, 0);

  button_filter = gtk_button_new ();
  gtk_widget_show (button_filter);
  gtk_box_pack_start (GTK_BOX (vbox_filter), button_filter, FALSE, FALSE, 0);

  alignment4 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment4);
  gtk_container_add (GTK_CONTAINER (button_filter), alignment4);

  hbox6 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox6);
  gtk_container_add (GTK_CONTAINER (alignment4), hbox6);

  image4 = gtk_image_new_from_stock ("gtk-clear", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image4);
  gtk_box_pack_start (GTK_BOX (hbox6), image4, FALSE, FALSE, 0);

  label10 = gtk_label_new_with_mnemonic ("Filter text");
  gtk_widget_show (label10);
  gtk_box_pack_start (GTK_BOX (hbox6), label10, FALSE, FALSE, 0);
  
  shortcuts.label (label10);

  button_discover_markup = gtk_button_new ();
  gtk_widget_show (button_discover_markup);
  gtk_box_pack_start (GTK_BOX (vbox), button_discover_markup, FALSE, FALSE, 0);

  alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (button_discover_markup), alignment3);

  hbox5 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox5);
  gtk_container_add (GTK_CONTAINER (alignment3), hbox5);

  image3 = gtk_image_new_from_stock ("gtk-zoom-fit", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image3);
  gtk_box_pack_start (GTK_BOX (hbox5), image3, FALSE, FALSE, 0);

  label8 = gtk_label_new_with_mnemonic ("Discover markup");
  gtk_widget_show (label8);
  gtk_box_pack_start (GTK_BOX (hbox5), label8, FALSE, FALSE, 0);

  shortcuts.label (label8);
  
  hbox_information = gtk_hbox_new (FALSE, 5);
  gtk_widget_show (hbox_information);
  gtk_box_pack_start (GTK_BOX (vbox), hbox_information, TRUE, TRUE, 0);

  image5 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image5);
  gtk_box_pack_start (GTK_BOX (hbox_information), image5, FALSE, TRUE, 0);

  label_information_ok = gtk_label_new ("Ok");
  gtk_widget_show (label_information_ok);
  gtk_box_pack_start (GTK_BOX (hbox_information), label_information_ok, FALSE, FALSE, 0);

  label_information_text = gtk_label_new ("Information");
  gtk_widget_show (label_information_text);
  gtk_box_pack_start (GTK_BOX (hbox_information), label_information_text, FALSE, FALSE, 0);
  gtk_label_set_line_wrap (GTK_LABEL (label_information_text), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label_information_text), 0, 0.5);

  gtk_label_set_mnemonic_widget(GTK_LABEL(label_filter), combobox_filter);

  g_signal_connect((gpointer) combobox_filter, "visibility-notify-event", G_CALLBACK(on_visibility_notify_event), gpointer(this));
  g_signal_connect((gpointer) button_discover_markup, "visibility-notify-event", G_CALLBACK(on_visibility_notify_event), gpointer(this));
  g_signal_connect((gpointer) label_information_text, "visibility-notify-event", G_CALLBACK(on_visibility_notify_event), gpointer(this));

  g_signal_connect ((gpointer) button_filter, "clicked", G_CALLBACK (on_button_filter_clicked), gpointer(this));
  g_signal_connect ((gpointer) button_discover_markup, "clicked", G_CALLBACK (on_button_discover_markup_clicked), gpointer(this));
  
  shortcuts.process();
}


WindowCheckUSFM::~WindowCheckUSFM()
{
}


void WindowCheckUSFM::on_button_discover_markup_clicked (GtkButton *button, gpointer user_data)
{
  ((WindowCheckUSFM *) user_data)->on_button_discover_markup();
}


void WindowCheckUSFM::on_button_discover_markup ()
{
}

  
void WindowCheckUSFM::on_button_filter_clicked (GtkButton *button, gpointer user_data)
{
  ((WindowCheckUSFM *) user_data)->on_button_filter();
}


void WindowCheckUSFM::on_button_filter ()
{
}


