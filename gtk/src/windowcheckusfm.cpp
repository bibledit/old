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
#include <glib.h>
#include "windowcheckusfm.h"
#include "floatingwindow.h"
#include "settings.h"
#include "shortcuts.h"
#include "gwrappers.h"
#include "gui.h" 
#include "utilities.h"
#include "versification.h"
#include "tiny_utilities.h"
#include "combobox.h"
#include "scripts.h"


WindowCheckUSFM::WindowCheckUSFM(GtkWidget * parent_layout, GtkAccelGroup *accelerator_group, bool startup):
FloatingWindow(parent_layout, widCheckUSFM, "Check USFM", startup)
// Window for checking USFM.
{
  // Initialize variables.
  textbuffer = NULL;
  
  Shortcuts shortcuts (0);
  
  vbox = gtk_vbox_new (FALSE, 6);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (vbox_client), vbox);

  checkbutton_verses_at_start = gtk_check_button_new_with_mnemonic ("Verses in raw text are at line start");
  gtk_widget_show (checkbutton_verses_at_start);
  gtk_box_pack_start (GTK_BOX (vbox), checkbutton_verses_at_start, FALSE, FALSE, 0);
  
  shortcuts.button (checkbutton_verses_at_start);
  connect_focus_signals (checkbutton_verses_at_start);

  vbox_filter = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_filter);
  gtk_box_pack_start (GTK_BOX (vbox), vbox_filter, TRUE, TRUE, 0);

  label_filter = gtk_label_new ("Filter");
  gtk_widget_show (label_filter);
  gtk_box_pack_start (GTK_BOX (vbox_filter), label_filter, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_filter), 0, 0.5);
  
  shortcuts.label (label_filter);

  combobox_filter = gtk_combo_box_new_text ();
  gtk_widget_show (combobox_filter);
  gtk_box_pack_start (GTK_BOX (vbox_filter), combobox_filter, FALSE, FALSE, 0);

  connect_focus_signals (combobox_filter);
  
  button_filter = gtk_button_new ();
  gtk_widget_show (button_filter);
  gtk_box_pack_start (GTK_BOX (vbox_filter), button_filter, FALSE, FALSE, 0);

  connect_focus_signals (button_filter);
  
  alignment4 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment4);
  gtk_container_add (GTK_CONTAINER (button_filter), alignment4);

  hbox6 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox6);
  gtk_container_add (GTK_CONTAINER (alignment4), hbox6);

  image4 = gtk_image_new_from_stock ("gtk-clear", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image4);
  gtk_box_pack_start (GTK_BOX (hbox6), image4, FALSE, FALSE, 0);

  label10 = gtk_label_new_with_mnemonic ("Filter text");
  gtk_widget_show (label10);
  gtk_box_pack_start (GTK_BOX (hbox6), label10, FALSE, FALSE, 0);
  
  shortcuts.label (label10);

  button_discover_markup = gtk_button_new ();
  gtk_widget_show (button_discover_markup);
  gtk_box_pack_start (GTK_BOX (vbox), button_discover_markup, FALSE, FALSE, 0);

  connect_focus_signals (button_discover_markup);
  
  alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (button_discover_markup), alignment3);

  hbox5 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox5);
  gtk_container_add (GTK_CONTAINER (alignment3), hbox5);

  image3 = gtk_image_new_from_stock ("gtk-zoom-fit", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image3);
  gtk_box_pack_start (GTK_BOX (hbox5), image3, FALSE, FALSE, 0);

  label8 = gtk_label_new_with_mnemonic ("Discover markup");
  gtk_widget_show (label8);
  gtk_box_pack_start (GTK_BOX (hbox5), label8, FALSE, FALSE, 0);

  shortcuts.label (label8);
  
  hbox_information = gtk_hbox_new (FALSE, 5);
  gtk_widget_show (hbox_information);
  gtk_box_pack_start (GTK_BOX (vbox), hbox_information, TRUE, TRUE, 0);

  image5 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image5);
  gtk_box_pack_start (GTK_BOX (hbox_information), image5, FALSE, TRUE, 0);

  label_information_ok = gtk_label_new ("Ok");
  gtk_widget_show (label_information_ok);
  gtk_box_pack_start (GTK_BOX (hbox_information), label_information_ok, FALSE, FALSE, 0);

  label_information_text = gtk_label_new ("Information");
  gtk_widget_show (label_information_text);
  gtk_box_pack_start (GTK_BOX (hbox_information), label_information_text, FALSE, FALSE, 0);
  gtk_label_set_line_wrap (GTK_LABEL (label_information_text), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label_information_text), 0, 0.5);

  gtk_label_set_mnemonic_widget(GTK_LABEL(label_filter), combobox_filter);

  g_signal_connect ((gpointer) button_filter, "clicked", G_CALLBACK (on_button_filter_clicked), gpointer(this));
  g_signal_connect ((gpointer) button_discover_markup, "clicked", G_CALLBACK (on_button_discover_markup_clicked), gpointer(this));
  
  shortcuts.process();
  
  // Load the available filters.  
  vector < ustring > filters = scripts_get_all();
  combobox_set_strings(combobox_filter, filters);
  combobox_set_index(combobox_filter, 0);
  
  // Main focused widget.
  last_focused_widget = button_discover_markup;
  gtk_widget_grab_focus (last_focused_widget);
}


WindowCheckUSFM::~WindowCheckUSFM()
{
}


void WindowCheckUSFM::on_button_discover_markup_clicked (GtkButton *button, gpointer user_data)
{
  ((WindowCheckUSFM *) user_data)->on_button_discover_markup();
}


void WindowCheckUSFM::on_button_discover_markup ()
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

    // Trim the line.
    lines[i] = trim (lines[i]);
    
    // Remove chapter markup.
    if (lines[i].find("\\c") != string::npos) {
      lines[i].clear();
      continue;
    }
    
    // Skip line starting with a backslash. The rationale is that this line already has markup.
    if (lines[i].substr(0, 1) == "\\")
      continue;

    // If the line is a number on its own, and the number agrees with the chapter number
    // that was set, it silently removes this line. But if it differs, an error comes up.
    if (discoveries_passed) {
      if (number_in_string(lines[i]) == lines[i]) {
        unsigned int number = convert_to_int(number_in_string(lines[i]));
        if (number == chapter) {
          lines[i].clear();
          continue;
        }
        ustring msg = "The line that contains " + lines[i] + " looks like a chapter number, but the number differs from the chapter that was set";
        gtk_label_set_text(GTK_LABEL(label_information_text), msg.c_str());
        discoveries_passed = false;
      }
    }
    
    // If the line has no number in it, 
    // and it ends with some type of punctuation,
    // it is considered a a normal paragraph.
    // If no punctuation at the end, it is a section heading.
    if (discoveries_passed) {
      if (number_in_string(lines[i]).empty()) {
        ustring last_character = lines[i].substr(lines[i].length() -1, 1);
        gunichar unichar = g_utf8_get_char(last_character.c_str());
        if (g_unichar_ispunct(unichar)) {
          lines[i].insert(0, "\\p ");
        } else {
          lines[i].insert(0, "\\s ");
        }
        continue;
      }
    }
    
    // If a number is found in the line, then this is considered a verse number.
    // The first time a number is found, a \p is prefixed.
    bool paragraph_open = false;
    if (discoveries_passed) {
      ustring output;
      ustring number = number_in_string(lines[i]);
      // Setting for having the number only at the start of the line.
      bool treat_as_normal_paragraph = false;
      if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_verses_at_start))) {
        if (lines[i].find (number) != 0) {
          number.clear();
          treat_as_normal_paragraph = true;
        }
      }
      if (treat_as_normal_paragraph) {

        // Normal paragraph.
        lines[i].insert(0, "\\p ");

      } else {

        // Find all verse numbers.   
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
          // Setting for discovering only first number in a paragraph.
          if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_verses_at_start))) {
            number.clear();
          }
        }

      }
      // Store line.
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
  gtk_text_buffer_set_text(textbuffer, newtext.c_str(), -1);

  // Restore cursor position.
  gtk_text_buffer_get_iter_at_offset(textbuffer, &iter, cursoroffset);
  gtk_text_buffer_place_cursor(textbuffer, &iter);

  // Update gui if all discoveries passed.
  if (discoveries_passed)
    on_editors_changed();
}

  
void WindowCheckUSFM::on_button_filter_clicked (GtkButton *button, gpointer user_data)
{
  ((WindowCheckUSFM *) user_data)->on_button_filter();
}


void WindowCheckUSFM::on_button_filter ()
// Filter the text.
{
  // Save the text from the buffer to disk.
  vector < ustring > lines;
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
  gchar *outputtext;
  g_file_get_contents(script_temporal_output_file().c_str(), &outputtext, NULL, NULL);
  if (outputtext) {
    gtk_text_buffer_set_text(textbuffer, outputtext, -1);
    g_free(outputtext);
  } else {
    gtk_text_buffer_set_text(textbuffer, "", -1);
  }
}


void WindowCheckUSFM::set_parameters (GtkTextBuffer * buffer, const ustring& project_in, unsigned int book_in, unsigned int chapter_in)
// This function is called if any of the parameters changed.
{
  // Set the new data;
  textbuffer = buffer;
  project = project_in;
  extern Settings *settings;
  versification = settings->projectconfig(project)->versification_get();
  book = book_in;
  chapter = chapter_in;
  // Set controls active if there's a textbuffer available.
  gtk_widget_set_sensitive (checkbutton_verses_at_start, textbuffer != NULL);
  gtk_widget_set_sensitive (label_filter, textbuffer != NULL);
  gtk_widget_set_sensitive (combobox_filter, textbuffer != NULL);
  gtk_widget_set_sensitive (button_filter, textbuffer != NULL);
  gtk_widget_set_sensitive (button_discover_markup, textbuffer != NULL);
  // Editors changed.
  gw_destroy_source(editors_changed_event_id);
  editors_changed_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 100, GSourceFunc(on_editors_changed_timeout), gpointer(this), NULL);
}


bool WindowCheckUSFM::on_editors_changed_timeout(gpointer user_data)
{
  ((WindowCheckUSFM *) user_data)->on_editors_changed();
  return false;
}

void WindowCheckUSFM::on_editors_changed()
// This function is called shortly after any of the Editors changed.
{
  // Clear event id.
  editors_changed_event_id = 0;

  // Consecutive checks.
  bool checks_going = true;

  // See whether a textbuffer is available.
  if (checks_going) {
    if (textbuffer == NULL) {
      gtk_label_set_text(GTK_LABEL(label_information_text), "Inactive. To activate, open or focus a project, and view the USFM code.");
      checks_going = false;
    }
  }
  
  // See whether text is available.
  if (checks_going) {
    GtkTextIter enditer;
    gtk_text_buffer_get_end_iter(textbuffer, &enditer);
    if (gtk_text_iter_get_offset(&enditer) < 5) {
      gtk_label_set_text(GTK_LABEL(label_information_text), "There is no text. Put text into the editor");
      checks_going = false;
    }
  }
  if (checks_going) {
    GtkTextIter enditer;
    gtk_text_buffer_get_end_iter(textbuffer, &enditer);
    if (gtk_text_iter_get_offset(&enditer) < 10) {
      gtk_label_set_text(GTK_LABEL(label_information_text), "There isn't much text. Put more text into the editor");
      checks_going = false;
    }
  }

  // Check whether the book seems ok.
  if (checks_going) {
    if (book == 0) {
      gtk_label_set_text(GTK_LABEL(label_information_text), "Unknown book");
      checks_going = false;
    }
  }

  // Check whether the chapter seems ok.
  if (checks_going) {
    if (chapter < 0) {
      gtk_label_set_text(GTK_LABEL(label_information_text), "Unknown chapter");
      checks_going = false;
    }
  }

  // Check whether the verse numbers are right.
  if (checks_going) {

    // Get the text from the textbuffer.
    vector < ustring > lines;
    textbuffer_get_lines(textbuffer, lines, false);
    
    // Get the actual verses, and the verses that don't start a line.
    vector < ustring > non_line_starters;
    vector < ustring > actual_verses = get_verses(&non_line_starters);
    set < ustring > actual_verses_set(actual_verses.begin(), actual_verses.end());

    // Get the required verses in the book/chapter.
    vector < ustring > required_verses;
    ustring last_verse = versification_get_last_verse(versification, book, chapter);
    for (unsigned int i = 1; i <= convert_to_int(last_verse); i++) {
      required_verses.push_back(convert_to_string(i));
    }
    set < ustring > required_verses_set(required_verses.begin(), required_verses.end());

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
      gtk_label_set_text(GTK_LABEL(label_information_text), missing_verses.c_str());
      checks_going = false;
    }
    // See whether there are extra verses.
    if (checks_going) {
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
        gtk_label_set_text(GTK_LABEL(label_information_text), extra_verses.c_str());
        checks_going = false;
      }
    }
    // See whether there are double verses.
    if (checks_going) {
      ustring double_verses;
      set < ustring > double_set;
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
        gtk_label_set_text(GTK_LABEL(label_information_text), double_verses.c_str());
        checks_going = false;
      }
    }
    // See whether there are verses out of sequence.
    if (checks_going) {
      for (unsigned int i = 0; i < actual_verses.size() - 1; i++) {
        unsigned int currentverse = convert_to_int(actual_verses[i]);
        unsigned int nextverse = convert_to_int(actual_verses[i + 1]);
        if (nextverse != currentverse + 1) {
          if (checks_going) {
            ustring msg("Verse " + actual_verses[i + 1] + " is out of sequence");
            gtk_label_set_text(GTK_LABEL(label_information_text), msg.c_str());
            checks_going = false;
          }
        }
      }
    }
    // See whether there are verses that don't start a line.
    if (checks_going) {
      ustring nostart;
      for (unsigned int i = 0; i < non_line_starters.size(); i++) {
        if (!nostart.empty())
          nostart.append(", ");
        nostart.append(non_line_starters[i]);
      }
      if (!nostart.empty()) {
        nostart.insert(0, "Verses that are not at the start of a line: ");
        gtk_label_set_text(GTK_LABEL(label_information_text), nostart.c_str());
        checks_going = false;
      }
    }

  }

  // Final okay info in gui.
  gui_okay(image5, label_information_ok, checks_going);
  if (checks_going) {
    gtk_label_set_text(GTK_LABEL(label_information_text), "Everything seems to be fine.");
  }
}


vector < ustring > WindowCheckUSFM::get_verses(vector < ustring > *non_line_starters)
// Gets the verse numbers from the textbuffer, and cleans them at the same time.
{
  vector < ustring > verses;
  GtkTextIter iter;
  ustring marker, verse;
  bool in_usfm = false;
  bool in_verse = false;
  bool last_marker_started_line = false;
  gtk_text_buffer_get_start_iter(textbuffer, &iter);
  do {
    gunichar character = gtk_text_iter_get_char(&iter);
    gchar buf[7];
    gint length = g_unichar_to_utf8(character, (gchar *) & buf);
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


