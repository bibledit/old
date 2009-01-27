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
#include <gtk/gtk.h>
#include "dialogunicode.h"
#include "utilities.h"
#include "dialoglistview.h"
#include "keyboard.h"
#include "shell.h"
#include "import.h"
#include "help.h"
#include "gwrappers.h"
#include "settings.h"
#include "tiny_utilities.h"

UnicodeDialog::UnicodeDialog(const ustring & filetoconvert)
{
  // Store variables
  filename = filetoconvert;

  unicodedialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(unicodedialog), "Convert to UTF-8 Unicode");
  gtk_window_set_position(GTK_WINDOW(unicodedialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(unicodedialog), TRUE);
  gtk_window_set_default_size(GTK_WINDOW(unicodedialog), 480, 480);

  dialog_vbox1 = GTK_DIALOG(unicodedialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  // First informative label
  label1 = gtk_label_new("The book you are now opening is not in the right Unicode format.\n" "It needs to be converted before use.\n\n" "An encoding has been selected, and the result after conversion is showing.\n" "Click the encoding and select another one, till you are happy with the result.\n" "Press OK, and the book will be converted.");

  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 4);
  gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

  button1 = gtk_button_new_with_mnemonic("");
  gtk_widget_show(button1);
  gtk_box_pack_start(GTK_BOX(vbox1), button1, FALSE, FALSE, 9);

  // Load the whole list of possible encodings.
  // Display the encoding selected last, if there is any, 
  // or else the first entry, if there is any.
  GwSpawn spawn("iconv");
  spawn.arg("-l");
  spawn.read();
  spawn.run();
  for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
    ustring s = spawn.standardout[i];
    s = trim(s);
    if (g_str_has_suffix(s.c_str(), "//"))
      s = s.substr(0, s.length() - 2);
    encodings.push_back(s);
  }
  extern Settings *settings;
  set < ustring > encodings_set(encodings.begin(), encodings.end());
  if (encodings_set.find(settings->genconfig.encoding_get()) != encodings_set.end()) {
    encoding = settings->genconfig.encoding_get();
  } else if (encodings.size() > 0) {
    encoding = encodings[0];
  }

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow1, TRUE, TRUE, 2);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  textview1 = gtk_text_view_new();
  gtk_widget_show(textview1);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), textview1);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textview1), FALSE);
  gtk_text_view_set_pixels_below_lines(GTK_TEXT_VIEW(textview1), 2);
  gtk_text_view_set_pixels_inside_wrap(GTK_TEXT_VIEW(textview1), 2);

  dialog_action_area1 = GTK_DIALOG(unicodedialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(unicodedialog, NULL, NULL);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(unicodedialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) button1, "clicked", G_CALLBACK(on_button1_clicked), gpointer(this));

  gtk_widget_grab_focus(button1);
  gtk_widget_grab_default(okbutton1);

  // Update GUI.
  set_gui();

}

UnicodeDialog::~UnicodeDialog()
{
  gtk_widget_destroy(unicodedialog);
}

int UnicodeDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(unicodedialog));
}

void UnicodeDialog::set_gui()
// If the user selects an encoding, this will show the example as well.
{
  gtk_button_set_label(GTK_BUTTON(button1), encoding.c_str());
  gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1)), "", -1);
  gchar *contents;
  g_file_get_contents(filename.c_str(), &contents, NULL, NULL);
  gchar *output;
  output = unicode_convert(contents, encoding);
  if (output) {
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1)), output, -1);
    g_free(output);
  } else {
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1)), "Conversion did not succeed with this encoding. Try another one.", -1);
  }
  g_free(contents);
}

void UnicodeDialog::on_button1_clicked(GtkButton * button, gpointer user_data)
{
  ((UnicodeDialog *) user_data)->on_button1();
}

void UnicodeDialog::on_button1()
{
  ListviewDialog dialog("Select an encoding", encodings, encoding, true, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    encoding = dialog.focus;
    extern Settings *settings;
    settings->genconfig.encoding_set(encoding);
    set_gui();
  }
}
