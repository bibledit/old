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


#include "utilities.h"
#include "shortcuts.h"


Shortcuts::Shortcuts(int dummy)
/*
In many dialogs there are various texts which need to have their underscore
character to get a shortcut.
Shortcuts need to be unique within one dialog for quick operation.
This objects adds shortcuts to the texts given to them, and ensures they 
are unique within the set given.
*/
{
  // Can't make a shortcut out of a space.
  unavailables.insert(" ");
}


Shortcuts::~Shortcuts()
{
  for (unsigned int i = 0; i < created_widgets.size(); i++) {
    gtk_widget_destroy (created_widgets[i]);
  }
}


void Shortcuts::stockbutton(GtkWidget * widget)
{
  if (widget == NULL)
    return;
  GtkStockItem stockitem;
  if (gtk_stock_lookup(gtk_button_get_label(GTK_BUTTON(widget)), &stockitem)) {
    ustring s(stockitem.label);
    size_t pos = s.find("_");
    pos++;
    if (pos < s.length())
      unavailables.insert(s.substr(pos, 1).casefold());
  }
  lastwidget = widget;
}


void Shortcuts::button(GtkWidget * widget)
{
  if (widget == NULL)
    return;
  widgets.push_back(widget);
  is_button.push_back(true);
  lastwidget = widget;
}


void Shortcuts::label(GtkWidget * widget)
{
  if (widget == NULL)
    return;
  widgets.push_back(widget);
  is_button.push_back(false);
  lastwidget = widget;
}


void Shortcuts::process()
// Removes any existing shortcuts, and insert new ones.
{
  // First pass: Go through the widgets and the ones that have the first 
  // character of their label not yet used as a shortcut, set it as the shortcut.
  // Store the other ones for the next pass.
  vector < GtkWidget * >widgets2;
  vector < bool > is_button2;
  for (unsigned int i = 0; i < widgets.size(); i++) {
    ustring label;
    if (is_button[i])
      label = gtk_button_get_label(GTK_BUTTON(widgets[i]));
    else
      label = gtk_label_get_text(GTK_LABEL(widgets[i]));
    replace_text(label, "_", "");
    if (!label.empty()) {
      ustring start = label.substr(0, 1).casefold();
      if (unavailables.find(start) == unavailables.end()) {
        label.insert(0, "_");
        if (is_button[i])
          gtk_button_set_label(GTK_BUTTON(widgets[i]), label.c_str());
        else
          gtk_label_set_text_with_mnemonic(GTK_LABEL(widgets[i]), label.c_str());
        unavailables.insert(start);
      } else {
        widgets2.push_back(widgets[i]);
        is_button2.push_back(is_button[i]);
      }
    }
  }
  // Second pass: use the next available character of a widget's label as a 
  // shortcut.
  for (unsigned int i = 0; i < widgets2.size(); i++) {
    ustring label;
    if (is_button2[i])
      label = gtk_button_get_label(GTK_BUTTON(widgets2[i]));
    else
      label = gtk_label_get_text(GTK_LABEL(widgets2[i]));
    replace_text(label, "_", "");
    for (unsigned int i2 = 1; i2 < label.length(); i2++) {
      ustring character = label.substr(i2, 1).casefold();
      if (unavailables.find(character) == unavailables.end()) {
        label.insert(i2, "_");
        if (is_button2[i])
          gtk_button_set_label(GTK_BUTTON(widgets2[i]), label.c_str());
        else
          gtk_label_set_text_with_mnemonic(GTK_LABEL(widgets2[i]), label.c_str());
        unavailables.insert(character);
        break;
      }
    }
  }
}


void Shortcuts::consider_assistant ()
// Considers the shortcuts normally used in an assistant in addition to the content of the page.
{
  create_widget (GTK_STOCK_HELP);
  create_widget (GTK_STOCK_CANCEL);
  create_widget (GTK_STOCK_GO_BACK);
  create_widget (GTK_STOCK_GO_FORWARD);
  create_widget (GTK_STOCK_GOTO_LAST);
}


void Shortcuts::create_widget (const gchar * stock)
{
  GtkWidget * widget;
  widget = gtk_button_new_from_stock (stock);
  stockbutton (widget);
  created_widgets.push_back (widget);
}

