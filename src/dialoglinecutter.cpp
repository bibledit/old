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

#include "dialoglinecutter.h"
#include "settings.h"
#include "projectutils.h"
#include "usfmtools.h"
#include "utilities.h"
#include "progresswindow.h"
#include "gtkwrappers.h"
#include "help.h"
#include "books.h"

LineCutterDialog::LineCutterDialog(int dummy)
{
  linecutterdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(linecutterdialog), "Line cutter for Hebrew text");
  gtk_window_set_position(GTK_WINDOW(linecutterdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint(GTK_WINDOW(linecutterdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(linecutterdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 4);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new("This tool assumes that the project now opened is Hebrew text, or any other right-to-left text, with visual ordering. It will cut the line in pieces no longer than a certain amount of characters.");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 0);
  gtk_label_set_line_wrap(GTK_LABEL(label1), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

  hbox1 = gtk_hbox_new(FALSE, 3);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, FALSE, 0);

  label2 = gtk_label_new("One line will have no more than");
  gtk_widget_show(label2);
  gtk_box_pack_start(GTK_BOX(hbox1), label2, FALSE, FALSE, 0);

  extern Settings *settings;

  spinbutton1_adj = gtk_adjustment_new(settings->session.line_cutter_for_hebrew_text_characters, 10, 500, 1, 10, 0);
  spinbutton1 = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton1_adj), 1, 0);
  gtk_widget_show(spinbutton1);
  gtk_box_pack_start(GTK_BOX(hbox1), spinbutton1, TRUE, TRUE, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton1), TRUE);

  label3 = gtk_label_new("characters");
  gtk_widget_show(label3);
  gtk_box_pack_start(GTK_BOX(hbox1), label3, FALSE, FALSE, 0);

  label4 = gtk_label_new("Choose OK to cut the lines.");
  gtk_widget_show(label4);
  gtk_box_pack_start(GTK_BOX(vbox1), label4, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label4), 0, 0.5);

  dialog_action_area1 = GTK_DIALOG(linecutterdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(linecutterdialog, NULL, NULL, NULL);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(linecutterdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(linecutterdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton1);
}

LineCutterDialog::~LineCutterDialog()
{
  gtk_widget_destroy(linecutterdialog);
}

int LineCutterDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(linecutterdialog));
}

void LineCutterDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((LineCutterDialog *) user_data)->on_ok();
}

void LineCutterDialog::on_ok()
{
  // Get and save the length to cut at.
  unsigned int cut_at = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton1));
  extern Settings *settings;
  settings->session.line_cutter_for_hebrew_text_characters = cut_at;

  // If the project is empty, bail out.  
  ustring project = settings->genconfig.project_get();
  if (project.empty())
    return;

  // Give a double warning.  
  if (gtkw_dialog_question(linecutterdialog, "Are you sure you wish to cut the lines?") == GTK_RESPONSE_NO)
    return;
  if (gtkw_dialog_question(linecutterdialog, "This will permanently modify your project.\nThe changes cannot be undone.\nAre you sure?") == GTK_RESPONSE_NO)
    return;

  // Get and go through all books in this project.
  vector < unsigned int >books = project_get_books(project);
  ProgressWindow progresswindow("Cutting lines", false);
  progresswindow.set_iterate(0, 1, books.size());
  for (unsigned bk = 0; bk < books.size(); bk++) {
    progresswindow.iterate();

    // Store the cut lines.
    vector < ustring > cutlines;

    // Get and go through all lines in this book.
    vector < ustring > lines = project_retrieve_book(project, books[bk]);
    for (unsigned int ln = 0; ln < lines.size(); ln++) {

      // Only cut a line if it is too long.
      if (lines[ln].length() > cut_at) {

        // Get the line and extract the marker, including the verse number if 
        // relevant.
        ustring line = lines[ln];
        ustring marker = usfm_extract_marker(line);
        if (marker == "v") {
          ustring verse = number_in_string(line);
          marker.append(" " + verse);
          line.erase(0, verse.length() + 1);
        }
        // Reverse the order of the characters for easier processing.
        line = string_reverse(line);

        // Parse the text into its separate words.
        Parse parse(line, false);

        // Build new lines, the length of each not exceeding the maximum 
        // character count.
        unsigned int newline_count = 0;
        ustring newline;
        while (!parse.words.empty()) {
          // If the line gets too long, store it.
          if (newline.length() + parse.words[0].length() > cut_at) {
            assemble(newline_count, marker, newline, cutlines);
            // If one single words is too long, still store it - nothing can be done.
          } else if (parse.words[0].length() > cut_at) {
            newline = parse.words[0];
            parse.words.erase(parse.words.begin());
            assemble(newline_count, marker, newline, cutlines);
            // In all other cases, add the word to the line.
          } else {
            if (!newline.empty())
              newline.append(" ");
            newline.append(parse.words[0]);
            parse.words.erase(parse.words.begin());
          }
        }
        if (!newline.empty())
          assemble(newline_count, marker, newline, cutlines);

        // If the line is not too long, just store it.  
      } else {
        cutlines.push_back(lines[ln]);
      }

    }

    // Store the processed lines into the book again.
    CategorizeChapterVerse ccv(cutlines);
    project_store_book(project, books[bk], ccv);

  }
}

void LineCutterDialog::assemble(unsigned int &newlinecount, ustring & marker, ustring & newline, vector < ustring > &cutlines)
{
  if (newlinecount > 0)
    marker = "nb";
  newlinecount++;
  newline = string_reverse(newline);
  newline.insert(0, "\\" + marker + " ");
  cutlines.push_back(newline);
  newline.clear();
}
