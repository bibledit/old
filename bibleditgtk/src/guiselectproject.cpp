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
#include <glib.h>
#include "guiselectproject.h"
#include "projectutils.h"
#include "dialoglistview.h"

SelectProjectGui::SelectProjectGui(int dummy)
{
}

SelectProjectGui::~SelectProjectGui()
{
}

void SelectProjectGui::build(GtkWidget * box, const gchar * labeltext, const ustring & project_in)
{
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox);
  gtk_box_pack_start(GTK_BOX(box), hbox, TRUE, TRUE, 0);

  label = gtk_label_new_with_mnemonic(labeltext);
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

  // Check whether the requested projects is there, if so, well, fine.
  // If not, take the first available project, if there are any projects at all.
  project = project_in;
  vector < ustring > projects = projects_get_all();
  set < ustring > projectset(projects.begin(), projects.end());
  if (projectset.find(project) == projectset.end()) {
    project.clear();
    if (!projects.empty())
      project = projects[0];
  }

  button = gtk_button_new_with_label(project.c_str());
  gtk_widget_show(button);
  gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 0);

  g_signal_connect((gpointer) button, "clicked", G_CALLBACK(on_button_clicked), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label), button);
}

void SelectProjectGui::on_button_clicked(GtkButton * button, gpointer user_data)
{
  ((SelectProjectGui *) user_data)->on_button();
}

void SelectProjectGui::on_button()
{
  if (project_select(project)) {
    gtk_button_set_label(GTK_BUTTON(button), project.c_str());
  }
}

void SelectProjectGui::focus()
{
  gtk_widget_grab_focus(button);
}

void SelectProjectGui::set_label(const ustring & text)
{
  gtk_label_set_text_with_mnemonic(GTK_LABEL(label), text.c_str());
}
