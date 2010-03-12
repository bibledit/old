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
#include "dialogreplacing.h"
#include "utilities.h"
#include "bible.h"
#include "usfm.h"
#include "usfmtools.h"
#include "projectutils.h"
#include "progresswindow.h"
#include "help.h"
#include "books.h"
#include "settings.h"
#include "tiny_utilities.h"

ReplacingDialog::ReplacingDialog(const vector < Reference > &references_in)
{
  extern Settings *settings;
  searchfor = settings->session.searchword;
  replacewith = settings->session.replaceword;
  mycasesensitive = settings->session.search_case_sensitive;
  myproject = settings->genconfig.project_get();
  ProjectConfiguration *projectconfig = settings->projectconfig(myproject);
  mylanguage = projectconfig->language_get();

  replacedialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(replacedialog), "Replace");
  gtk_window_set_destroy_with_parent(GTK_WINDOW(replacedialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(replacedialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  referencelabel = gtk_label_new("referencelabel");
  gtk_widget_show(referencelabel);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), referencelabel, FALSE, FALSE, 4);
  gtk_misc_set_alignment(GTK_MISC(referencelabel), 0, 0.5);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new("Do you wish to replace this text: ...");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 4);

  scrolledwindow4 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow4);
  gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow4, TRUE, TRUE, 0);

  textview1 = gtk_text_view_new();
  gtk_widget_show(textview1);
  gtk_container_add(GTK_CONTAINER(scrolledwindow4), textview1);
  gtk_widget_set_size_request(textview1, 500, 100);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textview1), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview1), GTK_WRAP_WORD);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textview1), FALSE);

  label2 = gtk_label_new("... with this text? It can be edited also.");
  gtk_widget_show(label2);
  gtk_box_pack_start(GTK_BOX(vbox1), label2, FALSE, FALSE, 4);

  scrolledwindow5 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow5);
  gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow5, TRUE, TRUE, 0);

  textview2 = gtk_text_view_new();
  gtk_widget_show(textview2);
  gtk_container_add(GTK_CONTAINER(scrolledwindow5), textview2);
  gtk_widget_set_size_request(textview2, 500, 100);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview2), GTK_WRAP_WORD);

  hbox3 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox3);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox3, FALSE, FALSE, 5);

  yesbutton = gtk_button_new_from_stock("gtk-yes");
  gtk_widget_show(yesbutton);
  gtk_box_pack_start(GTK_BOX(hbox3), yesbutton, FALSE, FALSE, 4);
  gtk_widget_set_size_request(yesbutton, 80, -1);
  GTK_WIDGET_SET_FLAGS(yesbutton, GTK_CAN_DEFAULT);

  nobutton = gtk_button_new_from_stock("gtk-no");
  gtk_widget_show(nobutton);
  gtk_box_pack_start(GTK_BOX(hbox3), nobutton, FALSE, FALSE, 4);
  gtk_widget_set_size_request(nobutton, 80, -1);

  allbutton = gtk_button_new();
  gtk_widget_show(allbutton);
  gtk_box_pack_start(GTK_BOX(hbox3), allbutton, FALSE, FALSE, 4);
  gtk_widget_set_size_request(allbutton, 80, -1);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(allbutton), alignment1);

  hbox4 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox4);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox4);

  image1 = gtk_image_new_from_stock("gtk-dialog-warning", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX(hbox4), image1, FALSE, FALSE, 0);

  label3 = gtk_label_new_with_mnemonic("_All");
  gtk_widget_show(label3);
  gtk_box_pack_start(GTK_BOX(hbox4), label3, FALSE, FALSE, 0);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_box_pack_start(GTK_BOX(hbox3), cancelbutton, FALSE, FALSE, 4);
  gtk_widget_set_size_request(cancelbutton, 80, -1);

  dialog_action_area1 = GTK_DIALOG(replacedialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(replacedialog, NULL, NULL, NULL);

  gtk_widget_grab_focus(yesbutton);
  gtk_widget_grab_default(yesbutton);

  // Get the buffers for later use.
  textbuffer1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
  textbuffer2 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview2));
  // Produce the tags for colouring the search/replace words.
  tag1 = gtk_text_buffer_create_tag(textbuffer1, "referencetag", "background", "khaki", NULL);
  tag2 = gtk_text_buffer_create_tag(textbuffer2, "referencetag", "background", "khaki", NULL);

  g_signal_connect((gpointer) yesbutton, "clicked", G_CALLBACK(replacedialog_on_yesbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) nobutton, "clicked", G_CALLBACK(replacedialog_on_nobutton_clicked), gpointer(this));
  g_signal_connect((gpointer) allbutton, "clicked", G_CALLBACK(replacedialog_on_allbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) cancelbutton, "clicked", G_CALLBACK(replacedialog_on_cancelbutton_clicked), gpointer(this));

  // Next one added to Glade's code.
  gtk_dialog_set_default_response(GTK_DIALOG(replacedialog), GTK_RESPONSE_OK);

  // Store results.
  for (unsigned int i = 0; i < references_in.size(); i++)
    references.push_back(references_in[i]);
  referencepointer = 0;
  set_gui();
}

ReplacingDialog::~ReplacingDialog()
{
  gtk_widget_destroy(replacedialog);
}

int ReplacingDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(replacedialog));
}

void ReplacingDialog::replacedialog_on_yesbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ReplacingDialog *) user_data)->on_yesbutton_clicked();
}

void ReplacingDialog::on_yesbutton_clicked()
{
  accept_change();
  referencepointer++;
  set_gui();
  if (referencepointer >= references.size())
    on_cancelbutton_clicked();
}

void ReplacingDialog::replacedialog_on_nobutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ReplacingDialog *) user_data)->on_nobutton_clicked();
}

void ReplacingDialog::on_nobutton_clicked()
{
  referencepointer++;
  set_gui();
  if (referencepointer >= references.size())
    on_cancelbutton_clicked();
}

void ReplacingDialog::replacedialog_on_allbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ReplacingDialog *) user_data)->on_allbutton_clicked();
}

void ReplacingDialog::on_allbutton_clicked()
{
  ProgressWindow progresswindow("Replacing", false);
  progresswindow.set_iterate(0, 1, references.size() - referencepointer);
  while (referencepointer < references.size()) {
    progresswindow.iterate();
    on_yesbutton_clicked();
  }
}

void ReplacingDialog::replacedialog_on_cancelbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ReplacingDialog *) user_data)->on_cancelbutton_clicked();
}

void ReplacingDialog::on_cancelbutton_clicked()
{
  gtk_dialog_response(GTK_DIALOG(replacedialog), GTK_RESPONSE_NONE);
}

void ReplacingDialog::set_gui()
// The GUI is filled with all data needed.
{
  if (referencepointer < references.size()) {
    // Show reference.
    gtk_label_set_text(GTK_LABEL(referencelabel), references[referencepointer].human_readable(mylanguage).c_str());
    // Get the verse.
    ustring line = project_retrieve_verse(myproject, references[referencepointer].book, references[referencepointer].chapter, references[referencepointer].verse);
    // Verse exists?
    if (!line.empty()) {
      // Parse into lines.
      ParseLine parseline(line);
      // Clear the two textviews.
      gtk_text_buffer_set_text(textbuffer1, "", -1);
      gtk_text_buffer_set_text(textbuffer2, "", -1);
      ustring searchword;
      if (mycasesensitive)
        searchword = searchfor;
      else
        searchword = searchfor.casefold();
      // Remove any previous highlights in both buffers.
      GtkTextIter startiter;
      GtkTextIter enditer;
      gtk_text_buffer_get_start_iter(textbuffer1, &startiter);
      gtk_text_buffer_get_end_iter(textbuffer1, &enditer);
      gtk_text_buffer_remove_tag(textbuffer1, tag1, &startiter, &enditer);
      gtk_text_buffer_get_start_iter(textbuffer2, &startiter);
      gtk_text_buffer_get_end_iter(textbuffer2, &enditer);
      gtk_text_buffer_remove_tag(textbuffer2, tag2, &startiter, &enditer);
      // As a verse may consist of more lines, deal with each.
      for (unsigned int i = 0; i < parseline.lines.size(); i++) {
        // Fill original text.
        ustring verse(parseline.lines[i]);
        gtk_text_buffer_insert_at_cursor(textbuffer1, verse.c_str(), -1);
        gtk_text_buffer_insert_at_cursor(textbuffer1, "\n", -1);
        // Storage for later colouring of text.
        vector < int >colourlines;
        vector < size_t > colourpositions;
        // Start producing modified text.
        ustring verse2;
        if (mycasesensitive)
          verse2 = verse;
        else
          verse2 = verse.casefold();
        // Colour words in the original window.
        size_t offposition = verse2.find(searchword);
        while (offposition != string::npos) {
          // Handle differences in length between normal text and casefold.
          if (!mycasesensitive) {
            ustring normal_text = verse.substr(0, offposition);
            size_t normal_length = normal_text.length();
            size_t casefold_length = normal_text.casefold().length();
            int difference = normal_length - casefold_length;
            offposition += difference;
          }
          GtkTextIter begin;
          GtkTextIter end;
          gtk_text_buffer_get_iter_at_line_offset(textbuffer1, &begin, i, offposition);
          gtk_text_buffer_get_iter_at_line_offset(textbuffer1, &end, i, offposition + searchword.length());
          gtk_text_buffer_apply_tag(textbuffer1, tag1, &begin, &end);
          offposition = verse2.find(searchword, offposition + searchword.length());
        }
        // Continue producing modified text.
        ustring sfm(parseline.lines[i]);
        // Replace the text, except in the \id line.
        if (!usfm_is_id(usfm_extract_marker(sfm))) {
          size_t offposition = verse2.find(searchword);
          while (offposition != string::npos) {
            if (!mycasesensitive) {
              ustring normal_text = verse.substr(0, offposition);
              size_t normal_length = normal_text.length();
              size_t casefold_length = normal_text.casefold().length();
              int difference = normal_length - casefold_length;
              offposition += difference;
            }
            verse.replace(offposition, searchword.length(), replacewith);
            colourlines.push_back(i);
            colourpositions.push_back(offposition);
            if (mycasesensitive)
              verse2 = verse;
            else
              verse2 = verse.casefold();
            offposition = verse2.find(searchword, offposition + replacewith.length());
          }
        }
        // Insert modified text into the second buffer.
        gtk_text_buffer_insert_at_cursor(textbuffer2, verse.c_str(), -1);
        gtk_text_buffer_insert_at_cursor(textbuffer2, "\n", -1);
        // Colour all replacements.
        for (unsigned int i = 0; i < colourlines.size(); i++) {
          GtkTextIter begin;
          GtkTextIter end;
          gtk_text_buffer_get_iter_at_line_offset(textbuffer2, &begin, colourlines[i], colourpositions[i]);
          gtk_text_buffer_get_iter_at_line_offset(textbuffer2, &end, colourlines[i], colourpositions[i] + replacewith.length());
          gtk_text_buffer_apply_tag(textbuffer2, tag2, &begin, &end);
        }
      }
    }
  }
}

void ReplacingDialog::accept_change()
/*
 * This function accepts the change from the second textview
 * and saves it into the book.
 */
{
  // Get textbuffer.
  GtkTextBuffer *buffer;
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview2));
  // Put the modified text back into the book.
  vector < ustring > verses;
  int linecount = gtk_text_buffer_get_line_count(GTK_TEXT_BUFFER(buffer));
  for (int i = 0; i < linecount; i++) {
    GtkTextIter begin;
    GtkTextIter end;
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &begin, i);
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &end, i + 1);
    gtk_text_iter_backward_char(&end);
    ustring s;
    s = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &begin, &end, false);
    s = trim(s);
    if (s.length())
      verses.push_back(s);
  }
  // Save the modified text.
  ustring data;
  for (unsigned int i = 0; i < verses.size(); i++) {
    if (!data.empty())
      data.append("\n");
    data.append(verses[i]);
  }
  if (verses.size() > 0) {
    project_store_verse(myproject, references[referencepointer].book, references[referencepointer].chapter, references[referencepointer].verse, data);
  }
}
