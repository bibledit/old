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
#include "dialogarea.h"
#include "utilities.h"
#include <sqlite3.h>
#include "sqlite_reader.h"
#include "notes_utils.h"
#include "completion.h"
#include "gwrappers.h"
#include "settings.h"
#include "help.h"
#include "tiny_utilities.h"
#include <glib/gi18n.h>

AreaDialog::AreaDialog(int dummy)
{
  areadialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(areadialog), _("Area selection"));
  gtk_window_set_position(GTK_WINDOW(areadialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(areadialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(areadialog));
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 2);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox1), 2);

  label4 = gtk_label_new(_("Select which part of the text to work on"));
  gtk_widget_show(label4);
  gtk_box_pack_start(GTK_BOX(vbox1), label4, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label4), 0, 0.5);

  GSList *radiobutton_raw_group = NULL;

  radiobutton_raw = gtk_radio_button_new_with_mnemonic(NULL, _("The raw _USFM text"));
  gtk_widget_show(radiobutton_raw);
  gtk_box_pack_start(GTK_BOX(vbox1), radiobutton_raw, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_raw), radiobutton_raw_group);
  radiobutton_raw_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_raw));

  radiobutton_all = gtk_radio_button_new_with_mnemonic(NULL, _("_All the text except the USFM codes"));
  gtk_widget_show(radiobutton_all);
  gtk_box_pack_start(GTK_BOX(vbox1), radiobutton_all, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_all), radiobutton_raw_group);
  radiobutton_raw_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_all));

  radiobutton_categories = gtk_radio_button_new_with_mnemonic(NULL, _("C_ertain categories specified below"));
  gtk_widget_show(radiobutton_categories);
  gtk_box_pack_start(GTK_BOX(vbox1), radiobutton_categories, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_categories), radiobutton_raw_group);
  radiobutton_raw_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_categories));

  checkbutton_id = gtk_check_button_new_with_mnemonic(_("_Identifiers"));
  gtk_widget_show(checkbutton_id);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_id, FALSE, FALSE, 0);

  checkbutton_intro = gtk_check_button_new_with_mnemonic(_("I_ntroductions"));
  gtk_widget_show(checkbutton_intro);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_intro, FALSE, FALSE, 0);

  checkbutton_heading = gtk_check_button_new_with_mnemonic(_("_Titles and headings"));
  gtk_widget_show(checkbutton_heading);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_heading, FALSE, FALSE, 0);

  checkbutton_chapter = gtk_check_button_new_with_mnemonic(_("Cha_pter text"));
  gtk_widget_show(checkbutton_chapter);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_chapter, FALSE, FALSE, 0);

  checkbutton_study = gtk_check_button_new_with_mnemonic(_("_Study notes"));
  gtk_widget_show(checkbutton_study);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_study, FALSE, FALSE, 0);

  checkbutton_notes = gtk_check_button_new_with_mnemonic(_("_Foot- and endnotes"));
  gtk_widget_show(checkbutton_notes);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_notes, FALSE, FALSE, 0);

  checkbutton_xref = gtk_check_button_new_with_mnemonic(_("C_rossreferences"));
  gtk_widget_show(checkbutton_xref);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_xref, FALSE, FALSE, 0);

  checkbutton_verse = gtk_check_button_new_with_mnemonic(_("_Verse text"));
  gtk_widget_show(checkbutton_verse);
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton_verse, FALSE, FALSE, 0);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(areadialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(areadialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(areadialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(areadialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  g_signal_connect((gpointer) radiobutton_raw, "toggled", G_CALLBACK(on_radiobutton_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_all, "toggled", G_CALLBACK(on_radiobutton_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_categories, "toggled", G_CALLBACK(on_radiobutton_toggled), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(companiondialog_on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton);

  // Set the values in the gui from the session object.
  extern Settings *settings;
  switch (settings->session.area_type) {
  case atRaw:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_raw), true);
    break;
  case atAll:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_all), true);
    break;
  case atSelection:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_categories), true);
    break;
  }
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_id), settings->session.area_id);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_intro), settings->session.area_intro);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_heading), settings->session.area_heading);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_chapter), settings->session.area_chapter);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_study), settings->session.area_study);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_notes), settings->session.area_notes);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_xref), settings->session.area_xref);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_verse), settings->session.area_verse);

  set_gui();
}


AreaDialog::~AreaDialog()
{
  gtk_widget_destroy(areadialog);
}


int AreaDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(areadialog));
}


void AreaDialog::companiondialog_on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((AreaDialog *) user_data)->on_ok();
}


void AreaDialog::on_radiobutton_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((AreaDialog *) user_data)->set_gui();
}


void AreaDialog::on_ok()
{
  extern Settings *settings;
  settings->session.area_type = get_area_type();
  settings->session.area_id = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_id));
  settings->session.area_intro = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_intro));
  settings->session.area_heading = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_heading));
  settings->session.area_chapter = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_chapter));
  settings->session.area_study = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_study));
  settings->session.area_notes = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_notes));
  settings->session.area_xref = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_xref));
  settings->session.area_verse = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_verse));
}


void AreaDialog::set_gui()
{
  AreaType areatype = get_area_type();
  switch (areatype) {
  case atRaw:
  case atAll:
    set_selectors_sensitive(false);
    break;
  case atSelection:
    set_selectors_sensitive(true);
    break;
  }
}


AreaType AreaDialog::get_area_type()
{
  AreaType returnvalue = atRaw;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_all)))
    returnvalue = atAll;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_categories)))
    returnvalue = atSelection;
  return returnvalue;
}


void AreaDialog::set_selectors_sensitive(bool sensitive)
{
  gtk_widget_set_sensitive(checkbutton_id, sensitive);
  gtk_widget_set_sensitive(checkbutton_intro, sensitive);
  gtk_widget_set_sensitive(checkbutton_heading, sensitive);
  gtk_widget_set_sensitive(checkbutton_chapter, sensitive);
  gtk_widget_set_sensitive(checkbutton_study, sensitive);
  gtk_widget_set_sensitive(checkbutton_notes, sensitive);
  gtk_widget_set_sensitive(checkbutton_xref, sensitive);
  gtk_widget_set_sensitive(checkbutton_verse, sensitive);
}


ustring area_information()
// Returns certain text that indicates the currently selected area to work on in the text.
{
  vector < ustring > areas;
  ustring text;
  extern Settings *settings;
  switch (settings->session.area_type) {
  case atRaw:
    areas.push_back(_("Raw text"));
    break;
  case atAll:
    areas.push_back(_("All text"));
    break;
  case atSelection:
    if (settings->session.area_id)
      areas.push_back(_("Identifiers"));
    if (settings->session.area_intro)
      areas.push_back(_("Introductions"));
    if (settings->session.area_heading)
      areas.push_back(_("Headings"));
    if (settings->session.area_chapter)
      areas.push_back(_("Chapter text"));
    if (settings->session.area_study)
      areas.push_back(_("Study notes"));
    if (settings->session.area_notes)
      areas.push_back(_("Foot- and endnotes"));
    if (settings->session.area_xref)
      areas.push_back(_("Crossreferences"));
    if (settings->session.area_verse)
      areas.push_back(_("Verse text"));
    break;
  }
  if (areas.size() > 3) {
    text = convert_to_string(int (areas.size())) + _(" Areas");
  } else {
    for (unsigned int i = 0; i < areas.size(); i++) {
      if (!text.empty())
        text.append(", ");
      text.append(areas[i]);
    }
  }
  if (areas.empty())
    text = _("No areas selected");
  return text;
}

