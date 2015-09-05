/*
 Copyright (©) 2003-2015 Teus Benschop.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include <bible/import_run.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/archive.h>
#include <filter/usfm.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/logs.h>
#include <database/books.h>
#include <bible/logic.h>
#include <styles/logic.h>


void bible_import_run (string location, string bible)
{
  Database_Logs::log ("Importing Bible data from location " + location + " into Bible " + bible);

  string folder = filter_archive_uncompress (location);
  if (folder != "") location = folder;
  vector <string> files;
  if (filter_url_is_dir (location)) {
    filter_url_recursive_scandir (location, files);
  } else {
    files.push_back (location);
  }
  
  for (auto & file : files) {
    if (filter_url_is_dir (file)) continue;
    Database_Logs::log ("Examining file for import: " + file);
    string success_message = "";
    string error_message = "";
    string data = filter_url_file_get_contents (file);
    if (data != "") {
      if (unicode_string_is_valid (data)) {
        
        // Check whether this is USFM data.
        bool id = data.find ("\\id ") != string::npos;
        bool c = data.find ("\\c ") != string::npos;
        if (id || c) {
          bible_import_usfm (data, bible);
        } else {
          int book = 1; // Todo get from gui.
          int chapter = 1; // Todo get from gui.
          bible_import_text (data, bible, book, chapter);
        }
        
      } else {
        Database_Logs::log ("The file does not contain valid Unicode UTF-8 text.", true);
      }
    } else {
      Database_Logs::log ("Nothing was imported.", true);
    }
  }
  
  Database_Logs::log ("Import Bible data has finished");
}


// Import the USFM in $data into $bible.
void bible_import_usfm (string data, string bible)
{
  string stylesheet = styles_logic_standard_sheet ();
  vector <BookChapterData> book_chapter_text = usfm_import (data, stylesheet);
  for (auto & data : book_chapter_text) {
    int book_number = data.book;
    int chapter_number = data.chapter;
    string chapter_data = data.data;
    if (book_number > 0) {
      Bible_Logic::storeChapter (bible, book_number, chapter_number, chapter_data);
      string book_name = Database_Books::getUsfmFromId (book_number);
      Database_Logs::log ("Imported " + book_name + " " + convert_to_string (chapter_number));
    } else {
      Database_Logs::log ("Could not import this data: " + chapter_data.substr (0, 1000));
    }
  }
}


// Import raw $text into $bible $book $chapter.
void bible_import_text (string text, string bible, int book, int chapter) // Todo
{
  Database_Logs::log ("Could not import this data: " + text.substr (0, 1000));

}


/* Todo Bibledit-Gtk code.

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
        ustring msg = _("The line that contains ") + lines[i] + _(" looks like a chapter number, but the number differs from the chapter that was set");
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
      gtk_label_set_text(GTK_LABEL(label_information_text), _("Inactive. To activate, open or focus a project, and view the USFM code."));
      checks_going = false;
    }
  }
  
  // See whether text is available.
  if (checks_going) {
    GtkTextIter enditer;
    gtk_text_buffer_get_end_iter(textbuffer, &enditer);
    if (gtk_text_iter_get_offset(&enditer) < 5) {
      gtk_label_set_text(GTK_LABEL(label_information_text), _("There is no text. Put text into the editor"));
      checks_going = false;
    }
  }
  if (checks_going) {
    GtkTextIter enditer;
    gtk_text_buffer_get_end_iter(textbuffer, &enditer);
    if (gtk_text_iter_get_offset(&enditer) < 10) {
      gtk_label_set_text(GTK_LABEL(label_information_text), _("There isn't much text. Put more text into the editor"));
      checks_going = false;
    }
  }
  
  // Check whether the book seems ok.
  if (checks_going) {
    if (book == 0) {
      gtk_label_set_text(GTK_LABEL(label_information_text), _("Unknown book"));
      checks_going = false;
    }
  }
  
  // Check whether the chapter seems ok.
  if (checks_going) {
    if (chapter < 0) {
      gtk_label_set_text(GTK_LABEL(label_information_text), _("Unknown chapter"));
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
      missing_verses.insert(0, _("Missing verses: "));
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
        extra_verses.insert(0, _("Extra verses: "));
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
            ustring msg(_("Verse ") + actual_verses[i + 1] + _(" is out of sequence"));
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
        nostart.insert(0, _("Verses that are not at the start of a line: "));
        gtk_label_set_text(GTK_LABEL(label_information_text), nostart.c_str());
        checks_going = false;
      }
    }
    
  }
  
  // Final okay info in gui.
  gui_okay(image5, label_information_ok, checks_going);
  if (checks_going) {
    gtk_label_set_text(GTK_LABEL(label_information_text), _("Everything seems to be fine."));
  }
}


*/