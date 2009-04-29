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
#include "dialognewresource.h"
#include "help.h"
#include "dialoglistview.h"
#include "gui.h"
#include "tiny_utilities.h"
#include "dialogbooknames.h"
#include "shell.h"
#include "gwrappers.h"
#include "directories.h"
#include "utilities.h"
#include "books.h"
#include "unixwrappers.h"
#include "gtkwrappers.h"
#include "progresswindow.h"
#include "dialogresourceconverter2.h"

NewResourceDialog::NewResourceDialog(const ustring & templatefile)
{
}


// URL constructor.

void NewResourceDialog::on_entry_url_constructor_changed(GtkEditable * editable, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_entry_url_constructor();
}

void NewResourceDialog::on_entry_url_constructor()
{
  url_constructor = gtk_entry_get_text(GTK_ENTRY(entry_url_constructor));
  url_constructor = trim(url_constructor);
}

bool NewResourceDialog::url_constructor_gui()
{
  bool ok = true;
  ustring text = "Enter the url constructor";
  if (url_constructor.empty())
    ok = false;
  if (ok && (url_constructor.find(resource_url_constructor_book()) == string::npos)) {
    ok = false;
    text.append(" - no \"");
    text.append(resource_url_constructor_book());
    text.append("\" found");
  }
  if (ok && (url_constructor.find(resource_url_constructor_chapter()) == string::npos)) {
    ok = false;
    text.append(" - no \"");
    text.append(resource_url_constructor_chapter());
    text.append("\" found");
  }
  if (ok && (url_constructor.find(resource_url_constructor_verse()) == string::npos)) {
    ok = false;
    text.append(" - no \"");
    text.append(resource_url_constructor_verse());
    text.append("\" found");
  }
  gui_okay(image_url_constructor_ok, label_url_constructor_ok, ok);
  gtk_label_set_text(GTK_LABEL(label_url_constructor), text.c_str());
  return ok;
}

// Index file constructor.

void NewResourceDialog::on_entry_index_constructor_changed(GtkEditable * editable, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_entry_index_constructor();
}

void NewResourceDialog::on_entry_index_constructor()
{
  index_constructor = gtk_entry_get_text(GTK_ENTRY(entry_index_constructor));
  index_constructor = trim(index_constructor);
}

bool NewResourceDialog::index_constructor_gui()
{
  bool ok = true;
  ustring text = "Enter the index file constructor";
  if (index_constructor.empty())
    ok = false;
  if (ok && (index_constructor.find(resource_url_constructor_book()) == string::npos)) {
    ok = false;
    text.append(" - no \"");
    text.append(resource_url_constructor_book());
    text.append("\" found");
  }
  if (ok && (index_constructor.find(resource_url_constructor_chapter()) == string::npos)) {
    ok = false;
    text.append(" - no \"");
    text.append(resource_url_constructor_chapter());
    text.append("\" found");
  }
  if (ok && (index_constructor.find(resource_url_constructor_verse()) == string::npos)) {
    ok = false;
    text.append(" - no \"");
    text.append(resource_url_constructor_verse());
    text.append("\" found");
  }
  gui_okay(image_index_constructor_ok, label_index_constructor_ok, ok);
  gtk_label_set_text(GTK_LABEL(label_index_constructor), text.c_str());
  return ok;
}


// Anchor names.

void NewResourceDialog::on_anchors_button_clicked(GtkButton * button, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_anchors_button();
}

void NewResourceDialog::on_anchors_button()
{
  BooknamesDialog dialog(anchors, "Enter the books as they are in the anchors", "Book in anchor");
  if (dialog.run() == GTK_RESPONSE_OK) {
    anchors = dialog.newbooks;
    if (anchors_gui()) {
    }
  }
}

bool NewResourceDialog::anchors_gui()
{
  bool ok = true;
  gtk_label_set_text(GTK_LABEL(label_anchors_short), "Enter the book names for in the anchors");
  if (anchors.empty())
    ok = false;
  ustring text;
  text.append("Books entered: ");
  text.append(convert_to_string(anchors.size()));
  gtk_label_set_text(GTK_LABEL(label_anchors_long), text.c_str());
  gui_okay(image_anchors_ok, label_anchors_ok, ok);
  return ok;
}

// Write anchors.

void NewResourceDialog::on_checkbutton_write_anchors_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_checkbutton_write_anchors();
}

void NewResourceDialog::on_checkbutton_write_anchors()
{
}

void NewResourceDialog::on_button_write_anchors_clicked(GtkButton * button, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_button_write_anchors();
}

void NewResourceDialog::on_button_write_anchors()
{
  /*
  ResourceConverter2Dialog dialog(workingdirectory);
  if (dialog.run() == GTK_RESPONSE_OK) {
    write_anchors_gui();
  }
  */
}

bool NewResourceDialog::write_anchors_gui()
{
  bool ok = true;
  ok = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_write_anchors));
  gui_okay(image_write_anchors_ok, label_write_anchors_ok, ok);
  ustring text;
  if (ok)
    text = "All anchors are considered to be there";
  else
    text = "Click button to write anchors";
  gtk_label_set_text(GTK_LABEL(label_write_anchors), text.c_str());
  return ok;
}

// Lower home page.

void NewResourceDialog::on_entry_lower_home_page_changed(GtkEditable * editable, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_entry_lower_home_page();
}

void NewResourceDialog::on_entry_lower_home_page()
{
  lower_home_page = gtk_entry_get_text(GTK_ENTRY(entry_lower_home_page));
  lower_home_page = trim(lower_home_page);
}

bool NewResourceDialog::lower_home_page_gui()
{
  bool ok = true;
  ustring text = "Enter the home page for the lower browser";
  if (lower_home_page.empty())
    ok = false;
  gui_okay(image_lower_home_page_ok, label_lower_home_page_ok, ok);
  gtk_label_set_text(GTK_LABEL(label_lower_home_page), text.c_str());
  return ok;
}

// Lower home page.

void NewResourceDialog::on_entry_lower_url_filter_changed(GtkEditable * editable, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_entry_lower_url_filter();
}

void NewResourceDialog::on_entry_lower_url_filter()
{
  lower_url_filter = gtk_entry_get_text(GTK_ENTRY(entry_lower_url_filter));
  lower_url_filter = trim(lower_url_filter);
}

bool NewResourceDialog::lower_url_filter_gui()
{
  bool ok = true;
  ustring text = "Enter the URL filter for the lower browser";
  if (lower_url_filter.empty())
    ok = false;
  gui_okay(image_lower_url_filter_ok, label_lower_url_filter_ok, ok);
  gtk_label_set_text(GTK_LABEL(label_lower_url_filter), text.c_str());
  return ok;
}


// Todo this one goes out once all's fine.
