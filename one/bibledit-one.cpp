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


#include "bibledit-one.h"


int main (int argc, char *argv[])
{
  // Initialize GTK.
  gtk_init (&argc, &argv);

  // Information dialog.
  GtkWidget *dialog = gtk_message_dialog_new(NULL, GtkDialogFlags(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT), GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", "Bibledit is already running");
  // Add cancel button so that pressing the Escape button will also close the window,
  // but hide this button so it's not visible, but Escape remains working.
  GtkWidget *cancel = gtk_dialog_add_button(GTK_DIALOG(dialog), "", GTK_RESPONSE_CANCEL);
  gtk_widget_hide(cancel);
  // Run dialog.
  gtk_dialog_run(GTK_DIALOG(dialog));
  // After through, destroy it.
  gtk_widget_destroy(dialog);

  // Done.
  return 0;
}
