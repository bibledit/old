/*
** Copyright (©) 2003-2012 Teus Benschop.
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
#include "dialogmerge.h"
#include "help.h"
#include "settings.h"
#include "gwrappers.h"
#include "screen.h"
#include "utilities.h"


MergeDialog::MergeDialog(const ustring & text)
{
  event_textbuffer = 0;

  extern Settings *settings;

  mergedialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(mergedialog), "Merge");
  gtk_window_set_position(GTK_WINDOW(mergedialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(mergedialog), TRUE);
  gtk_window_set_default_size(GTK_WINDOW(mergedialog), (int)(settings->genconfig.screen_width_get() * 0.8), (int)(settings->genconfig.screen_height_get() * 0.9));

  dialog_vbox1 = GTK_DIALOG(mergedialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  label = gtk_label_new("The merge operation needs manual intervention. Each place that needs intervention shows two buttons with the two alternatives. Please press the buttons with the correct text.");
  gtk_label_set_line_wrap(GTK_LABEL(label), true);
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_SHADOW_IN);

  textview = gtk_text_view_new();
  gtk_widget_show(textview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), textview);
  gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(textview), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD);

  textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));

  dialog_action_area1 = GTK_DIALOG(mergedialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(mergedialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(mergedialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(mergedialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  gtk_widget_set_sensitive(okbutton, false);

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));
  g_signal_connect_after((gpointer) textbuffer, "changed", G_CALLBACK(on_textbuffer_changed), gpointer(this));

  gtk_widget_grab_focus(textview);
  gtk_widget_grab_default(okbutton);

  load_text(text);
}


MergeDialog::~MergeDialog()
{
  gtk_widget_destroy(mergedialog);
}


int MergeDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(mergedialog));
}


void MergeDialog::load_text(ustring text)
{
  // Variables for loading text in the textview.
  size_t pos;
  GtkTextIter iter;

  // Preprocess empty replacements.
  preprocess_empty_replacements(text);

  // Goo through the text looking for markers and processing them.
  pos = text.find(merge_conflict_markup(1));
  while (pos != string::npos) {

    // Insert the bit of text before the first conflict marker.
    gtk_text_buffer_get_end_iter(textbuffer, &iter);
    gtk_text_buffer_insert(textbuffer, &iter, text.substr(0, pos).c_str(), -1);
    text.erase(0, pos + merge_conflict_markup(1).length());

    // Retrieve the first alternative.
    ustring alternative1;
    pos = text.find(merge_conflict_markup(2));
    if (pos != string::npos) {
      alternative1 = text.substr(1, pos - 2);
      text.erase(0, pos + merge_conflict_markup(2).length());
      if (alternative1.empty())
        alternative1 = empty_text();
    }
    // Insert a button with the first alternative as a label.
    gtk_text_buffer_get_end_iter(textbuffer, &iter);
    GtkTextChildAnchor *childanchor1 = gtk_text_buffer_create_child_anchor(textbuffer, &iter);
    GtkWidget *button1 = gtk_button_new_with_label(alternative1.c_str());
    gtk_text_view_add_child_at_anchor(GTK_TEXT_VIEW(textview), button1, childanchor1);
    gtk_widget_show_all(button1);
    g_signal_connect((gpointer) button1, "clicked", G_CALLBACK(on_mergebutton_clicked), gpointer(this));

    // Store data about first alternative.
    MergeButton mergebutton1;
    mergebutton1.childanchor = childanchor1;
    mergebutton1.button = button1;
    mergebutton1.text = alternative1;

    // Retrieve the second alternative.
    ustring alternative2;
    pos = text.find(merge_conflict_markup(3));
    if (pos != string::npos) {
      alternative2 = text.substr(1, pos - 2);
      text.erase(0, pos + merge_conflict_markup(3).length());
      if (alternative2.empty())
        alternative2 = empty_text();
    }
    // Insert a button with the second alternative as a label.
    gtk_text_buffer_get_end_iter(textbuffer, &iter);
    GtkTextChildAnchor *childanchor2 = gtk_text_buffer_create_child_anchor(textbuffer, &iter);
    GtkWidget *button2 = gtk_button_new_with_label(alternative2.c_str());
    gtk_text_view_add_child_at_anchor(GTK_TEXT_VIEW(textview), button2, childanchor2);
    gtk_widget_show_all(button2);
    g_signal_connect((gpointer) button2, "clicked", G_CALLBACK(on_mergebutton_clicked), gpointer(this));

    // Store data about second alternative.
    MergeButton mergebutton2;
    mergebutton2.childanchor = childanchor2;
    mergebutton2.button = button2;
    mergebutton2.text = alternative2;

    // Store the button pair.
    MergeButtonPair mergebuttonpair;
    mergebuttonpair.button1 = mergebutton1;
    mergebuttonpair.button2 = mergebutton2;
    buttonpairs.push_back(mergebuttonpair);

    // Next iteration.
    pos = text.find(merge_conflict_markup(1));
  }

  // Load remaining text in textview.
  gtk_text_buffer_get_end_iter(textbuffer, &iter);
  gtk_text_buffer_insert(textbuffer, &iter, text.substr(0, pos).c_str(), -1);

  // Scroll to beginning of buffer.
  gtk_text_buffer_get_start_iter(textbuffer, &iter);
  gtk_text_buffer_place_cursor(textbuffer, &iter);
  screen_scroll_to_iterator(GTK_TEXT_VIEW(textview), &iter);
}


void MergeDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((MergeDialog *) user_data)->on_okbutton();
}


void MergeDialog::on_okbutton()
{
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_start_iter(textbuffer, &startiter);
  gtk_text_buffer_get_end_iter(textbuffer, &enditer);
  reconciled_text = gtk_text_buffer_get_text(textbuffer, &startiter, &enditer, false);
  replace_text(reconciled_text, "  ", " ");
  replace_text(reconciled_text, "  ", " ");
}


void MergeDialog::on_textbuffer_changed(GtkTextBuffer * textbuffer, gpointer user_data)
{
  ((MergeDialog *) user_data)->on_textbuffer();
}


void MergeDialog::on_textbuffer()
{
  gw_destroy_source(event_textbuffer);
  event_textbuffer = g_timeout_add_full(G_PRIORITY_DEFAULT, 200, GSourceFunc(on_textbuffer_delayed), gpointer(this), NULL);
}


bool MergeDialog::on_textbuffer_delayed(gpointer user_data)
{
  ((MergeDialog *) user_data)->on_text_buffer_execute();
  return false;
}


void MergeDialog::on_text_buffer_execute()
{
  // Set the OK button sensitive if there are no conflicts anymore.
  bool conflicts_present = false;
  GtkTextIter iter;
  gtk_text_buffer_get_start_iter(textbuffer, &iter);
  do {
    if (gtk_text_iter_get_child_anchor(&iter)) {
      conflicts_present = true;
    }
  } while (gtk_text_iter_forward_char(&iter));
  gtk_widget_set_sensitive(okbutton, !conflicts_present);
}


void MergeDialog::on_mergebutton_clicked(GtkButton * button, gpointer user_data)
{
  ((MergeDialog *) user_data)->on_mergebutton(button);
}


void MergeDialog::on_mergebutton(GtkButton * button)
// If a merge button is clicked, it inserts the right text, and deletes
// the pair of buttons.
{
  GtkWidget *mybutton = GTK_WIDGET(button);
  for (unsigned int i = 0; i < buttonpairs.size(); i++) {
    bool match = false;
    if (mybutton == buttonpairs[i].button1.button)
      match = true;
    if (mybutton == buttonpairs[i].button2.button)
      match = true;
    if (match) {
      ustring text;
      if (mybutton == buttonpairs[i].button1.button) {
        text = buttonpairs[i].button1.text;
      }
      if (mybutton == buttonpairs[i].button2.button) {
        text = buttonpairs[i].button2.text;
      }
      if (text == empty_text())
        text.clear();
      GtkTextIter iter, iter2;
      gtk_text_buffer_get_iter_at_child_anchor(textbuffer, &iter, buttonpairs[i].button1.childanchor);
      gtk_text_buffer_place_cursor(textbuffer, &iter);
      iter2 = iter;
      gtk_text_iter_forward_char(&iter2);
      gtk_text_buffer_delete(textbuffer, &iter, &iter2);
      gtk_text_buffer_get_iter_at_child_anchor(textbuffer, &iter, buttonpairs[i].button2.childanchor);
      iter2 = iter;
      gtk_text_iter_forward_char(&iter2);
      gtk_text_buffer_delete(textbuffer, &iter, &iter2);
      if (!text.empty())
        gtk_text_buffer_insert_at_cursor(textbuffer, text.c_str(), -1);
    }
  }
}


ustring MergeDialog::empty_text()
// The empty text is for cases that there was no text.
{
  return "<empty>";
}


void MergeDialog::preprocess_empty_replacements(ustring & text)
/*
There are cases that one text is completely removed in a conflict situation.
The replacement for this text is empty. 
  
In the merge routine this shows as:

  <<<<<<< /home/joe/.bibledit_temp/merge/file1
  servant
  of
  Jesus
  Christ
  and
  =======
  >>>>>>> /home/joe/.bibledit_temp/merge/file3

And in the internal texts this shows as:

__conflict__marker__before__ servant of Jesus Christ and __conflict__marker__middle__ __conflict__marker__after__ 

This function handles this case so that it does not lead to confusion.
*/
{
  replace_text(text, merge_conflict_markup(1) + " " + merge_conflict_markup(2), merge_conflict_markup(1) + "  " + merge_conflict_markup(2));
  replace_text(text, merge_conflict_markup(2) + " " + merge_conflict_markup(3), merge_conflict_markup(2) + "  " + merge_conflict_markup(3));
}


