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
#include "windowsourcelanguages.h"
#include "help.h"
#include "window.h"
#include "keyterms.h"
#include "tiny_utilities.h"
#include "projectutils.h"
#include "settings.h"
#include "keyboard.h"
#include "dialogentry3.h"
#include "gtkwrappers.h"
#include "referenceutils.h"
#include <sqlite3.h>
#include "gwrappers.h"
#include "directories.h"
#include "utilities.h"
#include "bible.h"
#include "usfmtools.h"
#include "dialogeditlist.h"
#include "kjv.h"
#include "books.h"
#include "lexicons.h"
#include "dialogentry.h"
#include "robinson.h"
#include "sourcelanguage.h"


WindowSourceLanguages::WindowSourceLanguages(GtkAccelGroup * accelerator_group, bool startup, GtkWidget * parent_box):
WindowBase(widSourceLanguages, "Source languages", startup, 0, parent_box), reference(0, 0, "")
// Window for working with the source languages.  
{
  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_container_add(GTK_CONTAINER(window_vbox), scrolledwindow);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  htmlview = gtk_html_new();
  gtk_widget_show(htmlview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), htmlview);
  gtk_html_allow_selection(GTK_HTML(htmlview), true);

  g_signal_connect((gpointer) htmlview, "link-clicked", G_CALLBACK(on_html_link_clicked), gpointer(this));

  // Signal button.
  signal_button = gtk_button_new();

  // Main focused widget.
  last_focused_widget = htmlview;
  gtk_widget_grab_focus (last_focused_widget);
}


WindowSourceLanguages::~WindowSourceLanguages()
{
  // Destroy signal button.
  gtk_widget_destroy(signal_button);
}


void WindowSourceLanguages::go_to (const Reference& reference_in, const ustring& project_in)
{
  reference = reference_in;
  project = project_in;
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (project);
  language = projectconfig->language_get();
  html_link_clicked ("");
}


gboolean WindowSourceLanguages::on_html_link_clicked(GtkHTML * html, const gchar * url, gpointer user_data)
{
  ((WindowSourceLanguages *) user_data)->html_link_clicked(url);
  return true;
}


void WindowSourceLanguages::html_link_clicked (const gchar * url)
// Callback for clicking a link.
{
  // Store scrolling position for the now active url.
  GtkAdjustment * adjustment = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolledwindow));
  scrolling_position[active_url] = gtk_adjustment_get_value (adjustment);

  // Handle the back button and url mechanism.
  bool store_url = true;
  if (strcmp (url, "back") == 0) {
    if (url_stack.size() > 1) {
      active_url = url_stack[url_stack.size() - 2];
      store_url = false;
      url_stack.pop_back ();
    }
  } else {
    active_url = url;
  }

  // Maximum size of the url stack.
  if (url_stack.size() > 50) {
    url_stack.pop_front ();
  }

  // Start writing a html page.
  HtmlWriter2 htmlwriter ("");
  bool display_another_page = true;

  if (active_url.find ("goto ") == 0) {
    // Signal that a reference was clicked.
    ustring ref (active_url);
    ref.erase (0, 5);
    //reference.assign (references[active_entry]);
    //gtk_button_clicked(GTK_BUTTON(signal_button));
    display_another_page = false;
  }

  else if (active_url.find ("action") == 0) {
    // Go to the action page.
    html_write_action_page (htmlwriter);
  }

  else if (active_url.find ("tag ") == 0) {
    html_write_references (htmlwriter);
    main_morphologies.clear();
    main_strongs_numbers.clear();
    extra_strongs_numbers.clear();
    Parse parse (active_url.substr (3, 1000));
    for (unsigned int i = 0; i < parse.words.size(); i++) {
      if (parse.words[i].find ("morphology") == 0) {
        parse.words[i].erase (0, 10);
        main_morphologies.push_back (parse.words[i]);
      }
      if (parse.words[i].find ("strong") == 0) {
        parse.words[i].erase (0, 6);
        main_strongs_numbers.push_back (convert_to_int (parse.words[i]));
      }
    }
    html_write_morphology_and_strongs_definitions (htmlwriter);
  }

  else if (active_url.find ("strong ") == 0) {
    html_write_references (htmlwriter);
    extra_strongs_numbers.push_back (active_url.substr (7, 100));
    html_write_morphology_and_strongs_definitions (htmlwriter);
  }

  else if (active_url.find ("definition") == 0) {
    html_write_references (htmlwriter);
    EntryDialog dialog ("Strong's number", enter_strongs_number (), "");
    if (dialog.run() == GTK_RESPONSE_OK) {
      main_morphologies.clear();
      main_strongs_numbers.clear();
      extra_strongs_numbers.clear();
      extra_strongs_numbers.push_back (dialog.entered_value);
    } else {
      display_another_page = false;
    }
    html_write_morphology_and_strongs_definitions (htmlwriter);
  }

  else if (active_url.find ("searchtext") == 0) {
    display_another_page = false;
    EntryDialog dialog ("Search", "Please enter the text to search for", "");
    if (dialog.run() == GTK_RESPONSE_OK) {
      references = kjv_search_text (dialog.entered_value);
      gtk_button_clicked(GTK_BUTTON(signal_button));
    }
  }

  else if (active_url.find ("searchstrong") == 0) {
    display_another_page = false;
    EntryDialog dialog ("Strong's number", enter_strongs_number (), "");
    if (dialog.run() == GTK_RESPONSE_OK) {
      references = kjv_search_strong (dialog.entered_value);
      gtk_button_clicked(GTK_BUTTON(signal_button));
    }
  }

  else if (active_url.find ("addlanguage") == 0) {
    source_language_add_to_display ();
    html_write_references (htmlwriter);
  }

  else if (active_url.find ("removelanguage") == 0) {
    source_language_remove_from_display ();
    html_write_references (htmlwriter);
  }

  else {
    // Load the text.
    html_write_references (htmlwriter);
  }
  
  htmlwriter.finish();
  if (display_another_page) {
    // Load the page.
    GtkHTMLStream *stream = gtk_html_begin(GTK_HTML(htmlview));
    gtk_html_write(GTK_HTML(htmlview), stream, htmlwriter.html.c_str(), -1);
    gtk_html_end(GTK_HTML(htmlview), stream, GTK_HTML_STREAM_OK);
    // Scroll to the position that possibly was stored while this url was last active.
    gtk_adjustment_set_value (adjustment, scrolling_position[active_url]);
    // Store the url on the stack.
    if (store_url) {
      url_stack.push_back (active_url);
    }
  }
}


void WindowSourceLanguages::html_write_references (HtmlWriter2& htmlwriter)
{
  // Write action bar.
  html_write_action_bar (htmlwriter, true);

  // Go through all the source languages.
  extern Settings * settings;
  vector <ustring> names = settings->genconfig.source_language_names_get ();
  for (unsigned int i = 0; i < names.size(); i++) {

    // Get the verse with the tags.
    vector <ustring> words;
    vector <unsigned int> lemmata_positions;
    vector <unsigned int> lemmata_values;
    vector <unsigned int> morphology_positions;
    vector <ustring> morphology_values;
    source_language_get_lemmata_and_morphology (names[i], reference, words, lemmata_positions, lemmata_values, morphology_positions, morphology_values);

    // Write the verse with hyperlinks for the tags.
    if (!words.empty ()) {
      htmlwriter.paragraph_open ();
      for (unsigned int i = 0; i < words.size(); i++) {
        if (i) {
          htmlwriter.text_add (" ");
        }
        ustring link;
        for (unsigned int i2 = 0; i2 < lemmata_positions.size(); i2++) {
          if (lemmata_positions[i2] == i) {
            link.append (" ");
            link.append ("strong" + convert_to_string (lemmata_values[i2]));
          }
        }
        for (unsigned int i2 = 0; i2 < morphology_positions.size(); i2++) {
          if (morphology_positions[i2] == i) {
            link.append (" ");
            link.append ("morphology" + morphology_values[i2]);
          }
        }
        if (!link.empty()) {
          htmlwriter.hyperlink_add ("tag" + link, words[i]);
        } else {
          htmlwriter.text_add (words[i]);
        }
      }  
      htmlwriter.paragraph_close ();
    }
  }  

  // If there are no source languages currently displaying, give a link to add one.  
  if (names.empty()) {
    htmlwriter.paragraph_open ();
    htmlwriter.text_add ("No source language being displayed, ");
    htmlwriter.hyperlink_add ("addlanguage", "add one");
    htmlwriter.paragraph_close ();
  }
}


void WindowSourceLanguages::html_write_action_bar (HtmlWriter2& htmlwriter, bool topbar)
{
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add ("actions", "[actions]");
  htmlwriter.paragraph_close ();
}


void WindowSourceLanguages::html_write_action_page (HtmlWriter2& htmlwriter)
{
  // Write the link for going back to the references.
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add ("back", "[back]");
  htmlwriter.text_add (" ");
  htmlwriter.hyperlink_add ("", "[home]");
  htmlwriter.paragraph_close ();
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add ("searchtext", "Search for text");
  htmlwriter.paragraph_close ();
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add ("searchstrong", "Search on a Strong's number");
  htmlwriter.paragraph_close ();
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add ("definition", "Enter a Strong's number and view its definition");
  htmlwriter.paragraph_close ();
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add ("addlanguage", "Add");
  htmlwriter.text_add (" a source language to the ones being used or ");
  htmlwriter.hyperlink_add ("removelanguage", "remove");
  htmlwriter.text_add (" one");
  htmlwriter.paragraph_close ();
}


void WindowSourceLanguages::html_write_morphology_and_strongs_definitions (HtmlWriter2& htmlwriter)
{
  // Store the definitions.
  vector <ustring> definitions;
  
  // Main morphologies.
  for (unsigned int i = 0; i < main_morphologies.size(); i++) {
    bool in_new_testament = books_id_to_type (reference.book) == btNewTestament;
    if (in_new_testament) {
      definitions.push_back (robinson_define_parsing (main_morphologies[i]));
    } else {
      definitions.push_back (main_morphologies[i]);
    }
  }
  
  // Main Strong's definitions.
  for (unsigned int i = 0; i < main_strongs_numbers.size(); i++) {
    ustring main_definition = lexicons_get_definition (books_id_to_type (reference.book) == btNewTestament, main_strongs_numbers[i]);
    definitions.push_back (main_definition);
  }

  // Extra Strong's definitions.
  for (unsigned int i = 0; i < extra_strongs_numbers.size(); i++) {
    bool greek_lexicon = extra_strongs_numbers[i].substr (0, 1) == "G";
    ustring definition = lexicons_get_definition (greek_lexicon, convert_to_int (number_in_string (extra_strongs_numbers[i])));
    definitions.push_back (definition);
  }  
  
  // Display the definitions.
  for (unsigned int i = 0; i < definitions.size(); i++) {
    htmlwriter.paragraph_open ();
    ustring definition = definitions[i];
    while (!definition.empty()) {
      size_t strongs_link_position = definition.find ("STRONGS");
      if (strongs_link_position == string::npos) {
        htmlwriter.text_add (definition);
        definition.clear();
      } else if (strongs_link_position == 0) {
        definition.erase (0, 7);
        ustring greek_or_hebrew = definition.substr (0, 1);
        definition.erase (0, 1);
        ustring strongs_number = number_in_string (definition);
        size_t pos = definition.find (strongs_number);
        if (pos != string::npos) {
          definition.erase (0, pos + strongs_number.length());
        }
        htmlwriter.hyperlink_add ("strong " + greek_or_hebrew + strongs_number, strongs_number);
      } else {
        htmlwriter.text_add (definition.substr (0, strongs_link_position));
        definition.erase (0, strongs_link_position);
      }
    }
    htmlwriter.paragraph_close ();
  }  
}


const gchar * WindowSourceLanguages::enter_strongs_number ()
{
  return "Please enter a Strong's number,\n"
         "for example \"G5547\" for number 5547 in the Greek lexicon,\n"
         "or \"H430\" for number 430 in the Hebrew lexicon";
}
