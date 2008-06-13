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

#include "dialogimportrawtext.h"
#include "help.h"
#include "gtkwrappers.h"
#include "shortcuts.h"
#include "settings.h"
#include "books.h"
#include "combobox.h"
#include "utilities.h"
#include "versification.h"
#include "tiny_utilities.h"
#include "gui.h"
#include "gwrappers.h"
#include "categorize.h"
#include "projectutils.h"
#include "scripts.h"

ImportRawTextDialog::ImportRawTextDialog(int dummy) {
  Shortcuts shortcuts(0);

  importrawtextdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW (importrawtextdialog), "Import Raw Text");
  gtk_window_set_position(GTK_WINDOW (importrawtextdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_default_size(GTK_WINDOW (importrawtextdialog), 800, 600);
  gtk_window_set_type_hint(GTK_WINDOW (importrawtextdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (importrawtextdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  hbox1 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX (dialog_vbox1), hbox1, TRUE, TRUE, 0);

  vbox_controls = gtk_vbox_new(FALSE, 4);
  gtk_widget_show(vbox_controls);
  gtk_box_pack_start(GTK_BOX (hbox1), vbox_controls, FALSE, TRUE, 0);

  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox2);
  gtk_box_pack_start(GTK_BOX (vbox_controls), vbox2, FALSE, TRUE, 0);

  label_book = gtk_label_new("Book");
  gtk_widget_show(label_book);
  gtk_box_pack_start(GTK_BOX (vbox2), label_book, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_book), 0, 0.5);

  shortcuts.label(label_book);

  combobox_book = gtk_combo_box_new_text();
  gtk_widget_show(combobox_book);
  gtk_box_pack_start(GTK_BOX (vbox2), combobox_book, FALSE, TRUE, 0);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX (vbox_controls), hseparator1, FALSE, TRUE, 0);

  vbox3 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox3);
  gtk_box_pack_start(GTK_BOX (vbox_controls), vbox3, FALSE, FALSE, 0);

  label_chapter = gtk_label_new("Chapter");
  gtk_widget_show(label_chapter);
  gtk_box_pack_start(GTK_BOX (vbox3), label_chapter, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_chapter), 0, 0.5);

  shortcuts.label(label_chapter);

  combobox_chapter = gtk_combo_box_new_text();
  gtk_widget_show(combobox_chapter);
  gtk_box_pack_start(GTK_BOX (vbox3), combobox_chapter, FALSE, TRUE, 0);

  hseparator2 = gtk_hseparator_new();
  gtk_widget_show(hseparator2);
  gtk_box_pack_start(GTK_BOX (vbox_controls), hseparator2, FALSE, TRUE, 0);

  button_discover_markup = gtk_button_new();
  gtk_widget_show(button_discover_markup);
  gtk_box_pack_start(GTK_BOX (vbox_controls), button_discover_markup, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER (button_discover_markup), alignment1);

  hbox2 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox2);
  gtk_container_add(GTK_CONTAINER (alignment1), hbox2);

  image1 = gtk_image_new_from_stock("gtk-zoom-fit", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_box_pack_start(GTK_BOX (hbox2), image1, FALSE, FALSE, 0);

  label4 = gtk_label_new_with_mnemonic("Discover markup");
  gtk_widget_show(label4);
  gtk_box_pack_start(GTK_BOX (hbox2), label4, FALSE, FALSE, 0);

  shortcuts.label(label4);

  hseparator3 = gtk_hseparator_new();
  gtk_widget_show(hseparator3);
  gtk_box_pack_start(GTK_BOX (vbox_controls), hseparator3, FALSE, TRUE, 0);

  vbox_filter = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox_filter);
  gtk_box_pack_start(GTK_BOX (vbox_controls), vbox_filter, FALSE, FALSE, 0);

  label_filter = gtk_label_new("Filter");
  gtk_widget_show(label_filter);
  gtk_box_pack_start(GTK_BOX (vbox_filter), label_filter, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_filter), 0, 0.5);

  shortcuts.label(label_filter);

  combobox_filter = gtk_combo_box_new_text();
  gtk_widget_show(combobox_filter);
  gtk_box_pack_start(GTK_BOX (vbox_filter), combobox_filter, FALSE, FALSE, 0);

  button_filter = gtk_button_new();
  gtk_widget_show(button_filter);
  gtk_box_pack_start(GTK_BOX (vbox_filter), button_filter, FALSE, FALSE, 0);

  alignment2 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment2);
  gtk_container_add(GTK_CONTAINER (button_filter), alignment2);

  hbox4 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox4);
  gtk_container_add(GTK_CONTAINER (alignment2), hbox4);

  image2 = gtk_image_new_from_stock("gtk-clear", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image2);
  gtk_box_pack_start(GTK_BOX (hbox4), image2, FALSE, FALSE, 0);

  label7 = gtk_label_new_with_mnemonic("Filter text");
  gtk_widget_show(label7);
  gtk_box_pack_start(GTK_BOX (hbox4), label7, FALSE, FALSE, 0);

  shortcuts.label(label7);

  hseparator4 = gtk_hseparator_new();
  gtk_widget_show(hseparator4);
  gtk_box_pack_start(GTK_BOX (vbox_controls), hseparator4, FALSE, FALSE, 0);

  checkbutton_okay = gtk_check_button_new_with_mnemonic("Text is okay");
  gtk_widget_show(checkbutton_okay);
  gtk_box_pack_start(GTK_BOX (vbox_controls), checkbutton_okay, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_okay);

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_box_pack_start(GTK_BOX (hbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_SHADOW_IN);

  textview1 = gtk_text_view_new();
  gtk_widget_show(textview1);
  gtk_container_add(GTK_CONTAINER (scrolledwindow1), textview1);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW (textview1), GTK_WRAP_WORD);

  hbox3 = gtk_hbox_new(FALSE, 5);
  gtk_widget_show(hbox3);
  gtk_box_pack_start(GTK_BOX (dialog_vbox1), hbox3, FALSE, FALSE, 0);

  image_ok = gtk_image_new_from_stock("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image_ok);
  gtk_box_pack_start(GTK_BOX (hbox3), image_ok, FALSE, TRUE, 0);

  label_ok = gtk_label_new("");
  gtk_widget_show(label_ok);
  gtk_box_pack_start(GTK_BOX (hbox3), label_ok, FALSE, FALSE, 0);

  label_info = gtk_label_new("");
  gtk_widget_show(label_info);
  gtk_box_pack_start(GTK_BOX (hbox3), label_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_info), 0, 0.5);

  dialog_action_area1 = GTK_DIALOG (importrawtextdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (importrawtextdialog, &shortcuts, "import_raw_text");

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG (importrawtextdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG (importrawtextdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton1);
  shortcuts.stockbutton(okbutton1);
  shortcuts.process();

  gtk_label_set_mnemonic_widget(GTK_LABEL (label_book), combobox_book);
  gtk_label_set_mnemonic_widget(GTK_LABEL (label_chapter), combobox_chapter);
  gtk_label_set_mnemonic_widget(GTK_LABEL (label_filter), combobox_filter);

  textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (textview1));

  g_signal_connect ((gpointer) textbuffer, "changed",
      G_CALLBACK (on_textbuffer_changed),
      gpointer (this));
  g_signal_connect ((gpointer) combobox_book, "changed",
      G_CALLBACK (on_combobox_book_changed),
      gpointer (this));
  g_signal_connect ((gpointer) combobox_chapter, "changed",
      G_CALLBACK (on_combobox_chapter_changed),
      gpointer (this));
  g_signal_connect ((gpointer) button_discover_markup, "clicked",
      G_CALLBACK (on_button_discover_markup_clicked),
      gpointer (this));
  g_signal_connect ((gpointer) button_filter, "clicked",
      G_CALLBACK (on_button_filter_clicked),
      gpointer (this));
  g_signal_connect ((gpointer) checkbutton_okay, "toggled",
      G_CALLBACK (on_checkbutton_okay_toggled),
      gpointer (this));
  g_signal_connect ((gpointer) okbutton1, "clicked",
      G_CALLBACK (on_okbutton1_clicked),
      gpointer (this));

  gtk_widget_grab_focus(textview1);
  gtk_widget_grab_default(okbutton1);

  extern Settings * settings;
  project = settings->genconfig.project_get();
  versification = settings->projectconfig (project)->versification_get();
  vector <unsigned int> ids = books_type_to_ids(btUnknown);
  vector <ustring> books;
  for (unsigned int i = 0; i < ids.size(); i++) {
    books.push_back(books_id_to_english(ids[i]));
  }
  combobox_set_strings(combobox_book, books);
  on_combobox_book();
  gui_event_id = 0;
  text_was_discovered = false;
  gui();
  programmatically_setting_text = false;

  combobox_set_index(combobox_book, settings->session.import_raw_text_book);
  combobox_set_index(combobox_chapter, settings->session.import_raw_text_chapter);

  vector <ustring> filters = scripts_get_all();
  combobox_set_strings(combobox_filter, filters);
  combobox_set_index(combobox_filter, 0);
}

ImportRawTextDialog::~ImportRawTextDialog() {
  gtk_widget_destroy(importrawtextdialog);
}

int ImportRawTextDialog::run() {
  return gtk_dialog_run(GTK_DIALOG (importrawtextdialog));
}

void ImportRawTextDialog::on_textbuffer_changed(GtkEditable * editable, gpointer user_data) {
  ((ImportRawTextDialog *) user_data)->on_textbuffer();
}

void ImportRawTextDialog::on_textbuffer() {
  if (programmatically_setting_text)
    return;
  gui();
}

void ImportRawTextDialog::on_combobox_book_changed(GtkComboBox *combobox, gpointer user_data) {
  ((ImportRawTextDialog *) user_data)->on_combobox_book();
}

void ImportRawTextDialog::on_combobox_book() {
  book = books_english_to_id(combobox_get_active_string(combobox_book));
  if (book) {
    vector <unsigned int> chapters = versification_get_chapters(versification, book);
    combobox_set_strings(combobox_chapter, chapters);
    on_combobox_chapter();
  }
  gui();
}

void ImportRawTextDialog::on_combobox_chapter_changed(GtkComboBox *combobox, gpointer user_data) {
  ((ImportRawTextDialog *) user_data)->on_combobox_chapter();
}

void ImportRawTextDialog::on_combobox_chapter() {
  chapter = -1;
  ustring ch = combobox_get_active_string(combobox_chapter);
  if (!ch.empty())
    chapter = convert_to_int(ch);
  gui();
}

void ImportRawTextDialog::on_button_discover_markup_clicked(GtkButton *button, gpointer user_data) {
  ((ImportRawTextDialog *) user_data)->on_button_discover_markup();
}

void ImportRawTextDialog::on_button_discover_markup()
// Discovers the USFM markup of the text.
{
  // Save cursor location.
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, gtk_text_buffer_get_insert(textbuffer));
  gint cursoroffset = gtk_text_iter_get_offset(&iter);

  // Consecutive discoveries.
  bool discoveries_passed = true;

  // Get the text from the buffer.  
  vector <ustring> lines;
  textbuffer_get_lines(textbuffer, lines, false);

  // Go through the lines.
  for (unsigned int i = 0; i < lines.size(); i++) {

    // Skip empty line.
    if (lines[i].empty())
      continue;

    // Remove chapter markup.
    if (lines[i].find("\\c") != string::npos) {
      lines[i].clear();
      continue;
    }

    // Skip line starting with a backslash. The rationale is that this line already has some markup.
    if (lines[i].substr(0, 1) == "\\")
      continue;

    // If the line is a number on its own, and the number agrees with the chapter number
    // that was set, it silently removes this line. But if it differs, an error comes up.
    if (discoveries_passed) {
      if (number_in_string(lines[i]) == lines[i]) {
        int number = convert_to_int(number_in_string(lines[i]));
        if (number == chapter) {
          lines[i].clear();
          continue;
        }
        ustring msg = "The line that contains " + lines[i] + " looks like a chapter number, but the number differs from the chapter that was set";
        gtk_label_set_text(GTK_LABEL (label_info), msg.c_str());
        discoveries_passed = false;
      }
    }

    // If the line has no number in it at all, then it is considered a section heading.
    if (discoveries_passed) {
      if (number_in_string (lines[i]).empty()) {
        lines[i].insert(0, "\\s ");
        continue;
      }
    }

    // If a number is found in the line, then this is considered a verse number.
    // The first time a number is found, a \p is prefixed.
    bool paragraph_open = false;
    if (discoveries_passed) {
      ustring output;
      ustring number = number_in_string(lines[i]);
      while (!number.empty()) {
        if (!paragraph_open) {
          output.append("\\p");
          paragraph_open = true;
        }
        size_t pos = lines[i].find(number);
        if (pos > 0) {
          output.append(" " + lines[i].substr(0, pos));
          lines[i].erase(0, pos);
        }
        output.append("\n\\v ");
        output.append(number);
        output.append(" ");
        lines[i].erase(0, number.length());
        lines[i] = trim(lines[i]);
        number = number_in_string(lines[i]);
      }
      output.append(lines[i]);
      lines[i] = output;
    }

  }

  // Make one block of text.
  ustring newtext;
  for (unsigned int i = 0; i < lines.size(); i++) {
    if (lines[i].empty())
      continue;
    replace_text(lines[i], "  ", " ");
    replace_text(lines[i], " \n", "\n");
    newtext.append(lines[i]);
    newtext.append("\n");
  }

  // If no chapter marker is found, insert it at the top.
  if (newtext.find("\\c") == string::npos) {
    newtext.insert(0, "\\c " + convert_to_string(chapter) + "\n");
  }

  // Put the optionally modified text back in the buffer.
  programmatically_setting_text = true;
  gtk_text_buffer_set_text(textbuffer, newtext.c_str(), -1);
  programmatically_setting_text = false;

  // Restore cursor position, focus view so user can edit it.
  gtk_text_buffer_get_iter_at_offset(textbuffer, &iter, cursoroffset);
  gtk_text_buffer_place_cursor(textbuffer, &iter);
  gtk_widget_grab_focus(textview1);

  // Update flag.
  text_was_discovered = true;
  
  // Update gui if all discoveries passed.
  if (discoveries_passed)
    gui();
}

void ImportRawTextDialog::on_button_filter_clicked(GtkButton *button, gpointer user_data)
// Called by button filter.
{
  ((ImportRawTextDialog *) user_data)->on_button_filter();
}

void ImportRawTextDialog::on_button_filter()
// Filter the text.
{
  // Save the text from the buffer to disk.
  vector <ustring> lines;
  textbuffer_get_lines(textbuffer, lines, false);
  write_lines(script_temporal_input_file(), lines);

  // The filter to apply.
  ustring scriptname = combobox_get_active_string(combobox_filter);
  bool straightthrough = scriptname == scripts_straight_through();

  // Run filter.
  ustring error = script_filter(scriptname, straightthrough, script_temporal_input_file(), script_temporal_output_file());
  if (!error.empty())
    gw_message(error);

  // Show output in textview.  
  gchar * outputtext;
  g_file_get_contents(script_temporal_output_file().c_str(), &outputtext, NULL, NULL);
  if (outputtext) {
    gtk_text_buffer_set_text(textbuffer, outputtext, -1);
    g_free(outputtext);
  } else {
    gtk_text_buffer_set_text(textbuffer, "", -1);
  }
}

void ImportRawTextDialog::on_checkbutton_okay_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
  ((ImportRawTextDialog *) user_data)->gui();
}

void ImportRawTextDialog::on_okbutton1_clicked(GtkButton *button, gpointer user_data) {
  ((ImportRawTextDialog *) user_data)->on_okbutton();
}

void ImportRawTextDialog::on_okbutton() {
  // Get / save text.
  vector <ustring> lines;
  textbuffer_get_lines(textbuffer, lines, false);
  CategorizeChapterVerse ccv(lines);
  project_store_chapter(project, book, ccv);

  // Save book and increased chapter index.
  extern Settings * settings;
  settings->session.import_raw_text_book = combobox_get_active_index(combobox_book);
  int ch = combobox_get_active_index(combobox_chapter);
  ch++;
  settings->session.import_raw_text_chapter = ch;
}

void ImportRawTextDialog::gui()
// General GUI controller.
{
  // Optionally destroy previous timeout, create new one.
  gw_destroy_source(gui_event_id);
  gui_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 10, GSourceFunc (gui_timeout), gpointer(this), NULL);
}

bool ImportRawTextDialog::gui_timeout(gpointer user_data) {
  ((ImportRawTextDialog *) user_data)->gui_execute();
  return false;
}

void ImportRawTextDialog::gui_execute()
// General GUI controller.
{
  // Clear event id.
  gui_event_id = 0;

  // Whether text can be okayed.
  bool okayable = false;
  
  // Consecutive checks.
  bool checks_passed = true;

  // See whether text is available.
  vector <ustring> lines;
  textbuffer_get_lines(textbuffer, lines, false);
  if (checks_passed) {
    if (lines.size() < 2) {
      gtk_label_set_text(GTK_LABEL (label_info), "There is no text. Paste text into the editor");
      checks_passed = false;
    }
  }
  if (checks_passed) {
    if (lines.size() < 5) {
      gtk_label_set_text(GTK_LABEL (label_info), "There isn't enough text. Paste more text into the editor");
      checks_passed = false;
    }
  }

  // Check whether the book has been set.
  if (checks_passed) {
    if (book == 0) {
      gtk_label_set_text(GTK_LABEL (label_info), "Set the book");
      checks_passed = false;
    }
  }

  // Check whether the chapter has been set.
  if (checks_passed) {
    if (chapter < 0) {
      gtk_label_set_text(GTK_LABEL (label_info), "Set the chapter");
      checks_passed = false;
    }
  }

  // Check whether the verse numbers are right.
  if (checks_passed) {

    // Get the actual verses, and the verses that don't start a line.
    vector <ustring> non_line_starters;
    vector <ustring> actual_verses = get_verses(&non_line_starters);
    set <ustring> actual_verses_set(actual_verses.begin(), actual_verses.end());

    // Get the required verses in the book/chapter.
    vector <ustring> required_verses;
    ustring last_verse = versification_get_last_verse(versification, book, chapter);
    for (unsigned int i = 1; i <= convert_to_int(last_verse); i++) {
      required_verses.push_back(convert_to_string(i));
    }
    set <ustring> required_verses_set(required_verses.begin(), required_verses.end());

    // See whether any verses are missing.
    ustring missing_verses;
    for (unsigned int i = 0; i < required_verses.size(); i++) {
      if (actual_verses_set.find(required_verses[i]) == actual_verses_set.end()) {
        if (!missing_verses.empty())
          missing_verses.append(", ");
        missing_verses.append(required_verses[i]);
      }
    }
    if (!missing_verses.empty()) {
      missing_verses.insert(0, "Missing verses: ");
      gtk_label_set_text(GTK_LABEL (label_info), missing_verses.c_str());
      checks_passed = false;
    }

    // See whether there are extra verses.
    if (checks_passed) {
      ustring extra_verses;
      for (unsigned int i = 0; i < actual_verses.size(); i++) {
        if (required_verses_set.find(actual_verses[i]) == required_verses_set.end()) {
          if (!extra_verses.empty())
            extra_verses.append(", ");
          extra_verses.append(actual_verses[i]);
        }
      }
      if (!extra_verses.empty()) {
        extra_verses.insert(0, "Extra verses: ");
        gtk_label_set_text(GTK_LABEL (label_info), extra_verses.c_str());
        checks_passed = false;
      }
    }

    // See whether there are double verses.
    if (checks_passed) {
      ustring double_verses;
      set <ustring> double_set;
      for (unsigned int i = 0; i < actual_verses.size(); i++) {
        if (double_set.find(actual_verses[i]) != double_set.end()) {
          if (!double_verses.empty())
            double_verses.append(", ");
          double_verses.append(actual_verses[i]);
        }
        double_set.insert(actual_verses[i]);
      }
      if (!double_verses.empty()) {
        double_verses.insert(0, "Double verses: ");
        gtk_label_set_text(GTK_LABEL (label_info), double_verses.c_str());
        checks_passed = false;
      }
    }

    // See whether there are verses out of sequence.
    if (checks_passed) {
      for (unsigned int i = 0; i < actual_verses.size() - 1; i++) {
        unsigned int currentverse = convert_to_int(actual_verses[i]);
        unsigned int nextverse = convert_to_int(actual_verses[i+1]);
        if (nextverse != currentverse + 1) {
          if (checks_passed) {
            ustring msg("Verse " + actual_verses[i+1] + " is out of sequence");
            gtk_label_set_text(GTK_LABEL (label_info), msg.c_str());
            checks_passed = false;
          }
        }
      }
    }

    // See whether there are verses that don't start a line.
    if (checks_passed) {
      ustring nostart;
      for (unsigned int i = 0; i < non_line_starters.size(); i++) {
        if (!nostart.empty())
          nostart.append(", ");
        nostart.append(non_line_starters[i]);
      }
      if (!nostart.empty()) {
        nostart.insert(0, "Verses that are not at the start of a line: ");
        gtk_label_set_text(GTK_LABEL (label_info), nostart.c_str());
        checks_passed = false;
      }
    }
    
    // Text can be approved even if not okay.
    okayable = true;

  }

  // Set sensitivity of the Discover button.
  gtk_widget_set_sensitive(button_discover_markup, (book > 0) && (chapter >= 0));

  // Final okay info in gui.
  gui_okay(image_ok, label_ok, checks_passed);
  if (checks_passed) {
    gtk_label_set_text(GTK_LABEL (label_info), "Everything's fine. Press OK to import the text");
  }

  // Set sensitivity of "Text okay" button.
  if (!text_was_discovered || checks_passed) 
    okayable = false;
  if (!okayable)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_okay), false);
  gtk_widget_set_sensitive(checkbutton_okay, okayable);
  
  // Set sensitivity of Okay button.
  gtk_widget_set_sensitive(okbutton1, checks_passed || gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_okay)));
}

vector <ustring> ImportRawTextDialog::get_verses(vector <ustring> * non_line_starters)
// Gets the verse numbers from the textbuffer, and cleans them at the same time.
{
  vector <ustring> verses;
  GtkTextIter iter;
  ustring marker, verse;
  bool in_usfm = false;
  bool in_verse = false;
  bool last_marker_started_line = false;
  gtk_text_buffer_get_start_iter(textbuffer, &iter);
  do {
    gunichar character = gtk_text_iter_get_char(&iter);
    gchar buf[7];
    gint length = g_unichar_to_utf8(character, (gchar *) &buf);
    buf[length] = '\0';
    if (in_verse) {
      verse.append(buf);
    }
    if (g_unichar_isspace(character)) {
      in_usfm = false;
      in_verse = false;
      if (!marker.empty()) {
        if (marker == "v") {
          in_verse = true;
        }
        marker.clear();
      }
      if (!verse.empty()) {
        verses.push_back(trim(verse));
        if (!last_marker_started_line) {
          if (non_line_starters)
            non_line_starters->push_back(verse);
        }
        verse.clear();
      }
    }
    if (in_usfm) {
      marker.append(buf);
    }
    if (character == '\\') {
      in_usfm = true;
      last_marker_started_line = gtk_text_iter_starts_line(&iter);
    }
  } while (gtk_text_iter_forward_char(&iter));
  return verses;
}

