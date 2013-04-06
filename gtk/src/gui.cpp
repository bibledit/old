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
#include "gui.h"
#include "gtkwrappers.h"
#include "settings.h"
#include "projectutils.h"
#include "books.h"
#include "utilities.h"
#include "tiny_utilities.h"

void gui_okay(GtkWidget * image, GtkWidget * label, bool ok)
// Sets image and text in the GUI whether things are okay or not.
{
  if (ok) {
    gtk_image_set_from_stock(GTK_IMAGE(image), "gtk-yes", GTK_ICON_SIZE_BUTTON);
    gtk_label_set_text(GTK_LABEL(label), "Done:");
  } else {
    gtk_image_set_from_stock(GTK_IMAGE(image), "gtk-dialog-warning", GTK_ICON_SIZE_BUTTON);
    gtk_label_set_text(GTK_LABEL(label), "To do:");
  }
}

bool gui_double_question(GtkWidget * parent, ustring question1, ustring question2)
// Returns true only if the user replies "Yes" to both questions.
{
  if (question1.empty())
    question1 = "Are you sure you wish to make the changes?";
  if (gtkw_dialog_question(parent, question1.c_str()) == GTK_RESPONSE_NO)
    return false;
  if (question2.empty())
    question2 = "This will permanently modify your data.\n" "The changes cannot be undone easily.\n" "Are you sure?";
  if (gtkw_dialog_question(parent, question2.c_str()) == GTK_RESPONSE_NO)
    return false;
  return true;
}

ustring gui_book_selection_information(const ustring & project)
/*
Returns text to be displayed typically beside the "select books" button.
The text contains a bit of information about the current selection.
It returns a maximum of three units, e.g.:
No books (1 unit)
John, Hebrew (2 units)
Genesis, Exodus, Leviticus (3 units)
Old Testament, Mark (2 units)
etc.
*/
{
  // Settings
  extern Settings *settings;

  // Language.
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  ustring language = projectconfig->language_get();

  // Storage for the units.
  vector < ustring > units;

  // Message to return.
  ustring message;

  // Deal with empty selection.
  set < unsigned int >selection = settings->session.selected_books;
  if (selection.empty()) {
    message = "No books";
    return message;
  }
  // See whether OT / NT is in the selection.
  /*
     At first it only gave "Old Testament" and/or "New Testament" when all 39
     and/or 27 books were there.
     But it was preferrable that e.g. "Old Testament" would be given, not only when
     the whole OT was there, but also when all OT books that are currently in the 
     project were there. E.g. if the project contains Genesis and Exodus, and both
     are selected, it should also give "Old Testament".
   */
  vector < unsigned int >selectable_books = project_get_books(project);

  // Is the OT in the selection? And the NT?
  unsigned int selectable_ot_books = 0;
  unsigned int selectable_nt_books = 0;
  for (unsigned int i = 0; i < selectable_books.size(); i++) {
    BookType type = books_id_to_type(selectable_books[i]);
    if (type == btOldTestament)
      selectable_ot_books++;
    if (type == btNewTestament)
      selectable_nt_books++;
  }
  unsigned int selected_ot_books = 0;
  vector < unsigned int >ot_ids = books_type_to_ids(btOldTestament);
  for (unsigned int i = 0; i < ot_ids.size(); i++) {
    if (selection.find(ot_ids[i]) != selection.end())
      selected_ot_books++;
  }
  if ((selected_ot_books > 1) && (selectable_ot_books == selected_ot_books)) {
    units.push_back("Old Testament");
    for (unsigned int i = 0; i < ot_ids.size(); i++) {
      set < unsigned int >::iterator iter;
      iter = selection.find(ot_ids[i]);
      if (iter != selection.end())
        selection.erase(iter);
    }
  }
  unsigned int selected_nt_books = 0;
  vector < unsigned int >nt_ids = books_type_to_ids(btNewTestament);
  for (unsigned int i = 0; i < nt_ids.size(); i++) {
    if (selection.find(nt_ids[i]) != selection.end())
      selected_nt_books++;
  }
  if ((selected_nt_books > 1) && (selectable_nt_books == selected_nt_books)) {
    units.push_back("New Testament");
    for (unsigned int i = 0; i < nt_ids.size(); i++) {
      set < unsigned int >::iterator iter;
      iter = selection.find(nt_ids[i]);
      if (iter != selection.end())
        selection.erase(iter);
    }
  }
  // Deal with any remaining books.  
  if ((selection.size() + units.size()) > 3) {
    units.push_back(convert_to_string(int (selection.size())) + " books");
  } else {
    vector < unsigned int >all_ids = books_type_to_ids(btUnknown);
    for (unsigned int i = 0; i < all_ids.size(); i++) {
      if (selection.find(all_ids[i]) != selection.end()) {
        units.push_back(books_id_to_name(language, all_ids[i]));
      }
    }
  }

  // Assemble the message.
  for (unsigned int i = 0; i < units.size(); i++) {
    if (!message.empty())
      message.append(" + ");
    message.append(units[i]);
  }

  return message;
}
